
/*******************
 * MWHOTSPOT.H
 *******************/

#ifndef __MWHOTSPOT_H
#define __MWHOTSPOT_H



/* HotSport declarations */


STATUS InsertRichTextWithURL( NOTEHANDLE hNote, char* szURL, char* szText );


STATUS InsertPopup(NOTEHANDLE hNote,
                              char FAR * FAR *ppCDBuffer,
                              WORD FAR *pwItemSize,
                              WORD wBufferSize);

STATUS InsertButton(NOTEHANDLE hNote,
                               char FAR * FAR *ppCDBuffer,
                               WORD FAR *pwItemSize,
                               WORD wBufferSize);


STATUS InsertBar(NOTEHANDLE hNote,
                               char FAR * FAR *ppCDBuffer,
                               WORD FAR *pwItemSize,
                               WORD wBufferSize);


STATUS InsertAction(NOTEHANDLE hNote,
                               char FAR * FAR *ppCDBuffer,
                               WORD FAR *pwItemSize,
                               WORD wBufferSize);

STATUS InsertURL(NOTEHANDLE hNote,
                               char FAR * FAR *ppCDBuffer,
                               WORD FAR *pwItemSize,
                               WORD wBufferSize,
							   char* szURL,
							   char* szText);

STATUS InsertFile(NOTEHANDLE hNote,
                               char FAR * FAR *ppCDBuffer,
                               WORD FAR *pwItemSize,
                               WORD wBufferSize);


/* forward declarations for CD manipulation functions  */


BOOL PutPara (char far * far *ppBuf,
                         WORD wLength);

BOOL PutText(char far * far *ppBuf,
                        char far * pText,
                        WORD wLength, int Type);

STATUS PutButton(char far * far *pbRTItem,
                            char far * pButtonText,
                            WORD wLength,
                            WORD far *pwRTLength);

STATUS PutBar(char far * far *pbRTItem,
                            char far * pBarText,
                            WORD wLength,
                            WORD far *pwRTLength);


STATUS PutHotSpotEnd(char far * far *pbRTItem,
                                WORD wLength,
                                WORD far *pwRTLength);

STATUS PutV4HotSpotEnd(char far * far *pbRTItem,
                                WORD wLength,
                                WORD far *pwRTLength);

STATUS PutPabRef(char far * far *pbRTItem,
                            WORD wPabDefNumber,
                            WORD wLength,
                            WORD *pwRTLength);

/*  Error message definitions */

#define ERR_CREATING_RICHTEXT   (PKG_ADDIN + 10)

errortext(ERR_CREATING_RICHTEXT, "Error creating rich text item.")

#endif

