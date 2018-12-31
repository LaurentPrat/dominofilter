/***********************************************************************
 * Cette fonction ouvre les bases dont on a besoin pour traiter
 * les messages
 * INPUTS : rien
 * OUTPUT : STATUS
 * -- FIX NSFxxxClose().
  ***********************************************************************/
STATUS OpenDbs(void)
{
	STATUS				error = NOERROR;
	NOTEID				viewmb_id;
	DBHANDLE			hConfdb = NULLHANDLE;
	NOTEID				vca_id;
	HCOLLECTION			coll_ca	= NULLHANDLE;
	COLLECTIONPOSITION	coll_capos;
	DWORD				atraiter = 0;
	HANDLE				buffer_cahandle = NULLHANDLE;
	DWORD *				id_calist;
	NOTEHANDLE			hConfNote = NULLHANDLE;
	char				capath[MAXPATH];
	NUMBER				num = 0;
	char				temp[50];
	char				*loc;
	int					text_len, i;


	/* (re)Init */
	coll_ca				= NULLHANDLE;
	coll_bl				= NULLHANDLE;
	coll_bl_formula		= NULLHANDLE;
	coll_cr				= NULLHANDLE;
	coll_bs			    = NULLHANDLE;
	buffer_cahandle		= NULLHANDLE;
	hConfdb				= NULLHANDLE;
	hDb					= NULLHANDLE;


	/* On récupère le document de configuration */
	if ( !OSGetEnvironmentString("UNIPOWER_API", capath, sizeof(capath)-1) )
		strcpy(capath, "mw_api.nsf");

	/* Ouverture de la base des config générales */
	CheckPath(capath);/* For Unix */

	if ( error = NSFDbOpen(capath, &hConfdb) )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}

	if ( !EstVraiBase(hConfdb) )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hConfdb);
		return(error);
	}

	if ( error = NIFFindDesignNote(hConfdb, "($APIView)", NOTE_CLASS_VIEW, &vca_id) )
	{
		AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hConfdb);
		return(error);
	}


	if ( error = NIFOpenCollection(hConfdb, hConfdb, vca_id,0, /* 3.93.45 : concurrent access cause pb OPEN_REBUILD_INDEX */
								    NULLHANDLE, &coll_ca, NULL, NULL, NULL, NULL) )
	{
	    AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hConfdb);
		return(error);
	}


	if ( error = NIFFindByName(coll_ca, "Mail_Watcher", FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE, &coll_capos, &atraiter) )
	{
		NIFCloseCollection(coll_ca);
		NSFDbClose(hConfdb);
		AddInLogMessageText(ERR_FIND_CONFIG, error, NOMAPI, "Mail_Watcher", "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}


	if ( error = NIFReadEntries(coll_ca, &coll_capos, NAVIGATE_CURRENT, 0,NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID, &buffer_cahandle, NULL, NULL, &atraiter, NULL) )
	{
		if ( buffer_cahandle != NULLHANDLE )
				OSMemFree(buffer_cahandle);

		NIFCloseCollection(coll_ca);
		NSFDbClose(hConfdb);
		AddInLogMessageText(ERR_READ_CONFIG, error, NOMAPI, "Mail_Watcher", "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}


	if ( (buffer_cahandle == NULLHANDLE) || (atraiter == 0) )
	{
		if ( buffer_cahandle != NULLHANDLE )
			OSMemFree(buffer_cahandle);

		NIFCloseCollection(coll_ca);
		NSFDbClose(hConfdb);
		AddInLogMessageText(ERR_FIND_CONFIG, error, NOMAPI, "Mail_Watcher", "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}


	id_calist = (NOTEID *)OSLockObject(buffer_cahandle);

	if (error = NSFNoteOpen(hConfdb, id_calist[0], 0, &hConfNote))
	{
		if ( buffer_cahandle != NULL )
		{
			OSUnlockObject(buffer_cahandle);
			OSMemFree(buffer_cahandle);
		}

		NIFCloseCollection(coll_ca);
		NSFDbClose(hConfdb);
		AddInLogMessageText(ERR_OPEN_CONFIG, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}


	/* On peut (enfin !) récupérer les datas */
	NSFItemGetText(hConfNote, "dbpolice", dbsetup, sizeof(dbsetup)-1);
	NSFItemGetText(hConfNote, "AlertFrom", alertfrom, sizeof(alertfrom)-1);
	NSFItemGetText(hConfNote, "AlertSubject", alertsubj, sizeof(alertsubj)-1);
	NSFItemGetText(hConfNote, "AlertSendTo", alertst, sizeof(alertst)-1);
	NSFItemGetNumber(hConfNote, "pasint", &num);


	/* 3.81 : Names & address book field */
	NSFItemGetText(hConfNote, "mwDbNAB", dbnab, sizeof(dbnab)-1);
	if (!strlen(dbnab))
		strcpy(dbnab, "names.nsf");
	else
		CheckPath(dbnab);/* For Unix */


	#ifdef OS390
	/* Notes gave us an IEEE double which must be converted to native */
		ConvertIEEEToDouble(&num, &num);
	#endif

	if (num > 0)
		pasint = (int) num;
	else
		pasint = 0;


	/* Quelles tâches ? */
	NSFItemGetText(hConfNote, "advquota", temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')					/* Quota Filtering */
		advquota = TRUE;
	else
		advquota = FALSE;

	/* >>>> Statistics Collector <<<< */
	NSFItemGetText(hConfNote, "advstat", temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')					/* stats */
		advstat = TRUE;
	else
		advstat = FALSE;

	/* Option : Collect subject */
	NSFItemGetText(hConfNote, ADVSTAT_SUBJ, temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')					/* stats */
		advstat_subject = TRUE;
	else
		advstat_subject = FALSE;


	/* >>>> Message Archiver : MailCopy <<<< */
	NSFItemGetText(hConfNote, "advarch", temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')					/* archivage */
		advarch = TRUE;
	else
		advarch = FALSE;

	NSFItemGetText(hConfNote, "advblck", temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')					/* black list */
		advblck = TRUE;
	else
		advblck = FALSE;

	/* ---------------------------------------------------------------------------------*/
	/* Read Some string from configuration doc                                          */
	/* in use  AppendMailHeaderToBody                                                   */
	if (!NSFItemGetText(hConfNote, "M20", OverQuota_From_value,	sizeof(OverQuota_From_value)	-1))
		strcpy(OverQuota_From_value, "MailWatcher");

	if (!NSFItemGetText(hConfNote, "M21", BlackList_Notification_From_Header,	sizeof(BlackList_Notification_From_Header)	-1))
		strcpy(BlackList_Notification_From_Header, "From");

	if (!NSFItemGetText(hConfNote, "M22", BlackList_Notification_To_Header,		sizeof(BlackList_Notification_To_Header)	-1))
		strcpy(BlackList_Notification_To_Header, "To");

	if (!NSFItemGetText(hConfNote, "M23", BlackList_Notification_Subject_Header, sizeof(BlackList_Notification_Subject_Header)-1))
		strcpy(BlackList_Notification_Subject_Header, "Subject");

	/* ---------------------------------------------------------------------------------*/

	NSFItemGetText(hConfNote, "advform", temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')					/* formule de filtrage */
		advform = TRUE;
	else
		advform = FALSE;


	/* Return notification to sender when blacklisted address have been found */
	NSFItemGetText(hConfNote, "advnotify_blacklist", temp, sizeof(temp)-1);
	if ((temp[0] | 0x20) == '1')
		advnotify_blacklist = TRUE;
	else
		advnotify_blacklist = FALSE;


	/* ------------------------------------------------------------------------------------------------------
	/**       >>>>>>>>      Other arguments for Stats, File, time between document processing <<<<<<<<<<   **/
	if ( advstat )
	{
		NSFItemGetNumber(hConfNote, "statint", &num);

		#ifdef OS390
		/* Notes gave us an IEEE double which must be converted to native */
			ConvertIEEEToDouble(&num, &num);
		#endif

		if (num > 0)
			statint = (int) num;
		else
			statint = 5;

		NSFItemGetText(hConfNote, "statfile", StatFile, sizeof(StatFile)-1);

	} /* end of  ( advstat ) */



	/**** Other arguments for : Formula Filtering 'Subject & 'Body *****/
	if ( advform )
	{
	 	NSFItemGetText(hConfNote, "M20", ndffrom, sizeof(ndffrom)-1);
		if (!strlen(ndffrom))
			strcpy(ndffrom, "MailWatcher");

		NSFItemGetText(hConfNote, "AdvFormSubj", ndfsubject, sizeof(ndfsubject)-1);
		loc = strstr(ndfsubject, "%s");
		if (!loc)
			strcat(ndfsubject, " (%s)");

		/***** Message Text (SMTP) filtering formula ******/
		nb_formula_Messagetxt = NSFItemGetTextListEntries(hConfNote, "AdvFormText");

		for ( i = 0; i < nb_formula_Messagetxt; i++ )
		{
			text_len = NSFItemGetTextListEntry(hConfNote, "AdvFormtext", (WORD) i, ndftextmessage[i], (WORD) MAXTXTMESSAGE-1);
			ndftextmessage[i][text_len] = '\0';
		}

	}/* end of  ( advform ) */


	/**** Other arguments for : Quota Formula Filtering 'Subject & 'Body *****/
	if ( advquota )
	{
		NSFItemGetText(hConfNote, "AdvquotaSubj", advquotaSubject, sizeof(advquotaSubject)-1);

		/***** Message Text quota filter ******/
		nb_quota_Messagetxt = NSFItemGetTextListEntries(hConfNote, "AdvquotaText");

		for (i = 0; i < nb_quota_Messagetxt; i++)
		{
			text_len = NSFItemGetTextListEntry(hConfNote,
											   "AdvQuotaText",
											   (WORD) i,
											   advquotaTextMessage[i],
											   (WORD) MAXTXTMESSAGE-1);

			advquotaTextMessage[i][text_len] = '\0';
		}

		/* - Check if the attached files are to be deleted - */
		advquota_del_attach_file = FALSE;

		if ( NSFItemGetText(hConfNote, "AdvQuota_RemoveAttach", temp, sizeof(temp)-1) )
		{
			if (strcmp(temp, "1") == 0)
			advquota_del_attach_file = TRUE;
		}

		/* - 3.93 :  Quota Exception - */
		advquota_exception = FALSE;
		if (NSFItemGetText(hConfNote, "AdvQuota_exception", temp, sizeof(temp)-1))
		{
			if (strcmp(temp, "1") == 0)
			advquota_exception = TRUE;

			/* read arguments : set to FALSE if error */
			if ( GetArgumentIsToSkipMailWatcherQuota(hConfNote, overquota_field_values, overquota_form_List) )
			{
				advquota_exception = FALSE;
				FreeMemory_GetFieldsList(overquota_form_List);
				FreeMemory_GetFieldsList(overquota_field_values);
			}
		}

		/* - 3.93.4 : Quota Resend */
		advquota_resend = FALSE;
		if (NSFItemGetText(hConfNote, "AdvQuota_ResendAction", temp, sizeof(temp)-1))
		{
			if (strcmp(temp, "1") == 0)
			advquota_resend = TRUE;

			/* get the Delivery Failure form name */
			if (! NSFItemGetText(hConfNote, "AdvQuota_ResendAction_FormName", AdvQuota_ResendAction_FormName, sizeof(AdvQuota_ResendAction_FormName)-1))
				strcpy(AdvQuota_ResendAction_FormName, DELIVERY_REPORT_FORM );
		}

	} /* end of  ( advquota ) */



	/*----------------------------------------------------------------*/
	/**** Other arguments for : BlackListed address notification  *****/

	/* Global	: BlackList_Address_Subject    ->(Subject suffix)
	*			: nb_BlackList_Address_Message ->(nb of lines)
	*			: BlackList_Address_Message    ->(Body message)
	*/

	if ( advnotify_blacklist )
	{

		NSFItemGetText(hConfNote, "AdvBL_Notify_Subj", BlackList_Address_Subject, sizeof(BlackList_Address_Subject)-1);

		/***** Line of the Text message ******/
		nb_BlackList_Address_Message = NSFItemGetTextListEntries(hConfNote, "AdvBL_Notify_Text");

		for ( i = 0; i < nb_BlackList_Address_Message; i++ )
		{
			text_len = NSFItemGetTextListEntry(hConfNote,
											   "AdvBL_Notify_Text",
											   (WORD) i,
											   BlackList_Address_Message[i],
											   (WORD) MAXTXTMESSAGE-1);
			BlackList_Address_Message[i][text_len] = '\0';
		}
	} /* end of  ( advquota ) */



	/*----------------------------------------------------------------*/

    /** If one of the option is ENABLE... let's do it *****/
	if ( advarch || advblck || advform  || advquota )
	{
		/* ------------ already open ----
		* hConfdb	: database ( mw_api )
		* coll_ca	: collection ( mw_api )
		* hConfNote	: mail_watcher ( document in mw_api)
		------------------------------------------------------------------*/
		/* ----------------------------------------------------------------
		 * hDb		 : setup ( database ) : not closed
		(1) advblck  : coll_bl, coll_bl_formula ( collection ) : not closed
		(2) advarch  : coll_cr ( collection ) : not closed
		(3) advform  : coll_bs ( collection ) : not closed
		(4) advquota : coll_quota ( collection ) : not closed
         *----------------------------------------------------------------*/

		CheckPath(dbsetup); /** For Unix **/

		if ( error = NSFDbOpen(dbsetup, &hDb) )
	    {
			if ( hConfNote != NULLHANDLE )
				NSFNoteClose(hConfNote);

			if ( buffer_cahandle != NULL )
			{
				OSUnlockObject(buffer_cahandle);
				OSMemFree(buffer_cahandle);
			}

			if ( coll_ca != NULLHANDLE )
				NIFCloseCollection(coll_ca);

			if ( hConfdb != NULLHANDLE )
				NSFDbClose(hConfdb);

			AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, dbsetup);
			return(error);
	    }

	    if ( !EstVraiBase(hDb) )
	    {
			if ( hConfNote != NULLHANDLE )
				NSFNoteClose(hConfNote);

			if ( buffer_cahandle != NULL )
			{
				OSUnlockObject(buffer_cahandle);
				OSMemFree(buffer_cahandle);
			}

			if ( coll_ca != NULLHANDLE )
				NIFCloseCollection(coll_ca);

			if ( hConfdb != NULLHANDLE )
				NSFDbClose(hConfdb);

			if ( hDb != NULLHANDLE )
				NSFDbClose(hDb);

			hDb = NULLHANDLE;

			AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, dbsetup);
			return(1);
		}


	    /*** Read the Black list views (2) ***/
		if ( advblck )
		{
			/* To read in a certain order - set the rules priority */
			if ( error = NIFFindDesignNote(hDb, "($BlackList)", NOTE_CLASS_VIEW, &viewmb_id) )
			{
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI,"($BlackList)", dbsetup);
				return(error);
		  }

		  /* Get the HCOLLECTION Handle */
		  if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_bl, NULL, NULL, NULL, NULL) )
		  {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI,"($BlackList)", dbsetup);
				return(error);
		  }

		  /* From Version 3.61a, From & To & Formule view */
		  /* Vue des black list  (From & To & Formule ) */
		  if ( error = NIFFindDesignNote(hDb, "($BlackList_Formula)", NOTE_CLASS_VIEW, &viewmb_id) )
		  {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;
				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($BlackList_Formula)", dbsetup);
				return(error);
		  }


		  /* --- Get the HCOLLECTION Handle --- */
		  if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_bl_formula, NULL, NULL, NULL, NULL) )
		  {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI,"($BlackList_Formula)", dbsetup);
				return(error);
		  }
		}


		/*** Read the Mailcopy view *****/
		if ( advarch )
		{
			/* Vue des Cpt Crochet */
		  if ( error = NIFFindDesignNote(hDb, "($Arch)", NOTE_CLASS_VIEW, &viewmb_id) )
		  {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( coll_bl_formula != NULLHANDLE )
					NIFCloseCollection(coll_bl_formula);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($Arch)", dbsetup);
				return(error);
		  }

		  if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0,NULLHANDLE, &coll_cr, NULL, NULL,NULL, NULL) )
		  {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( coll_bl_formula != NULLHANDLE )
					NIFCloseCollection(coll_bl_formula);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb= NULLHANDLE;

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($Arch)", dbsetup);
				return(error);
		  }
		}

		/*** Read the Formula filtering view ***/
		if ( advform )
		{
			/* Vue des formules de filtre */
		    if (error = NIFFindDesignNote(hDb, "($BlocSend)", NOTE_CLASS_VIEW, &viewmb_id))
		    {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( coll_bl_formula != NULLHANDLE )
					NIFCloseCollection(coll_bl_formula);

				if ( coll_cr != NULLHANDLE )
					NIFCloseCollection(coll_cr);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($BlocSend)", dbsetup);
				return(error);
		    }

		    if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_bs, NULL, NULL, NULL, NULL) )
		    {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( coll_bl_formula != NULLHANDLE )
					NIFCloseCollection(coll_bl_formula);

				if ( coll_cr != NULLHANDLE )
					NIFCloseCollection(coll_cr);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				hDb = NULLHANDLE;

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($BlocSend)", dbsetup);
				return(error);
		    }
		}/* end of (advform) */



		/*********** advquota ********************************************/
		if ( advquota )
		{
			/* All Person with "ETATPOLICE = "S" in the MW_SETUP.NSF database */
		    if ( error = NIFFindDesignNote(hDb, "(POLICES)", NOTE_CLASS_VIEW, &viewmb_id) )
		    {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				/* close all the Other collection */
				if ( coll_bl_formula != NULLHANDLE )
					NIFCloseCollection(coll_bl_formula);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl); /* rules with priority order */

				if ( coll_cr != NULLHANDLE )
					NIFCloseCollection(coll_cr);

				if ( coll_bs != NULLHANDLE )
					NIFCloseCollection(coll_bs);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				/* Close MW_SETUP.NSF */
				NSFDbClose(hDb);
				hDb= NULLHANDLE;

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "(POLICES)", dbsetup);
				return(error);
		    }

			if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_quota, NULL, NULL, NULL, NULL) )
		    {
				if ( hConfNote != NULLHANDLE )
					NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULL )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				if ( coll_ca != NULLHANDLE )
					NIFCloseCollection(coll_ca);

				if ( coll_bl_formula != NULLHANDLE )
					NIFCloseCollection(coll_bl_formula);

				if ( coll_bl != NULLHANDLE )
					NIFCloseCollection(coll_bl);		/* rules with priority order */

				if ( coll_cr != NULLHANDLE )
					NIFCloseCollection(coll_cr);

				if ( coll_bs != NULLHANDLE )
					NIFCloseCollection(coll_bs);

				if ( hConfdb != NULLHANDLE )
					NSFDbClose(hConfdb);

				NSFDbClose(hDb);
				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "(POLICES)", dbsetup);
				return(error);
		    }
		 }
	}/* end of (advquota) */

    /** Close API Configuration document and database **/
	if ( hConfNote != NULLHANDLE )
		NSFNoteClose(hConfNote);

	if ( buffer_cahandle != NULL )
	{
		OSUnlockObject(buffer_cahandle);
		OSMemFree(buffer_cahandle);
	}

	if ( coll_ca != NULLHANDLE )
		NIFCloseCollection(coll_ca);

	if ( hConfdb != NULLHANDLE )
		NSFDbClose(hConfdb);

	return(NOERROR);
}

