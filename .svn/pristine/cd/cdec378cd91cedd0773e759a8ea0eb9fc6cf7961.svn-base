/*********************************************************************************** 
* File : hnote2skip.h
*			: Lotus Notes C API, V4,V5,V6.
* ----------: --------------------------------------------------------------------
* PURPOSE	: Implemented little procedure that are required to skip some process
* 			: when the hNote contains special fields.
************************************************************************************/

/* standard c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/* win32 */
#ifdef WIN32
	#include <windows.h>
#endif

/* notes API include */
#include "global.h"
#include <nsfnote.h>


/* main Prototype funct */
STATUS			GetArgumentIsToSkipMailWatcherQuota(NOTEHANDLE hNote, char** Over_Quota_Field_Doc_OK, char** Over_Quota_Doc_OK);
int				IsToSkipMailWatcherQuota(NOTEHANDLE hNote, char* FieldValue, char** field_List1, char** field_List2 );

/* based on secondary prototype */
int				SkipNoteHandle(NOTEHANDLE notemail_handle, char** field_List);
int				SkipNoteHandleWithFieldValueChar(NOTEHANDLE notemail_handle, char* FieldValue, char** field_List);
STATUS			GetFieldsList(NOTEHANDLE note_handle, char* Skip_Archive_Field, char** field_List);
void			FreeMemory_GetFieldsList(char** field_List);
void			UpperCase(char *n);

/* constant */
#define			MAXFIELDVALUE					256
#define			MAXFIELDNAME					100
#define			DEFAULT_FORM_VALUE				"MEMO"
	
/* Fields */
#define			OVER_QUOTA_DOC_TYPE_NOT2STOP	"over_quota_exception_form"
#define			OVER_QUOTA_DOC_FIELD_NOT2STOP	"over_quota_exception_fields"


