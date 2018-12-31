/**********************************************************************************
* file		: hnote2skip.c
*			: Lotus Notes C API, V4,V5,V6.
* ----------: --------------------------------------------------------------------
* PURPOSE	: little procedures that are required to skip some email from (Quota) 
*			: process when the hNote contains special fields.
***********************************************************************************/
#include "hnote2skip.h"

/*********************************** AS400 Patch *******************************/
#ifdef OS400
#define MW_EBCDIC
#define __STRING_CODE_SET__ 850
#pragma convert(850)
/* no more include with QADRT package from IBM */
#endif


/*** OS390 Patch *******/
#ifdef OS390
 #include "_Ascii_a.h"   /* must be the last include ! */
#endif



/****************************************************************************************
* GetArgumentIsToSkipMailWatcherQuota
* ----------------------------------------------------------------------------------------
* PURPOSE	: The function reads argument "mail_watcher" from the configuration doc
* INPUT		: conf doc handle
*			: pointer to char** (arguments arrays)
* OUTPUT	: STATUS (error / NOERROR).
*			: if error => should disable the function : over_quota_exception <- FALSE.
******************************************************************************************/
STATUS GetArgumentIsToSkipMailWatcherQuota(NOTEHANDLE hNote, char** Over_Quota_Field_Doc_OK, char** Over_Quota_Doc_OK)
{
	STATUS	error = NOERROR;
	
	/* Get argument from hnote */
	error = GetFieldsList(hNote, OVER_QUOTA_DOC_TYPE_NOT2STOP, Over_Quota_Doc_OK);

	if (error == NOERROR)
		error = GetFieldsList(hNote, OVER_QUOTA_DOC_FIELD_NOT2STOP, Over_Quota_Field_Doc_OK);

	/* end */
	return(error);

} /* end of : GetArgumentIsToSkipMailWatcherQuota */



/*****************************************************************************************
* IsToSkipMailWatcherQuota
* ----------------------------------------------------------------------------------------
* PURPOSE	: The function returns 1 if the email is not to be stopped
* INPUT		: 
* OUTPUT	: int (TRUE/FALSE).
******************************************************************************************/
int IsToSkipMailWatcherQuota (NOTEHANDLE hNote, char* FieldValue, char** field_List1, char** field_List2 )
{
	int		hNoteToBeSkip = 0;
	char	FieldValueRead[MAXFIELDVALUE];

	/* ----- Conditions field value -----*/
	if (!NSFItemGetText(hNote, FieldValue, FieldValueRead, sizeof(FieldValueRead) -1))
		strcpy(FieldValueRead, DEFAULT_FORM_VALUE);
	else
		UpperCase(FieldValueRead);

	/* - (1) Check compare to argt values  - Skip if empty */
	if ( *field_List1 != NULL )
	{
		hNoteToBeSkip = SkipNoteHandleWithFieldValueChar(hNote, FieldValueRead, field_List1);		
	}
	
	/* continue if not finished yet :) */
	/* (2) : Check Fields presence */
	if (hNoteToBeSkip == 0)
	{
		/* Loop over fields */
		hNoteToBeSkip = SkipNoteHandle(hNote, field_List2);
	}
	
	/* end */
	return(hNoteToBeSkip);

} /* end of SkipMailWatcherQuota */


/******************************************************************************************
* SkipNoteHandle
* ----------------------------------------------------------------------------------------
* PURPOSE	: the function returns 1 if at least one field specified in the field_list has
*			: been found whatever are the field values.
* INPUT		: DHANDLE		-> current database
*			: NOTEHANDLE	-> current document
*			: char**		-> List of fields (type : Text)*
* OUTPUT	: Int			-> 0 if one of the fields is set !
*			:				-> 1 if none of the fields is set
*******************************************************************************************/
int SkipNoteHandle(NOTEHANDLE notemail_handle, char** field_List)
{

	int		i = 0;
	int		r = 0;

	/* Loop over the field_List : with n is the number of field so => NB Field_List[n+1]= NULL */
	while(*field_List!=NULL)
	{
		/* read the field */
		if ( r = NSFItemIsPresent(notemail_handle, *field_List, (WORD) strlen(*field_List)) )
			break;

		/* next */
		*field_List++;
	}

	/* end : return = 1 if record has exit before the end */
	return(r);
}



/*****************************************************************************************************
* SkipNoteHandleWithFieldValueChar.
* ----------------------------------------------------------------------------------------------------
* PURPOSE	: the function returns 1 if at least one field specified in the field_list has
*			: been found with the CORRECT value compared to the FieldValue argument.
* INPUT		: char*		Field value.
*			: char**	field_list.
* OUTPUT	: Int		-> 0 if one of the fields is set !
*			:			-> 1 if none of the fields is set
******************************************************************************************************/
int SkipNoteHandleWithFieldValueChar(NOTEHANDLE notemail_handle, char* FieldValue, char** field_List)
{

	int		i = 0;
	int		r = 0;

	/* Loop over the field_List */
	while(*field_List != NULL)
	{
		/* check, if found then break */
		if ( strcmp(*field_List, FieldValue) == 0)
		{
			r = 1;
			break;
		}
		/* next */
		*field_List++;
	}

	/* end : return = 1 if has exit before the end */
	return(r);
}




/***************************************************************************************
* GetFieldsList
* -------------------------------------------------------------------------------------
* INPUT		: NOTEHANDLE	-> Current Doc handle
*			: char*			-> Skip_Field
* OUTPUT	: STATUS		-> error
*			: char**		-> field_List
*****************************************************************************************/
STATUS GetFieldsList(NOTEHANDLE note_handle, char* Skip_Field, char** field_List)
{
	STATUS	error = NOERROR;
	int		i	= 0;
	int		nb_field = 0;
	WORD	typechamp;

	/* Type ? */
	NSFItemInfo(note_handle, Skip_Field, (WORD) strlen(Skip_Field), NULL, &typechamp, NULL, NULL);

	if (typechamp == TYPE_TEXT)
	{
		field_List[i] = malloc(MAXFIELDVALUE);

		/* enough memory ? */
		if (field_List[i] != NULL)
		{
			if(!NSFItemGetText(note_handle, Skip_Field, field_List[i], MAXFIELDVALUE -1))
				field_List[i] = NULL;
			else
				UpperCase(field_List[i]);
		}

		field_List[i+1]= NULL;
	}
	else
	if (typechamp == TYPE_TEXT_LIST)
	{
		nb_field = NSFItemGetTextListEntries(note_handle, Skip_Field);

		for (i = 0; i < nb_field; i++)
		{
			field_List[i] = malloc(MAXFIELDVALUE);
			if (field_List[i] != NULL )
			{
				if (NSFItemGetTextListEntry(note_handle, Skip_Field, (WORD) i, field_List[i], (WORD) MAXFIELDVALUE -1))
					UpperCase(field_List[i]);
			}
		}

		/* (i++); end of list */
		field_List[i]= NULL;
	}
    else 
	{
		/* i = 0 */
		field_List[i] = NULL;
		return(-1);
	}
		
	/* end */
	return(NOERROR);
}




/**************************************************************************************************
* FreeMemory for : GetFieldsList
*
* INPUT		: NOTEHANDLE	-> Current Doc handle
*			: char*			-> Skip_Field
*
* OUTPUT	: STATUS		-> error
*****************************************************************************************************/
void FreeMemory_GetFieldsList(char** field_List)
{
	int		i	= 0;

	/* loop & free memory */
	while ( field_List[i] != NULL )
	{
		if (field_List[i] != NULL)
			free( field_List[i] );
		i++;	
	}
}
