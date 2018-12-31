/**********************************************************************
 * Fichier MWADVT.H
 *
 * --------------------------------------------------------------------
 * 29/11/99 . DocID (char - 32)
 * 04/12/00 : BLNames(1024)
 * 01/05/02	: define ERR_UPDATE_DOC.
 * 31/05/03	: add MW_DocID, Subject to adv_stat struct
 * 23/05/04 : 3.93.8
 *********************************************************************/
#define NOMAPI			"MWADVT"

#ifdef V4
#define API_VERSION		"(4). 3.95.23"
#else
#define API_VERSION		"3.95.23"
#endif


/* inline */
#define mw_MIN(a,b)        ((a) > (b) ? (b) : (a))

/* based on FONTID.H */
#define SMALL_FOREIGN_FONT_ID			FontSetFaceID(FontSetSize(NULLFONTID,8),FONT_FACE_TYPEWRITER)


#define MAXDEBUGLINE					513						/* debug buffer -> file */
#define MAXDELETEATTACH					250						/* Max of file than can be deleted */
#define	MAXFIELDVALUE					256
#define MAX_DOC_FIELD_TEXT_SIZE			32000					/* Max TEXT size for one Notes Record			*/
#define	MAX_FIELD_TEXT_SIZE				15000					/* Lotus Notes Max size for text field (list)	*/
#define MAX_RECIPIENTS_FIELD_SIZE		15000					/* (15K -> 15360 )Max size of recipients field	*/
#define MAXRECLEN						1024
#define MAXLGSTAT						100
#define MAXTXTMESSAGE					3000					/* max message line size, the rest is cut off */
#define MAXRULESARRAY					9999					/* max rules is defined there add 1 */
#define MAXMEMORULESIZE					2000					/* String containing all the rules # applied to a message */
#define MAXMEMORULECTR					400						/* Max count for memo rules : 400 = len(9999 + ",") = 5 * 400 = 2000 */
#define DOCID							33						/* lenght of Lotus Notes document unique ID */
#define MAXMAILBOX						8						/* number of possible mail.box */
#define	MAX_BL_FORMULA					1024					/* lenght of Lotus Notes Macro formula in Black List rules */
#define MAXEVAL							3						/* Number of rules allowed for evaluation copy (debug) */
#define MAXMEMORYRULESREF				10
#define BLACKLIST_RULES_MEMORY_MAX_CMD	40
#define MAX_FILTER_FORWARD_ADDRESS		100
#define MAXMWTAG						20							/* mwtag item max size */
#define	PREDICAT_ITEM_SEP				'|'
#define PREDICAT_ITEM_SEP_CHAR			"|"							/* char of PREDICAT_ITEM_SEP */

#define DEFAULT_RULES_MEMORY_REF		"MWBL"						/* size must be < MAXMEMORYRULESREF */
#define MWTAGFIELD						"mwtag"						/* MailWatcher Task list field */
#define MAILWATCHER_MWADVT				"MWADVT"					/* mwtag list : MailWatcher MWADVT item */
#define MAILWATCHER_MWGZIP				"MWGZIP"					/* mwtag list : MailWatcher MWGZIP item */
#define	COPYDBDEFAULTVIEW				"($All)"					/* mailcopy database default view  */
#define SKIPEXTENSIONMGR				"NOT_EM"					/* Special field */


/* 3.94.59 : MWSARC dependencies */
#define MAIL_ORIGDOCLINK				"MWORIGDOCLINK"				/* string : MAXUSERNAME */
#define MAIL_DOCLINK					"MWDOCLINK"					/* string : MAXUSERNAME */


/* 3.94.16 : MWKAV dependencies */
#define MWKAV_CLEAN_DELETE_IF_FAILS		"MWKAV_CLEAN_DELETE_FAILS"	/* string : "0" or "1" */
#define MWKAV_CLEAN_DELETE_LOG			"MWKAV_CLEAN_LOG"			/* string : "0" or "1" */
#define MWKAV_CLEAN_NB_VIRUS			"MWKAV_NB_CLEANABLE"		/* string (number) of virus */
#define MAILWATCHER_MWKAVCLEAN			"MWKAVCLEAN"				/* mwtag list : MailWatcher MWKAV item */


#define BLACKLIST_RULES_MEMORY_REF		"BL_RulesMemory_Ref"		/* BlackList Doc : Reference for hasrun++ cf rulesmem.c */
#define BLACKLIST_RULES_MEMORY_REF_CMD	"BL_RulesMemory_Ref_Cmd"	/* Blaclist doc the evaluation expression */
#define BLACKLIST_EXPIRED_DATE			"BL_ExpirationDate"			/* Blacklist doc : expiration date		*/
#define BLACKLIST_START_DATE			"BL_StartDate"				/* Blacklist doc : start date			*/
#define BLACKLIST_DETACHFILES_YN		"BList_DetachAttach_ToDisk"
#define BLACKLIST_DETACHFILE_EXTENSION	"BList_DetachAttach_Extension"
#define BLACKLIST_DETACHFILE_PATH		"BList_DetachAttach_Path"
#define BLACKLIST_DETACHFILE_LINK		"BList_DetachAttach_HostLink"
#define BLACKLIST_RULES_PREDICAT_LIST	"BList_Predicats_List"		/* string : list */
#define BLACKLIST_PREDICAT_FIELD_PREFIX	"Pred_"						/* string : fieldname prefix */
#define	MAIL_LOW_PRIORITY				"l"							/* Low priority value */


#define MW_MAILDUPLICATE				"MWDuplm"					/* mark this email as a duplicate		*/
#define MW_DOCID_TRACKER				"ODSW41"					/* MailWatcher message ancestor tracker */
#define MAILNOTE_RECIPIENTS_FIELD		"recipients"				/* mail recipients field name			*/
#define	MAILNOTE_FROM_FIELD				"from"						/* Sender address                       */
#define MAILNOTE_INTERNET_FROM_FIELD	"SMTPOriginator"			/* SMTP email address <= FROM			*/
#define	DELIVERY_REPORT_FORM			"DeliveryReport"			/* OverQuota return form				*/
#define OVERQUOTA_PRINCIPAL				"MailSavedPrincipal_MWOQ"
#define	OVERQUOTA_FORM					"MailSavedForm_MWOQ"
#define OVERQUOTA_SUBJECT				"MailSaveSubject_MWOQ"
#define OVERQUOTA_DELIVERYREPORT		"MailSaveDeliveryReport_MWOQ"
#define OVERQUOTA_KEEP_PRIVATE			"MailSaveKeepPrivate_MWOQ"
#define OVERQUOTA_RECIPIENTS			"MailSaveRecipients_MWOQ"

#define	DELIVERY_REPORT_FORM			"DeliveryReport"			/* OverQuota return form				*/

#define INBOX_ICON						"_ViewIcon"					/* number */
#define ICON_STOPPED					97
#define ICON_ALERTED					15



/* Fields */
#define	ADVSTAT_SUBJ					"advstat_subj"			/* Statistics collector "Subject" Enable/Disable */
#define BLIST_MAILCOPY_FOLDERNAME		"MailCopy_Folder"
#define ADV_RULES_DB					"rulesfile"



/* names.nsf : System fields */
#define	NAB_SERVER_VIEW					"($Servers)"
#define NAB_SERVER_HOST_FULLNAME_FIELD	"SMTPFullHostDomain"	/* Fully qualified Internet host name */

/* constants */
#define	MACRO_REQUIRING_MIME2CD			"@ABSTRACT"		/* UPPERCASE ! : the macro formula */
#define HEADER_SEP						 "-----------------------------------------------------------------------------------------------------------------------------------------------------------------"
#define MIME_INTERNET_ADDRESS_FORMAT	"*<*@*.*>*"				/* MIME address format (wildcard) */
#define MW_MACRO_FIELD_NB_RECIPIENTS	"MW_NB_RECIPIENTS"		/* Number */


/* Stat struct record */
struct advt_stat
{
	char	Date[11];											/* Date serveur du traitement */
	char	Time[11];											/* Heure serveur du traitement */
	char	PostedDate[11];										/* Date d'emission du message */
	char	PostedTime[11];										/* Heure d'emission du message */
	char	Sender[MAXUSERNAME];
	char	HomeServer[MAXUSERNAME];
	int		nbRec;												/* nb de destinataire (après split des groupes */
	char	Rec[MAXRECLEN];										/* rec1, rec2, rec3, .... en [Abbreviate] */
	char	Form[50];
	long	Taille;												/* taille réel en bytes */
	int		Objets;												/* nb object (attached file, OLE etc.) */
	char	Ext[MAXRECLEN];										/* Extensions des fichiers rattachés (ext1, ext2, ...) */
	BOOL	Sign;
	BOOL	Crypt;
	BOOL	DeliveryConfirm;
	BOOL	ReturnReceipt;
	int		DeliveryPriority;									/* 1 = low, 2 = normal, 3 = high */
	int		nbServer;											/* nb de serveur(s) qu'a traversé le message */
	long	DeliverTime;										/* durée en sec. entre emission et traitement */
	int		nbArch;												/* nb de sauvergarde auto */
	BOOL	sf;													/* message filtre par formule */
	int		nbBl;												/* nb de recipient re-rootés (black list) */
	char	DocID[DOCID];										/* Notes Document unique ID for cross reference multi routers */
	char	BLNames[1024];										/* Reference all the Black list applied (take the first 100) - a ref <= 10 char */
	char	MW_DocID[DOCID+1];									/* Original Notes Document unique ID seen by a MailWatcher server */
	char	subject[MAXRECLEN];
	char	ExtSize[MAXRECLEN];									/* 3.95.13 : ExtSize=Ext : concact size of attachment type ( size1, size2, size[Objets] ) */
};


#define DO_INIT						"Doing initialization"
#define INIT_DONE					"%s : initialization ended"
#define ETAT_WAIT					API_VERSION " Idle Task"
#define ETAT_ACTIVE					"Processing message(s)"
#define UPDATE_VIEW					"%s : updating working views in '%s'"
#define ETAT_UPDATE_VIEW			"Rebuilding views indexes"
#define DO_END						"Terminating"
#define POLSTOP						"Program MailWatcher Advanced Services aborted"
#define API_ENDED					"%s : task ended"
#define API_STILL_LOADED			"%s : The addin task is still loaded on this server (%s')"
#define ERR_REMOTE					"%s : Check that the server '%s' is an administrator. MWADVT cannot be launched !"
#define ERR_OPEN_DB					"%s : Error while opening the database '%s'"
#define ERR_FIND_VIEW				"%s : View '%s' not found in the database '%s'"
#define ERR_OPEN_VIEW				"%s : Error while opening the view '%s' in the database '%s'"
#define ERR_READ_VIEW				"%s : Error while reading the view '%s' in the database '%s'"
#define ERR_READ_FIELD				"%s : Error while reading field '%s' in rules '%s'"
#define ERR_OPEN_NOTE				"%s : Error while opening the document 'NT%08lX' in the database '%s'"
#define ERR_FORCE_MWTAG				"%s : Error MWTAG '%s' entry not valid - reset 'NT%08lX' in '%s'"
#define ERR_OPEN_GROUP				"%s : Error while opening the group : '%s'"
#define ERR_UPDATE_COLL				"%s : Error while updating notes collection: '%s'"
#define ERR_FIND_CONFIG				"%s : Unable to find '%s' in the view '%s' within '%s'"
#define ERR_READ_CONFIG				"%s : Unable to read '%s' in the view '%s' within '%s'"
#define ERR_OPEN_CONFIG				"%s : Error while opening the configuration document in %s"
#define ERR_DEL_RS					"%s : Error while dropping the document 'NT%08lX' to the router (%d)"
#define ERR_CREATE_DOC				"%s : Error creating a new document in the database '%s'"
#define ERR_CREATE_DOC_READER		"%s : Error setting reader access while creating a new document in the database '%s'"
#define ERR_CREATE_DOCLINK			"%s : Error creating a DocLink for new document in the database '%s'"
#define ERR_UPDATE_DOC				"%s : Error updating a document in the database '%s'"
#define ERR_MWADVT					"A major error has occured. MWADVT add-in task ABORTED on server "
#define ERR_0						"Error while getting main parameters. Check the LOG database"
#define ERR_1						"Error while opening working dbs / views. Check the LOG database"
#define ERR_2						"Error while reading MAIL.BOX. Check the LOG database"
#define ERR_3						"Evaluation copy out of date"
#define ERR_4						"Error while uploading the Black/Red list rules"
#define ERR_5						"Error while copying e.mail to %s, check Black/Red list rules"
#define ERR_OPEN_FILTER_DOC			"%s : Error while opening the filter document '%NT%08lX' in '%s'"
#define ERR_COMPILE_FORMULA			"%s : Error while compiling formula in doc '%s' from '%s' (offset %d)"
#define ERR_COMPUTE_START			"%s : Error while initializing formula in doc '%s' from '%s'"
#define ERR_UPDATE					"%s : Error while updating %s %s"
#define DUMP_STATS					"%s : dumping message statistics to '%s'"
#define ERR_FILTER_CANNOT_ATTACH	"'%s' can not attach '%s'"
#define FOLDER_NOT_FOUND			"Folder '%s' not found in '%s'"
#define ERR_CREATE_FOLDER			"Error can not created '%s' in '%s'"
#define ERR_MOVE_TO_FOLDER			"Error while moving doc to '%s' in '%s'"
#define ERR_DETACH_FILE				"%s : BlackList Rules : Error while detaching file %s"
#define ERR_DETACH_FILE_LINK		"%s : BlackList Riles : Error while creating link %s %s"
#define ERR_DUMPING_RULES_MEMORY	"%s : Error while creating rules memory save file '%s'"
#define ERR_COPY_FIELD_TO_NOTE		"%s : Error while transfering '%s' to temporary hNote"



/* Temp 3.94.44 */
/*	#define POST_MWADVSRVMWTAGLIST		"MWGZIP" */
/* Temp 3.94.44 */
/* 3.94.51   #define POST_MWADVSRVMWTAGLIST		"MWGZIP" */
#define	POST_MWADVSRVMWTAGLIST		MAILWATCHER_MWGZIP "," MAILWATCHER_MWKAVCLEAN

