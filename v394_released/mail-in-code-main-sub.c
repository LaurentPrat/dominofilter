/*****************************************************************************************
 *	MailInforJournaling( );
 *
 *	INPUT    : current DB (mail.box), current email (hnote)
 *           : char* Mail-in Address
 *  OUTPUT	 : STATUS
*********************************************************************************************/
STATUS	MailInForJournaling(DBHANDLE hMB, NOTEHANDLE hNote, char * MailInAddress)
{
	STATUS			error = NOERROR;
	NOTEHANDLE		hNewNote = NULLHANDLE;

	/* Create a in memory copy of current email */
	error = DuplicateMail(&hMB, hNote, &hNewNote, 0);
	
	/* if error : Mark current hNote to remain "HOLD",   Update FailureReason */
	if ( error )
	{
		AddInLogMessageText(ERR_CREATE_JOURNAL_DOC, error, NOMAPI, "mail(x).box");
		NSFItemSetText(hNote, "FAILUREREASON", ERR_JOURNAL_FAILURE_REASON, (WORD) strlen(ERR_JOURNAL_FAILURE_REASON));		
		NSFItemSetText(hNote, MW_KEEP_MSG_HOLD, "1",1);
		return(error);
	}

    /* ---------- Copy OK -------------------------*/
	/* (1) Remove OLD error field */
	NSFItemDelete(hNewNote, MW_KEEP_MSG_HOLD, (WORD) strlen(MW_KEEP_MSG_HOLD));
	NSFItemDelete(hNewNote, "FAILUREREASON",      13);

	/* -- Do it also for hNote if was there --- */
	if ( NSFItemIsPresent(hNote, MW_KEEP_MSG_HOLD, (WORD) strlen(MW_KEEP_MSG_HOLD)) )
	{
		NSFItemDelete(hNote,  MW_KEEP_MSG_HOLD, (WORD) strlen(MW_KEEP_MSG_HOLD));
		NSFItemDelete(hNote, "FAILUREREASON",      13);
	}

	/* (2) Map fields from HNote to hNewNote with different names :  BlindCopyTo and From */
	NSFItemDuplicateTextList(hNewNote, MAILNOTE_BLINDCOPY_FIELD, "JRN_BlindCopyTo", FALSE /* BOOL AllowDuplicateItem */);
	NSFItemDuplicateTextList(hNewNote, MAILNOTE_FROM_FIELD, "JRN_From", FALSE);


	/* ------------- Prepare new copy for routing ------------------- */

	/* Delete current Recipients and set to the Mail-in Db */
	NSFItemDelete(hNewNote,MAILNOTE_RECIPIENTS_FIELD, (WORD) strlen(MAILNOTE_RECIPIENTS_FIELD) );
	NSFItemSetText(hNewNote,MAILNOTE_RECIPIENTS_FIELD, MailInAddress, (WORD) strlen(MailInAddress));
	
	/* Force FROM to the same value to avoid delivery failure notice */
	NSFItemSetText(hNewNote, MAILNOTE_FROM_FIELD, MailInAddress, (WORD) strlen(MailInAddress));
	
	/* Release mail to Router */
	NSFItemDelete(hNewNote, "RoutingState", 12);


	/* ------------- Set Copy before saving ---------------------------*/
	/* remove tempo fields */
	NSFItemDelete(hNewNote, "MW_BLMemo",         9);
	NSFItemDelete(hNewNote, "MW_SKIPRULE",      11);
	NSFItemDelete(hNewNote, MW_MAILDUPLICATE,   (WORD) strlen(MW_MAILDUPLICATE));
	NSFItemDelete(hNewNote, "FAILUREREASON",    13);
	NSFItemDelete(hNewNote, "MWTAG",            5);
	NSFItemDelete(hNewNote, "MW_NB_RECIPIENTS", 16);
    error = NSFNoteUpdate(hNewNote, UPDATE_FORCE);

	/* Validate the new copy is OK otherwise lock the original mail for further process */
	if ( error )
	{
	    /* Mark current hNote to remain "HOLD" -  Update FailureReason */
		AddInLogMessageText(ERR_CREATE_JOURNAL_DOC, error, NOMAPI, "mail(x).box");
		NSFItemSetText(hNote, "FAILUREREASON", ERR_JOURNAL_FAILURE_REASON, (WORD) strlen(ERR_JOURNAL_FAILURE_REASON));

		/* Mark current hNote to remain "HOLD",   Update FailureReason */
		NSFItemSetText(hNote, MW_KEEP_MSG_HOLD, "1",1);
	}

	/* -- Release memory & exit */
	NSFNoteClose(hNewNote);
	return(error);

}
