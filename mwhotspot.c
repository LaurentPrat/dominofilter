/*******************************************************************************

   File:        mwhotspot.c
   Program:     mwhotspot
   Purpose:     create popups, sections, buttons, a URL,
                in a rich text field.

******************************************************************************
* 30/03/2006	: FIX in InsertURL : 	memset(*ppCDBuffer, NULL, URLSize + 2 );
*				: as the buffer may get +1 if pointer is not odd. !!
******************************************************************************/

/*
 *  OS includes
 */

#if !defined(MAC)
#if !defined(UNIX)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#endif
#include <string.h>

/*
 *  Notes API includes
 */

#include <global.h>
#include <nsf.h>
#include <osmem.h>
#include <ods.h>
#include <names.h>
#include <editods.h>
#include <editdflt.h>
#include <nsfnote.h>
#include <ixport.h>
#include <ixedit.h>
#include <osmisc.h>
#include <oserr.h>
#include <stdnames.h>
#include <kfm.h>
#include <colorid.h>		/* NOTES_COLOR_BLACK */

/*
 *  Local includes
 */

#include "mwhotspot.h"

STATUS InsertRichTextWithURL( NOTEHANDLE hNote, char* szURL, char* szText )
{

    HANDLE    hMem;
    STATUS    sError;         /* Notes error status        */
    char FAR  *pCDBuffer;
    char FAR  *pCDBufferStart;
    WORD      wItemSize = 0, wBufferSize = MAXONESEGSIZE;

	/*
	*  Allocate a buffer and build the CD record 
	*/

    if (sError = OSMemAlloc(0, wBufferSize, &hMem))
        return (ERR_MEMORY);

    if ((pCDBuffer = (char far *) OSLockObject(hMem)) == NULL)
        return (ERR_MEMORY);

    memset(pCDBuffer, '\0', wBufferSize);
    pCDBufferStart = pCDBuffer;


	/*
	*  Now insert one CDPARAGRAPH records.
	*/

    sError = PutPara(&pCDBuffer, (WORD) (wBufferSize - (pCDBuffer - pCDBufferStart)));

    if (sError == FALSE)
    {
        OSUnlockObject(hMem);
        OSMemFree(hMem);
        return (sError);
    }


	/*
	 * Next, insert a URL into the rich text field.
	 */
    if ( sError = InsertURL(hNote, &pCDBuffer, &wItemSize, wBufferSize, szURL, szText ))
    {
        OSUnlockObject(hMem);
        OSMemFree(hMem);
        return (ERR(sError));
    }


	/*
	 *  All done creating the richtext field.  Now append it to the note.
	 */
     if ( sError = NSFItemAppend(hNote,
                                0,
                                "Body",
                                (WORD) strlen("Body"),
                                TYPE_COMPOSITE,
                                pCDBufferStart,
                                (DWORD) (pCDBuffer-pCDBufferStart)) )

     {
        OSUnlockObject(hMem);
        OSMemFree(hMem);
        return (ERR(sError));
     }

    OSUnlockObject(hMem);
    OSMemFree(hMem);

    return (NOERROR);

}



/*************************************************************************
 * InsertButton
 *      To define the button:
 *
 *      - Append to the CD buffer a CDHOTSPOTBEGIN record, with the
 *        flag member set to HOTSPOTREC_REC_BUTTON, and with the other
 *        data members set appropriately. The formula to be run when
 *        the button is activated is appended to the CDHOTSPOTBEGIN record.
 *
 *      - A CDBUTTON record, which defines the appearance of the button.
 *
 *      - A CDHOTSPOTEND record, to mark the end of the button definition.
 *
 *
 * Inputs:     hNote       - handle to the new note being created.
 *             ppCDBuffer  - handle to buffer containing CD record being
 *                           created.
 *             pwItemSize  - current length of item.
 *             wBufferSize - total size of CD buffer.
 *
 * Outputs:    None.
 *************************************************************************/

STATUS InsertButton(NOTEHANDLE hNote,
                    char FAR * FAR *ppCDBuffer,
                    WORD FAR *pwItemSize,
                    WORD wBufferSize)

{
    STATUS         sError;                   /* Notes error status        */
    CDHOTSPOTBEGIN pHotSpotBegin;
    char           szButtonText[] = "Print...";
    char           szButtonFormula[] = "@Command([FilePrint])";
    char far      *bufPtr;

    FORMULAHANDLE  hFormula;
    char *pFormula;        /* pointer to compiled formula. */
    WORD wFormulaLen = 0;
    WORD wdc;              /* "We Don't Care" - We're not interested in some
                            * of the info passed back by NSFFormulaCompile(),
                            * but the function call requires the addresses of
                            * several words to be passed in. The address of
                            * this word is used for all parameters in which
                            * we have no interest.
                            */

    /*
     * Set various flags
     */

    pHotSpotBegin.Type = HOTSPOTREC_TYPE_BUTTON;
    pHotSpotBegin.Flags = HOTSPOTREC_RUNFLAG_BEGIN |
                                  HOTSPOTREC_RUNFLAG_NOBORDER;

/*
 *  Compile the button formula, append compiled formula to
 *  HotSpotBegin data structure.
 */

    sError = NSFFormulaCompile(NULL,
                               0,
                               szButtonFormula,
                               (WORD)strlen(szButtonFormula),
                               &hFormula,
                               &wFormulaLen,
                               &wdc, &wdc, &wdc, &wdc, &wdc);

    if (sError)
    {
        return (ERR(sError));
    }

        pHotSpotBegin.DataLength = wFormulaLen;

/*
 *  Set the header info.
 */

    pHotSpotBegin.Header.Signature = SIG_CD_HOTSPOTBEGIN;
    pHotSpotBegin.Header.Length = ODSLength(_CDHOTSPOTBEGIN)
                                    + pHotSpotBegin.DataLength;


    *pwItemSize += pHotSpotBegin.Header.Length;

    if (*pwItemSize %2)
        *pwItemSize +=1;

    ODSWriteMemory( (void far * far *)ppCDBuffer, _CDHOTSPOTBEGIN, &pHotSpotBegin, 1 );

    pFormula = (char far *) OSLockObject(hFormula);

    memcpy(*ppCDBuffer,
            pFormula,
            wFormulaLen);

    OSUnlockObject(hFormula);    /* unlock and free formula's memory.  */

    OSMemFree(hFormula);

    *ppCDBuffer += wFormulaLen;

    if (((LONG)*ppCDBuffer) % 2)
        *ppCDBuffer += 1;


/*
 * Done with the CDHOTSPOTBEGIN. Now add a CDBUTTON item to define the
 * appearance of the button being inserted.
 */

    bufPtr = *ppCDBuffer;
    if (sError = PutButton(ppCDBuffer,
                      (char *)szButtonText,
                      (WORD)(wBufferSize - *pwItemSize),
                      pwItemSize))
    {
        return (ERR(sError));
    }

    *pwItemSize += (WORD) (*ppCDBuffer - bufPtr);

/*
 * Done with the CDBUTTON item.  Now insert a CDHOTSPOTEND item.
 */

    if (sError = PutHotSpotEnd(ppCDBuffer,
                           (WORD)(wBufferSize - *pwItemSize),
                           pwItemSize))
    {
        return (ERR(sError));
    }

/*
 *  Done defining the button.
 */

    return (NOERROR);
}



/***********************************************************************
 * InsertURL
 *                The steps needed to do this are:
 *
 *                - Create an appropriate CDV4HOTSPOTBEGIN record
 *                  and append it to the buffer.
 *
 *                - Add a CDTEXT record that will contain the URL address
 *                - Add a CDHOTSPOTEND record to mark the end of the hotspot.
 *
 *
 *
 * Inputs:     hNote       - handle to the new note being created.
 *             ppCDBuffer  - Handle to buffer containing CD record being
 *                           created.
 *             pwItemSize  - current length of item.
 *             wBufferSize - total size of CD buffer.
 *
 * Outputs:    None.
 *************************************************************************/

STATUS InsertURL(NOTEHANDLE hNote,
                 char FAR * FAR *ppCDBuffer,
                 WORD FAR *pwItemSize,
                 WORD wBufferSize,
				 char* szURL,
				 char* szText )

{
    STATUS         sError;
    CDHOTSPOTBEGIN HotSpotBegin;
    char far       *bufPtr;
	size_t			URLSize;

/*
 *  Set various flags
 */

    HotSpotBegin.Type  = HOTSPOTREC_TYPE_HOTLINK;
    HotSpotBegin.Flags = HOTSPOTREC_RUNFLAG_BEGIN |
                         HOTSPOTREC_RUNFLAG_NOBORDER |
                         HOTSPOTREC_RUNFLAG_INOTES;

/*
 *  Set data length
 */

    URLSize = strlen(szURL) +1;		/* sizeof */
	HotSpotBegin.DataLength = (WORD) URLSize;

/*
 *  Set the header info.
 */

    HotSpotBegin.Header.Signature = SIG_CD_V4HOTSPOTBEGIN;
    HotSpotBegin.Header.Length    = ODSLength(_CDHOTSPOTBEGIN) + HotSpotBegin.DataLength;

    *pwItemSize += HotSpotBegin.Header.Length;

    if (*pwItemSize %2)
        *pwItemSize +=1;

	/* 1.6.4 : save buffer address */
	bufPtr = *ppCDBuffer;

    ODSWriteMemory( (void far * far *)ppCDBuffer, _CDHOTSPOTBEGIN, &HotSpotBegin, 1 );
	memcpy(*ppCDBuffer, szURL, URLSize);

    *ppCDBuffer += URLSize;	

    if (((LONG) *ppCDBuffer) % 2)
        *ppCDBuffer += 1;

	memset(*ppCDBuffer,'\0',1);

	/*
	 * Put text with appears for the URL
	 */
	
	sError = PutText(ppCDBuffer, szText, (wBufferSize - *pwItemSize), HOTSPOTREC_TYPE_HOTLINK );

    if (sError == FALSE)
    {
        return (sError);
    }
	
	/*
	* Done with the CDTEXT item.  Now insert a CDHOTSPOTEND item.
	*/

    if (sError = PutV4HotSpotEnd(ppCDBuffer, (wBufferSize - *pwItemSize), pwItemSize))
    {
        return (ERR(sError));
    }

    
	*pwItemSize += (WORD) (*ppCDBuffer - bufPtr);

/*
 *  Done constructing URL.
 */

    return (NOERROR);
}


/***********************************************************************
 * InsertFile
 *                The steps needed to do this are:
 *
 *                - Call NSFNoteAttachFile to create a $FILE item.
 *
 *                - Create an appropriate CDHOTSPOTBEGIN record
 *                  and append it to the buffer.
 *
 *                - Add a CDTEXT record that will contain the name of the
 *                  attached file.
 *
 *                - Add a CDHOTSPOTEND record to mark the end of the hotspot.
 *
 *
 * Inputs:     hNote       - handle to the new note being created.
 *             ppCDBuffer  - Handle to buffer containing CD record being
 *                           created.
 *             pwItemSize  - current length of item.
 *             wBufferSize - total size of CD buffer.
 *
 * Outputs:    None.
 *************************************************************************/


STATUS InsertFile(
        NOTEHANDLE hNote,
        char FAR * FAR *ppCDBuffer,
        WORD FAR *pwItemSize,
        WORD wBufferSize)
{
    STATUS         sError;
    CDHOTSPOTBEGIN HotSpotBegin;
    char far       *bufPtr;

#if !defined(UNIX)
    char szSourceFileName[] =
        "c:\\api_v5\\samples\\richtext\\hotspot\\readme.txt";
#else
    char szSourceFileName[] =
        "/opt/lotus/notesapi/samples/richtext/hotspot/readme.txt";
#endif

    char szAttachLinkName[] = "attach.lnk";
    char szAttachFileName[] = "readme.txt";

    if (sError = NSFNoteAttachFile( hNote,
                                    ITEM_NAME_ATTACHMENT,
                                    (WORD) strlen(ITEM_NAME_ATTACHMENT),
                                    szSourceFileName,
                                    szAttachLinkName,
                                    EFLAGS_INDOC | HOST_LOCAL | COMPRESS_NONE))
    {
        return(ERR(sError));
    }

/*
 *  Set various flags
 */

    HotSpotBegin.Type = HOTSPOTREC_TYPE_FILE;
    HotSpotBegin.Flags = HOTSPOTREC_RUNFLAG_BEGIN | HOTSPOTREC_RUNFLAG_NOBORDER;

/*
 *  Set data length
 */

    HotSpotBegin.DataLength = (WORD) ( strlen(szAttachLinkName) + strlen(szAttachFileName) + 2);

/*
 *  Set the header info.
 */

    HotSpotBegin.Header.Signature = SIG_CD_HOTSPOTBEGIN;
    HotSpotBegin.Header.Length = ODSLength(_CDHOTSPOTBEGIN) + HotSpotBegin.DataLength;

    *pwItemSize += HotSpotBegin.Header.Length;

    if (*pwItemSize %2)
        *pwItemSize +=1;

    bufPtr = *ppCDBuffer;

    ODSWriteMemory( (void far * far *)ppCDBuffer, _CDHOTSPOTBEGIN, &HotSpotBegin, 1 );

    memcpy(*ppCDBuffer, szAttachLinkName, sizeof(szAttachLinkName));
    *ppCDBuffer += sizeof(szAttachLinkName);

    memcpy(*ppCDBuffer, szAttachFileName, sizeof(szAttachFileName));
    *ppCDBuffer += sizeof(szAttachFileName);

    if (((LONG) *ppCDBuffer) % 2)
        *ppCDBuffer += 1;

    sError = PutText(ppCDBuffer, szAttachFileName, (WORD)(wBufferSize - *pwItemSize), 0);

    if (sError == FALSE)
    {
        return (sError);
    }

/*
 * Done with the CDTEXT item.  Now insert a CDHOTSPOTEND item.
 */

    if (sError = PutHotSpotEnd(ppCDBuffer,
                           (WORD)(wBufferSize - *pwItemSize),
                           pwItemSize))
    {
        return (ERR(sError));
    }

    *pwItemSize += (WORD) ( *ppCDBuffer - bufPtr );

/*
 *  Done constructing file attachment.
 */

    return (NOERROR);
}

/**********************************************************************
 *
 *   PutPara
 *       API function to write a CDPARAGRAPH segment to a buffer that will
 *       be written as a rich text field to Notes
 *
 *   Arguments:
 *       char far * far *ppBuf Address of pointer to next available byte in
 *                             buffer containing the item being created.
 *       WORD wLength          Number of bytes remaining in the buffer.
 *
 *    Returns:
 *        BOOL TRUE if Successful
 *        BOOL FALSE if Not Successful
 *
 *
 *   Comments:
 *
 ***********************************************************************/

BOOL PutPara( char far * far *ppBuf,
                         WORD wLength )
{
    CDPARAGRAPH CDPara;
    BYTE        bLength =  (BYTE) ODSLength(_CDPARAGRAPH);
    BYTE        bSig  =  (BYTE)SIG_CD_PARAGRAPH;


    /* If not enough space in buffer for this paragraph, then exit.   */

    if (wLength < ODSLength(_CDPARAGRAPH))
        return (FALSE);

/* Fill in PARAGRAPH item structure  */

    CDPara.Header.Length = bLength;/* (BYTE) ODSLength(_CDPARAGRAPH); */
    CDPara.Header.Signature = bSig;/* SIG_CD_PARAGRAPH; */

/*
 *  Convert the CDPARAGRAPH item to Notes Canonical format and store it in
 *  the ODS buffer.
 */

    ODSWriteMemory( (void far * far *)ppBuf, _CDPARAGRAPH, &CDPara, 1 );

    return (TRUE);
}


/**********************************************************************
 *
 *   PutHotSPotEnd
 *
 *   Arguments:
 *       char *pbRTItem       Ptr to buffer containing the CD record
 *                            being created.
 *       WORD wLength         Number of bytes remaining in the buffer.
 *       WORD FAR *pwRTLength Current length of the CD record;
 *
 *   Returns:
 *       STATUS NOERROR if Successful
 *       STATUS ERR_CREATING_RICHTEXT if Not Successful
 *
 *   Comments:
 *
 ***********************************************************************/

STATUS PutHotSpotEnd(char far * far *pbRTItem, WORD wLength,
                          WORD FAR *pwRTLength)
{
    CDHOTSPOTEND pCDHotSpotEnd;

/*
 *  if nothing in CD record or current record len is 0, then exit.
 */

    if (pbRTItem == NULL || pwRTLength == (WORD far *)NULL)
        return (ERR_CREATING_RICHTEXT);

/*
 *  If not enough space in buffer for this CDHotSpotEnd, then exit.
 */

    if (wLength < ODSLength(_CDHOTSPOTEND))
        return (ERR_CREATING_RICHTEXT);

/*
 *  Fill in CDHOTSPOTEND item structure
 */



    pCDHotSpotEnd.Header.Length = (BYTE)ODSLength(_CDHOTSPOTEND);
    pCDHotSpotEnd.Header.Signature = SIG_CD_HOTSPOTEND;

    ODSWriteMemory( (void far * far *)pbRTItem, _CDHOTSPOTEND,
                    &pCDHotSpotEnd, 1 );


/*
 *  Adjust current record length, forcing to an even byte count.
 */

    *pwRTLength += pCDHotSpotEnd.Header.Length;

    if (*pwRTLength % 2)
        (*pwRTLength)++;

    return (NOERROR);
}

/**********************************************************************
 *
 *   PutV4HotSPotEnd
 *
 *   Arguments:
 *       char *pbRTItem       Ptr to buffer containing the CD record
 *                            being created.
 *       WORD wLength         Number of bytes remaining in the buffer.
 *       WORD FAR *pwRTLength Current length of the CD record;
 *
 *   Returns:
 *       STATUS NOERROR if Successful
 *       STATUS ERR_CREATING_RICHTEXT if Not Successful
 *
 *   Comments:
 *
 ***********************************************************************/

STATUS PutV4HotSpotEnd(char far * far *pbRTItem, WORD wLength,
                          WORD FAR *pwRTLength)
{
    CDHOTSPOTEND pCDHotSpotEnd;

/*
 *  if nothing in CD record or current record len is 0, then exit.
 */

    if (pbRTItem == NULL || pwRTLength == (WORD far *)NULL)
        return (ERR_CREATING_RICHTEXT);

/*
 *  If not enough space in buffer for this CDHotSpotEnd, then exit.
 */

    if (wLength < ODSLength(_CDHOTSPOTEND))
        return (ERR_CREATING_RICHTEXT);

/*
 *  Fill in CDHOTSPOTEND item structure
 */



    pCDHotSpotEnd.Header.Length = (BYTE)ODSLength(_CDHOTSPOTEND);
    pCDHotSpotEnd.Header.Signature = SIG_CD_V4HOTSPOTEND;

    ODSWriteMemory( (void far * far *)pbRTItem,
                   _CDHOTSPOTEND, &pCDHotSpotEnd, 1 );


/*
 *  Adjust current record length, forcing to an even byte count.
 */

    *pwRTLength += pCDHotSpotEnd.Header.Length;

    if (*pwRTLength % 2)
        (*pwRTLength)++;

    return (NOERROR);
}



/**********************************************************************
 *
 *    PutButton
 *        API function to write a "rich text" button segment
 *        to a buffer that will be written as a rich text field to Notes.
 *
 *   Arguments:
 *       char *pbRTItem       ptr to buffer containing the CD record
 *                            being created.
 *       char *pButtonText    ptr to text to display on button.
 *       WORD wLength         Number of bytes remaining in the buffer.
 *       WORD FAR *pwRTLength Current length of the CD record;
 *
 *    Returns:
 *        STATUS NOERROR if Successful
 *        STATUS ERR_CREATING_RICHTEXT if Note Successful
 *
 *    Comments:
 *
 **********************************************************************/

STATUS PutButton(char far * far *pbRTItem, char *pButtonText,
                            WORD wLength, WORD FAR *pwRTLength)

{
    CDBUTTON       pcdButton;    /* begin a run of text */
    FONTIDFIELDS   *pFont;       /* font definitions in rich text field */
    WORD           wTextLength;

/*
 *  if nothing in CD record or current record len is 0, then exit.
 */

    if (pbRTItem == NULL || pwRTLength == (WORD far *)NULL)
        return (ERR_CREATING_RICHTEXT);

    if (pButtonText != NULL)
        wTextLength = (WORD) strlen(pButtonText);
    else
        wTextLength = 0;

    if ((ODSLength(_CDBUTTON) + wTextLength) > wLength)
        return (ERR_CREATING_RICHTEXT);

/*
 *  Fill in the BUTTON item structure.
 */

    pcdButton.Header.Signature = SIG_CD_BUTTON;
    pcdButton.Header.Length = ODSLength(_CDBUTTON) + wTextLength;


    pcdButton.Flags = 0;          /* No flags set for button. */
    pcdButton.Width = 2*ONEINCH;  /* Button is 2 inches wide. */
    pcdButton.Height = 0;
    pcdButton.Lines = 1;

 /*
  *  Fill in the font information for this button. Note that
  *  we must overlay the FONTIDFIELDS structure onto the
  *  pbRTItem.button.FontID data item.
  */

    pFont = (FONTIDFIELDS*)&pcdButton.FontID;

    pFont->Face = FONT_FACE_SWISS;
    pFont->Attrib = 0;
    pFont->Color = NOTES_COLOR_BLACK;
    pFont->PointSize = 10;

    ODSWriteMemory( (void far * far *)pbRTItem, _CDBUTTON, &pcdButton, 1 );

 /*
  *  Append text string to CDTEXT record.
  */
    if (pButtonText != NULL)
        {
        memcpy(*pbRTItem, pButtonText, wTextLength);
                *pbRTItem += wTextLength;
        }

    if (((LONG) *pbRTItem) % 2)
        *pbRTItem += 1;

    *pwRTLength += pcdButton.Header.Length;

    if (*pwRTLength % 2)
        (*pwRTLength)++;

    return (NOERROR);
}


/**********************************************************************
 *
 *    PutBar
 *        API function to write a "rich text" bar segment
 *        to a buffer that will be written as a rich text field to Notes.
 *
 *   Arguments:
 *       char *pbRTItem       ptr to buffer containing the CD record
 *                            being created.
 *       char *pBarText              ptr to text to display on bar.
 *       WORD wLength         Number of bytes remaining in the buffer.
 *       WORD FAR *pwRTLength Current length of the CD record;
 *
 *    Returns:
 *        STATUS NOERROR if Successful
 *        STATUS ERR_CREATING_RICHTEXT if Note Successful
 *
 *    Comments:
 *
 **********************************************************************/

STATUS PutBar(char far * far *pbRTItem, char *pBarText,
                            WORD wLength, WORD FAR *pwRTLength)

{
    CDBAR           pcdBar;       /* begin a run of text */
    FONTIDFIELDS   *pFont;        /* font definitions in rich text field */
    WORD            wTextLength;
    WORD            color;

/*
 *  if nothing in CD record or current record len is 0, then exit.
 */

    if (pbRTItem == NULL || pwRTLength == (WORD far *)NULL)
        return (ERR_CREATING_RICHTEXT);

    if (pBarText != NULL)
        wTextLength = (WORD) strlen(pBarText);
    else
        wTextLength = 0;

    if ((ODSLength(_CDBAR) + wTextLength + ODSLength(_WORD) + 1)> wLength)
        return (ERR_CREATING_RICHTEXT);

/*
 *  Fill in the BAR item structure.
 */

    pcdBar.Header.Signature = SIG_CD_BAR;
    pcdBar.Header.Length = ODSLength(_CDBAR) + wTextLength + ODSLength(_WORD) + 1;


    pcdBar.Flags = BARREC_HAS_COLOR;

    SETBORDERTYPE(pcdBar.Flags, BARREC_BORDER_NONE);

 /*
  *  Fill in the font information for this button. Note that
  *  we must overlay the FONTIDFIELDS structure onto the
  *  pbRTItem.button.FontID data item.
  */

    pFont = (FONTIDFIELDS*)&pcdBar.FontID;

    pFont->Face = FONT_FACE_SWISS;
    pFont->Attrib = 0;
    pFont->Color = NOTES_COLOR_BLACK;
    pFont->PointSize = 10;

    ODSWriteMemory( (void far * far *)pbRTItem, _CDBAR, &pcdBar, 1 );
        color = NOTES_COLOR_DKCYAN;
    ODSWriteMemory( (void far * far *)pbRTItem, _WORD, &color, 1 );

 /*
  *  Append text string to CDBAR record.
  */
    if (pBarText != NULL)
        {
                strcpy(*pbRTItem, pBarText);
                *pbRTItem += 1;
                *pbRTItem += wTextLength;
        }

    if (((LONG) *pbRTItem) % 2)
        *pbRTItem += 1;

    *pwRTLength += pcdBar.Header.Length;

    if (*pwRTLength % 2)
        (*pwRTLength)++;

    return (NOERROR);
}


/**********************************************************************
 *
 *    PutPabRef
 *        API function to write a "rich text" paragraph
 *        attribute block (PAB) reference segment to a
 *        buffer that will be written as a rich text field to Notes
 *
 *   Arguments:
 *       BYTE **ppbRTItem     Handle to buffer containing the CD record
 *                            being created.
 *       WORD wPabDefNumber   The ID of the PabDef to use in this PabRef.
 *       WORD wLength         Number of bytes remaining in the buffer.
 *       WORD FAR *pwRTLength Current length of the CD record;
 *
 *    Returns:
 *        STATUS NOERROR if Successful
 *        STATUS ERR_CREATING_RICHTEXT if Not Successful
 *
 *    Comments:
 *
 **********************************************************************/

STATUS PutPabRef(char far * far *ppbRTItem, WORD wPabDefNumber,
                            WORD wLength, WORD FAR *pwRTLength)
{
    CDPABREFERENCE  pcdPabRef;      /* style reference for this para */

/*
 *  if nothing in CD record or current record len is 0, then exit.
 */

    if (*ppbRTItem == NULL || pwRTLength == (WORD far *)NULL)
        return (ERR_CREATING_RICHTEXT);

/*
 *  If not enough space in buffer for this struct, then exit.
 */

    if (wLength < ODSLength(_CDPABREFERENCE))
        return (ERR_CREATING_RICHTEXT);

/*
 *  Fill in PABREF item structure.
 */


    pcdPabRef.Header.Signature = SIG_CD_PABREFERENCE;
    pcdPabRef.Header.Length    = (BYTE)ODSLength(_CDPABREFERENCE);
    pcdPabRef.PABID            = wPabDefNumber;

    *pwRTLength += ODSLength(_CDPABREFERENCE);

    if (*pwRTLength % 2)
        (*pwRTLength)++;

    ODSWriteMemory( (void far * far *)ppbRTItem, _CDPABREFERENCE, &pcdPabRef, 1 );

    return (NOERROR);
}

/**********************************************************************
 *
 *    PutText
 *        API function to write a "rich text" text segment 
 *        to a buffer that will be written as a rich text field to Notes.
 * 
 *   Arguments:
 *       char far * far *ppBuf Address of pointer to next available byte in 
 *                            buffer containing the item being created.
 *       char far * pText          Far ptr to text to go into CD record.
 *       WORD wLength         Number of bytes remaining in the buffer. 
 *       int  Type            Text Type - only checks for 
 *                                 HOTSPOTREC_TYPE_HOTLINK
 *                            otherwise pass 0.
 *    Returns:
 *        BOOL TRUE if Successful
 *        BOOL FALSE if Note Successful
 *
 *    Comments:
 *
 **********************************************************************/

BOOL PutText(char far * far *ppBuf,
                        char far * pText,
                        WORD wLength,
                        int Type)

{
    CDTEXT         CDText;        /* begin a run of text */
    FONTIDFIELDS   *pFont;        /* font definitions in rich text field */
    WORD           wTextLength;
    WORD           w;

    if (pText != NULL)
        wTextLength = (WORD) strlen(pText);
    else
        wTextLength = 0;

    w = ODSLength(_CDTEXT)+wTextLength;
    
    if ( wLength < w)

    /* (ODSLength(_CDTEXT) + wTextLength) > wLength */

       
        return (FALSE);

    /* Fill in the TEXT item structure.       */
    
    CDText.Header.Signature = SIG_CD_TEXT;
    CDText.Header.Length = ODSLength(_CDTEXT) + wTextLength;
  
   /*
    *  Fill in the font information for this run of text. Note that
    *  we must overlay the FONTIDFIELDS structure onto the
    *  CDText.FontID data item.
    */

    pFont = (FONTIDFIELDS*)&CDText.FontID;
    
    pFont->Face = FONT_FACE_SWISS;

 /*
  * Set the text to bold and underline if this is a Hotlink (URL).
  */
    
    if (Type == HOTSPOTREC_TYPE_HOTLINK) 
      pFont->Attrib = ISBOLD | ISUNDERLINE;
    else
      pFont->Attrib = 0;
 
    pFont->Color = NOTES_COLOR_BLACK;
    pFont->PointSize = 10;


/*
 *  Convert the CDTEXT item to Notes Canonical format and store it in
 *  the ODS buffer.
 */

    ODSWriteMemory( (void far * far *)ppBuf, _CDTEXT, &CDText, 1 );

    if (pText != NULL)
    {
        memcpy( *ppBuf, pText, wTextLength );  /* Append string to CDTEXT. */
        *ppBuf += wTextLength;
    }

	/* Get the address and check if is even, make even otherwise */    
	if ( ((LONG) (*ppBuf)) % 2)
        *ppBuf += 1;

	memset(*ppBuf,'\0',1);
	        
    return (TRUE);
}


