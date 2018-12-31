/************************************************************************************
 * Programme MWADVT - Projet Mail Watcher Version 3.x
 * Visual C++ 5.0 et API Notes 4.5 + Notes 4.51
 * V1.0 / NT
 * AS/400 : max line = 228 char
 * --------------------------------------------------------------------------------
 * Version 3.01
 * 01/09/99			Laurent Prat.
 * Lecture du champs Formule_txt, pour former un message
 * en "clair" à l'utilisateur quand son mail ne passe pas les formules de filtre
 * --------------------------------------------------------------------------------
 * Version 3.10
 * 05/11/99			Laurent Prat.
 * Add a message before writting the rules applied during the filtering
 * evaluation and returned FALSE... so Delivery failure is sent back.
 *
 * 06/11/99			Laurent Prat.
 * Add the disclaimer messages + boolean for enable/disable
 * --------------------------------------------------------------------------------
 * 15/11/99			Laurent Prat.
 * Black / Red List. Add matching functions
 * --------------------------------------------------------------------------------
 * 29/11/99  Document unique ID in the Stat
 * --------------------------------------------------------------------------------
 * 6/01/2000		Laurent Prat
 *            Move the Error messageBox "No Doc modified since last"
 *            when openning Mail.Box
 *
 * ---------------------------------------------------------------------------------
 * 02/02/2000		Laurent Prat
 *					Unix code, checkPath + Boolean.
 * ---------------------------------------------------------------------------------
 * Version 3.61 - Multi mail.box
 * 30/03/2000		Laurent Prat.
 * ---------------------------------------------------------------------------------
 * 04/04/2000	- Fix ProcBlackList (safe_recipient) keep the original routing format
 * except when the RouteThru is TRUE.
 *
 *				=> new specs
 *
 *				void SimpleProcCrochet(NOTEHANDLE hNote, char* basearc)
 *
 * 01/04/00 : bug Fix : sMatch, Pb with multiple "*" in matching format
 * 26/04/00 : bug Fix : Free 'malloc' memory  (advblck) refresh...
 * --------------------------------------------------------------------------------
 * 27/04/00 : Change code for Unix, Compilation Warning.
 * NSFNoteGetSize(DBHANDLE hMb, NOTEHANDLE nh, int *pj)
 *
 * --------------------------------------------------------------------------------
 * 19/05/00 : MWADVT 3.61a
 * integrate and EvalFormula when sMatch returns TRUE... if EvalFormula returns FALSE
 * then continue to look for a Matching Rules, if the Formula Field is not empty of
 * course
 * ( new parameters )
 * GetMatchKey(NOTEHANDLE hNote, char *)
 * Error , cast (NOTEID) pass Key = (From & To) to the LogErr (size issue !!)
 * New views : 	(coll_bl_formula); view From & To & formula
 * => OpendDb
 * => CloseDb
 * => view refresh every 60'
 * => Read entry in ProcBL with coll_bl to define the priority
 * Key as a new size ( check out in mwadvt.h : MAX_BL_FORMULA)
 * --------------------------------------------------------------------------------
 * 30/5/2000 : Purge RecOK if exists
 * --------------------------------------------------------------------------------
 * 01/06/00 : MWADVT 3.61b
 * Add a special flag [MW_RAPP] (for MailWatcher Route After Post Processing)
 * to the Notes Document if the parameter : MW_RAPP_Flag.
 * store in the Black/Red list document is "YES"
 * 07/06/00
 * void SimpleProcCrochet(NOTEHANDLE hNote, char* basearc, STATUS MW_RAPP_Bool, STATUS MW_RAPP_AGAIN_Bool)
 * work out the hCopy document within SimpleProcCrochet.
 * --------------------------------------------------------------------------------
 * 15/06/2000 : MWADVT 3.61c
 * Make "Black List" able to handle multiple rules for one FROM, TO
 * int ProcBlackList(NOTEHANDLE hNote)
 *
 * Boolean Cond to Stop when no more rules are matching or reaching
 * a special end of list of "Special rules" => End
 *
 * -- TO DO (Configuration) -----------------------+
 * Argument in NoteHandle = DO_NOT_CONTINUE exists.!
 * ------------------------------------------------+
 *	if (GetMatchKey(hNote, clef, &RulesCtr) == TRUE)
 * --------------------------------------------------------------------------------
 * 20/06/2000. Fix ProcBlackList
 * add a if () before NSFItemGetText (4 fields)
 * -------------------------------------------------------------------------------
 * Fix : 3.61 (28/6/2000)
 * because of ScanMail, remove ADVSRV = 1
 * NOT NECESSARY as MailWAtcher Extension Manager DO NOT catch NOTESUPDATE
 * inside the MAIL.BOX
 * NSFItemDelete(hNote, "ADVSRV",6);
 *
 * --------------------------------------------------------------------------------
 * 05/07/2000 : nmwadvt_361d
 * Integrate the new ProcBlackList with GROUPS.
 * including the source code of a new file : match.c
 * --------------------------------------------------------------------------------
 * 09/07/2000 : mwadvt_361e
 * Add the Exception (From and To) to the ProcBlackList module.
 * Change Data structure (BL_Rules)
 * Change BlackList loading, freeMemory and
 * new use of MMatch() to test exception (from, to) with
 * int Except_Match(char* From, char* To, BLrules CurrentRules)
 * ---------------------------------------------------------------------------------
 * 16/10/2000 : Fix > dumpstat()
 * If the database does not exists the counter continue to grow
 * this provoque an Out of Range => GPF!!
 * reset index; lgstat = 0; when returning.
 * --------------------------------------------------------------------------------
 * 15/11/2000 : Eval version, using compilation process.
 * --------------------------------------------------------------------------------
 * 29/11/2000 : Fix > sMatch, for String Match (see comments in sMatch())
 * ---------------------------------------------------------------------------------
 * 04/12/2000 : mwadvt_361f - (Start Date)
 * modif in : ProcBlackList
 *
 * Create a new e.mail when Appending a text to the original email
 * this block includes all the Routing elements : (From, To, Forward).
 * then the Text is append to the orginal body (hnote)
 *
 * strcat, all the rules applied to the current e.mail with "," as separator !!
 *
 * new sub :
 * void SetRecOK_FromToForward()
 * void NewEmailBlackList()
 * --------------------------------------------------------------------------------
 * 28/12/00.
 * Bug fix : FreeMemory. - crash if black list contains no group !
 * --------------------------------------------------------------------------------
 * 22/03/2001 : new release 3.7
 * --------------------------------------------------------------------------------
 * 23/01/2001 : compile AIX; API 5.06a; Vers OS : 4.331
 * --------------------------------------------------------------------------------
 * 29/03/2001 : Evaluation copy, used MAXEVAL (#define )
 * --------------------------------------------------------------------------------
 * 03/04/2001 : Call DumpBlackList - as the first Black Load is done
 * --------------------------------------------------------------------------------
 * 10/04/2001 : Fix, ProcBlackList.
 * Append Text (Y/N), test if NbRec> 1.
 * PB : the disclaimer was not appended when NbRec=1
 * ---------------------------------------------------------------------------------
 * 22/04/2001	: New	:  sMatch in IsInGroup : 22/4/01
 * This is to allow Meta string in Groups for ProcBlacList
 * ( see )
 * SpUser*		sSearchUser(SpUser*, char*);
 * int			IsInGroupWmeta(char*, char*);
 *
 * ---------------------------------------------------------------------------------
 * 23/04/2001	: Fix	: Group inheritance...
 * The members in groups data structure upload has been corrected...
 * the GroupName was added to the user group list only when the user
 * was explicitly name as a member, the Groups in Groups hierarchy was missing.
 * ( see )
 * void		GetUserInGroup(char* GroupName, GrpList**);
 * SpUser*	InsertUser(SpUser* Current, char* UserName, char* GroupName, GrpList*);
 * void		PopGroupHistory(GrpList**);
 *
 * --------------------------------------------------------------------------------
 * Release 3.71 (Beta).
 * Special code for OS390.
 * Change history :
 * 1) The BlackList rules separator : µ has been replaced by | (pipe)
 * 2) Conversion for NUMBER (Notes IEEE) to OS390 native format to do calculation
 * conversion back to write to the Notes Doc.
 * 3) Fix : PopGroupHistory (freeing memory when removing the first element).
 *
 * ---------------------------------------------------------------------------------
 * 29/5/01 : Release 3.71a => 3.71 (Gold).
 * Fix : Principal = From.
 * Remove the two following lines :
 *
 * 						NSFItemGetText(hNote, "from", from, sizeof(from)-1);
 *						NSFItemSetText(hNote, "Principal", from, (WORD) strlen(from));
 *
 * --------------------------------------------------------------------------------
 * 09/5/01 : Release 3.71a => 3.71 (Gold).
 * Fix	: FreeMemory.
 * GPF on GrpFirst when no black list rules were setup.
 *
 * ---------------------------------------------------------------------------------
 * 5/7/01 : Mime format. NsfNoteOpenExt(RAW_MIME).
 * Support MIME. Do not convert MIME to CD.
 *
 * changes the way Notes Document are openned and saved.
 * First of all the message is open and converted to Notes format.
 * if (NSFItemIsPresent(hNote, "$NoteHasNativeMIME",  18)
 *         NSFNoteOpenExt(hMb, noteid[0], OPEN_RAW_MIME, &hNote);
 *
 * API fonction Changes needed :
 * CompoundTextAddParagraph(Ext) . need to add Ext at the end of the CoumpoundText...
 *
 * Compile with 5.03 API.
 *
 * ----Modif history ---
 * .ProcBlackList.
 *
 * if (NSFItemGetText(hNoteFiltre, "BList_MessageAppend", bl_choice, sizeof(bl_choice)-1))
 *	>> test on FULL NOTES FORMAT !!
 *
 * -----------------------------------------------------------------------------------
 * Release 3.71a
 * 9/07/01.
 *
 * Remove the test on $NoteHasNativeMime...
 * Always open using Extended (OPEN_RAW_MIME).
 * Test the $NoteHasNativeMime before modifying the BODY fields in ProcBlackList
 * NB => V4 only support NSFNoteOpen !!, need to put #ifdef V4...
 *
 * Add int ApplyAction) in SetRecOK_FromToForward(),NewEmailBlackList()
 * -----------------------------------------------------------------------------------
 * Release 3.8
 * 01/10/01
 * Fix : ProcBlackList.
 * MIME and Append Text was skipped... verbose has been added.
 * if $NoteHasNativeMIME then bl_choice[0]='0'
 * ------------------------------------------------------------------------------------
 * Release 3.8 (+patch 3.8a)
 * 15/10/01.
 * new :
 * > MWDebug = FALSE; debug mode based on notes.ini env.
 * new :
 * > char* param. in order to open another names.nsf like db to read the Black& Red list group
 * >STATUS OpenNAB(char* dbPath), database filename read from OpenDb()=> stored in dbnab global char*
 *
 * Fix :
 * >OpenNAB + BlackRedList init
 * >if (advblck) : advblck is reset to FALSE if an error occurs during OpenNAB
 *
 * Fix :
 * > NewEmailBlackList
 * Save the original item : "MW_MAILDELAY" in the new mail
 * new argument : char BListMemo to save the rules already applied to the original message
 * when a new one is creating because of a body field modification.
 * NSFItemSetText(hMes, "MW_BLMemo", BListMemo, (WORD) strlen(BListMemo))
 *
 * > ProcBlackList.
 * Read MW_BLMemo to initialisze BListMemo.
 *
 * new
 * > int GetNbRulesApplied(char * string, const char *seps)
 * set NbRulesApply with fonction instead of a field
 *
 * Fix :
 * > GetUserInGroup
 * Do (PopGroupHistory) if the group is not found in the names.nsf !
 * -----------------------------------------------------------------------------------
 * 20/11/01
 * Fix : init the Mail.box first name : mail.box or mail1.box
 * int			InitMailBoxCtr = 0;
 * -------------------------------------------------------------------------------
 * Build 3.8a b5.
 * Move the recipients statistics loop just after SplitGroup & before ProcBlackList
 * -----------------------------------------------------------------------------------
 * Build 3.8a b6
 * Remove the @Domain that might be at the end the group name before doing the
 * split group : RemoveNotesDomain(temp)
 * -------------------------------------------------------------------------------
 * Release 3.9
 * 05/11/01 - D0
 *
 * new : (todo)
 * > Integrate the MWTAG field : This field defined within ProcBlackList is used to
 * create a sequence job list. if MWTAG exists the mail RoutingState is not delete nor ADVSRV.
 * -----------------------------------------------------------------------------------------------
 * Remove ProcDisclaimer. => it is part of ProcBlackList.
 * Add : int QuotaFilter(NOTEHANDLE hNote, char* )
 * in order to activate some Router Quota filtering rules.
 * -----------------------------------------------------------------------------------------------
 * 25/01/2002.
 * NonDeliveryBlackList : new argument char* from.
 *
 * 01/02/02
 * NonDeliveryBlackList : if sendto ="0" then DO NOT use the original From value
 * -----------------------------------------------------------------------------------------------
 * 26/02/02
 * Fix in ProcBlackList : if (MW_SKIPRULE[0]=='2') infinite loop as BL_Rules++ was missing.
 * -----------------------------------------------------------------------------------------------
 * 01/05/02
 * change in DumpStat	: add test on NSFItem* : close notes mw_stat db and reset stat counter lgstat = 0.
 * change in OpenNAB	: if the ($VIMGROUP) collection can not be opened => set BlackList boolean = 0.
 * -----------------------------------------------------------------------------------------------
 * Release 3.91
 *
 * 17/05/02
 * new in ProcBlackList	: new set "Importance" field to a specific value - if (parameters is set).
 * field in BlackList	: BList_LowImportance. -Text-
 *
 * 19/05/02
 * new in ProcBlackList	: call new function (AppendDocLink)
 *						: Use the mailcopy database and the document to create a document to the
 *						: new memo document.
 * 29/05/02
 * new in ProcBlackList	: DeleteFields, DeleteAttachfile
 *
 * 07/05/02
 * new environment var	: MWREFRESHVIEWMODE=n (n integer - atoi()).
 *						: set the HaveElapsed timer. in minutes.. time before doing refresh
 *						: init = -1., so 0 means all the time !!
 *
 * new in ProcBlackList	: Log all the Blacklisted address => RecNotOk (Field List).
 *						: implemented in 'SetRecOK_FromToForward'
 *						: called from ProcBlackList.
 * 02/07/02				:
 * new global var		: advquota_del_attach_file = TRUE;
 *						: To remove attached files when OverQuota is applied.
 *						: new argument in "Mail_Watcher" configuration document
 *	in OpenDbs			: AdvQuota_RemoveAttach
 *
 * -----------------------------------------------------------------------------------------------
 * 17/07/02				: MWREFRESHVIEWMODE=NEVER
 *                      : to disable the refresh views.
 * -----------------------------------------------------------------------------------------------
 * 03/09/02				:
 * new var				: in ProcBlackList - send New Memo section -
 *						: Field containing a SendTo address : BList_DF_To_Fwd
 * -----------------------------------------------------------------------------------------------
 * 08/09/02				: Change Rules structure : 'BlackListRules' Add a new member : rank (int).
 * -----------------------------------------------------------------------------------------------
 * 15/09/02				:
 * change in			: SetRecOK_FromToForward, ProcBlackList, NewEmailBlackList.
 *						: in order to implement a Rules memory regarding the BlackListed address (Deliver To : Y/N).
 *						: The BLACK&RED LIST RULES has a new field : [BList_StopMail_Value]
 *						: values are "Majority (1)", "**Priority** (2)", ">> Veto << (3)".
 *						: bRecOK is set a the value which is evaluated at the end to put the recipient
 *						: in 'RecOK' or 'RecNotOK'. This last field is used to implemented the sender
 *						: special "BlackListed" notification.
 * -----------------------------------------------------------------------------------------------
 * 02/10/02				: Check when a filter document is read in the Black&RedList if the
 *						: Expiration Date /and/or/ StartedDate.
 * -----------------------------------------------------------------------------------------------
 * 25/11/02				: Append the text - change the order to prefixe
 *						: From and principal fields are reset to "MailWatcher"
 * -----------------------------------------------------------------------------------------------
 * 03/02/03				: ProcBlackList - fix : Duplicate message
 *						: Delete $FILE + DeleteField + (close in UpdateNewNoteMail).
 * -----------------------------------------------------------------------------------------------
 * 21/02/03				: Special Fix pour Anti-virus which are "HOLDing" the message compose by
 *						: the ProcBlackList add the MWTAG in other to have MailWatcher at work again
 *						: and free the messages
 * ------------------------------------------------------------------------------------------------
 * 13/03/03				: Fix SplitGroup in order to NOT TO split Group that are not members of
 *						: "$MailGroups", plus SplitGroup skips the FROM address if found in splitted
 *						: group's recipients
 * ------------------------------------------------------------------------------------------------
 * 31/03/03				: Fix WildCard engine with a new version.
 *						: wildcard.c + wildcard.h
 * ------------------------------------------------------------------------------------------------
 * 04/03/03				: Fix in 3.91.
 *						: Upload the argument "This Rules Terminates" in order to apply this
 *						:action even if the Rules is to be skipped because of an Exception match
 *						: new field in BLrules structure : Argt_ThisRulesTermintes (int).
 * ------------------------------------------------------------------------------------------------
 * 12/05/03				: Fix in 3.91
 *						: This Rules terminates is to be applied for every recipients even if it has
 *						: been already applied once
 *						:
 *						: TrendMicro misused of ADVSRV so we are rename this entry to MWADVSRV !!
 * ------------------------------------------------------------------------------------------------
 * 15/05/03				: Fix in 3.91
 *						: Add mwnum++ in the mail duplicates - see ProcBlackList()
 *						: void MarkNoteNum(NOTEHANDLE hNote);
 * 22/05/03				: Processus those that are DEAD in order to build rules that can delete them
 *						: automatically
 * ------------------------------------------------------------------------------------------------
 * 23/05/03				: Release 3.92
 *						: NonDeliveryBlackList: append subject parameter if there is an original
 *                      : subject
 *
 * 28/05/03				: Release 3.92
 *						: Remove temporary fields. (DuplicateMail)
 *
 * 31/05/03				: Fix : filename extension.
 *						: Add field to adv_stat (see mwadvt.h) : nb field = 26
 *						: Collect subject if advstat_subject = TRUE;
 *						: Upload all user alias (user type and group type, see GetBlackListRules)
 *                      :
 * 01/06/03				: ProcBlackList : Disclaimer : Convert MIME to CD by closing + reopening the
 *                      : NOTEHANDLE
 * ------------------------------------------------------------------------------------------------
 * 19/06/03				: Release 3.92
 *						: Read argument	: advstat_subject (mw_api)
 * 26/06/03				: Make sure that the READER values do not contains @Domain
 *						: Fix : AppendReaderItem
 * ------------------------------------------------------------------------------------------------
 * 27/06/03				: Fix 391,392 => (Build-12 3.91)
 *						: mw_reader_access in samplecopy. (Cf : AppendReaderItem)
 *						: mwnum++ (MarkNoteNum)
 *						: MoveToFolder(see ProcBlackList)
 * ------------------------------------------------------------------------------------------------
 * 30/06/03				: Add argt to SimpleProcCrochet (...., char* FolderName);
 * 02/07/03				: Fix overFlow on Recipients field, new fonction SplitGroup_email.
 * 03/07/03				: Since MailWatcher is doing some message splitting - ProcBlackList -
 *						: variable "ApplyAction" has to be changed in order to identify the same
 *						: mail and its duplication.
 *						: > MW_MAILDUPLICATE <
 * 18/07/03				: Add FROM to reader access in [setReaderAccess]
 * 18/07/03				: Fix : Memory leak in [GetUserAlias]
 * ------------------------------------------------------------------------------------------------
 * 24/07/03				: 2 news reference string in "mailwatcher" :
 *						: M21, M22 for BlackList_Notification_From_Header[100];
 *						: BlackList_Notification_To_Header[100];
 *
 * 						: NonDeliveryBlackList (+ DocLink) : can now remove $FILE
 * ----------------------------------------------------------------------------------------------------------
 * 28/07/03				: Force NIFUpdateCollection in QuotaFilter.
 * -----------------------------------------------------------------------------------------------------------
 * 30/07/03				: GetNbRulesApplied (init : SaveToken = NULL);
 *						: Add reader access : szServeur
 * ------------------------------------------------------------------------------------------------------------
 * 31/07/03				: Memory leak : GetUserAlias
 * -------------------------------------------------------------------------------------------------------------
 * 03/08/03				: fix memory leak : void FreeMemory()
 *						: memory of global memory : DumpGrpList
 *						: free(GrpFirst->GroupName)
 *						: --
 *						: fix memory leak : void PopGroupHistory(GrpList** GrpFirst)
 *						: free(Begin->GroupName);
 *						:
 *						: fix memory leak : GetUserInGroup()
 *						: free (buffer_fhandle) : OsMemFree if not NULL.
 *						: Call PopGroupHistory on error (return) conde
 *						: --
 *						: fix memory leask : CheckRecipientField()
 *						: DeleteList( L ); instead of DeleteList( P );
 *						: --
 *						: fix memory leak : AddInMain
 *						: if (idtable != NULL) => OSMemFree
 *						: --
 *						: change data type : GetBlackListRules
 *						: ORIGINATORID				BL_MainOID;
 *						: change data type : AddinMain
 *						: ORIGINATORID				MainOID;
 * -------------------------------------------------------------------------------------------------------------
 * 08/08/03				: change OSMemFree in [ MoveToFolder ] => IIDDestroyTable(hIDTable);
 * -------------------------------------------------------------------------------------------------------------
 * 04/09/03				: Support MIME Format in OverQuota procedure.
 *						: in AddinMain, Check the note_id after ProcBlackList return as hNote may have been changed
 *						: Revision : 3.92.4
 * -------------------------------------------------------------------------------------------------------------
 * 08/09/03				: ProcBlackList ()
 *						: Convert a copy of hNote to CD format if $HasMime...
 *						: Conversion should be done only if rules contains "Macro".
 *						: new global var : int			RulesWithMacro = 1;
 * ------------------------------------------------------------------------------------------------------------
 * 22/09/03				: Release 3.93
 *						: Add process : IsToSkipMailWatcherQuota. (hnote2skip.c/.h)
 *						:
 *						: new : ProcBlackList()
 *						: 'SMTPOriginator' field is used instead of FROM when field is not empty.
 *						: new : NSFGetAttachmentFileName()
 * 23/09/03				: new : DeleteAttachFileWithException()
 * 25/10/03				: new : AddMEssageToEmail, add Subject Prefix
 * 29/10/03				: new : CleanUpAddress(char * useraddress)
 * 31/10/03				: new : SaveOnDiskAttachFileWithException()
 * 15/11/03				: add argt : include Detach file link (ftp://hostname/path/filename)
 * 19/11/03				: recover from 3.92.7 : M20 field from "mail_watcher" as for From value in
 *						: OverQuota reply
 *						: add RemoveInternetDomain_Shortname in UpdateReaderAccess on copied email
 *						: use M20 for FROM field in OverQuota process : OverQuota_From_value
 * 10/12/03				: Revision 3.92.9, Add source code GetUserAddressFullName
 *						: update: AppendReaderItem, setReaderAccess
 * 15/01/04				: Revision 3.92.11 (included)
 * 29/01/04				: Revision 3.92.12,3.92.13 (included)
 * 29/03/04				: Revision 3.92.14
 * -------------------------------------------------------------------------------------------------------------
 * 05/04/04				: Add [ ParseSMTPaddress ], SMTPAddress support when send internal.. the recipient field
 *						: is to be checked in order to decode 'encoded address'
 *						: Add [ GetDominoHostName ]
 *						: Get szServeur (HostName) : current domino server in szServeurHost (GV)
 * 08/04/04				: Revision 3.92.15
 * 15/04/04				: Change in ParseSMTPaddress, quote issue
 * 03/05/04				: revision 3.92.16
 *						: Fix in QuotaFilter.
 * 04/05/04				: revision 3.92.17
 *						: fix in SimpleProcCrochet + Doclink version
 * 13/05/04				: revision 3.93.5
 *						: change : OverQuota, add argument (, BOOL)
 *						: new : NSFItemDuplicateTextList
 *						: revision 3.93.6
 *						: change in NonDeliveryBlackList (_DocLink)
 *						: Change From & Principal if from is not equal to "0"
 * 21/05/04				: revision 3.93.7
 *						: change in parseSMTPaddress, case of recipients ="<"... => crash !-(GPF)
 * 22/05/04				: revision 3.93.8
 *						: change in parseSMTPaddress, check not null first.
 *						: change in CheckRecipientField (Advanced(P))
 * 24/05/04				: change in parseSMTPaddress, use char c - test p[0] not ZERO.
 *						: revision 3.93.9
 *						: change in OverQuota : Do not change the Subject if the Body is not to be changed too
 *						: do not modify FailureReason if already exist.
 *						: + various.
 *						: change in ProcBlackList : set nbrec = 1 if recipients field does not exists. recipients = "";
 *						: (sendToBis) use "To" (Notification) if To is not empty
 *						:
 *						: change in UpdateRecipientsList; check if recipients is not "" before appending values
 *						: change in BlackListedAddressNotification; same change as UpdateRecipientsList.
 * 27/05/04				: revision 3.93.10
 *						: change in OverQuota :check out when "resend" actions is used Special Quota field
 * 28/05/04				: revision 3.93.11
 *						: change in SaveOnDiskAttachFileWithException
 *						: change in MakeLinkFromPath
 * 08/06/04				: revision 3.93.12
 *						: change in OverQuota : fix Response deletion during replication
 *						: process for memo, calendar.
 *						: revision 3.93.13
 *						: Spam fight : get "SendTo" field when "Recipients" field is Empty
 *						: change in OverQuota : remove "$Ref" - changes link with 3.93.12
 * 30/06/04				: revision 3.93.14
 *						: fix in IsToSkipMailWatcherQuota (*field_list)
 *						: fix in ProcBlackList when Hnotefilter can not be opened => lookup handle not allocated.
 * 01/07/04				: change in OverQuota : Save "DeliveryReport" field - remove it when OverQuota is TRUE
 * 05/07/04				: revision 3.93.15
 *						: fix in 3.92.x not integrated : (29/3/04)
 * 12/07/04				: Revision 3.93.16
 *						: change in IsInGroup : Support variable forms for UserName.
 * -------------------------------------------------------------------------------------------------------------------
 * 15/09/04				: Revision 3.93.17, 3.93.18
 *						: change in OverQuota.
 *						: Patch AIX/LEXMARK/NOTEID.
 *						: need to generate a new UNID for OverQuota notification.
 * ------------------------------------------------------------------------------------------------------------------
 *						: Revision 3.93.19
 *						: change in OverQuota
 *						: Stop using DuplicateMail but 'TemporaryhNoteMIME2CD' Use SMTPOriginator instead of From
 *						: new version of TemporaryhNoteMIME2CD
 *						: change in NonDeliveryBlackList, NonDeliveryBlackList_DocLink
 *					    :
 * 22/09/04				: Revision 3.93.20
 *						: fix in ProcBlackList.
 *						: (nbrec - i -1 >=1); changed in (nbrec - (i-1)) >= 1).
 *						: nbrec = number of recipient, i the current recipients in the list.
 *						: duplicate if the hnote contains more than one recipients left
 *						:
 *						: Revision 3.93.21
 *						: new in GetBlackListRules
 *						: if Body field are to be appended then RulesWithMacro is set to "1"
 *						: to have 2 handles in ProcBlackist. one with raw MIME the other with MIME to CD conversion
 *						:
 * 07/10/04				: Revsion 3.93.22
 *						: change in AddMessageToEmail
 *						: support BList_Message and BList_Message_RT (for Rich Text)
 *						:
 * 22/11/04				: Revision 3.93.27
 *						: change in : GetUserAllAlias
 *						: Read the [MailAddress] & [ccMailLocation] fields as a valid possible addresses for
 *						: a user for filtering purposes.
 * -------------------------------------------------------------------------------------------------------------------
 * FREEZE : 3.93.27
 * -------------------------------------------------------------------------------------------------------------------
 * 11/12/2004			: release 3.94
 *						: add memory rules project files ( rulesmem.c, rulesmem.h )
 *						: new boolean : advblck_rm
 *						: new parameters for ProcBlacList
 *						: ProcBlackList(..., RulesList* L_rules, RulesList** StartL);
 *						: new : int	CheckBlackListRulesMemoryValidation( )
 *						: split files, create mwstrjob.c/h
 * 14/12/2004			: revision 3.94.2
 *						: rename list.c, list.h => mwlist.c mwlist.h
 *						: fix : remove AlerteAdmin(1) if OpenDbs fails
 *						: revision 3.94.3
 *						: fix : MMatch, wrong return if pattern : 42,24,44
 *						: revision 3.94.4
 *						: include "debug mode" from 3.93.29 ( MWBLACKLISTDEBUG )
 * 17/12/2004			: revision 3.94.5
 *						: include fix : OverQuota 3.93.30
 * 10/01/2005			: include fix : OverQuota 3.93.32
 *						: include spc : AppendMailHeaderToBody - 3.93.31
 * 06/02/2005			: revision 3.94.8
 *						: new function for Kaspersky A.V integration.
 *						: STATUS DeleteAttachFileByName()
 * 11/02/2005			: revision 3.94.9
 *						: new functions :
 * --------------------------------------------------------------------------------------------------------------
 * 28/02/05				: INTEGRATION of : Revision 3.93.33
 *						: Add new vote in SetRecOK_FromToForward : * No Influence * => + ZERO for RecOk.
 *						: Leave or remove "ReturnReceipt" when copy email into DB.( RemoveReturnReceipt )
 *						: change in : SimpleProcCrochet, add BOOL as last argument
 *						: change in : SimpleProcCrochet_DocLink, and BOOL as last argument
 * --------------------------------------------------------------------------------------------------------------
 * 17/03/05				: INTEGRATION of : Revision 3.93.35
 *						: OverQuota Mgt.
 *						: rename NSFNoteGetSize as mw_NSFNoteGetSize ( compilation 6.5 - conflict with native API )
 * ----------------------------------------------------------------------------------------------------------------
 * 18/03/05				: revision 3.94.14
 *						: ProcBlackList : Append Field Details in Body text Append ("BList_MessageAppend_Field_Values")
 *						: revision 3.94.15/16
 *						: Add new TAG : MWTAG, MWKAVCLEAN + Argument in ProcBlackList
 * ----------------------------------------------------------------------------------------------------------------
 * 23/03/05 			: INTEGRATION of  Revision 3.93.36
 *						: Remove '$KeepPrivate="1"' when OverQuota is sent back restore it on the resend
 * ----------------------------------------------------------------------------------------------------------------
 *						: revision 3.94.19
 *						: change in : ProcBlackList, init bRecOK = 0 instead of brecOK = 1 for neutral rules.
 *						: revision 3.94.24
 *						: change in : NonDelivery_DocLink - Append Original Header ( Top / Bottom )
 * ----------------------------------------------------------------------------------------------------------------
 * 14/04/2005			: revision 3.94.25
 *						: The rules are not stored in the same db as the quota.
 *						: a new Db is supplied : rulefile ( field in mail_watcher document )
 *						: new DBHandle ( hrulesdb )
 * ----------------------------------------------------------------------------------------------------------------
 * 15/04/2005			: revision 3.94.26
 *						: Set $MailWatcherServerName in the notes.ini
 *						: change in GetDominoHostName - open/close the real names.nsf and not the dbnab string
 *						: which is the 'rules group' database that migth not be the names.nsf
 *						: if the HostName is null then use the Domino server name.
 * ----------------------------------------------------------------------------------------------------------------
 * 18/04/05				: INTEGRATION of Revision 3.93.37
 *						: revision 3.94.27
 *						: OverQuota : IntendedRecipients : SendTo/CopyTo/BCC should be the same.
 * ----------------------------------------------------------------------------------------------------------------
 * 16/05/05				: INTEGRATION of revision 3.93.38
 *						: fix in AddMessageToEmail: prevent CompoundTextClose if no discard.
 * 22/06/05				: revision 3.94.29
 *						: change in SimpleAppendDocLink. reformat the URL to copied document
 *						: <a href= xxx> web : %s <\a>
 * 14/09/05				: INTEGRATION of revision 3.93.42
 *						: change in ProcBlackList : BList_DF_Header[0] = '\0'
 * ----------------------------------------------------------------------------------------------------------------
 * 09/11/05				: revision 3.94.32 - 33
 *						: work out ODSW41, Collect Extension (.xxx) - overflow. 
 *						: in struct advt_stat: char	Ext[MAXRECLEN]; used to be [150]
 *						: fix in : mw_NSFNoteGetSize
 * ----------------------------------------------------------------------------------------------------------------
 * 23/11/05				: revision 3.94.34 - 35
 *						: incoming mail must be open in RichTEXT for stats. & some filter rules.
 *						: new hNoteCD in AddinMain Open/close if STATS to be collected
 *						: change in GetBlackRedListRules : RulesWithMacro = MacroForCDOnly(filter_formula)
 *						:
 *						: revision 3.94.36-37
 *						: fix overflow : (never happened) : stat.form[50] <- qui.
 *						: change : get the second in the stats.
 *						: Add : mwhotspot.c,.h.
 *						: change in 'SimpleAppendDocLink' : new way to append a URL (mwhotspot) 
 * ----------------------------------------------------------------------------------------------------------------
 * 09/02/06			    : revision 3.94.38
 *						: Control the MWTAG list entry - top of the list when MWTAG is not "MWADVT"
 *						: if the entry is not include in then notes.ini variable : MWADVSRVMWTAG = mwgfile,mwspam
 *						: then the MWTAG entry is forged to "MWADVT" that's it !
 *						: read the notes.ini entry : MWADVSRVMWTAG, set the flag : MWADVSRVMWTAG = 1
 * ----------------------------------------------------------------------------------------------------------------
 * 18/02/06			    : revision 3.94.39
 *						: Do not abbreviated the reader access item in Copied mail. (MailCopy / ProcBlackList)
 *						: change in : GetUserAddressFullName(...., BOOL Abbreviate_it );
 *						:
 * 20/02/06				: revision 3.94.40
 *						: Fix strstr, reverse string & motif.
 *						:
 * 08/03/06				: revision 3.94.41
 *						: in revision 3.94.34, the RulesWithMacro = MacroForCDOnly(filter_formula) has been replaced
 *						: by RulesWithMacro = 1; There is a side effect with some fields when they get changed in memory 
 *						: which could be 'tested' by some macro formula. 
 *						: a particular field is RECIPIENTS.
 *						: Actions : Force when reopening the hnote to convert it to rich text, the RECIPIENTS FIELD SAVING.
 *						: new : STATUS	MoveFieldFromNote2Note(NOTEHANDLE FromNote, NOTEHANDLE ToNote, char* FieldName )
 *						:
 * 11/03/06				: revision 3.94.42
 *						: Reset mail.box openning time periodically.
 *						:
 * 13/03/06				: revision 3.94.43
 *						: DocLink (1 = DocLink Notes Only, DocLink = 2 => 1 + URL)
 *						: change in : ProcBlackList, SimpleAppendDocLink
 *						:
 * 20/03/06             : revision 3.94.44
 *						: reset MWTAG, but MWGZIP is allowed : TODO: configure Post MWADVT TAG.
 *						:
 * 21/03/06				: revision 3.94.45-46
 *						: save a MW_NbRecipients ( Number ) in temporary mail when the Recipients List is to large.
 *						: Purge temporary field : NSFItemDelete(hNote, MW_MACRO_FIELD_NB_RECIPIENTS..)
 *						:
 * 03/04/06				: revision 3.94.49 - Fix : mwhotspot.c ( InsertURL ).
 * -----------------------------------------------------------------------------------------------------------------
 * 19/04/06  >>> CODE FREEZE 3.94 <<
 * -----------------------------------------------------------------------------------------------------------------
 * 20/04/06				: revision 3.95.1
 *						: Implemente the PREDICATS. field/data associated to email ( from/to ) in ProcBlacList
 *						: Predicats can be used as arguments value.
 *						: TO->FORWARD_ADDRESS ( in the Forward Email To arguments )
 *						: new : GetUserPredicat(username, "CompanyName", &PredicatList);
 * 22/04/06				: revision 3.95.2
 *						: GrpList* SearchGroupWithMeta(GrpList* Current, char* GroupName)
 *						: ReadFirstUserPredicat(c)
 *						: revision 3.95.3
 *						: change in : GetBlackRedListRules.	
 *						: read a field that contains all the predicats of the rules
 *						: change in : GetUserAllAlias, use the prev. field to upload predicats in SpUser List.
 *						:
 *						: revision 3.95.5
 *						: change in : GetFilterRulesParameterOrPredicats
 *						: reset predicat value to "" if not found.
 *						:
 * 18/05/06				: revision 3.95.6
 *						: new : int ReadAllUserPredicats()
 *						:
 * 23/05/06				: revision 3.95.7
 *						: integrate : 3.94.51 ( fix in GetFieldInList())
 *						:
 * 31/05/06				: revision 3.95.8
 *						: fix "Importance" => "DeliveryPriority".
 *						: 
 * 07/06/06				: revision 3.95.9
 *						: INTEGRATION OF revision 3.93.45 
 *						: change in OverQuota : change FROM/PRINCIPAL for overquota test.
 * 16/10/06				: revision 3.95.10
 *						: INTEGRATION OF revision 3.94.56
 *						:
 * 14/11/06				: revision 3.95.11-12
 *						: Add a special field in the notification when a DocLink has been inserted
 *						: this field is to be used by MWSARC like module to construct document unique key
 *						: based on $MessageID
 *						: NSFItemSetText(hMes, "MWDOCLINK", mailcopyto - $MESSAGEID);
 *						:
 * 15/11/06				: revision 3.95.13
 *						: collect attachment size after the document size. (use ,) as separator.
 *						: change in mw_NSFNoteGetSize : add ExtSize field.
 *						:
 * 16/11/06				: revision 3.95.14
 *						: NIFOpenCollection ( 4th argt - set to ZERO - all time ).
 *						:
 * 19/01/06				: revision 3.95.15
 *						: Fix in RemoveNotesDomain()
 *						: make sure that the remove @domain are not internet domain
 *						: test in loc2 for "." on the right side.
 *						: integrate ( revision 3.94.62 )
 *						: fix in GetBlackListRules :
 *						: rulesw       = abs((int) RulesPriority);
 *						:
 * 05/03/07				: revision 3.95.17
 *						: fix in OpenDbs()
 *						:
 * 02/04/07				: revision 3.95.16 : fix regression : OpenDbs ( old OpenDbs_bak ) 
 * 23/05/07				: revision 3.95.18-20 : Align code with .h version defintion.
 *						: bRecOK = -1;		if email to be deleted, update the BlackListed List of address.
 *						: this field is to be used by Notification.
 *						: No use of ReturnVal = 1 when Email is to be deleted because of exceptions that
 *						: still can received the email: DeleteEmail => bRecOk=-1 => remove from Recipients Fields
 *						: so appended to Blacklisted list.
 * 25/05/07				: revision 3.95.21
 *						: Fix : ScanMail AIX - Exclude Task issues.
 *						: add new environment variable :  MWADVSRVOTHERSERVICESNOEXCLUDE"
 *						: new globale variable : AntiVirus_MailWatcherTask_NotExclude
 *						: change in : DuplicateMail ( add NOT_EM )
 *						: if ( NSFItemIsPresent(hNote, "Mailbox_AppLock", 15))
 * 22/10/2008			: revision 3.95.22
 *						: add resource files - recompile with VC++ 8
 * 11/01/2011			: revision 3.95.23
 *						: Comment a faulty error that appears after V8 migration
 * --[ TO DO ]------------------------------------------------------------------------------------------------------
 *
 * (1) Each module should leave a list of field to be deleted when the last MWTAG is removed!
 *
 * (2) Use the ( MWBLACKLISTDEBUG ) in ProcBlackList.
 *  FROM/TO/FORMULA for Match + Except + Rules Memory conditions.
 *
 ***********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
	#include <windows.h>
#endif

#include <ctype.h>

/* Notes API include files */
#include <global.h>
#include <addin.h>
#include <nsfdb.h>
#include <nif.h>
#include <osmem.h>
#include <nsfnote.h>
#include <ostime.h>
#include <osmisc.h>
#include <misc.h>
#include <osenv.h>
#include <stdnames.h>
#include <kfm.h>
#include <dname.h>
#include <lookup.h>
#include <nsfsearc.h>
#include <lookup.h>
#include <mailserv.h>
#include <nsfobjec.h>
#include <idtable.h>
#include <nsferr.h>
#include <easycd.h>
#include <log.h>		/* logEventText */
#include <nsfdata.h>
#include <foldman.h>
#include <osfile.h>		/* OSPathConstruct */


/* Special MW headers */
#include "mwadvt.h"
#include "wildcard.h"
#include "mwlist.h"
#include "hnote2skip.h"
#include "rulesmem.h"
#include "mwstrjob.h"
#include "mwhotspot.h"		/* call in SimpleAppendDocLink - for URL */


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


/***************************************************************/
/*                                                             */
/* Special Data type for Black List & Red List Rules structure */
/* and matching process                                        */
/*                                                             */
/***************************************************************/
typedef struct BlackListRules
{
	char* From;
	char* To;
	char* Except_From;					/* Exception From */
	char* Except_To;
	char* Formula;
	char* Ref;							/* unique rules reference */
	int	w;
	int Except_w;						/* Exception parameter type [11=>33] */
	int AfterAllRule;					/* default is ZERO, if AfterAllRule = 1 */
	int	rank;							/* rules rank in the list - view ($BlackList) */
	int Argt_ThisRulesTermintes;		/* Argument stored in BlackList document */
} BLrules;


typedef struct GroupListElement
{
	char* GroupName;
	struct GroupListElement* Next;
} GrpList;


typedef struct SpecialUserListElement
{
	char* UserName;
	GrpList* GroupList;
	struct SpecialUserListElement* Next;
} SpUser;


/* Function prototypes */
STATUS		OpenDbs(void);
STATUS		CloseDbs(void);
BOOL		IsPrevInst();
BOOL		EstVraiBase(DBHANDLE);
BOOL		sMatch(char*, char*);
int			GetPos(char* a, char x);
int			ProcBlackList(NOTEHANDLE *, DBHANDLE,char* mail_ID, RulesList* L_rules, RulesList** StartL, DBHANDLE);
STATUS		GetBlackRedListRules(HCOLLECTION coll_bl, DBHANDLE);
void		AlerteAdmin(int);
void		SplitGroup(NOTEHANDLE, char*);
int			ProcFormula(NOTEHANDLE);
BOOL		EvalFormula(NOTEHANDLE, char *, char *);
void		ProcCrochet(NOTEHANDLE, DBHANDLE);
void		dumpstat(void);
NUMBER		mw_NSFNoteGetSize(DBHANDLE, NOTEHANDLE, int *);
void		supprespace(char *);
void		NonDeliveryFilter(char *, char *, char *);
void		AddMessageToEmail(NOTEHANDLE fNote, NOTEHANDLE* hNote, char* fieldName, DBHANDLE, int*, NOTEHANDLE*, NOTEHANDLE, int);
void		CheckPath(char *Filename);
void		RemoveNotesDomain(char *n);
void		SimpleProcCrochet(NOTEHANDLE hNote, char* basearc, STATUS MW_RAPP_Bool, STATUS MW_RAPP_AGAIN_Bool, char*, BOOL);
void		NonDeliveryBlackList(DBHANDLE, NOTEHANDLE, char* from, char *sendto, char* sendtobis, char*, char * SubjectHeader, char *origsubject, NOTEHANDLE hBL_Note, char*, DBHANDLE *, char*, char*,char*);
STATUS		CheckMonoMailBox();
void		SetRecOK_FromToForward(char* emetteur, char* safe_recipient, NOTEHANDLE hNote, NOTEHANDLE hNoteFiltre, char* fieldname, char* OrigSubject, int AddMessageToEmail, int ApplyAction, char* BListMemo, int BL_Rules,
								   int*, DBHANDLE, char* emetteurabr, char* recipientabr);
void		SetEmailTempNumFied(NOTEHANDLE hMes,int BL_Rules, int BL_NbRulesApplied);
int			GetNbRulesApplied(char * string, const char *seps, int a);


/********************************************/
/*                                          */
/* functions used to get the matching rules */
/*                                          */
/********************************************/

STATUS		OpenNAB(char *);
STATUS		CloseNAB();
int			MMatch(char*, char*, BLrules);
int			Except_Match(char*, char*, BLrules);
int			IsInGroup(char*,  char*);
SpUser*		SearchUser(SpUser*, char*);
GrpList*	SearchGroup(GrpList*, char* GroupName);
GrpList*	InsertGroup(GrpList* Current, char* GroupName);
void		CreateUserList(SpUser* SpecialUserList);
void		CreateGroupList(GrpList* GroupList);
void		BuildSpecialUserList();
BOOL		GetMatchKey(NOTEHANDLE hNote, char* Key, char* from, char* to, int* i);
BOOL		IsaGroupInNab(char* GroupName);
void		FreeMemory();
void		DumpBlackList(SpUser *FirstEle);

/* New	: sMatch in IsInGroup : 22/4/01 */
SpUser*		sSearchUser(SpUser*, char*);
int			IsInGroupWmeta(char*, char*);

/* Fix	: 23/4/01 : Group inheritance... */
void		GetUserInGroup(char* GroupName, GrpList**, char**, int);
SpUser*		InsertUser(SpUser* Current, char* UserName, char* GroupName, GrpList*);
void		PopGroupHistory(GrpList**);

/* 3.9 */
void		RemoveFromMWTAG(NOTEHANDLE, char*, char*);
void		AppendToMWTAG(NOTEHANDLE, char*, char*);
int			QuotaFilter(NOTEHANDLE hNote, char*);
int			OverQuota(NOTEHANDLE* hNote, NOTEHANDLE* hCopy, int* IsNewDoc, DBHANDLE mailbox, BOOL); /*3.93.4 add BOOL, change or not the body field to Resend actions */

/* 3.91 */
STATUS		GetReplicaID(DBHANDLE, TIMEDATE*);
STATUS		AppendDocLink(DBHANDLE, char*, NOTEHANDLE, char*, HANDLE*);
STATUS		MailCopyGetDocLinkDetail(char*, DBHANDLE, NOTEHANDLE, TIMEDATE*, char*,UNID*, UNID*);
STATUS		SimpleAppendDocLink(NOTEHANDLE, TIMEDATE, UNID, UNID, char*, HANDLE, int DocLinkType);
void		SimpleProcCrochet_DocLink(NOTEHANDLE, char*, STATUS, STATUS,int*, char*, TIMEDATE*, UNID*, UNID*, char*, BOOL, char*);
void		NonDeliveryBlackList_DocLink(DBHANDLE, NOTEHANDLE, char*,char*,char*,char*, char*,char*, NOTEHANDLE, int, char*, TIMEDATE, UNID, UNID, char*, DBHANDLE*, char*, char*,char*,char*);
STATUS		DeleteFieldInNote(NOTEHANDLE note_handle_rules, NOTEHANDLE note_handle, char* fieldNameList, char* RulesRef);
STATUS		DeleteAttachFile(NOTEHANDLE note_handle, char* AttachFieldName);
STATUS		DuplicateMail(DBHANDLE*, NOTEHANDLE, NOTEHANDLE*, int );
STATUS		UpdateNewMailNote(NOTEHANDLE hNote);
int			UpdateRecipientsList(NOTEHANDLE hNote);
STATUS		BlackListedAddressNotification(NOTEHANDLE hNote, DBHANDLE *);
STATUS		AppendReaderItem (NOTEHANDLE  hNote, char* fieldname, char * szReader, int check);
STATUS		setReaderAccess(NOTEHANDLE hnote, char* fieldname, char* readersField, NOTEHANDLE tnote);
STATUS		AppendMailHeaderToBody(NOTEHANDLE, HANDLE*, char*, char*, char*);
int			CheckBlackListRulesPeriod(NOTEHANDLE hNote);
STATUS		GetBlackListAction(BLrules*, NOTEHANDLE);
int			GetBlackListAction_BOOL(NOTEHANDLE note_handle, char* fieldname);
void		MarkNoteNum(NOTEHANDLE hNote, NUMBER*);

/* 3.92 */
void		Trim(char* in, char* out);
STATUS		MakeStringFromNotesAddressList(NOTEHANDLE hNote, char* fieldname, char* out_string, int max_len);
STATUS		GetUserAllAlias(char* username, GrpList* GrpHistory, char** FieldPredicatList, int pred_nbitem);
void		ResetRulesw(int*, int From_User, int To_User);
int			GetUserAlias(char* username, char* fieldname, GrpList* GrpHistory, int PredicatsFound, GrpList* PredicatList);
char*		GetWord(char * string, const char *seps, int a);
STATUS		NoteHandleNativeConvertMIME2CD(NOTEHANDLE* hNote, DBHANDLE hDB, NOTEID note_id);
STATUS		DeleteNote(DBHANDLE, NOTEHANDLE hNote, NOTEID note_id);
STATUS		MoveToFolder(DBHANDLE mail_handle, char* mailfile, char* FolderName, NOTEHANDLE notehandle);
STATUS		CreateFolder(DBHANDLE db, char* FolderName, NOTEID* foldid);

/* 3.92a */
STATUS		SplitMail(DBHANDLE* hMB, NOTEHANDLE hNote, NOTEHANDLE* hCopy);
int			isUserAddress( char* temp_noDomain );
STATUS		CheckRecipientField(DBHANDLE mailbox, NOTEHANDLE hNote, char* FiedName, WORD* nbUserFound, char * FromValue);
STATUS		GetAllAddressesFromField(NOTEHANDLE hNote, char* FieldName, char* FromValue, WORD* nbUserFound, List UserAddresses, Position P, int recurse);

/* 3.92b */
STATUS		SaveStatDoc(NOTEHANDLE hNoteStat, NUMBER nbchamp, char* StatFile);
STATUS		CreateStatDoc(DBHANDLE hStatdb, NOTEHANDLE* hNoteStat, char* StatFile );
void		LogLine(char *file_name, char *line, int display_time);
void		RemoveAllDomain_ShortName(char *n);


/* 3.93 */
int			MacroForCDOnly(char * filter_formula);
int			TemporaryhNoteMIME2CD(DBHANDLE hDB, NOTEHANDLE hNote, NOTEHANDLE* hNewNoteCD);
STATUS		DeleteNoteWith_NoteHandle(DBHANDLE hDB, NOTEHANDLE hNewNoteCD);
void		NSFGetAttachmentFileName(BLOCKID bidv, char* FileName, char* FileExtension);
STATUS		DeleteAttachFileWithException(NOTEHANDLE note_handle, char* AttachFieldName, char* ExtensionList);
int			CleanUpAddress(char * useraddress);
STATUS		SaveOnDiskAttachFileWithException(NOTEHANDLE* note_handle, char* AttachFieldName, char* ExtensionList, char* filepath, char* HostDetachFileLink, int* bNewNote, DBHANDLE hDB, int, int, char*);
void		RemoveInternetDomain_ShortName(char *n);
STATUS		DuplicateEmailForBodyChange(NOTEHANDLE *hNote, int* bNewNote, DBHANDLE hDB);
void		MakeLinkFromPath(char * Filename);
STATUS		AppendBlankLineToBodyField(NOTEHANDLE hNote);
STATUS		AppendLineToBodyField(NOTEHANDLE hNote, char* szMessageText);
STATUS		DetachFileToDisk(NOTEHANDLE note_handle, BLOCKID TobeDetached, char * DetachFileName);
int			SaveOnDiskAttach_MakeUniqueName(char** FileNameList, int NbTobeDetach, char *FileName);
int			IsInString(char* char_in, char* char_ref);
int			GetUserAddressFullName(char* user_address, char* user_fullname, char* fieldname, BOOL abbreviate_it);
int 		parseSMTPaddress(char* FullAddress, char* ShortAddress);
STATUS		GetDominoHostName(DBHANDLE,char*,char*,char*);
STATUS		NSFItemDuplicateTextList(NOTEHANDLE hNote, char* Field_in, char* Field_out, BOOL);

/* 3.94 */
int			CheckBlackListRulesMemoryValidation(NOTEHANDLE hNote, RulesList* L_rules, char* RulesRef );
int			RulesMemoryCount(char* RulesMemoryRef, char* TmpBLName, RulesList* L_rules, RulesList ** StartL );
int			WriteToDebugLog(char* filename, NOTEHANDLE hnote, char* fieldname);

/* 3.94.6 */
STATUS		DeleteAttachFileByName(NOTEHANDLE note_handle, char* AttachedFileName);
STATUS		NSFGetAttachmentFullFileName(BLOCKID bidv, char* FileName, int MaxLength);
int			IsInFieldValueList(char** field_List, char* FieldValue);
void		FreeMemory_GetFieldInList( char** field_List, int nbitem );

/* STATUS		GetFieldInList(NOTEHANDLE note_handle, char* FieldName, char** field_List, int MAXItemSize, int* nbitem); 3.94.51 */
STATUS		GetFieldInList(NOTEHANDLE note_handle, char* FieldName, char** field_List, int MAXItemSize, int MAXList, int* nbitem);


STATUS		AppendThreeTextFieldToBodyWithHeader(NOTEHANDLE, HANDLE* hCompound, DWORD,char*,char*,char*,char*,int, char);
STATUS		AppendTwoTextFieldToBodyWithHeader(NOTEHANDLE, HANDLE* hCompound, DWORD,char*,char*,char*,int, char);
STATUS		AppendTextFieldToBodyWithHeader(NOTEHANDLE, HANDLE* hCompound, DWORD,char*,char*,int, char);
STATUS		AppendTextListToBodyUsingFieldArgument(NOTEHANDLE hNote,HANDLE* hCompound, DWORD, char*, char*, char*, char*,int, char);
STATUS		ProcBl_AppendFieldDetailsToEmailBody(NOTEHANDLE hNoteFiltre, NOTEHANDLE hNoteEmail, char* sheader, char* FieldRef);
STATUS		GetDatabaseHandle(char* servername, char * mailfile, DBHANDLE * mail_handle, BOOL vUnix);
STATUS		MoveFieldFromNote2Note(NOTEHANDLE FromNote, NOTEHANDLE ToNote, char* FieldName );
void		InitMailBoxOpeningTime( TIMEDATE* retoldtime, int);

/* 3.95.1 */
int			GetUserPredicat(char* username, char* fieldname, GrpList** PredicatList);
void		FreeGrpList(GrpList* L );
GrpList*	SearchGroupWithMeta(GrpList* Current, char* GroupName);
int			ReadFirstUserPredicat(char* UserName, char* Predicat, SpUser* SpecialUserListGlobal, char* PredicatValue);
void		LogErr(char *message, STATUS err);
STATUS		GetFilterRulesParameterOrPredicats(NOTEHANDLE hNote, char* fieldname, char* field_value, int MaxArgtSize,char* FROM, char* TO, SpUser* HeadofList);
/* 3.95.6 */
int			ReadAllUserPredicats(char* UserName, char* Predicat, SpUser* SpecialUserListGlobal, char** PredicatValue, int MaxPredicatSize, int MaxPredicatList);
STATUS		MakeTextListFieldStringWithSep(NOTEHANDLE hNote, char* fieldname, char* stritem, char* sep );
STATUS		GetFilterRulesParameterOrAllPredicats(NOTEHANDLE hNote, char* fieldname, NOTEHANDLE target_hNote, char* targetfield_value, 
											  int MaxArgtSize, char* FROM, char* TO, SpUser* HeadofList, int MaxPredicatSize, int MaxPredicatList);




/******************************************************************************/
/******************************* Variables globales ***************************/
/******************************************************************************/
char		szServeur[MAXUSERNAME];
char		szServeurHost[MAXUSERNAME];
char		dbsetup[MAXPATH];
char		dbrules[MAXPATH];
char		szDomain[50];
int			pasint;
int			statint;
DBHANDLE	hDb; /* MW_SETUP.NSF handle */
DBHANDLE	hDbRules;	/* MW_SETUP or MW_RULES */
static		HCOLLECTION coll_bl;
static		HCOLLECTION coll_bl_formula;
static		HCOLLECTION coll_cr;
static		HCOLLECTION coll_bs;
static		HCOLLECTION coll_quota; /* view : (POLICES) -> 3.9 */

struct		advt_stat statmw[MAXLGSTAT];
int			lgstat;
char		StatFile[MAXPATH];
BOOL		advstat, advstat_subject, advarch, advblck, advform, advnotify_blacklist,advblck_rm;
char		alertfrom[MAXUSERNAME];
char		alertsubj[150];
char		alertst[MAXUSERNAME];
char		ndffrom[MAXUSERNAME];
char		ndfsubject[300];
char		advquotaSubject[300];
char		ndftextmessage[100][MAXTXTMESSAGE];
char		advquotaTextMessage[100][MAXTXTMESSAGE];
int			nb_formula_Messagetxt = 0;
int			nb_quota_Messagetxt =0;
char		Formula_Txt[100][MAXTXTMESSAGE];
int			nb_formula_txt = 0;
int			MaxRules = 0;
int			BL_AFTERALL = 0;

/* Advanced Quota */
char		OverQuota_From_value[MAXUSERNAME];
char*		overquota_field_values[100];
char*		overquota_form_List[100];
BOOL		advquota, advquota_del_attach_file, advquota_exception, advquota_resend;
char		AdvQuota_ResendAction_FormName[100];

/* BlackList Proc address notification */
char		BlackList_Address_Subject[300]; /*  ->(Subject suffix) */
int			nb_BlackList_Address_Message = 0; /*  ->(nb of lines) */
char		BlackList_Address_Message[100][MAXTXTMESSAGE]; /* ->(Body message) */
char		BlackList_Notification_From_Header[100];		/* M21 */
char		BlackList_Notification_To_Header[100];			/* M22 */
char		BlackList_Notification_Subject_Header[100];		/* M23 */


/***********************************************/
/*                                             */
/*  Global variables for Black List            */
/*                                             */
/***********************************************/
SpUser*		SpecialUserListGlobal;
SpUser*		FirstEle;
BLrules		BlackRedList[MAXRULESARRAY];
int			RulesWithMacro = 0;		/* set to 1 when Macro have been found in the rules */
DBHANDLE	hDbNAB;					/* handle onto the names.nsf */
GrpList*	DumpGrpList;			/* List of group, recursive checking */
static		HCOLLECTION coll_grp;	/* handle on the names.nsf group view */
char		dbnab[MAXPATH];			/* Names & Address book database name */


/*** For Unix  set to TRUE before compil ***/
#ifdef UNIX
	BOOL vUnix = TRUE;
#else
	BOOL vUnix = FALSE;
#endif

/* DEBUG MODE */
BOOL	MWBLACKLISTDEBUG;
char	LogString[512];
BOOL	AntiVirus_MailWatcherTask_NotExclude = FALSE;			/* AntiVirus TEST */


/********************************************************************
 * Cette fct est le point d'entree a tout add-in serveur Lotus Notes.
 * Apres initialisation, cette fonction entre dans une boucle qui
 * permet de traiter tout nouveau message.
 *
 * INPUTS : int argc        non utilise
 *          char *argv      non utilise
 * OUTPUT : STATUS          code erreur Notes (ou NOERROR)
 ********************************************************************/
STATUS far PASCAL  AddInMain (HMODULE hModule, int argc, char *argv[])
{
	STATUS		sErreur;
	HANDLE		hOldStatusLine;
	HANDLE		hStatusLineDesc;
	HMODULE		hMod;
	NOTEHANDLE	hNote;
	NOTEHANDLE	hNoteCD;		/* 3.94.34 : open & close a CD version of the hNote */
	NUMBER		num;

	/* Fix 3.71a char		from[MAXUSERNAME]; */

	TIMEDATE		retoldtime[MAXMAILBOX], retnewtime[MAXMAILBOX];
	DBHANDLE		hMb;
	static			HANDLE idtable;
	DWORD			ncmesat;
	NOTEID			note_id;
	char			qui[MAXUSERNAME];
	char			quiabr[MAXUSERNAME];
	char			FromValue[MAXUSERNAME];
	char			FromValue_abbr[MAXUSERNAME];
	char			DocUniqueID[DOCID];
	char			sMW_DocID[DOCID+1];
	char			MW_Doc_Subject[MAXRECLEN];		/* see mwadvt.h */
	ORIGINATORID	MainOID;
	TIME			current;
	WORD			wnb, wbcl, wlen;
	int				pj;
	int				ctr;
	char			filename[MAXPATH];
	int				MWDebug;
	int				HaveElapsed;
	char			MW_SKIPRULE[3];
	int				mwtag = 0;
	char			TagListEle[MAXMWTAG];
    int				InitMailBoxCtr = 0;
	int				QuotaEnd = 0;
	int				MW_RefreshView = 180;		/* default value : if negative => nil */
	int				mailduplicate = 0;
	NOTEHANDLE		hCopy = NULLHANDLE;
	int				IsNewDocQuota = 0;
	NOTEHANDLE		hNoteQuotaToBeDeleted = NULLHANDLE;
	/* 3.94	*/
	RulesList*		L_rules	= NULL;
	RulesList**		StartL = NULL;
	int				MWADVSRVMWTAG = 0;
	int				mwtag_ready = 0;
	char			szMWADVSRVMWTAGLIST[512];		/* nb :todo : MAXTAGLIST - to be shared with advsrv */


	/* Init - check out if a task is already running */
	MWDebug = FALSE;
	if (OSGetEnvironmentString("MWDEBUGMODE", szDomain, sizeof(szDomain)-1))
		MWDebug = TRUE;

	if (MWDebug == FALSE)
	{
		if (IsPrevInst())
			return(NOERROR);
	}

	/*** debug mode ***/
	MWBLACKLISTDEBUG = FALSE;
	if (OSGetEnvironmentString("MWBACKLISTDEBUG", szDomain, sizeof(szDomain)-1))
		MWBLACKLISTDEBUG = TRUE;

	/* 3.94.65 : Exclude Task env */
	AntiVirus_MailWatcherTask_NotExclude = FALSE;			/* for antiVirus support TEST */
	if ( OSGetEnvironmentString("MWADVSRVOTHERSERVICESNOEXCLUDE", szDomain, sizeof(szDomain)-1)) 
	{
		if ( szDomain[0] == '1' )
			AntiVirus_MailWatcherTask_NotExclude = TRUE;
	}


	/** Tunning : Time before refreshing working view (3.91) **/
	if (OSGetEnvironmentString("MWREFRESHVIEWMODE", szDomain, sizeof(szDomain)-1))
	{
		if (strcmp(szDomain,"NEVER") == 0)
			MW_RefreshView = -1;
		else
			MW_RefreshView = atoi(szDomain);
	}

	/** 3.94.38 : Check module before MWADVT notes.ini entry */
	if ( OSGetEnvironmentString("MWADVSRVMWTAG", szMWADVSRVMWTAGLIST, sizeof(szMWADVSRVMWTAGLIST)-1) )
	{
		UpperCase(szMWADVSRVMWTAGLIST);	
		MWADVSRVMWTAG = 1;
	}


	/** Show off **/
	AddInQueryDefaults (&hMod, &hOldStatusLine);
	AddInDeleteStatusLine (hOldStatusLine);

	hStatusLineDesc = AddInCreateStatusLine(NOMAPI);
	AddInSetDefaults (hMod, hStatusLineDesc);
	AddInSetStatusText(DO_INIT);


	/**( On ouvre la base de travail )**/
	if (sErreur = OpenDbs())
	{
		/* disable since 3.93.27 : could crash the server as db are not fully init
		so char parameters are not initilazed : AlerteAdmin(1);  */
		return(NOERROR);
	}

	lgstat = 0;

	/* Domaine de messagerie */
	OSGetEnvironmentString("Domain", szDomain, sizeof(szDomain)-1);


	/* Time between two blackList reload within the : 'while (!AddInIdle())' loop */
	if (MWDebug == FALSE)
		HaveElapsed = 180;
	else
		HaveElapsed = 1;

	/* Tunning */
	if (MW_RefreshView > -1)
		HaveElapsed = MW_RefreshView;


	/* Upload in memory the filtering rules (to do error checking !!!)*/
	if (advblck)
	{
		/* Build the Special Black List data structure, containing people in Groups */
		if( sErreur = OpenNAB(dbnab))
		{
			AlerteAdmin(4);
			return(sErreur);
		}

		/* Get Domino Server full DNS name : szServeur = launcher's id name */
		if (MWDebug == FALSE)
			GetDominoHostName (hDbNAB, dbnab, szServeur, szServeurHost);


		if (advblck) /* advblck is reset to FALSE if an error has occured during OpenNAB */
		{
			/* Continue */
			BuildSpecialUserList();
			FirstEle = SpecialUserListGlobal; /* save the starting point address */

			if (sErreur = GetBlackRedListRules(coll_bl, hDbRules))
				AlerteAdmin(4);

			/* NAB & group collection closing */
			if(sErreur = CloseNAB())
				AlerteAdmin(4);

			/* Set the global list at the first Ele */
			if (FirstEle != SpecialUserListGlobal)
				SpecialUserListGlobal = FirstEle; /* reset */

			/* debug : DumpBlackList to a file */
			DumpBlackList(FirstEle);

			/* --- 3.94 : init rules memory ---*/
			L_rules = MakeEmptyRulesList();
			if ( L_rules )
			{
				StartL = &L_rules;
				advblck_rm = TRUE;
				RulesMemoryRestoreFromFile(L_rules, RULES_MEMORY_DEFAULT_FILE, StartL );
			}
		}
	}

	/* : 3.94.42 : set opening base timedate : now -5 years  */
	InitMailBoxOpeningTime( retoldtime, MAXMAILBOX);

	/***** Loop over tha MAXMAILBOX time array 	
	for (ctr = 0; ctr < MAXMAILBOX; ctr++)
	{
		OSCurrentTIMEDATE(&retoldtime[ctr]);
		TimeDateAdjust(&retoldtime[ctr], 0, 0, 0, 0, 0, -5);
	}
	/***/

	/* Initialisation terminée */
	AddInLogMessageText(INIT_DONE, NOERROR, NOMAPI);
	AddInSetStatusText(ETAT_WAIT);

 	/* reset */
	if (CheckMonoMailBox())
		InitMailBoxCtr = -1;	/* Start at Ctr++ => Ctr = 0 => filename init to mail.box */
	else
		InitMailBoxCtr = 0;		/* Start at Ctr++ => Ctr = 1 => filename init to mail1.box */


	/* Lancement de la boucle */
	while (!AddInIdle())
	{

		/******************************************************************************************/
		/* - Refreshing working views  - */
		/* should be 180 */
		if (MW_RefreshView > -1)
		{
			 if (AddInMinutesHaveElapsed(HaveElapsed))
			 {
				AddInLogMessageText(UPDATE_VIEW, NOERROR, NOMAPI, dbrules);
				AddInSetStatusText(ETAT_UPDATE_VIEW);

				/* : 3.94.42 : set opening base timedate : now -5 years  */
				InitMailBoxOpeningTime( retoldtime, MAXMAILBOX);

				if (advblck)
				{
					/* View : From & To & Formule */
					if (!(sErreur= NIFUpdateCollection(coll_bl_formula)))
					{
						/* rules with priority order */
						if (!(sErreur = NIFUpdateCollection(coll_bl)))
						{
							if (advblck) /* advblck is reset to FALSE if an error occurs during OpenNAB */
							{
								/* Build the Special Black List data structure, containing people in Groups */
								if( sErreur = OpenNAB(dbnab))
									AlerteAdmin(4);

								/* Continue ? */
								if (advblck)
								{
									/* Free the memory */
									FreeMemory();
									BuildSpecialUserList();
									FirstEle = SpecialUserListGlobal; /* save the starting point address */

									if (sErreur = GetBlackRedListRules(coll_bl, hDbRules))
										AlerteAdmin(4);

									/* NAB closing */
									if(sErreur = CloseNAB())
											AlerteAdmin(4);

									/* Set the global list at the first Ele */
									if (FirstEle != SpecialUserListGlobal)
										SpecialUserListGlobal = FirstEle; /* reset */

									/* debug : DumpBlackList to a file */
									DumpBlackList(FirstEle);
								}
							}

						} /* Collection updated correctly : coll_bl */
						else
							AddInLogMessageText(ERR_UPDATE_COLL, sErreur, NOMAPI, "coll_bl");

					} /* Collection updated correctly : coll_bl_formula */
					else
						AddInLogMessageText(ERR_UPDATE_COLL, sErreur, NOMAPI, "coll_bl_formula");
				}

				if (advarch)
				{
					if (sErreur = NIFUpdateCollection(coll_cr))
						AddInLogMessageText(ERR_UPDATE_COLL, sErreur, NOMAPI, "coll_cr");
				}


				if (advform)
				{
					if (sErreur = NIFUpdateCollection(coll_bs))
					AddInLogMessageText(ERR_UPDATE_COLL, sErreur, NOMAPI, "coll_bs");
				}

				AddInSetStatusText(ETAT_WAIT);
			} /* Every 180 minutes */
		} /* if > - 1 */

/*******************************************************************************************/



		/* On dump toutes les N minutes les stats */
		if (AddInMinutesHaveElapsed(statint))
		{
			/* Y a t-il des stats à dumper ? */
			if (lgstat)
				dumpstat();
		}	/* fin dump des stats */


		/*** On regarde de temps à autre ce qu'il y a à traiter ***/
		if (AddInMinutesHaveElapsed((DWORD) pasint))
		{
			/* reset */
			ctr = InitMailBoxCtr;	/* if InitMailBoxCtr = -1 => Ctr++ => Ctr = 0 => filename init to mail.box */

			sErreur = NOERROR;

			while ((!sErreur) && ctr < MAXMAILBOX)
			{
				/* Check mail(x).Box */
				ctr++;

				/* To stop : tell mwadvt quit */
				if (AddInShouldTerminate())
				  break;

				/*** Multi mail.box ***/
				if (ctr)
					sprintf(filename,"mail%1d.box", ctr);
				else
					strcpy(filename, "mail.box");


				/*** Open the mail(x).box ***/
				sErreur = NSFDbOpenExtended(filename, 0, NULLHANDLE, &retoldtime[ctr], &hMb, &retnewtime[ctr], NULL);

				if (sErreur == ERR_NO_MODIFIED_NOTES)
				{
					retoldtime[ctr] = retnewtime[ctr];
					sErreur = NOERROR;

					continue; /***** (RESET THE ERROR => Go and try the new mail(x).box) ******/
				}


				if (sErreur)
				{
					/****** (STOP do not try to open the next mail(x).box *****/
					continue;
				}


				/********************************************************/
				/*														*/
				/* NB : at this point the mail.box (=> filename) exists */
				/* and the mail(x).box is opened (hMb)                  */
				/*														*/
				/********************************************************/
				idtable = NULLHANDLE;
				if (sErreur = NSFDbGetModifiedNoteTable(hMb, NOTE_CLASS_DATA,
									 retoldtime[ctr], &retnewtime[ctr], &idtable))
				{

					NSFDbClose(hMb);

					if (sErreur == ERR_NO_MODIFIED_NOTES)
					{
						retoldtime[ctr] = retnewtime[ctr];
						sErreur = NOERROR;  /***** (RESET THE ERROR => Go and try the new mail(x).box) ******/

						/* release memory */
						if (idtable != NULL)
							OSMemFree(idtable);

						continue;
					}

					AddInLogMessageText("%s : err NSFGetModifiedNoteTable mail.box", sErreur, NOMAPI);
					sErreur = NOERROR;  /***** (RESET THE ERROR => Go and try the new mail(x).box) ******/

					/* release memory */
					if (idtable != NULL)
						OSMemFree(idtable);

					continue;
				}



				/****************************************************/
				/*                                                  */
				/*       Working out documents for filename         */
				/*                                                  */
				/****************************************************/
				OSLockObject(idtable);
				ncmesat = 0L;

				AddInSetStatusText(ETAT_ACTIVE);

				while (IDScan(idtable, (FLAG)(ncmesat++==0L), &note_id))
				{
					if (AddInShouldTerminate())
					  break;

					/* More control over the note_id address */
                    if (IDIsPresent(idtable, note_id))
					{

						/* if DELETE Flag => jump next */
						if (RRV_DELETED & note_id)
						 continue;

#ifdef V4
						/*( Standard Notes Open )*/
						 if (sErreur = NSFNoteOpen(hMb, note_id, 0, &hNote))
						 {
							AddInLogMessageText(ERR_OPEN_NOTE, sErreur, NOMAPI, note_id, "mail.box");
							continue;
						  }
#else

						/******* Special V5 Open - Keep MIME part native */
						if (sErreur = NSFNoteOpenExt(hMb, note_id, OPEN_RAW_MIME, &hNote))
						{
							/* 3.95.23 : Invalid error with V8 API */
							/* AddInLogMessageText(ERR_OPEN_NOTE, sErreur, NOMAPI, note_id, "mail.box"); */
							sErreur = NOERROR;
							continue;
						}
#endif

							/* Mark as Mime */
							/* ISMIME = 1; */
							/*	End if (NSFItemIsPresent($NoteHasNativeMIME)					} */


						/* Is the DEAD flag a real DEAD one ??? */
						if (!NSFItemIsPresent(hNote, "RoutingState", 12))
						{
							NSFNoteClose(hNote);
							continue;
						}

						/* 3.92.15 : Read RoutingState : Must be HOLD or DEAD */
					    NSFItemGetText(hNote,"RoutingState",TagListEle, sizeof(TagListEle) - 1);
						if(strcmp(TagListEle,"DEAD") && strcmp(TagListEle,"HOLD"))
						{
							NSFNoteClose(hNote);
							continue;
						}

						/* Fix : 22/05/03 */
						/* processus those that are DEAD in order to build rules that can delete them automatically */
						/* On vérifie que le message n'est pas réellement mort ! */
						/* if (NSFItemIsPresent(hNote, "FailureReason", 13))
						{
							NSFNoteClose(hNote);
							continue;
						}
						*/


						/***** Patch TREND MICRO  *****/

						if (NSFItemIsPresent(hNote, "MB_OtherServices", 16))
						{
							NSFNoteClose(hNote);
							continue;
						}

						/* 3.94.66 */
						if ( NSFItemIsPresent(hNote, "Mailbox_AppLock", 15))
						{
							NSFNoteClose(hNote);
							continue;
						}

						/***** End of Patch TREND MICRO ****/


						/* Make sure that MWADVT is the current task on the MWTAG field */
						/* read the first element : this element is UpperCased already  */
						NSFItemGetTextListEntry(hNote, MWTAGFIELD, 0 ,TagListEle, sizeof(TagListEle) - 1);


						/* 3.94.38 : upgrades : ( if different then Close  ! ) */
						if ( strcmp(TagListEle, MAILWATCHER_MWADVT) )
						{
							mwtag_ready = 0;

							/* 3.94.38 : reset MWTAG list if entry is not valid */
							if ( ( strlen( TagListEle ) > 0 ) && ( MWADVSRVMWTAG == 1 ) )
							{
								/* if not found in the list = mwtag is not correct that will hold all the mail for a non existing task ! */
								/* 3.94.44 : POST_MWADVSRVMWTAGLIST ( post MWADVT modules TAG ) */
								if  ( ( strstr(szMWADVSRVMWTAGLIST, TagListEle) == NULL ) && ( strstr( POST_MWADVSRVMWTAGLIST, TagListEle ) == NULL ) )  
								{
									 NSFItemSetText(hNote, MWTAGFIELD, MAILWATCHER_MWADVT, (WORD) strlen(MAILWATCHER_MWADVT) );
									 AddInLogMessageText(ERR_FORCE_MWTAG, NOERROR, NOMAPI, TagListEle, note_id, "mail.box");	/* 3.94.40 : display TagListEle */
									 mwtag_ready = 1;
								}
							}

							if ( mwtag_ready == 0 )
							{
								NSFNoteClose(hNote);
								continue;
							}
						}


						/*******************************************/
						/* >>>>>>>> hNote is open     <<<<<<<<<<<< */
	                    /*******************************************/

						/********* First control that can skip all the rest : QUOTA ********/
						wnb                   = 0;
						QuotaEnd              = 0;
						IsNewDocQuota         = 0;
						hCopy                 = NULLHANDLE;
						hNoteQuotaToBeDeleted = NULLHANDLE;

						if ( advquota )
						{
							/* QuotaEnd = 1; if it is not necessary to continue the process */
							if ( advquota_exception == 1 )
							{
								QuotaEnd = IsToSkipMailWatcherQuota (hNote, "FORM", overquota_form_List, overquota_field_values);
							}
							/* QuotaEnd must still be equal to ZERO to continue */
							if ( QuotaEnd == 0 )
							{
								/* QuotaEnd = 1 if the user is to be stopped */
								QuotaEnd = OverQuota(&hNote, &hCopy, &IsNewDocQuota, hMb, advquota_resend);

								/* if OverQuota - a new doc might have been created, this new doc is to be deleted later */
								if ( QuotaEnd == 1 )
								{
									if ( IsNewDocQuota == 1 )
									{
										hNoteQuotaToBeDeleted = hNote;	/* will be closed later */
										hNote = hCopy;
									}
								}

								/* if OverQuota is applied (== 1) then check next condition */
								if ((QuotaEnd == 1) && (advquota_del_attach_file))
								{
									DeleteAttachFile(hNote, "$FILE");
								}
							}
							/* Reset to ZERO : Quota not to be applied */
							else
							{
								QuotaEnd = 0;
							}
						}

						/* Skip all the rest if QuotaEnd = 1 */
						if ( QuotaEnd == 0 )
						{
							/******************** start getting message stats **********************/
							if (advstat)
							{
								mailduplicate = 1;

								/* Avant de commencer à remplir on fait de la place si 	besoin */
								if ( lgstat >= MAXLGSTAT )
									dumpstat(); /* lgstat is reset to ZERO */

								/* 3.94.45 ( NSFItemIsPresent ) 
								 * Collect is done for non splitted email */
								if ( NSFItemIsPresent(hNote, MW_MAILDUPLICATE, (WORD) strlen(MW_MAILDUPLICATE) ) == NULL )
								{
									/* [ 3.94.34 : Use a convert to CD hnote ] */
									hNoteCD = NULLHANDLE;
									sErreur = NSFNoteOpen(hMb, note_id, 0, &hNoteCD);

									if ( sErreur != NOERROR )
										hNoteCD = NULLHANDLE;

									mailduplicate = 0;

									/* >>> stats <<<< */
									OSCurrentTIMEDATE(&current.GM);
									TimeGMToLocal(&current);
									sprintf(statmw[lgstat].Date, "%02d/%02d/%04d", current.day, current.month, current.year);
									/* 3.94.36 : log the second */
									sprintf(statmw[lgstat].Time, "%02d:%02d:%02d", current.hour, current.minute, current.second);
									
									/* Date read from email */
									NSFItemGetTime(hNote, "PostedDate", &current.GM);
									TimeGMToLocal(&current);
									sprintf(statmw[lgstat].PostedDate, "%02d/%02d/%04d", current.day, current.month, current.year);
									/* 3.94.36 : log the second */
									sprintf(statmw[lgstat].PostedTime, "%02d:%02d:%02d", current.hour, current.minute, current.second);

									if (!NSFItemGetText(hNote, "From", qui, sizeof(qui)-1))
										strcpy(qui, "Mail Router");
									
									DNAbbreviate(0L, NULL, qui, quiabr, MAXUSERNAME, NULL);
									strcpy(statmw[lgstat].Sender, qui);

									/* now save the FromValue */
									strcpy(FromValue, qui);
									RemoveNotesDomain(FromValue);
									UpperCase(FromValue);
									DNAbbreviate(0L, NULL, FromValue, FromValue_abbr, MAXUSERNAME, NULL);

									/* RouteServers */
									if (!NSFItemGetTextListEntry(hNote, "RouteServers", 0, qui, sizeof(qui)-1))
										strcpy(qui, szServeur);
									strcpy(statmw[lgstat].HomeServer, qui);

									/* Form field - .3.94.36 over flow 50 is max :  */
									NSFItemGetText(hNote, "Form", qui, 50 -1);
									strcpy(statmw[lgstat].Form, qui);

									/* taille + nb objets [ 3.94.34 : uses hNoteCD instead of hNote ] */
									num = 0;
									pj  = 0;
									if ( hNoteCD != NULLHANDLE )
									{
										num = mw_NSFNoteGetSize(hMb, hNoteCD, &pj);	/* hNoteCD, hNote */
										NSFNoteClose(hNoteCD);
										hNoteCD = NULLHANDLE;
									}

									statmw[lgstat].Taille = (long) num;
									statmw[lgstat].Objets = pj;

									/* Document unique ID (29/11/99) */
									NSFNoteGetInfo(hNote, _NOTE_OID, &MainOID);
									sprintf(DocUniqueID, "%08X%08X%08X%08X",
										MainOID.File.Innards[1],
										MainOID.File.Innards[0],
										MainOID.Note.Innards[1],
										MainOID.Note.Innards[0]);

									strcpy(statmw[lgstat].DocID, DocUniqueID);

									/* ---------------------------------------------------------------------*/
									/* MailWatcher Doc ancestor tracker                                     */
									/** set this ID as the original or leave "0" for an original one       **/
									/* 3.94.32-33 */
									NSFItemGetText(hNote, MW_DOCID_TRACKER, sMW_DocID, sizeof(sMW_DocID)-1);
									if ( strlen(sMW_DocID) == 0 )
										strcpy(sMW_DocID, "0"); /* for the stat this document has not ancestor */
									
									/* save the MW_DOC_TRACKER field into the current note */
									NSFItemSetText(hNote, MW_DOCID_TRACKER, DocUniqueID, (WORD) strlen(DocUniqueID));
									
									/* write MW_DOCID_TRACKER or "0" to the stats struct */
									strcpy(statmw[lgstat].MW_DocID, sMW_DocID);
									/* ---------------------------------------------------------------------*/

									/* Save subject if (activated)                                          */
									MW_Doc_Subject[0]='\0';
		                            if (advstat_subject)
										NSFItemGetText(hNote, "subject", MW_Doc_Subject, sizeof(MW_Doc_Subject)-1);
				                    strcpy(statmw[lgstat].subject, MW_Doc_Subject);

									/** Signed ***/
									NSFItemGetText(hNote, "Sign", qui, sizeof(qui)-1);
									if (qui[0] == '1')
										statmw[lgstat].Sign = TRUE;

									/*** Encrypted ***/
									NSFItemGetText(hNote, "Encrypt", qui, sizeof(qui)-1);
									if (qui[0] == '1')
										statmw[lgstat].Crypt = TRUE;

									NSFItemGetText(hNote, "DeliveryReport", qui, sizeof(qui)-1);
									if ((qui[0] | 0x20) == 'c')
										statmw[lgstat].DeliveryConfirm = TRUE;

									NSFItemGetText(hNote, "ReturnReceipt", qui, sizeof(qui)-1);
									if (qui[0] == '1')
										statmw[lgstat].ReturnReceipt = TRUE;

									NSFItemGetText(hNote, "DeliveryPriority", qui, sizeof(qui)-1);
									if ((qui[0] | 0x20) == 'h')
										statmw[lgstat].DeliveryPriority = 3;
									else if ((qui[0] | 0x20) == 'n')
										statmw[lgstat].DeliveryPriority = 2;
									else if ((qui[0] | 0x20) == 'l')
										statmw[lgstat].DeliveryPriority = 1;
									else
										statmw[lgstat].DeliveryPriority = 0;

									/**********************************************************/
									/* Finishing the Stats - Get the original Recipient list */
									/*********************************************************/
									wnb = NSFItemGetTextListEntries(hNote, "Recipients");
									statmw[lgstat].nbRec = (int) wnb;

									for ( wbcl=0; wbcl<wnb; wbcl++ )
									{
										wlen = NSFItemGetTextListEntry(hNote, "Recipients", wbcl, qui, sizeof(qui)-1);
										if ( wlen < MAXUSERNAME )
											qui[wlen] = '\0';

										DNAbbreviate(0L, NULL, qui, quiabr, MAXUSERNAME, NULL);
										if (strlen(statmw[lgstat].Rec) + strlen(quiabr) + 2 > MAXRECLEN - 3)
										{
											strcat(statmw[lgstat].Rec, "...");
											/* 3.8a b5 size excedeed => break */
											break;
										}
										else
										{
											if (strlen(statmw[lgstat].Rec))
											{
									 			strcat(statmw[lgstat].Rec, ", ");
												strcat(statmw[lgstat].Rec, quiabr);
											}
											else
												strcpy(statmw[lgstat].Rec, quiabr);
										}

									} /* end for */
								} /* end mailduplicate */
							} /* end of stats */
							else
							{
								/* 3.94.37 : get nb rec anyway even when Stats is disable */
								wnb = NSFItemGetTextListEntries(hNote, "Recipients");

								/* Get the Sender */
								if (!NSFItemGetText(hNote, "From", qui, sizeof(qui)-1))
									strcpy(qui, "Mail Router");
								
								/* now save the FromValue */
								strcpy(FromValue, qui);
								RemoveNotesDomain(FromValue);
								UpperCase(FromValue);
								DNAbbreviate(0L, NULL, FromValue, FromValue_abbr, MAXUSERNAME, NULL);
							}

							/*--------------------------------------------------------------------------------------*/
							/* Split recipient groups if found                                                      */
							/* 3.93.13 */
							if ( wnb == 0 )
								CheckRecipientField(hMb, hNote, "SendTo", &wnb, FromValue_abbr);
							else
								CheckRecipientField(hMb, hNote, MAILNOTE_RECIPIENTS_FIELD, &wnb, FromValue_abbr);

							/* Get the right number of recipients */
							if (advstat)
							{
								if (mailduplicate == 0)
								{
									statmw[lgstat].nbRec = (int) wnb;
								}
							}


							/* -------------------------------------------------------------------------------------*/
							/* ---------------------------------- FILTERING AREA -----------------------------------*/
							/***** Filter based on formula - if TRUE => delete *****/
							if (advform)
							{
								if (ProcFormula(hNote))
								{
									NSFNoteClose(hNote);
						 			NSFNoteDelete(hMb, note_id, UPDATE_FORCE);
									statmw[lgstat].sf = TRUE;

									/* skip this record */
									if ( advstat )
									{
										if (mailduplicate == 0)
											lgstat++;
									}

						 			continue;
								}

							}

							/******* Mail Copy ******/
							if (advarch)
								ProcCrochet(hNote, hDbRules);


							/****** Procédure de Black List *****/
							if (advblck)
							{
								/** if Returns 1 => Delete **/
								if (ProcBlackList(&hNote, hMb, statmw[lgstat].DocID, L_rules, StartL, hDbRules ) == 1)
								{
									/* Check the note_id as hNote may have been changed */
									NSFNoteGetInfo(hNote, _NOTE_ID, &note_id);
									NSFNoteClose(hNote);
					 				NSFNoteDelete(hMb, note_id, UPDATE_FORCE);

									/* skip this record */
									if ( advstat )
									{
										if (mailduplicate == 0)
											lgstat++;
									}

									continue;
								}
							}


							/* add 1 on mwnum : Mark as processed */
							MarkNoteNum(hNote, &num);

							/* Fix : 3.61 (28/6/2000) */
							/* because of ScanMail, remove ADVSRV = 1 */
							/* NOT NECESSARY as MailWAtcher Extension Manager DO NOT catch NOTESUPDATE */
							/* inside the MAIL.BOX */

						} /* QuotaEnd */

						/* 3.9 : Read if the MWTAG field new task have been set within the hNote */
						mwtag = 0; /* re-init*/
						mwtag = NSFItemGetTextListEntries(hNote, MWTAGFIELD);

						/* Release the mail if NO=>	NSFItemSetText(hNote, "MW_SKIPRULE", "1", 1); has been SET */
						NSFItemGetText(hNote, "MW_SKIPRULE", MW_SKIPRULE, sizeof(MW_SKIPRULE)-1);

						/* Verbose */
#ifdef VERBOSE
						AddInLogMessageText("BFS : MW_SKIPRULE: %s ", NOERROR, MW_SKIPRULE);
#endif

						if (MW_SKIPRULE[0] == '\0' || MW_SKIPRULE[0] == '2')
						{

							/* Remove now the task "MailWatcher : MWADVT" from the task list */
							if (mwtag > 0)
							{
								RemoveFromMWTAG(hNote,MWTAGFIELD, MAILWATCHER_MWADVT);
								mwtag = mwtag - 1; /* one Tag has been removed ! */
							}

							/* remove tempo fields */
							NSFItemDelete(hNote, "MwTemp",                           6);
							NSFItemDelete(hNote, "RecOK",                            5);
							NSFItemDelete(hNote, "RecNotOk",		                 8);
							NSFItemDelete(hNote, "MW_BLMemo",                        9);
							NSFItemDelete(hNote, "MW_SKIPRULE",                      11);
							NSFItemDelete(hNote, MW_MAILDUPLICATE,            (WORD) strlen(MW_MAILDUPLICATE));
							/* NSFItemDelete(hNote, MW_MACRO_FIELD_NB_RECIPIENTS,(WORD) strlen(MW_MACRO_FIELD_NB_RECIPIENTS));*/ /* 3.94.46  - 3.94.56 : remove the deletion */

							if (QuotaEnd == 0)
								statmw[lgstat].nbServer = (int) num;

							/* On libère le doc pour que le routeur prenne la suite           */
							/* Fix : 3.71a */
							/* NSFItemGetText(hNote, "from", from, sizeof(from)-1);           */
							/* NSFItemSetText(hNote, "Principal", from, (WORD) strlen(from)); */

							/* the mwtag is there to signal that some other mailwatcher task have been scheduled ! */
							/* so do not release the 'RoutingState' + 'Extension manager mark up'	*/
							if (mwtag == 0)
							{
								if (sErreur = NSFItemDelete(hNote, "RoutingState", 12))
								{
									AddInLogMessageText(ERR_DEL_RS, sErreur, NOMAPI, note_id, 1);
									NSFNoteClose(hNote);

									/* skip this record */
									if ( advstat )
									{
										if (mailduplicate == 0)
											lgstat++;
									}
									continue;
								}
							} /* mwtag */
						}
						/* Plan for second pass */
						else
							NSFItemSetText(hNote, "MW_SKIPRULE", "2", 1);

						/* Update and close anyway */
						if (sErreur = NSFNoteUpdate(hNote, UPDATE_FORCE))
							AddInLogMessageText(ERR_DEL_RS, sErreur, NOMAPI, note_id, 2);

						NSFNoteClose(hNote);

						/* a new doc has been created for the quota */
						if ( IsNewDocQuota == 1 )
						{
							sErreur = DeleteNote(hMb, hNoteQuotaToBeDeleted, 0);
							NSFNoteClose(hNoteQuotaToBeDeleted);
						}

						/* On incrémente l'indice des stats */
						if (advstat & (QuotaEnd == 0))
						{
							/* skip this record */
							if (mailduplicate == 0)
								lgstat++;
						}

					} /* if (IDIsPresent(idtable_handle, note_id)) */

				} /* fin balayage des documents à traiter */

				retoldtime[ctr] = retnewtime[ctr];
				OSUnlockObject(idtable);
				OSMemFree(idtable);
				NSFDbClose(hMb);
				AddInSetStatusText(ETAT_WAIT);

			} /* End for one mail.box defined by "filename" **/

		} /* fin interval */


	} /* Fin de la boucle principale */


	/* release memory for Quota Filtering : Exception */
	if ( advquota_exception )
	{
		FreeMemory_GetFieldsList(overquota_form_List);
		FreeMemory_GetFieldsList(overquota_field_values);
	}

	/* releasing Black&Red List memory */
	if (advblck)
	{
		FreeMemory();

		/* 3.94 - rules memory */
		if ( advblck_rm )
		{
			RulesList*		Header;

			Header = *StartL;
			if ( RulesMemoryDumpToFile(Header, RULES_MEMORY_DEFAULT_FILE ) )
					AddInLogMessageText(ERR_DUMPING_RULES_MEMORY, NOERROR, NOMAPI, RULES_MEMORY_DEFAULT_FILE );

			/* destroy the list */
			Header = *StartL;
			DestroyRulesMemoryList( Header );
		}
	}


	/* Phase de fin de l'API */
	if (advstat && lgstat)
		dumpstat();

	AddInSetStatusText(DO_END);
	CloseDbs();
	AddInLogMessageText(API_ENDED, NOERROR, NOMAPI);

	return (NOERROR);
}


/*************************************************************************
 * PROG		: OpenDbs
 * PURPOSE	: Open DBs and Collections used by Mail_Watcher doc options
 * INPUTS : rien
 * OUTPUT : STATUS
 * -- FIX NSFxxxClose().
 ***********************************************************************/
STATUS OpenDbs_bak(void)
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
	NUMBER				num = 0;
	char				temp[50];
	char				*loc;
	int					text_len, i;
	char				serverpath[MAXPATH];
	char				FullPath[MAXPATH];
	char				capath[MAXPATH];



	/* (re)Init */
	coll_ca				= NULLHANDLE;
	coll_bl				= NULLHANDLE;
	coll_bl_formula		= NULLHANDLE;
	coll_cr				= NULLHANDLE;
	coll_bs			    = NULLHANDLE;
	buffer_cahandle		= NULLHANDLE;
	hConfdb				= NULLHANDLE;
	hDb					= NULLHANDLE;


	/* Main Config DB path */
	if (!OSGetEnvironmentString("UNIPOWER_API", capath, sizeof(capath)-1))
	{
		/* new config : new days : 3.94.25 */
		if (!OSGetEnvironmentString("MWATCHER_API", capath, sizeof(capath)-1))
			strcpy(capath, "mw_api.nsf");
	}

	/* convert path separators to OS */
	CheckPath(capath);/* For Unix */

	/* DEBUG MODE - server configuration */
	OSGetEnvironmentString("UNIPOWER_API_SERVER", serverpath, sizeof(serverpath)-1);

	if ( strlen( serverpath) )
	{
		OSPathNetConstruct(NULL, serverpath, capath, FullPath);
		error = NSFDbOpen(FullPath, &hConfdb);
	}
	else
		error = NSFDbOpen(capath, &hConfdb);

	/* std error */
	if ( error )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}

	/* Make sure DB is a NSF */
	if ( !EstVraiBase(hConfdb) )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hConfdb);
		return(error);
	}

	/* Get main Lookup view for MW Modules */
	if ( error = NIFFindDesignNote(hConfdb, "($APIView)", NOTE_CLASS_VIEW, &vca_id) )
	{
		AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hConfdb);
		return(error);
	}


	/* Get main Mail_Watcher core engine configuration document - Collection  */
	if ( error = NIFOpenCollection(hConfdb, hConfdb, vca_id,0, /* 3.93.45 : concurrent access cause pb OPEN_REBUILD_INDEX */
								    NULLHANDLE, &coll_ca, NULL, NULL, NULL, NULL) )
	{
	    AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hConfdb);
		return(error);
	}

	/* Get main Mail_Watcher core engine configuration document - Document  */
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




/***********************************************************************
 * Cette fonction ouvre les bases dont on a besoin pour traiter
 * les messages
 * INPUTS : rien
 * OUTPUT : STATUS
  ***********************************************************************/
STATUS OpenDbs(void)
{
	STATUS				error			= NOERROR;
	NOTEID				viewmb_id;
	DBHANDLE			hConfdb			= NULLHANDLE;
	NOTEHANDLE			hConfNote		= NULLHANDLE;
	HANDLE				buffer_cahandle = NULLHANDLE;
	NOTEID				vca_id;
	HCOLLECTION			coll_ca			= NULLHANDLE;
	COLLECTIONPOSITION	coll_capos;
	DWORD				atraiter		= 0;
	DWORD *				id_calist;
	char				capath[MAXPATH];
	char				serverpath[MAXPATH];
	char				FullPath[MAXPATH];
	NUMBER				num				= 0;
	char				temp[50];
	char *				loc;
	int					text_len		= 0;
	int					i				= 0;

	/* (re)Init - GV */
	coll_bl				= NULLHANDLE;
	coll_bl_formula		= NULLHANDLE;
	coll_cr				= NULLHANDLE;
	coll_bs			    = NULLHANDLE;
	coll_quota			= NULLHANDLE;
	hDb					= NULLHANDLE;


	/* On récupère le document de configuration */
	if (!OSGetEnvironmentString("UNIPOWER_API", capath, sizeof(capath)-1))
	{
		/* new config : new days : 3.94.25 */
		if (!OSGetEnvironmentString("MWATCHER_API", capath, sizeof(capath)-1))
			strcpy(capath, "mw_api.nsf");
	}

	/* Ouverture de la base des config générales */
	CheckPath(capath);/* For Unix */

	/* DEBUG MODE - server configuration */
	OSGetEnvironmentString("UNIPOWER_API_SERVER", serverpath, sizeof(serverpath)-1);

	if ( strlen( serverpath) )
	{
		OSPathNetConstruct(NULL, serverpath, capath, FullPath);
		error = NSFDbOpen(FullPath, &hConfdb);
	}
	else
		error = NSFDbOpen(capath, &hConfdb);

	/* std error */
	if ( error )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}

	if (!EstVraiBase(hConfdb))
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, capath);
		AddInLogMessageText(POLSTOP, error);

		if ( hConfdb != NULLHANDLE )
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

	if ( error = NIFOpenCollection(hConfdb, hConfdb, vca_id, 0 /* OPEN_REBUILD_INDEX */, NULLHANDLE, &coll_ca, NULL, NULL, NULL, NULL))
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

	if ( error = NIFReadEntries(coll_ca, &coll_capos, NAVIGATE_CURRENT, 0, NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID, &buffer_cahandle, NULL, NULL, &atraiter, NULL) )
	{
		if ( buffer_cahandle != NULL )
			OSMemFree(buffer_cahandle);

		NIFCloseCollection(coll_ca);
		NSFDbClose(hConfdb);
		AddInLogMessageText(ERR_READ_CONFIG, error, NOMAPI, "Mail_Watcher", "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}

	if ( (buffer_cahandle == NULLHANDLE) || (atraiter == 0))
	{
		NIFCloseCollection(coll_ca);
		NSFDbClose(hConfdb);
		AddInLogMessageText(ERR_FIND_CONFIG, error, NOMAPI, "Mail_Watcher", "($APIView)", capath);
		AddInLogMessageText(POLSTOP, error);
		return(error);
	}

	id_calist = (NOTEID *)OSLockObject(buffer_cahandle);

	if (error = NSFNoteOpen(hConfdb, id_calist[0], 0, &hConfNote))
	{
		if ( buffer_cahandle != NULLHANDLE )
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



	/* --- defined objects ---------
    hConfDb
	coll_ca
	hConfNote
	buffer_cahandle
	------------------------------*/
	 

	/* On peut (enfin !) récupérer les datas */
	if ( ! NSFItemGetText(hConfNote, "dbpolice", dbsetup, sizeof(dbsetup)-1) )
		strcpy(dbsetup, "mw_setup.nsf");

	NSFItemGetText(hConfNote, "AlertFrom", alertfrom, sizeof(alertfrom)-1);
	NSFItemGetText(hConfNote, "AlertSubject", alertsubj, sizeof(alertsubj)-1);
	NSFItemGetText(hConfNote, "AlertSendTo", alertst, sizeof(alertst)-1);
	NSFItemGetNumber(hConfNote, "pasint", &num);

	/* 3.94.25 : default = the original db  */
	if ( !NSFItemGetText(hConfNote, ADV_RULES_DB, dbrules, sizeof(dbrules)-1))
		strcpy( dbrules, dbsetup);

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


	/** Other arguments for Stats, File, time between document processing ****/
	if (advstat)
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
	}


	/**** Other arguments for : Formula Filtering 'Subject & 'Body *****/
	if (advform)
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

		for (i = 0; i < nb_formula_Messagetxt; i++)
		{
			text_len = NSFItemGetTextListEntry(hConfNote, "AdvFormtext", (WORD) i, ndftextmessage[i], (WORD) MAXTXTMESSAGE-1);
			ndftextmessage[i][text_len] = '\0';
		}
	}


	/**** Other arguments for : Quota Formula Filtering 'Subject & 'Body *****/
	if (advquota)
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

		if (NSFItemGetText(hConfNote, "AdvQuota_RemoveAttach", temp, sizeof(temp)-1))
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

	} /* end of Quota Argument loading */



	/*----------------------------------------------------------------*/
	/**** Other arguments for : BlackListed address notification  *****/

	/* Global	: BlackList_Address_Subject    ->(Subject suffix)
	*			: nb_BlackList_Address_Message ->(nb of lines)
	*			: BlackList_Address_Message    ->(Body message)
	*/
	if (advnotify_blacklist)
	{

		NSFItemGetText(hConfNote, "AdvBL_Notify_Subj", BlackList_Address_Subject,
			                                           sizeof(BlackList_Address_Subject)-1);

		/***** Line of the Text message ******/
		nb_BlackList_Address_Message = NSFItemGetTextListEntries(hConfNote, "AdvBL_Notify_Text");

		for (i = 0; i < nb_BlackList_Address_Message; i++)
		{
			text_len = NSFItemGetTextListEntry(hConfNote,
											   "AdvBL_Notify_Text",
											   (WORD) i,
											   BlackList_Address_Message[i],
											   (WORD) MAXTXTMESSAGE-1);
			BlackList_Address_Message[i][text_len] = '\0';
		}
	}



	/*----------------------------------------------------------------*/

    /** If one of the option is ENABLE... let's do it *****/
	if ( advarch || advblck || advform  || advquota )
	{
		/* quota - other services */
		CheckPath(dbsetup); /** For Unix **/
		CheckPath(dbrules);

		if (error = NSFDbOpen(dbsetup, &hDb))
		{
			NSFNoteClose(hConfNote);		/* => globale */
			
			if ( buffer_cahandle != NULLHANDLE )
			{
				OSUnlockObject(buffer_cahandle);
				OSMemFree(buffer_cahandle);
			}

			NIFCloseCollection(coll_ca);
			NSFDbClose(hConfdb);
			AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, dbsetup);
			return(error);
		}

		if ( !EstVraiBase(hDb) )
		{
			NSFNoteClose(hConfNote);
			
			if ( buffer_cahandle != NULLHANDLE )
			{
				OSUnlockObject(buffer_cahandle);
				OSMemFree(buffer_cahandle);
			}

			NIFCloseCollection(coll_ca);
			NSFDbClose(hConfdb);
			NSFDbClose(hDb);

			AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, dbsetup);
			return(1);
		}


		/* 3.94.25 : split : dbsetup holds the quota data     */
		/* a DB mw_rules is to be read as it is <> of dbsetup */
		if ( strcmp( dbsetup, dbrules) )
		{
			if ( error = NSFDbOpen(dbrules, &hDbRules) )
			{
				NSFNoteClose(hConfNote);		/* => globale */
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);
				NSFDbClose(hDb);			/* mw_setup */
				AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, dbrules);
				return(error);
			}

			/* 3.94.30 : Fix hDb => hDbRules */
			if (!EstVraiBase(hDbRules))
			{
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);
				NSFDbClose(hDb);			/* mw_setup <> mw_rules */
				NSFDbClose(hDbRules);

				AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, dbrules);
				return(1);
			}
		}
		else
			hDbRules = hDb;  /* use the same ! */


	    /*** Read the Black list views (2) ***/
		if ( advblck )
		{

			/* To read in a certain order - set the rules priority
			3.94.25 : hDb is now hDbRules */
			if (error = NIFFindDesignNote(hDbRules, "($BlackList)", NOTE_CLASS_VIEW, &viewmb_id))
			{
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}


				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI,"($BlackList)", dbrules);
				return(error);
			}

			/* Get the HCOLLECTION Handle */
			if ( error = NIFOpenCollection(hDbRules, hDbRules, viewmb_id, 0, NULLHANDLE, &coll_bl, NULL, NULL, NULL, NULL) )
			{
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI,"($BlackList)", dbrules);
				return(error);
			}

			/* From Version 3.61a, From & To & Formule view */
			/* Vue des black list  (From & To & Formule ) */
			if ( error = NIFFindDesignNote(hDbRules, "($BlackList_Formula)", NOTE_CLASS_VIEW, &viewmb_id) )
			{
				NSFNoteClose(hConfNote);

				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NIFCloseCollection(coll_bl); /* rules with priority order */
				NSFDbClose(hConfdb);

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI,"($BlackList_Formula)", dbrules);
				return(error);
			}

			/* Get the HCOLLECTION Handle */
			 if ( error = NIFOpenCollection(hDbRules, hDbRules, viewmb_id, 0, NULLHANDLE, &coll_bl_formula, NULL, NULL, NULL, NULL) )
			{
					NSFNoteClose(hConfNote);
					
					if ( buffer_cahandle != NULLHANDLE )
					{
						OSUnlockObject(buffer_cahandle);
						OSMemFree(buffer_cahandle);
					}

					NIFCloseCollection(coll_ca);
					NIFCloseCollection(coll_bl); /* rules with priority order */
					NSFDbClose(hConfdb);

					/* one or two db . */
					if ( hDbRules == hDb )
						NSFDbClose(hDb);
					else
					{
						NSFDbClose(hDbRules);
						NSFDbClose(hDb);
					}
					AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI,"($BlackList_Formula)", dbrules);
					return(error);
			}
		} /* advblck */


		/*** Read the Mailcopy view *****/
		if ( advarch )
		{
		  /* Vue des Cpt Crochet */
		  if (error = NIFFindDesignNote(hDb, "($Arch)", NOTE_CLASS_VIEW, &viewmb_id))
		  {
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);
				
				if ( advblck )
				{
					if ( coll_bl_formula != NULLHANDLE )
						NIFCloseCollection(coll_bl_formula);

					if ( coll_bl != NULLHANDLE )
						NIFCloseCollection(coll_bl); /* rules with priority order */
				}

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}
				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($Arch)", dbsetup);
				return(error);
		  }

		  if (error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_cr, NULL, NULL, NULL, NULL) )
		  {
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);

				if ( advblck )
				{
					if ( coll_bl_formula != NULLHANDLE )
						NIFCloseCollection(coll_bl_formula);
				
					if ( coll_bl != NULLHANDLE )
						NIFCloseCollection(coll_bl); /* rules with priority order */
				}

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($Arch)", dbsetup);
				return(error);
		  }
		}

		/*** Read the Formula filtering view ***/
		if (advform)
		{
			/* Vue des formules de filtre */
		  if (error = NIFFindDesignNote(hDb, "($BlocSend)", NOTE_CLASS_VIEW, &viewmb_id))
		  {
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);

				if ( advblck )
				{
					if ( coll_bl_formula != NULLHANDLE )
						NIFCloseCollection(coll_bl_formula);
				
					if ( coll_bl != NULLHANDLE )
						NIFCloseCollection(coll_bl); /* rules with priority order */
				}

				if ( advarch )
				{
					if ( coll_cr != NULLHANDLE )
						NIFCloseCollection(coll_cr);
				}

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($BlocSend)", dbsetup);
				return(error);
		  }

			if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_bs, NULL, NULL, NULL, NULL) )
		  {
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);
				
				if ( advblck )
				{
					if ( coll_bl_formula != NULLHANDLE )				
						NIFCloseCollection(coll_bl_formula);

					if ( coll_bl != NULLHANDLE )
						NIFCloseCollection(coll_bl); /* rules with priority order */
				}

				if ( advarch )
				{
					if ( coll_cr != NULLHANDLE )				
						NIFCloseCollection(coll_cr);
				}

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($BlocSend)", dbsetup);
				return(error);
		  }
		}

		/*********** advquota ********************************************/
		if (advquota)
		{
			/* All Person with "ETATPOLICE = "S" in the MW_SETUP.NSF database */
			if (error = NIFFindDesignNote(hDb, "(POLICES)", NOTE_CLASS_VIEW, &viewmb_id))
			{
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);

				/* close all the Other collection */
				if ( advblck )
				{
					if ( coll_bl_formula != NULLHANDLE )
						NIFCloseCollection(coll_bl_formula);

					if ( coll_bl != NULLHANDLE )
						NIFCloseCollection(coll_bl); /* rules with priority order */
				}

				if ( advarch )
				{
					if ( coll_cr != NULLHANDLE )				
						NIFCloseCollection(coll_cr);
				}

				if ( advform )
				{
					if ( coll_bs != NULLHANDLE )
						NIFCloseCollection(coll_bs);
				}
                
				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "(POLICES)", dbsetup);
				return(error);
		    }

			if ( error = NIFOpenCollection(hDb, hDb, viewmb_id, 0, NULLHANDLE, &coll_quota, NULL, NULL, NULL, NULL) )
		    {
				NSFNoteClose(hConfNote);
				
				if ( buffer_cahandle != NULLHANDLE )
				{
					OSUnlockObject(buffer_cahandle);
					OSMemFree(buffer_cahandle);
				}

				NIFCloseCollection(coll_ca);
				NSFDbClose(hConfdb);
				
				if ( advblck )
				{
					if ( coll_bl_formula != NULLHANDLE )				
						NIFCloseCollection(coll_bl_formula);
					
					if ( coll_bl != NULLHANDLE )
						NIFCloseCollection(coll_bl); /* rules with priority order */
				}

				if ( advarch )
				{
					if ( coll_cr != NULLHANDLE )
						NIFCloseCollection(coll_cr);
				}

				if ( advform )
				{
					if ( coll_bs != NULLHANDLE )
						NIFCloseCollection(coll_bs);
				}

				/* one or two db . */
				if ( hDbRules == hDb )
					NSFDbClose(hDb);
				else
				{
					NSFDbClose(hDbRules);
					NSFDbClose(hDb);
				}

				AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "(POLICES)", dbsetup);
				return(error);
		  }

		}/* end of : advquota */

	}/* end of : advquota | adv ... */

    /** Close API Configuration document and database **/
	NSFNoteClose(hConfNote);
	
	if ( buffer_cahandle != NULLHANDLE )
	{
		OSUnlockObject(buffer_cahandle);
		OSMemFree(buffer_cahandle);
	}

	NIFCloseCollection(coll_ca);
	NSFDbClose(hConfdb);
	return(NOERROR);
}



/***********************************************************************
 * Cette fonction ferme les bases
 * INPUTS : rien
 * OUTPUT : STATUS
 ***********************************************************************/
STATUS CloseDbs(void)
{
	STATUS erreur;

	if (advblck)
	{
		NIFCloseCollection(coll_bl_formula);
		NIFCloseCollection(coll_bl); /* rules with priority order */
	}

	if (advarch)
		NIFCloseCollection(coll_cr);

	if (advform)
		NIFCloseCollection(coll_bs);

	if (advquota)
		NIFCloseCollection(coll_quota);

	/* one or two db . */
	if ( hDbRules == hDb )
		erreur  = NSFDbClose(hDb);
	else
	{
		erreur = NSFDbClose(hDbRules);
		erreur = NSFDbClose(hDb);
	}
	return(erreur);
}


/**********************************************************************
 * Cette fct verifie qu'on ne lance pas plusieurs fois la passerelle
 * INPUTS : rien
 * OUTPUT : BOOL        TRUE pour prevInstace
 *                      FALSE pour !prevInstance
 *********************************************************************/
BOOL IsPrevInst(int *demsrv)
{
	STATUS sErreur;
	HANDLE hRetInfos = NULLHANDLE;
	char *pBuffer;
	BOOL ret;
	int i;
	char *loc;

  SECKFMGetUserName(szServeur);

  for (i=0; i<15; i++)
  {
	/* On attend que le serveur soit complétement démarré */
	/* Sleep(3000); */
    sErreur = NSFRemoteConsole(szServeur, "Show Tasks", &hRetInfos);
    if (sErreur == NOERROR)
			break;
  }

  if (sErreur != NOERROR)
  {
    AddInLogMessageText(ERR_REMOTE, sErreur, NOMAPI, szServeur);
    return(TRUE);
  }

  if (hRetInfos != NULLHANDLE)
  {
    pBuffer = OSLock (char, hRetInfos);
		loc = strstr(pBuffer, NOMAPI);
    if (loc)
		{
			AddInLogMessageText(API_STILL_LOADED, NOERROR, NOMAPI, szServeur);
			ret = TRUE;
		}
    else
			ret = FALSE;

    OSUnlock(hRetInfos);
	OSMemFree(hRetInfos);
  }



  return(ret);
}

/************************************************************************
 * Fct EstVraiBase
 * INPUT : DBHANDLE
 * OUPUT : BOOL = TRUE si NSF
 *              = FALSE si non OK
 ************************************************************************/
BOOL EstVraiBase(DBHANDLE hDbTest)
{
	STATUS erreur;
	USHORT dbret;

	if (hDbTest == NULL)
		return(FALSE);

  erreur = NSFDbModeGet(hDbTest, &dbret);
  if (erreur)
    return(FALSE);

  if (dbret == DB_LOADED)
    return(TRUE);
  else
    return(FALSE);
}


/***********************************************************************
 * AlerteAdmin
 * Envoi d'un message à l'Administrateur pour tout pb rencontré
 * INPUT : code err
 * OUPUT : rien
 ***********************************************************************/
void AlerteAdmin(int errcode)
{
	STATUS erreur;
	DBHANDLE hMb;
	NOTEHANDLE hMes;
	TIMEDATE tdCurrent;
	char *errmes[] = {ERR_0, ERR_1, ERR_2, ERR_3, ERR_4};
	char temp[ 3 * MAXUSERNAME ];
	char temp2[ 3 * MAXUSERNAME ];
	char *prec;
	char *loc;
	char filename[10];



	/* multi by default */
	strcpy(filename,"mail1.box");

	if (CheckMonoMailBox())
		strcpy(filename,"mail.box");



	/* Création du mail */
	if (erreur = NSFDbOpen(filename, &hMb))
	{
		AddInLogMessageText(ERR_OPEN_DB, erreur, NOMAPI, filename);
		return;
	}

	/* On vérifie que c'est une vraie base */
	if (!EstVraiBase(hMb))
	{
		NSFDbClose(hMb);
		AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, filename);
		return;
	}

	if (erreur = NSFNoteCreate(hMb, &hMes))
	{
		AddInLogMessageText(ERR_CREATE_DOC, erreur, NOMAPI, filename);
		NSFDbClose(hMb);
		return;
	}

	/* Masque */
	NSFItemSetText(hMes, "Form", "Memo", 4);

	/* emetteur */
	NSFItemSetText(hMes, "From", alertfrom, (WORD) strlen(alertfrom));

	strcpy(temp, alertst);
	do
	{
		loc = strchr(temp, ';');
		if (loc)
		{
			prec = loc+1;
			temp[loc-temp] = '\0';
		}
		supprespace(temp);
		NSFItemAppendTextList(hMes, "SendTo", temp, (WORD) strlen(temp), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", temp, (WORD) strlen(temp), FALSE);
		if (loc)
		{
			strcpy(temp2, prec);
			strcpy(temp, temp2);
		}
	}

	while(loc);

	/* date d'envoi */
	OSCurrentTIMEDATE(&tdCurrent);
	NSFItemSetTime(hMes, "PostedDate", &tdCurrent);

	/* Sujet */
	NSFItemSetText(hMes, "Subject", alertsubj, (WORD) strlen(alertsubj));

	/* Body */
	sprintf(temp, "%s '%s'. %s", ERR_MWADVT, szServeur, errmes[errcode]);
	NSFItemSetText(hMes, "Body", temp, (WORD) strlen(temp));

	/* Champ pour savoir si EM bloque ou pas */
	NSFItemSetText(hMes, "NOT_EM", "1", 1);

	if (erreur = NSFNoteUpdate(hMes, UPDATE_FORCE))
		AddInLogMessageText(ERR_UPDATE, erreur, NOMAPI, "a message for admin");

	NSFNoteClose(hMes);
	NSFDbClose(hMb);

	return;
}



/********************************************************************************
* QuotaFilter.(Router Quota filtering.)
*
* if the Sender is in the list he is STOPPED by the Quota so he can not
* send any mail. Returns the original mail with some info appended at the end
*
* INPUT  : Current message hNote.
* OUTPUT : integer. True => return this mail and skip the rest.

********************************************************************************/
int QuotaFilter(NOTEHANDLE hNote, char * emetteurabr)
{

	STATUS				sErreur;
	COLLECTIONPOSITION	coll_pos;
	DWORD				atraiter;

	/* Force a refresh ! */
	if (sErreur = NIFUpdateCollection(coll_quota))
		return(0);

	/* Look up the the Sender in the view  : "(POLICES)"  : using : [ coll_quota ]   */
	sErreur = NIFFindByName(coll_quota,
							emetteurabr,
							FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE,
							&coll_pos,
							&atraiter);
	if (sErreur)
		return(0);

	/* Return */
	return(atraiter > 0);

}


/***********************************************************************
* Check Over Quota from Sender
* if OverQuota returns 1 all the other operation should be skipped
* as the current e.mail should go thru the mail.box.
* ----------------------------------------------------------------------
* This procedure is called if (advquota == 1)
*
* INPUT		: NOTEHANDLE (hNote)
* OUTPUT	: integer
* ------------------------------------------------------------------------------------------------------
* Revision	: 3.92.4
* 04/09/03	: Support MIME Format
*
* revision	: 3.92.16
*			: Fix in QuotaFilter.
* 03/05/04	: Change the FORM name when sending back the message to the "Stopped" users
*			: DO not change the FROM value but only PRINCIPAL
*			: Remove temporary field for "Resend" actions otherwise the email is not STOPPED by
*			: the extension manager has it looks like being already processed.
* -------------------------------------------------------------------------------------------------------
* revision  : 3.93.4.1
*			: Check if SMTPOriginator is there if so UpdateBody is force to 1.
* revision  : 3.93.9
*			: change subject for non notes clients.
* revision  : 3.93.10
* revision  : 3.93.12 : Calendar get deleting during replication.
* revision	: 3.93.15 : delete when OverQuota is Yes  then restore "DeliveryReport" field when necessary
*			: Make sure that the 'Resend' document are visible in the ($sent) view ().
* revision  : 3.93.18
*			: reset the NOTE_ID (UNID) before returning the OverQuota mail to the Sender.
* revision  : 3.93.19
*			: Stop using DuplicateMail but 'TemporaryhNoteMIME2CD'
* ----------------------------------------------------------------------------------------------------------
* revision  : 3.93.35
*			: Change the DOCID if the Doc has some attachment ! corrupt the message so it can not be
*			: updated and is detroyed !!
*			: fix : if Doc Has Attachment then it must be duplicated instead of forcing a new _NOTE_ID
*			: and the original must be be destroyed before the new one get UPDATED.. wait before
*			: deleting the original note - this action must be done at the very end
*			: --
*			: new argt : NOTEHANDLE* hCopy, int* IsNewDoc.
* revision  : 3.93.37
* revision  : 3.93.45
***********************************************************************************************************/
int OverQuota(NOTEHANDLE* hNote, NOTEHANDLE* hCopy, int* IsNewDoc, DBHANDLE hDB, BOOL advquota_resend)
{
	STATUS			erreur;
	char			emetteur[MAXUSERNAME];
	char			emetteur_overquota[MAXUSERNAME];
	char			OrigPrincipal[MAXUSERNAME];
	char			OrigSubject[2500];
	char			NewSubject[5000];
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	int				OverQuotaApplied = 0;	/* by default : not applied */
	int				i=0;
	NOTEHANDLE		hNewNote;				/* Rich Text body format for appending notification text */
	char			OrigForm[100];
	char			OrigDeliveryReport[100];
	char			OrigKeepPrivate[4];
	int				UpdateBody = 0;
	NUMBER			icone = 0;


	/* Message Sender */
	if (!NSFItemGetText(*hNote, "From", emetteur, sizeof(emetteur)-1))
		strcpy(emetteur, "Mail Router");

	/* 3.93.19 */
	if ( NSFItemGetText(*hNote, "SMTPOriginator", OrigPrincipal, MAXUSERNAME -1) )
		strcpy(emetteur, OrigPrincipal);

	/* 3.93.10 Message Principal */
	NSFItemGetText(*hNote, "Principal", OrigPrincipal, sizeof(OrigPrincipal)-1);
    NSFItemGetText(*hNote, "DeliveryReport", OrigDeliveryReport, sizeof(OrigDeliveryReport)-1);

	/* 3.93.45 : OverQuota. : if principal is set then use it instead of From (emetteur) */
	if ( strlen(OrigPrincipal) )
		strcpy(emetteur_overquota, OrigPrincipal);
	else
		strcpy(emetteur_overquota, emetteur);

	/* 3.93.45 : test with emetteur_overquota */
	if (QuotaFilter(*hNote, emetteur_overquota))
	{
		/* Update body field for SMTP / (not notes clients) */
		if ( NSFItemIsPresent(*hNote, MAILNOTE_INTERNET_FROM_FIELD, (WORD) strlen(MAILNOTE_INTERNET_FROM_FIELD)) )
			UpdateBody = 1;

		/* read the original subject */
		if (!NSFItemGetText(*hNote, "subject", OrigSubject, sizeof(OrigSubject)-1))
		strcpy(OrigSubject, "");

		/* change subject */
		if (! NSFItemIsPresent(*hNote, OVERQUOTA_FORM, (WORD) strlen(OVERQUOTA_FORM)))
		{
			sprintf(NewSubject, "%s : %s", advquotaSubject, OrigSubject);
			NSFItemSetText(*hNote, "Subject", NewSubject, (WORD) strlen(NewSubject));
		}

		/* 3.93.32 */
		icone = ICON_STOPPED;
		NSFItemSetNumber(*hNote, INBOX_ICON, &icone);

		/* 3.92.16 : new Item 'DeliveryReport' */
		NSFItemGetText(*hNote, "Form", OrigForm, sizeof(OrigForm)-1);

		/* 3.93.30 */
		UpperCase(OrigForm);

		/* 3.93.30 : if UpdateBody is still ZERO -> no SMTPOriginator then use the option advquota_resend ( TRUE => updateBody = 0 ) */
		if ( UpdateBody == 0 )
			UpdateBody = ( advquota_resend == TRUE ? 0 : 1 );

		/* will not update the body for the following forms */
		if ( (( strcmp(OrigForm, "NOTICE") == 0) | ( strcmp (OrigForm, "TASK") == 0 )) | ( UpdateBody == 0 ) )
		{
			/* 3.93.9 : multiple try when stopped */
			if (! NSFItemIsPresent(*hNote, OVERQUOTA_FORM, (WORD) strlen(OVERQUOTA_FORM)))
			{
				NSFItemSetText( *hNote, "MailSavedForm",	OrigForm,				(WORD) strlen( OrigForm ));
				NSFItemSetText( *hNote, "FailureReason",	advquotaSubject,		(WORD) strlen( advquotaSubject ));
				NSFItemSetText( *hNote, "OriginalSubject",	OrigSubject,			(WORD) strlen( OrigSubject ));

				/* 3.93.36 */
				if ( NSFItemGetText( *hNote, "$KeepPrivate", OrigKeepPrivate, sizeof(OrigKeepPrivate) - 1 ))
				{
					NSFItemSetText( *hNote, OVERQUOTA_KEEP_PRIVATE,	OrigKeepPrivate,(WORD) strlen( OrigKeepPrivate ));
					NSFItemDelete(  *hNote, "$KeepPrivate",                         (WORD) strlen("$KeepPrivate"));
				}

				/* 3.93.10 quota special field */
				NSFItemSetText( *hNote, OVERQUOTA_FORM,			  OrigForm,		(WORD) strlen( OrigForm ));
				NSFItemSetText( *hNote, OVERQUOTA_PRINCIPAL,	  OrigPrincipal,(WORD) strlen( OrigPrincipal ));
				NSFItemSetText( *hNote, OVERQUOTA_SUBJECT,		  OrigSubject,	(WORD) strlen( OrigSubject ));

				/* 3.93.15 : save and remove option like "Trace Path", "DeliveryReport",... */
				NSFItemSetText( *hNote, OVERQUOTA_DELIVERYREPORT, OrigDeliveryReport,	(WORD) strlen( OrigDeliveryReport ));
				NSFItemDelete( *hNote, "DeliveryReport", (WORD) strlen("DeliveryReport"));

				/* 3.93.37 : save the original recipients (once) */
				NSFItemDuplicateTextList(*hNote, "Recipients", OVERQUOTA_RECIPIENTS, FALSE);

			}
			/* 3.93.32 - set form name */
			NSFItemSetText( *hNote, "Form", AdvQuota_ResendAction_FormName,	(WORD) strlen(AdvQuota_ResendAction_FormName) );
			UpdateBody = 0;
		}


		/* save Recipients item into  [ IntendedRecipients ] */
		/* 3.93.37 : use 'SendTo' rather than 'Recipients' .. do not append CC;BCC in IntendedRecipients */
		if ( UpdateBody == 0 )
			NSFItemDuplicateTextList(*hNote, "SendTo", "IntendedRecipient", FALSE);

		/* Replace all the Recipients => put "emetteur" instead => returns to from */
		/* 3.93.45 : put emetteur_overquota: the email is sent back in db used to send it... */
		NSFItemDelete(*hNote, "Recipients", 10);
		NSFItemSetText(*hNote, "Recipients", emetteur_overquota, (WORD) strlen(emetteur_overquota));

		/* 3.92.16 DO NOT CHANGE THE FROM */
		/* NSFItemSetText(*hNote, "From", OverQuota_From_value, (WORD) strlen(OverQuota_From_value)); */

		/* 3.92.16 : Change only the Principal : use the M20 mail_watcher configuration field : FROM field */
		NSFItemSetText(*hNote, "Principal", OverQuota_From_value, (WORD) strlen(OverQuota_From_value));

		/* Remove temporary field for "resend" */
		NSFItemDelete( *hNote, "MWADVSRV", 8);
		NSFItemDelete( *hNote, "MWTAG", 5);

		/* update body when necessary */
		if ( UpdateBody == 1 )
		{
			/*------------------------- RAW MIME in Body field --------------------------*/
			if ( NSFItemIsPresent(*hNote, "$NoteHasNativeMIME", 18) || ( NSFItemIsPresent(*hNote, "MIME_Version", 12)) )
			{
				/* Update the current hNote then convert safely to MIME using the native conversion made by
				  a "reopening" of the same NOTE_ID  */
				erreur = NSFNoteUpdate(*hNote, UPDATE_FORCE);
				if ( erreur == NOERROR )
				{
					if ( TemporaryhNoteMIME2CD(hDB, *hNote, &hNewNote) )
					{
						/* close hNote then get the new version : hNewNote (RichText body instead of MIME format) */
						NSFNoteClose(*hNote);
						*hNote = hNewNote;
					}
				}
			} /* test if "$NoteHasNativeMIME" is there */


			/* Add Body to another one : API_Conf handle :  Body (Copy item from one document to another ) */
			if (erreur = CompoundTextCreate(*hNote, "Body", &hCompound))
				return(erreur);

			CompoundTextInitStyle(&Style);
			if (erreur = CompoundTextDefineStyle( hCompound, "Normal", &Style, &dwNormalStyle))
			{
				CompoundTextDiscard(hCompound);
				return(erreur);
			}

			/* => add text */
			for (i = 0; i < nb_quota_Messagetxt; i++)
			{

#ifdef V4
				if (erreur = CompoundTextAddParagraph
#else
				if (erreur = CompoundTextAddParagraphExt
#endif
								  (hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  advquotaTextMessage[i],
                                  (DWORD) strlen(advquotaTextMessage[i]),
                                  NULLHANDLE))
				{
					CompoundTextDiscard(hCompound);
					return(erreur);
				}
			}

			if (erreur = CompoundTextClose (hCompound, 0, 0L, NULL, 0))
				CompoundTextDiscard(hCompound);
		} /* UpdateBody is true */
		else /* update the 'FailureReason' field  : if does not exists yet (3.93.9) */
		{
			if ( (!NSFItemIsPresent(*hNote, "FailureReason", (WORD) strlen("FailureReason"))) && ( UpdateBody == 1)  )
			{
				/* => add text to the field */
				for (i = 0; i < nb_quota_Messagetxt; i++)
				{
					NSFItemAppendTextList( *hNote, "FailureReason", advquotaTextMessage[i], (WORD) strlen(advquotaTextMessage[i]), FALSE );
				}
			}
		}

		/* 3.93.12, .17.18 : reset some fields to unlink this note - with the Calendar or Task entry */
		if ( UpdateBody == 0 )
		{
			/* 3.93.35 */
			if ( DuplicateMail(&hDB, *hNote, hCopy, 1) == NOERROR )
				*IsNewDoc = 1;

				/* -------------------------------------------------------------
					   TO BE DONE WHEN NO MORE UPDATE ARE TO BE DONE ON THE COPY
					  THERE IS AN ISSUE WITH ATTACHMENTS -> DeleteNote(hDB, *hNote, 0);
				----------------------------------------------------------------- */
		}

		/* Mark as done */
		OverQuotaApplied = 1;
	}
	/* 3.93.10 : OverQuota  : check out when resend field have been used... */
	else
	{
		/* Restore fields if exists */
		if ( NSFItemIsPresent(*hNote, OVERQUOTA_FORM, (WORD) strlen(OVERQUOTA_FORM) ))
		{
			/* 3.93.32 remove Icon */
			NSFItemDelete(*hNote, INBOX_ICON, (WORD) strlen(INBOX_ICON) );

			/* 3.93.36 */
			NSFItemDelete( *hNote, "MailSavedForm",	 (WORD) strlen("MailSavedForm"));
			NSFItemDelete( *hNote, "FailureReason",	 (WORD) strlen("FailureReason"));
			NSFItemDelete( *hNote, "OriginalSubject",(WORD) strlen("OriginalSubject"));

			/* 3.93.37 : OverWrite Recipients fields if SendTo = Original SendTo */
			if ( NSFItemIsPresent(*hNote, OVERQUOTA_RECIPIENTS, (WORD) strlen(OVERQUOTA_RECIPIENTS)) )
				NSFItemDuplicateTextList(*hNote, OVERQUOTA_RECIPIENTS, "Recipients", FALSE);
			NSFItemDelete(*hNote, OVERQUOTA_RECIPIENTS, (WORD) strlen(OVERQUOTA_RECIPIENTS));

			/* read/write : form */
			if ( NSFItemGetText(*hNote, OVERQUOTA_FORM, OrigForm, sizeof(OrigForm)-1) )
				NSFItemSetText(*hNote, "Form", OrigForm, (WORD) strlen(OrigForm));
			NSFItemDelete(*hNote, OVERQUOTA_FORM,    (WORD) strlen(OVERQUOTA_FORM));

			/* read/write : Principal */
			if ( NSFItemGetText(*hNote, OVERQUOTA_PRINCIPAL, OrigPrincipal, sizeof(OrigPrincipal)-1) )
				NSFItemSetText(*hNote, "Principal", OrigPrincipal, (WORD) strlen(OrigPrincipal));
			NSFItemDelete(*hNote,  OVERQUOTA_PRINCIPAL,        (WORD) strlen(OVERQUOTA_PRINCIPAL));

			/* read/write : Subject */
			if ( NSFItemGetText(*hNote, OVERQUOTA_SUBJECT, OrigSubject, sizeof(OrigSubject)-1) )
				NSFItemSetText(*hNote, "subject", OrigSubject, (WORD) strlen(OrigSubject));
			NSFItemDelete(*hNote,  OVERQUOTA_SUBJECT,      (WORD) strlen(OVERQUOTA_SUBJECT));

			/* read/write : DeliveryReport */
			if ( NSFItemGetText(*hNote, OVERQUOTA_DELIVERYREPORT, OrigDeliveryReport, sizeof(OrigDeliveryReport)-1) )
				NSFItemSetText(*hNote, "DeliveryReport", OrigDeliveryReport, (WORD) strlen(OrigDeliveryReport));
			NSFItemDelete(*hNote,  OVERQUOTA_DELIVERYREPORT, (WORD) strlen(OVERQUOTA_DELIVERYREPORT));

			/* 3.93.36 : read/write : KeepPrivate */
			if ( NSFItemGetText(*hNote, OVERQUOTA_KEEP_PRIVATE, OrigKeepPrivate, sizeof(OrigKeepPrivate)-1) )
				NSFItemSetText(*hNote, "$KeepPrivate", OrigKeepPrivate, (WORD) strlen(OrigKeepPrivate));
			NSFItemDelete(*hNote,  OVERQUOTA_KEEP_PRIVATE, (WORD) strlen(OVERQUOTA_KEEP_PRIVATE));

		}
	}

	/* return */
	return(OverQuotaApplied);
}

/************************************************************************
 * ProcFormula	- Gestion des formules de control d'envoi
 *	Inputs : NOTEHANDLE = message
 *	Output : int = 0 si OK, 1 si message bloqué
 ************************************************************************/
int ProcFormula(NOTEHANDLE hNote)
{
 	STATUS sErreur;
	char emetteur[MAXUSERNAME];
	char emetteurabr[MAXUSERNAME];
	COLLECTIONPOSITION coll_pos;
	DWORD atraiter;
	HANDLE buffer_fhandle = NULLHANDLE;
	DWORD *id_flist;
	NOTEHANDLE hNoteFiltre;
	char formule[300];
	char origsubj[2000];
	int text_len, i;



	/* Qui emet le message ? */
	if (!NSFItemGetText(hNote, "From", emetteur, sizeof(emetteur)-1))
		strcpy(emetteur, "Mail Router");
	DNAbbreviate(0L, NULL, emetteur, emetteurabr, MAXUSERNAME, NULL);

	sErreur = NIFFindByName(coll_bs, emetteurabr,
										     FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE,
												 &coll_pos, &atraiter);
	if (sErreur)
		return(0);

	/* On prend la formule */
	if (sErreur = NIFReadEntries(coll_bs, &coll_pos, NAVIGATE_CURRENT, 0,
															NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID,
															&buffer_fhandle, NULL, NULL, &atraiter, NULL))
	{
		if (buffer_fhandle != NULL)
			OSMemFree(buffer_fhandle);
		AddInLogMessageText(ERR_READ_VIEW, sErreur, NOMAPI, "($BlocSend)", dbsetup);
		return(0);
	}

	if ((buffer_fhandle != NULLHANDLE) && (atraiter > 0))
	{
		id_flist = (NOTEID *)OSLockObject(buffer_fhandle);
		if (sErreur = NSFNoteOpen(hDb, id_flist[0], 0, &hNoteFiltre))
		{
			OSUnlockObject(buffer_fhandle);
			OSMemFree(buffer_fhandle);
			AddInLogMessageText(ERR_OPEN_FILTER_DOC, sErreur, NOMAPI, id_flist[0], dbsetup);
			return(0);
		}

		NSFItemGetText(hNoteFiltre, "Formule", formule, sizeof(formule)-1);


		/* Create the Array of Formula in Text */
		nb_formula_txt = NSFItemGetTextListEntries(hNoteFiltre, "formule_txt");

		for (i = 0; i < nb_formula_txt; i++)
		{

			text_len = NSFItemGetTextListEntry(hNoteFiltre, "formule_txt", (WORD) i, Formula_Txt[i], (WORD) MAXPATH-1);
			Formula_Txt[i][text_len] = '\0';
		}


		NSFNoteClose(hNoteFiltre);

		OSUnlockObject(buffer_fhandle);
		OSMemFree(buffer_fhandle);
	}


	/* Le message satisfait-il à la formule ? */
	if (!EvalFormula(hNote, formule, emetteur))
		return(0);

	/* On alerte le sender qu'il s'est fait gauler..... */
	NSFItemGetText(hNote, "Subject", origsubj, sizeof(origsubj)-1);
	NonDeliveryFilter(emetteur, formule, origsubj);
	return(1);
}


/************************************************************************
 * ProcBlackList	- Gestion des black list
 * Modifs
 * *******************
 * 12/11/99	LP
 * Black / Red, Filter on both address using '* and ?
 * Actions : Stop, Forward, append message, return to sender.
 * *******************
 * 15/11/99	LP
 * Add boolean to append or not the message text to the e.mail
 * ***************************
 * 3/12/99	LP
 * fix, the router as append in the From field only if the mail is returned
 * to sender.
 * ***************************
 * 21/12/99
 * Add : Route thru parameters
 * ****************************
 * 4/04/2000
 * Fix : use safe_recipient to keep the recipient with its orginal format
 * pb of removing the @domaine all the time => can cause some bad effect
 * when routing the message. safe_recipient is replace by recipient & RouteThru
 * when RouteThru is TRUE
 * ********************************
 * 01/06/2000
 * Add boolean to append a field "MW_RAPP" to the e.mail before being copied to another database
 * ----------------------------------------------------------------------------------------------
 * TO DO
 * -----
 * Read the delivery failure parameters
 * Core message + subject header.
 *
 *****************************************************************************************************/
int ProcBlackList(NOTEHANDLE *hNote, DBHANDLE hDB, char* mail_ID, RulesList* L_rules, RulesList** StartL, DBHANDLE hDbRules)
{
	STATUS sErreur, RouteThruBool, MW_RAPP_Bool, MW_RAPP_AGAIN_Bool;
	char OrigSubject[2500];
	char BList_DF_Header[100];
	char emetteur[MAXUSERNAME];
	char emetteurabr[MAXUSERNAME];
	char recipient[MAXUSERNAME];
	char safe_recipient[MAXUSERNAME];
	char recipientabr[MAXUSERNAME];
	char blacklister[MAXUSERNAME];
	char RouteThru[MAXUSERNAME];
	char MailCopy[MAXPATH];
	char bl_choice[4];
	char MW_RAPP_Flag[4];
	COLLECTIONPOSITION coll_pos;
	DWORD atraiter;
	WORD nbrec, i;
	char clef[(2*MAXUSERNAME)+MAX_BL_FORMULA+DOCID+5];
	HANDLE buffer_fhandle = NULLHANDLE;
	DWORD *id_flist;
	NOTEHANDLE hNoteFiltre;
	int nbbl = 0;
	int	ReturnVal = 0;
	int BL_Rules = 0;
	double BL_RulesInit = 0;
	int GetMatchKeyFlag = 0;
	int save_strcat_blref = 0;
	char TmpBLName[9];					/* Max code size is 10 !!! */
	char BListName[1024];				/* use to strcat all the rules applied to the e.mail  (Max is 1024/10 => 100 !) */
	char RefRules[10];					/* format : 9 X "9" + "," */
    char BListMemo[MAXMEMORULESIZE];	/* Dimension fixe..Attention !!! */
    int  ApplyAction = 0;
	int  NbRulesApply = 0;
	char MW_SKIPRULE[3];
	char sendToBis[MAXUSERNAME];
	char NewEmailFromAddress[MAXUSERNAME];
	int	 AfterAllOnly = 0;				/* 3.9 : Skip the common rules => onky After All are skipped */

	/* 3.91 */
	int			DocLink = 0;
	char		ViewName[100];
	TIMEDATE	replica_ID;
	UNID		Note_UNID;
	UNID		View_UNID;
	char		NewMemo_Fwd[MAXUSERNAME];
	int			bRecOK = 0;				/* Add to => RecOK if True / RecNotOK if not */
	int			bRecNotOk = 0;			/* Flag when Recipients have been blacklisted */
	NOTEHANDLE	hNoteSafe;				/* save the original mail handle */
	NOTEHANDLE	hNewNote;				/* current original email splitting handle */
	NOTEHANDLE	hNewNoteCD;				/* Original mail open with MIME to CD conversion */
	int			bNewNote = 0;			/* Boolean, flag email spliting  */
	int			bNewNoteCD = 0;			/* Boolean, flag original mail conversion MIME to CD */
	char		Original_Body[4];
	char		Original_Body_Attach[4];
	/* 3.92*/
	char		FolderName[MAXUSERNAME];
	char		MailDuplicate[4];
	/* 3.93 */
	NOTEHANDLE	hNote4Rules;
	char		SMTPOriginator[MAXUSERNAME];
	char		BList_RemoveAttach_Extension[MAX_BL_FORMULA];
	char		DetachHostLink[MAXPATH];
	BOOL		RemoveReturnReceipt = TRUE;
	/* 3.94 */
	char		RulesMemoryRef[MAXMEMORYRULESREF];
	char		BList_RemoveAttach_FiedList[MAXUSERNAME];
	STATUS		error = NOERROR;
	char		Original_MessageID[MAXUSERNAME];
	/* 3.95 */
	char*		PredicatValue = NULL;
	int			MatchRulesRank = 0;


	/* Verbose : Display Doc ID Current Message */
    /*	AddInLogMessageText("DOC ID:%s ", NOERROR, statmw[lgstat].DocID );*/

	/* Init */
	FolderName[0]        ='\0';
	BListName[0]         ='\0';
	AfterAllOnly         = 0;
	Original_MessageID[0]='\0';


	/* init the string that stored the rules applied to "original messages" */
	/* if MW_BLMemo does not exists => BListMemo[0]='\0' */
	/* Keep this value for the statistics */
	NSFItemGetText(*hNote, "MW_BLMemo", BListMemo, (WORD) sizeof(BListMemo)-1);

	/* SMTPOriginator field */
	NSFItemGetText(*hNote, MAILNOTE_INTERNET_FROM_FIELD, SMTPOriginator, sizeof(SMTPOriginator)-1);

	/* 3.94.58 : $MessageID */
	NSFItemGetText(*hNote, "$MESSAGEID", Original_MessageID, sizeof(Original_MessageID)-1);


	/* FROM field */
	if (!NSFItemGetText(*hNote, "From", emetteur, sizeof(emetteur)-1))
		strcpy(emetteur, "Mail Router");

	/* Choose the Sender email address format */
	if (strlen(SMTPOriginator))
		DNAbbreviate(0L, NULL, SMTPOriginator, emetteurabr, MAXUSERNAME, NULL);
	else
		DNAbbreviate(0L, NULL, emetteur, emetteurabr, MAXUSERNAME, NULL);

	/* Original Subject */
	if (!NSFItemGetText(*hNote, "Subject", OrigSubject, sizeof(OrigSubject)-1))
		OrigSubject[0]='\0';

	/* (Patch : 30/5/2000) : Purge RecOK if exists */
	NSFItemDelete(*hNote, "RecOK", 5);

	/* balayage des destinataires */
	nbrec = NSFItemGetTextListEntries(*hNote, "Recipients");

	/* 3.93.9 case of recipients field does not exists; this is to force to get into the loop : recipients = "" */
	if ( nbrec == 0 )
		nbrec = 1;

	/* Read from hnote and init to ZERO if none */
	NbRulesApply = GetNbRulesApplied(BListMemo, ",",1); /* count the number of rules applied to a single message */

	/* Read if MW_SKIPRULE is there...*/
	NSFItemGetText(*hNote, "MW_SKIPRULE",MW_SKIPRULE, sizeof(MW_SKIPRULE)-1);

	/* Check if this email is an original or a copy (splitted message) */
	if ( ! NSFItemGetText(*hNote, MW_MAILDUPLICATE, MailDuplicate, sizeof(MailDuplicate)-1) )
		MailDuplicate[0]='\0';


	/* ---------------new in 3.93 -/ fix : 3.94.41 / -----------*/
	/* Convert the hNote to CD. */
	if ( RulesWithMacro )
	{
		bNewNoteCD = TemporaryhNoteMIME2CD(hDB, *hNote, &hNewNoteCD);

		/* 3.94.41 : need to transfert the Recipients up to date */
		if ( bNewNoteCD )
		{
			/* log if could not tranfert Recipients field to Temporary hNote for macros formula */
			if ( error = MoveFieldFromNote2Note(*hNote, hNewNoteCD, "Recipients" ) )
				AddInLogMessageText(ERR_COPY_FIELD_TO_NOTE, error, NOMAPI, "Recipients");

			/* : 3.94.45 : Original Nb of Recipients */
			if ( error = MoveFieldFromNote2Note(*hNote, hNewNoteCD, MW_MACRO_FIELD_NB_RECIPIENTS ) )
				AddInLogMessageText(ERR_COPY_FIELD_TO_NOTE, error, NOMAPI, MW_MACRO_FIELD_NB_RECIPIENTS);
		}
	}
	/* ------ end of new in 3.93  ------------*/


	/*** Loop over Recipient list ****/
	for (i=0; i<nbrec; i++)
	{

		bRecOK = 0;		/* 3.94.19 : recipient is not blacklisted by default */
		bNewNote = 0;	/* Work on the original email : hNote */

		NSFItemGetTextListEntry(*hNote, "Recipients", i, recipient, sizeof(recipient)-1);

		/* Keep a safe copy of the recipient.. with all the route : @domaineA@domaineB */
		strcpy(safe_recipient, recipient);

		RemoveNotesDomain(recipient);
		DNAbbreviate(0L, NULL, recipient, recipientabr, MAXUSERNAME, NULL);

		/* Read from the hnote or init BL_Rules */
		BL_Rules = 0;
		MatchRulesRank = 0;

		/* Reinit if no skip */
		if (MW_SKIPRULE[0]=='2')
		{
			BL_Rules = GetNbRulesApplied(BListMemo, ",",0); /* get the last rules number in BLmemo */
			AfterAllOnly = 1;
			/* verbose */
#ifdef VERBOSE
			AddInLogMessageText("Rules Reinit : %d for %s ", NOERROR, BL_Rules, recipientabr);
#endif
		}

		GetMatchKeyFlag = 0;		/* To set RECOK correctly */
		hNoteSafe = *hNote;			/* save the original mail handle */

		UpperCase(emetteurabr);
		UpperCase(recipientabr);



		/* ------------------------------------------------ TEST -----------------------------------------------------------------
		if ( ReadAllUserPredicats(emetteurabr, "DOCUMENTACCESS", SpecialUserListGlobal, &PredicatValue, MAXUSERNAME, 20) )
		{
			printf("%s:%s\n", "COMPANYNAME", PredicatValue);
			MakeTextListFieldStringWithSep(*hNote, "TEST_CompanyName", PredicatValue, PREDICAT_ITEM_SEP_CHAR);
			NSFNoteUpdate(*hNote, UPDATE_FORCE);

			if ( PredicatValue != NULL )
				free(PredicatValue);
			PredicatValue = NULL;
		}
		------------------------------------------------------------- end of TEST -----------------------------------------------*/

		/*-------------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------- for one couple (From, Recipients(i) ) --------------------------------------------------------*/

		/* Continue until BL_Rules<MaxRules */
		while (BL_Rules < MaxRules)
		{
			/* Skip until AFTERALL type of rules */
			if ( (AfterAllOnly) && (BlackRedList[BL_Rules].AfterAllRule == 0) )
			{
				BL_Rules++;
				continue;
			}

			/* Give the right hNote to calculate the rules */
			if ( bNewNoteCD == 1 )
				hNote4Rules = hNewNoteCD;
			else
				hNote4Rules = *hNote;

			/* Side effect : BL_Rules (++) */
			if (GetMatchKey(hNote4Rules, clef, emetteurabr, recipientabr, &BL_Rules) == TRUE)
			{

				/* Verbose */
#ifdef VERBOSE
				AddInLogMessageText("r : %s => %d", NOERROR, recipientabr, BL_Rules );
#endif

				if (Except_Match(emetteurabr, recipientabr, BlackRedList[BL_Rules -1]) == 0)
				{

					sErreur = NIFFindByName(coll_bl_formula, clef, FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE,
							    &coll_pos, &atraiter);

					if (sErreur)
					{
		 				NSFItemAppendTextList(*hNote, "RecOK", recipient, (WORD) strlen(recipient), FALSE);
						AddInLogMessageText(ERR_READ_VIEW, sErreur, NOMAPI, "FindByName : key not found", dbrules);

						/* Jump to While start... (+1 RulesCtr) */
						continue;
					}

					/* On prend le blacklister */
					buffer_fhandle = NULLHANDLE;
					if (sErreur = NIFReadEntries(coll_bl_formula, &coll_pos, NAVIGATE_CURRENT, 0,
										NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID,
										&buffer_fhandle, NULL, NULL, &atraiter, NULL))
					{
		 				NSFItemAppendTextList(*hNote, "RecOK", recipient, (WORD) strlen(recipient), FALSE);
						AddInLogMessageText(ERR_READ_VIEW, sErreur, NOMAPI, "($BlackList_Formula)", dbrules);

						if (buffer_fhandle != NULLHANDLE)
							OSMemFree(buffer_fhandle);

						/* Jump to While start... (+1 RulesCtr) */
						continue;
					}


					/* Document containing the rules is there !! */
					/* at least one filter rule has been found    */

					if ((buffer_fhandle != NULLHANDLE) && (atraiter > 0))
					{
						id_flist = (NOTEID *)OSLockObject(buffer_fhandle);
						if (sErreur = NSFNoteOpen(hDbRules, id_flist[0], 0, &hNoteFiltre))
						{
								/* 3.93.14 : release buffer not NULL /free memory /do not close hNoteFiltre */
								OSUnlockObject(buffer_fhandle);
								OSMemFree(buffer_fhandle);

								NSFItemAppendTextList(*hNote, "RecOK", recipient, (WORD) strlen(recipient), FALSE);
								AddInLogMessageText(ERR_OPEN_FILTER_DOC, sErreur, NOMAPI, id_flist[0], dbrules);

								/* Jump to While start... (+1 RulesCtr) */
								continue;
						}

						/* - Save Rules # - */
						MatchRulesRank = BL_Rules;

						/*=================================[ Actions ]=========================================*/
						/*    Special rules : should be skipped for now if the message is not ready yet        */
						/*    Check the type of rules                                                          */

						/* -3.91- Expired or not started */
						if (BlackRedList[BL_Rules -1].AfterAllRule > 0)
						{

							if (MW_SKIPRULE[0]=='\0' || MW_SKIPRULE[0]=='1')
							{
								/* Verbose */
#ifdef VERBOSE
								AddInLogMessageText("Skiping :( %d) %s > %s ", NOERROR, BL_Rules,emetteurabr, recipientabr );
#endif

								/* Mark up the document with a flag that will keep it HOLD or DEAD for more turn */
								strcpy(MW_SKIPRULE,"1");
								NSFItemSetText(*hNote, "MW_SKIPRULE", "1", 1);
								NSFItemSetText(*hNote, "MW_BLMemo", BListMemo, (WORD) strlen(BListMemo));

								/* Fix : 29/3/04 : Close the hNoteFiltre */
								NSFNoteClose(hNoteFiltre);

								/* Free up the NifReadEntries returned buffer */
								OSUnlockObject(buffer_fhandle);

								/* Release the View buffer returned by NifReadEntries */
								OSMemFree(buffer_fhandle);

								/* Jump to While start... (+1 RulesCtr) */
								continue;
							}
							/* Verbose */
#ifdef VERBOSE
							AddInLogMessageText("Acting :( %d) %s > %s ", NOERROR, BL_Rules,emetteurabr, recipientabr );
#endif
						}

		/*===================[ Make sure that the selected filter document is not Expired or not Started ]======*/
						if (CheckBlackListRulesPeriod(hNoteFiltre))
						{
							/* Fix : 29/3/04 : Close the hNoteFiltre */
							NSFNoteClose(hNoteFiltre);

							/* Free up the NifReadEntries returned buffer */
							OSUnlockObject(buffer_fhandle);

							/* Release the View buffer returned by NifReadEntries */
							OSMemFree(buffer_fhandle);

							/* Jump to While start... (+1 RulesCtr) */
							continue;
						}

						/*=========================[ TEST IF THE RULES # has been applied yet or not ]==========*/

						/* Save the ref as the rule # */
						sprintf(RefRules, "%d,",BL_Rules);
						ApplyAction = 0;


						/* 3.94 - Check memory rules only one time per message ! */
						if (!strstr(BListMemo, RefRules))
						{
							/* ======================= [ Rules Memory condition - if set it must be evaluated now ] ====*/
							if ( CheckBlackListRulesMemoryValidation(hNoteFiltre, L_rules, RulesMemoryRef ) )
							{
								/* Even the rules is skipped it counts ++, still do not count if we are not over a Duplicated email  --*/
								if ( !(MailDuplicate[0] == '1') )
								{
									NSFItemGetText(hNoteFiltre, "BLCode", TmpBLName, sizeof(TmpBLName)-1);
									RulesMemoryCount( RulesMemoryRef, TmpBLName, L_rules, StartL );
								}

								/* Fix : 29/3/04 : Close the hNoteFiltre */
								NSFNoteClose(hNoteFiltre);

								/* Free up the NifReadEntries returned buffer */
								OSUnlockObject(buffer_fhandle);

								/* Release the View buffer returned by NifReadEntries */
								OSMemFree(buffer_fhandle);

								/* Jump to While start... (+1 RulesCtr) */
								continue;
							}
						}


						/*=========================[ Real actions are taking place now ]====================*/

						GetMatchKeyFlag = 1;
						NbRulesApply++;

						/*==================================================================================*/
						/* === [          Physical change : need to duplicate the mail  ]              ==== */

						/*----------------------------------------------------------------------------------*/
						/*============[  FROM HERE THE ORIGINAL EMAIL CAN GET CHANGED !!!!! ]===============*/
						/* The rule definition contains a flags : if set to 1, we need to work out on
						/* a new "memory" copy of the current hnote (the original email)                    */


							/* 3.93 *************************************************/
							/* Detach Attached Files / DO NOT REMOVE :)             */
							/********************************************************/

							/* Read rule argt */
							if (NSFItemGetText(hNoteFiltre, BLACKLIST_DETACHFILES_YN, bl_choice, sizeof(bl_choice)-1) )
							{
								if (strcmp(bl_choice, "1") == 0)
								{
									/* get file path */
									if (!NSFItemGetText(hNoteFiltre, BLACKLIST_DETACHFILE_PATH, MailCopy, sizeof(MailCopy)-1) )
										MailCopy[0]='\0';

									/* get specified extensions ? */
									if (! NSFItemGetText(hNoteFiltre, BLACKLIST_DETACHFILE_EXTENSION, BList_RemoveAttach_Extension, sizeof(BList_RemoveAttach_Extension)-1) )
										BList_RemoveAttach_Extension[0]='\0';

									/* add a link to the attachments in the current hnote ? */
									if (! NSFItemGetText(hNoteFiltre, BLACKLIST_DETACHFILE_LINK, DetachHostLink, sizeof(DetachHostLink)-1) )
										DetachHostLink[0]='\0';

									SaveOnDiskAttachFileWithException(hNote, "$FILE", BList_RemoveAttach_Extension, MailCopy, DetachHostLink, &bNewNote, hDB, nbrec, i, mail_ID);
								}
							} /* end of : Detach File */



							/* 3.91 ************************************************/
							/*         Delete Fields                               */
							/*         Work if field is not empty                  */
							/*-----------------------------------------------------*/
							if (NSFItemGetText(hNoteFiltre, "BList_RemoveField", bl_choice, sizeof(bl_choice)-1))
							{
								if (strlen(bl_choice))
								{
									if ( (( nbrec - (i - 1)) >= 1) && (bNewNote == 0))
									{
										/* Duplicate the current Note */
										if (sErreur = DuplicateMail(&hDB, *hNote, &hNewNote, 0))
										{
											AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, "mail(x).box");
										}
										else
										{
											/* NOERROR => Save and set the flag */
											bNewNote = 1;
											*hNote    = hNewNote;
											DeleteFieldInNote(hNoteFiltre, *hNote, "BList_RemoveField", TmpBLName);
										}
									}
									/* For [From, Recipient(i)], the email has been already duplicated */
									else
										DeleteFieldInNote(hNoteFiltre, *hNote, "BList_RemoveField", TmpBLName);
								}
							}

							/* 3.91 *************************************************/
							/*         Change/get current email values              */
							/*         Set Importance to "Low" value == "3"         */
							/*------------------------------------------------------*/
							if (NSFItemGetText(hNoteFiltre, "BList_LowImportance", bl_choice, sizeof(bl_choice)-1))
							{
								if (strcmp(bl_choice, "1") == 0)
								{
										NSFItemSetText(*hNote, "Importance", "3", 1);
										NSFItemSetText(*hNote, "DeliveryPriority", MAIL_LOW_PRIORITY, (WORD) strlen(MAIL_LOW_PRIORITY));
								}
							}


						/* =[ 12/05/03 ]= fix : This Rules terminates is to be applied for every recipients even if it has
						been already applied once == */

						/* Check if the rules is the LAST that will be checked !!! */
						if (NSFItemGetText(hNoteFiltre, "BList_MW_LAST_Flag", MW_RAPP_Flag, sizeof(MW_RAPP_Flag)-1))
							BL_Rules = MaxRules; /* So force the end of the while loop */



						/* ===============================================================================*/
						/* ============ Skip - Action applied only one time per message ==================*/
						if (!strstr(BListMemo, RefRules))
						{

							ApplyAction = 1;

							/* Save the current rules # in the rules list */
							if (NbRulesApply < MAXMEMORULECTR)
							  strcat(BListMemo, RefRules);

							/* Verbose */
#ifdef VERBOSE
							AddInLogMessageText(BListMemo, NOERROR, NOMAPI);
#endif

							/* Get the Black List name if any */
							strcpy(TmpBLName,"No-Ref");	/* init */
							if(NSFItemGetText(hNoteFiltre, "BLCode", TmpBLName, sizeof(TmpBLName)-1))
							{
								save_strcat_blref = 1;
								if (nbbl < 100)
								{
									strcat(BListName, TmpBLName);
									strcat(BListName, ",");
								}
							}

							/* Make sure that we are not over a Duplicated email */
							if ( !(MailDuplicate[0] == '1') )
							{
								/* 3.94. increment counter for rules memory */
								RulesMemoryCount( RulesMemoryRef, TmpBLName, L_rules, StartL );

								/* Mail Copy to a none MailInDatabase */
								/* 3.95 : MailCopyBool = NSFItemGetText(hNoteFiltre, "MailCopy", MailCopy, sizeof(MailCopy)-1); */
								GetFilterRulesParameterOrPredicats(hNoteFiltre, "MailCopy", MailCopy, sizeof(MailCopy), emetteurabr, recipientabr, FirstEle);

								if ( strlen(MailCopy) > 0 )
								{
									/* 3.92 : Read FolderName if any */
									/* 3.95 : old version with no predicats : NSFItemGetText(hNoteFiltre, BLIST_MAILCOPY_FOLDERNAME, FolderName, sizeof(FolderName)-1); */
									GetFilterRulesParameterOrPredicats(hNoteFiltre, BLIST_MAILCOPY_FOLDERNAME, FolderName, sizeof(FolderName), emetteurabr, recipientabr, FirstEle);

									/* Check out if the MW_RAPP has to be stored in the current e.mail */
									MW_RAPP_Bool = NSFItemGetText(hNoteFiltre, "BList_MW_RAPP_Flag", MW_RAPP_Flag, sizeof(MW_RAPP_Flag)-1);

									/* Read the flag : Checked again after Post Processing (Yes/No) */
									MW_RAPP_AGAIN_Bool = NOERROR;
									if (MW_RAPP_Bool)
										MW_RAPP_AGAIN_Bool = NSFItemGetText(hNoteFiltre, "BList_MW_RAPP_Again_Flag", MW_RAPP_Flag, sizeof(MW_RAPP_Flag)-1);

									/* in the Loop (From X Recipient) is this a new rule ? */

									/* 3.93.33 */
									RemoveReturnReceipt = TRUE;
									if ( NSFItemGetText(hNoteFiltre, "BList_RemoveReturnReceipt", bl_choice, sizeof(bl_choice)-1) )
									{
										if ( bl_choice[0]== '0')
											RemoveReturnReceipt = FALSE;
									}


									/* 3.91 ************************************************/
									/*         Copy the hNote to another Notes Database    */
									/* includes DocLink details + ViewName                 */
									/*-----------------------------------------------------*/
									if ( NSFItemGetText(hNoteFiltre, "BList_IncludeDocLink", bl_choice, sizeof(bl_choice)-1) )
									{
										DocLink = atoi(bl_choice);

										if ( DocLink != 0 )
										{
											if (! NSFItemGetText(hNoteFiltre, "MailCopy_DefaultView", ViewName, sizeof(ViewName)-1))
								 				strcpy(ViewName, COPYDBDEFAULTVIEW);
											
											/* 3.93 : use hNoteSafe instead of *hNote */
											SimpleProcCrochet_DocLink(hNoteSafe, MailCopy, MW_RAPP_Bool, MW_RAPP_AGAIN_Bool, &DocLink, ViewName, &replica_ID, 
												&Note_UNID, &View_UNID, FolderName, RemoveReturnReceipt, Original_MessageID );										
										}
										else
											SimpleProcCrochet(hNoteSafe, MailCopy, MW_RAPP_Bool, MW_RAPP_AGAIN_Bool, FolderName, RemoveReturnReceipt);
									}

								} /* end if : MailCopyBool */

							} /* end of MailDuplicate */


							/* 3.9 **************************************************/
							/* Additional Module : MWGZIP. Append 'MWGZIP' to MWTAG */
							/*------------------------------------------------------*/
							if (NSFItemGetText(hNoteFiltre, "BList_ZipAttach", bl_choice, sizeof(bl_choice)-1))
							{
								if (strcmp(bl_choice, "1") == 0)
									AppendToMWTAG(*hNote, MWTAGFIELD, MAILWATCHER_MWGZIP);
							}

							/* 3.94.16 ***********************************************/
							/*    Additional Module : MWKAVCLEAN. Append it to MWTAG */
							/*********************************************************/
							if (NSFItemGetText(hNoteFiltre, "BList_KAVClean", bl_choice, sizeof(bl_choice)-1))
							{
								if (strcmp(bl_choice, "1") == 0)
								{
									/* check first that there are some "cleanable" files  */
									if ( NSFItemGetText(*hNote, MWKAV_CLEAN_NB_VIRUS, bl_choice, sizeof(bl_choice) - 1) )
									{
										AppendToMWTAG(*hNote, MWTAGFIELD, MAILWATCHER_MWKAVCLEAN);
										/*--- other argt to be passed in the email itself ----*/

										/* Actions #1 : Purge if fails */
										if ( NSFItemGetText(hNoteFiltre, "BL_AV_Actions_Fails_1", bl_choice, sizeof(bl_choice) - 1))
											NSFItemSetText(*hNote, MWKAV_CLEAN_DELETE_IF_FAILS, bl_choice, (WORD) strlen(bl_choice));

										/* Actions # 2 : Log ? */
										if ( NSFItemGetText(hNoteFiltre, "BList_MessageAppend_Field_Values", bl_choice, sizeof(bl_choice) - 1))
											NSFItemSetText(*hNote, MWKAV_CLEAN_DELETE_LOG, bl_choice, (WORD) strlen(bl_choice));
									}
								}
							}


							/* Delete the mail */
							ReturnVal = 0;
							if (NSFItemGetText(hNoteFiltre, "BList_DeleteMail", bl_choice, sizeof(bl_choice)-1))
							{
								if (strcmp(bl_choice, "1") == 0)
								{	/* i = nbrec; : do not skip the end .. new mail might be created for the others recipients */
									/* ReturnVal = 1; */ /*  3.95.19 : not used anymore */
									bRecOK = -1;		 /* 3.95.18 : Verbose mode display 'BlackListed' address */
								}
							}

						} /* Rules Actions only once */


						/*******************************************************************************/
						/**** Following actions can be applied more than one time to the same e.mail ***/
						/*******************************************************************************/

                        /* Do the following actions only if the mail is not going to be deleted anyway */
						/* 3.95.19 : if (ReturnVal == 0) */
						/* { */

							/* Route Thru */
							if (RouteThruBool = NSFItemGetText(hNoteFiltre, "RouteThru", RouteThru, sizeof(RouteThru)-1))
							{
								if (RouteThruBool)
								{
									strcat(recipient, RouteThru);
									strcpy(safe_recipient, recipient); /* the new address format */
								}
							}


							/* Disable some action if the mail is a Duplicate */
							if ( MailDuplicate[0] == '1' )
								ApplyAction = 0;

							/* --------------------------------------------------------------------------*/
							/* Append a text message : read the field and create a body field            */
							if (NSFItemGetText(hNoteFiltre, "BList_MessageAppend", bl_choice, sizeof(bl_choice)-1))
							{
								if (bl_choice[0] =='1')
								{
									/* 3.93.24 : Create a new message even if there is only ONE RECIPIENT LEFT !!
									has the email might be MIME and we need to append R.Text field */
									/* (before 3.93.24) : if ( (nbrec - (i - 1) >= 1) && (bNewNote == 0) ) */
									if ( bNewNote == 0 )
									{
										/* Duplicate the current Note */
										if (sErreur = DuplicateMail(&hDB, *hNote, &hNewNote, 0))
										{
											AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, "mail(x).box");
										}
										else
										{
											/* NOERROR => Save and set the flag */
											bNewNote = 1;
											*hNote    = hNewNote;
										}
									}

									/* Append the text (hNoteFiltre - BList_Message) to the current (hNote) */
									AddMessageToEmail(hNoteFiltre, hNote, "BList_Message", hDB, &bNewNote, &hNewNote, hNewNoteCD, bNewNoteCD);

									/* 3.94.14 : Append Field List at the bottom of the current body */
									if ( NSFItemGetText(hNoteFiltre, "BList_MessageAppend_Field_Values", bl_choice, sizeof(bl_choice)-1))
									{
										if (bl_choice[0] =='1')
											ProcBl_AppendFieldDetailsToEmailBody(hNoteFiltre, *hNote, "BList_MessageAppend_Field_Titles", "BList_MessageAppend_Field_Names");
									}

									/* Apply the routing conditions */
									SetRecOK_FromToForward(emetteur, safe_recipient, *hNote, hNoteFiltre, "RecOK",OrigSubject, 0, ApplyAction, BListMemo, BL_Rules, &bRecOK,hDB, emetteurabr, recipientabr);

								} /* end of NSFItemGetText(..."BList_MessageAppend"....) */
								else
									SetRecOK_FromToForward(emetteur, safe_recipient, *hNote, hNoteFiltre, "RecOK", OrigSubject, 0, ApplyAction, BListMemo, BL_Rules, &bRecOK, hDB, emetteurabr, recipientabr);

							} /* Could not read the field : BList_MessageAppend  */
							else
								SetRecOK_FromToForward(emetteur, safe_recipient, *hNote, hNoteFiltre, "RecOK", OrigSubject, 0, ApplyAction, BListMemo, BL_Rules,&bRecOK, hDB, emetteurabr, recipientabr);


							/* -----------	Remove Attachments : here after the Body changes
											has when it gets duplicated from the original
											Body field keep the attachments				----------------*/

							/* 3.91 ************************************************/
							/*         Delete Attach Files                         */
							/*-----------------------------------------------------*/
							if (NSFItemGetText(hNoteFiltre, "BList_RemoveAttach", bl_choice, sizeof(bl_choice)-1))
							{
								if (strcmp(bl_choice, "1") == 0)
								{
									if ( (nbrec - (i - 1) >= 1) && (bNewNote == 0) )
									{
										/* Duplicate the current Note */
										if (sErreur = DuplicateMail(&hDB, *hNote, &hNewNote, 0))
										{
											AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, "mail(x).box");
										}
										else
										{
											/* NOERROR => Save and set the flag */
											bNewNote = 1;
											*hNote   = hNewNote;
										}
									}

									/* Process Attachements */
									if ( NSFItemGetText(hNoteFiltre, "BList_RemoveAttach_Extension", BList_RemoveAttach_Extension, sizeof(BList_RemoveAttach_Extension)-1) )
										DeleteAttachFileWithException(*hNote, "$FILE", BList_RemoveAttach_Extension);
									else
									{
										/* if FIELD value is specified */
										if ( NSFItemGetText(hNoteFiltre, "BList_RemoveAttach_FieldList", BList_RemoveAttach_FiedList, sizeof(BList_RemoveAttach_FiedList)-1) )
											DeleteAttachFileByName(*hNote, BList_RemoveAttach_FiedList);
										else
											DeleteAttachFile(*hNote, "$FILE");
									}
								}
							}

					/* 	} Not used when DELETE EMAIL is set to 'yes' */   /* ReturnVal == 0 */

						/******************************** END OF ROUTING CONDITIONS ************************************/

						/*************************************************************************/
						/*             >>>> TO BE APPLIED ONLY ONE TIME PER E.MAIL <<<<          */
						/* Since 3.9 ------------------------------------------------------------*/
						/* this warning is regarding the fact that the message might be routed   */
						/* more than one time to the original Sender (From)                      */
						/*************************************************************************/

						/* New message : Delivery failure or notification whatsoever */
						/* Field : BList_DFMail (==1) */
						/* Field : BList_DF_To (text -> emetteur) */
						/* Field : BList_DF_Header (text -> strcat. OrigSubject ) */

						/* To do */
						if (NSFItemGetText(hNoteFiltre, "BList_DFMail", bl_choice, sizeof(bl_choice)-1))
						{
							if (strcmp(bl_choice,"1") == 0)
							{
								/* Read : From  : can be empty */
								NSFItemGetText(hNoteFiltre, "BList_DF_From", NewEmailFromAddress, sizeof(NewEmailFromAddress)-1);
								/* no more default 	strcpy(NewEmailFromAddress, "MailWatcher"); */

								/* Read the Original sender = 'From' address only one time per email : hNote */
								if (ApplyAction == 1)
								{
									if ( !(MailDuplicate[0] == '1') )
									{
										/* Read ( To ) if empty => put the FROM field instead */
										if (!NSFItemGetText(hNoteFiltre, "BList_DF_To", blacklister, sizeof(blacklister)-1))
												strcpy(blacklister, emetteur);
									}
									else
									{
										/* do not send a message back to from again and again ! => ApplyAction == 0 */
										strcpy(blacklister,"0");
									}
								}
								else
								/* do not send a message back to from again and again ! => ApplyAction == 0 */
									strcpy(blacklister,"0");

								/* Read : Subject header : if not : "Re : " */
								if (!NSFItemGetText(hNoteFiltre, "BList_DF_Header", BList_DF_Header, sizeof(BList_DF_Header)-1))
									BList_DF_Header[0] = '\0'; /* 3.93.42 */

								/* Append Original (To) the Recipients List if "1" is selected */
								strcpy(sendToBis,"NULL");
								if (NSFItemGetText(hNoteFiltre, "BList_DF_OrigTo", bl_choice, sizeof(bl_choice)-1))
								{
									if ( (strcmp(bl_choice,"1") == 0) && (strlen(safe_recipient)) )		/* 3.93.9 */
										strcpy(sendToBis, safe_recipient);
								}



								/* ----------- Forward : one more address for this new memo --------------- */
								/* FIX : 3.92.11 : limit this action : Apply one time only */
								if (ApplyAction == 1)
								{
									if ( !(MailDuplicate[0] == '1') )
									{
										/* Forward : one more address for this new memo */
										if (!NSFItemGetText(hNoteFiltre, "BList_DF_To_Fwd", NewMemo_Fwd, sizeof(NewMemo_Fwd)-1))
											strcpy(NewMemo_Fwd, "NULL");
									}
									else
										strcpy(NewMemo_Fwd, "NULL");
								}
								else
									strcpy(NewMemo_Fwd, "NULL");



								/*------------- Append Original (BODY) "1" is selected ------------------*/
								strcpy(Original_Body,"0");
								if (NSFItemGetText(hNoteFiltre, "BList_DF_OrigBody", bl_choice, sizeof(bl_choice)-1))
										strcpy(Original_Body, bl_choice);

								/* check if Delete attachment is set to "Yes" or "No" */
								strcpy(Original_Body_Attach,"0");
								if (NSFItemGetText(hNoteFiltre, "BList_DF_OrigBody_Attach", bl_choice, sizeof(bl_choice)-1))
										strcpy(Original_Body_Attach, bl_choice);


								/* Call the BlackListDeliveryFailure : 3.94.43 : DocLink is (0,1,2..,n)*/
								if ( DocLink !=  0 )
									NonDeliveryBlackList_DocLink(hDB, *hNote, NewEmailFromAddress, blacklister, sendToBis, NewMemo_Fwd, BList_DF_Header, OrigSubject, hNoteFiltre, DocLink, ViewName, replica_ID,
										Note_UNID, View_UNID, Original_Body, &hDB, emetteur, safe_recipient,Original_Body_Attach, Original_MessageID);
								else
									NonDeliveryBlackList(hDB, *hNote, NewEmailFromAddress, blacklister, sendToBis, NewMemo_Fwd, BList_DF_Header, OrigSubject, hNoteFiltre, Original_Body, &hDB, emetteur, safe_recipient,Original_Body_Attach);
							}
						}

				/*	}  ApplyAction == 1 */


						 /**********************************************************/
						 /*    >>>> END OF ACTION => E.MAIL <<<<                   */
						 /**********************************************************/

						/**** Close filter rule doc ****/
						NSFNoteClose(hNoteFiltre);

						/* Free up the NifReadEntries returned buffer */
						OSUnlockObject(buffer_fhandle);

						/* Release the View buffer returned by NifReadEntries */
						OSMemFree(buffer_fhandle);

					} /* End of IF => Document containing the rules is there !! */

					nbbl++;

				} /** Exception From, To test **/

				/*=====================================================================================*/
				/*--------------- Check out for a set of forced actions even for Exceptions -----------*/

				if(BlackRedList[BL_Rules -1].Argt_ThisRulesTermintes)
					BL_Rules = MaxRules; /* So force the end of the while loop */


				/*=====================================================================================*/


			} /** Return the filter rules - GetMatchKey **/


#ifdef VERBOSE

		if (bRecOK < 0)
			AddInLogMessageText("RULES:%d <%s> [Blacklisted] vote:%d", NOERROR, MatchRulesRank, safe_recipient, bRecOK);
			/* printf("(PROCESSING RULES : %d) %s is Blacklisted up to : %d\n", BL_Rules, safe_recipient, bRecOK); */
		else
			AddInLogMessageText("RULES:%d <%s> [ OK ] vote:%d", NOERROR, MatchRulesRank, safe_recipient, bRecOK );
			/* printf("(PROCESSING RULES : %d) %s is -- OK -- up to : %d\n", BL_Rules, safe_recipient, bRecOK); */
#endif

		} /** End while.. apply multiples rules **/


#ifdef VERBOSE

		if (bRecOK < 0)
			AddInLogMessageText("(= END =) %s is >>Blacklisted<<, vote:%d", NOERROR, safe_recipient, bRecOK);
			/* printf("(= END =) %s is >>Blacklisted<<, vote:%d \n", safe_recipient, bRecOK); */
		else
			AddInLogMessageText("(= END =) %s is -- OK -- vote: %d", NOERROR, safe_recipient, bRecOK);
			/* printf("(= END =) %s is -- OK -- vote: %d \n", safe_recipient, bRecOK); */
#endif


		/** add to the RecOK for non matching **/
		if (!GetMatchKeyFlag)
		{
			NSFItemAppendTextList(*hNote, "RecOK", safe_recipient, (WORD) strlen(safe_recipient), FALSE);
		}
		else
		{
			/* Log the fact that this 'safe_recipient' has been blacklisted
			in the original document not in a new copy : 3.93.33 : change <=0 to < 0 as there is now
			neutral rules which add 0 to routing vote but the Recipients has matches this rule */
			if ( bRecOK < 0 )
			{
				bRecNotOk = 1;

				if (bNewNote == 1)
					NSFItemAppendTextList(hNoteSafe, "RecNotOK", safe_recipient, (WORD) strlen(safe_recipient), FALSE);
				else
					NSFItemAppendTextList(*hNote,     "RecNotOK", safe_recipient, (WORD) strlen(safe_recipient), FALSE);
			}
			else
			/* this recipient has not been blacklisted => save the address in hNote (current copy)
			 for recreating the 'recipient' during the final update */
				NSFItemAppendTextList(*hNote, "RecOK", safe_recipient, (WORD) strlen(safe_recipient), FALSE);
		}

		/*-----------------------------------------------------------------------*/
		/* >>>>>>>>>> End of one couple (From, Recipient[i])        <<<<<<<<<<<< */

		/* Restore the hNote !! if necessary */
		if (bNewNote == 1)
		{
			/* overwrite recipient field with RecOk
			   Document has been saved already so some field needs to be purged...
			   => if recipients field is empty then this doc will be deleted by the router */
			UpdateRecipientsList(*hNote);

			UpdateNewMailNote(*hNote);

			/* end update & close proc */
			*hNote = hNoteSafe;
			ReturnVal = 0;
		}

	} /* End of Recipients list iteration */


	/* --------- 3.93.19 do not Purge but just close the temporary hNote --------*/
	if ( bNewNoteCD )
	{
		NSFNoteClose( hNewNoteCD );
		/* sErreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
	}

	/*-----------------------------------------------------------------------*/
	/*----------- Work out within the original email ------------------------*/

	if (ReturnVal == 0)
		ReturnVal = UpdateRecipientsList(*hNote);


	/* Store Stats : number and reference of the rules that have been applied on this email */
	/* if some Recipients are still there */
	if (advstat)
	{
		/* Nb of black list rules applied */
		statmw[lgstat].nbBl = nbbl;

		/* strcat, all the rules applied to the current e.mail with ";" as separator !! */
		if (save_strcat_blref)
			strcpy(statmw[lgstat].BLNames, BListName);

	} /* end of stats collect */



	/* if Option 'advnotify_blacklist' is enable then send to the original Sender
	   a notification when 'To' have been blacklisted */
	if (advnotify_blacklist & bRecNotOk)
		BlackListedAddressNotification(*hNote, &hDB);

	return ReturnVal;
}


/*************************************************************************
 * Fct EvalFormula - Applique une formule à un message
 *      Inputs : NOTEHANDLE     = message sur lequel on applique la formule
 *      char *formula = formule à appliquer
 *      NOTEID = ID du document (pour erreur de compilation)
 *      Output : BOOL = 1 si formule vérifiée ou 0
 *************************************************************************/
BOOL EvalFormula(NOTEHANDLE hNote, char *formula, char* noteid)
{
	STATUS sErreur;
	FORMULAHANDLE hFormula;
	HCOMPUTE hCompute;
	BOOL retform;
	WORD hFormulaLen;
	STATUS rcErreur;
	WORD rcErreurL;
	WORD rcErreurC;
	WORD rcErreurO;
	WORD rcErreurS;
	char *pFormula;

	/* Compilation de la formule initiale */
	sErreur = NSFFormulaCompile(NULL, 0, formula, (WORD) strlen(formula),
														 &hFormula, &hFormulaLen, &rcErreur, &rcErreurL,
														 &rcErreurC, &rcErreurO, &rcErreurS);
	if (sErreur)
	{
		AddInLogMessageText(ERR_COMPILE_FORMULA, sErreur, NOMAPI, noteid, dbrules, rcErreurO);
		return(FALSE);
	}

	pFormula = OSLockObject(hFormula);
	sErreur = NSFComputeStart(0, pFormula, (HCOMPUTE far *) &hCompute);
	if (sErreur)
	{
		OSUnlockObject(hFormula);
		OSMemFree(hFormula);
		AddInLogMessageText(ERR_COMPUTE_START, sErreur, NOMAPI, noteid, dbrules);
		return(FALSE);
	}

	/* Formule vérifiée ? */
	sErreur = NSFComputeEvaluate(hCompute, hNote, NULLHANDLE, 0,
																	&retform, 0, 0);

	NSFComputeStop(hCompute);
	OSUnlockObject(hFormula);
	OSMemFree(hFormula);

	/* Retour de la fonction */
	return(retform);
}


/*********************************************************************
 * Fct spécial pirate : recopie le message émis ou reçu dans une autre
 * base....
 * Input  : NOTEHANDLE = message à copier
 * Output : void
 *********************************************************************/
void ProcCrochet(NOTEHANDLE hNote, DBHANDLE hDb)
{
	STATUS sErreur;
	char emetteur[MAXUSERNAME];
	char emetteurabr[MAXUSERNAME];
	COLLECTIONPOSITION coll_pos;
	HANDLE buffer_fhandle= NULLHANDLE;
	DWORD *id_flist;
	NOTEHANDLE hNoteFiltre;
	DWORD atraiter;
	char temp[MAXPATH];
	DBHANDLE hPirate;
	NOTEHANDLE hCopy;
	OID new_oid;
	WORD nbelem, ctr;
	int nbarch = 0;

	/** Qui emet le message ? **/
	if (!NSFItemGetText(hNote, "From", emetteur, sizeof(emetteur)-1))
		strcpy(emetteur, "Mail Router");

	NSFItemAppendTextList(hNote, "MwTemp", emetteur, (WORD) strlen(emetteur), FALSE);
	nbelem = NSFItemGetTextListEntries(hNote, "Recipients");

	for (ctr=0; ctr < nbelem; ctr++)
	{
	 	NSFItemGetTextListEntry(hNote, "Recipients", ctr, emetteur, sizeof(emetteur)-1);
		NSFItemAppendTextList(hNote, "MwTemp", emetteur, (WORD) strlen(emetteur), FALSE);
	}

	nbelem = NSFItemGetTextListEntries(hNote, "MwTemp");
	for (ctr=0; ctr < nbelem; ctr++)
	{
		NSFItemGetTextListEntry(hNote, "MwTemp", ctr, emetteur, sizeof(emetteur)-1);
		DNAbbreviate(0L, NULL, emetteur, emetteurabr, MAXUSERNAME, NULL);

		sErreur = NIFFindByName(coll_cr, emetteurabr,
											     FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE,
													 &coll_pos, &atraiter);
		if (sErreur)
			continue;


		/* On récupère le nom de la base des backups */
		if (sErreur = NIFReadEntries(coll_cr, &coll_pos, NAVIGATE_CURRENT, 0,
									NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID,
									&buffer_fhandle, NULL, NULL, &atraiter, NULL))
		{
			if (buffer_fhandle != NULL)
				OSMemFree(buffer_fhandle);

			AddInLogMessageText(ERR_READ_VIEW, sErreur, NOMAPI, "($Crochet)", dbrules);
			continue;
		}

		if ((buffer_fhandle == NULLHANDLE) || (atraiter == 0))
		{
			if (buffer_fhandle != NULL)
				OSMemFree(buffer_fhandle);

			AddInLogMessageText(ERR_OPEN_FILTER_DOC, sErreur, NOMAPI, 0x0, dbrules);
			continue;
		}

		id_flist = (NOTEID *)OSLockObject(buffer_fhandle);

		if (sErreur = NSFNoteOpen(hDb, id_flist[0], 0, &hNoteFiltre))
		{
			OSUnlockObject(buffer_fhandle);
			OSMemFree(buffer_fhandle);

			AddInLogMessageText(ERR_OPEN_FILTER_DOC, sErreur, NOMAPI, id_flist[0], dbrules);
			continue;
		}

		NSFItemGetText(hNoteFiltre, "BaseArc", temp, sizeof(temp)-1);
		NSFNoteClose(hNoteFiltre);

		OSUnlockObject(buffer_fhandle);
		OSMemFree(buffer_fhandle);

		/* Ouverture de la base cible */
		CheckPath(temp); /** For Unix **/

		if (sErreur = NSFDbOpen(temp, &hPirate))
		{
			AddInLogMessageText(ERR_OPEN_DB, sErreur, NOMAPI, temp);
			continue;
		}

		if (!EstVraiBase(hPirate))
		{
			AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, temp);
			NSFDbClose(hPirate);
			continue;
		}

		if (sErreur = NSFNoteCopy(hNote, &hCopy))
		{
			NSFDbClose(hPirate);
			AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, temp);
			continue;
		}

		if (sErreur = NSFDbGenerateOID(hPirate, &new_oid))
		{
			NSFNoteClose(hCopy);
			NSFDbClose(hPirate);
			AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, temp);
			continue;
		}

		NSFNoteSetInfo(hCopy, _NOTE_OID, &new_oid);
		NSFNoteSetInfo(hCopy, _NOTE_ID, NULL);
		NSFNoteSetInfo(hCopy, _NOTE_DB, &hPirate);


		/* on vire certains champs ;) */
		NSFItemDelete(hCopy,"ReturnReceipt", 13);

		/* On vire le champ temporaire */
		NSFItemDelete(hCopy, "MwTemp", 6);


		sErreur = NSFNoteUpdate(hCopy, UPDATE_FORCE);
		NSFNoteClose(hCopy);
		NSFDbClose(hPirate);
		if (sErreur)
			AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, temp);
		else
			nbarch++;

	} /* fin de balayage emetteur / destinataires */



	/* MaJ des stats */
	statmw[lgstat].nbArch = nbarch;

	return;

}









/***********************************************************************
 * Cette fct supprime tous les espaces superflus. Equivalente a la fct
 * Notes @Trim
 * INPUTS : chaine a analyser
 * OUTPUT : chaine traitee
 **********************************************************************/
void supprespace(char *chaine)
{
	unsigned int i;              /* balaye toute la chaîne */
	int j;                       /* décalage vers la gauche */
	int k;                       /* booleen espace trouvé / non trouvé */

  k = 0;
  for (i=0; chaine[i] != '\0'; i++)
  {
    if (chaine[i] == 32)
		{
			if (k==0)
		    k = 1;
			else
	    {
		for (j=i; chaine[j] != '\0'; j++)
					chaine[j] = chaine[j+1];
		chaine[j] = '\0';
		i--;
	    }
		}
    else
			k = 0;
  }

  /* touches finales */
  if (chaine[0] == 32)
  {
    for (i=1; i<strlen(chaine); i++)
	chaine[i-1] = chaine[i];
		chaine[strlen(chaine)-1] = '\0';
  }

  if (chaine[strlen(chaine)-1] == 32)
    chaine[strlen(chaine)-1] = '\0';
  return;
}


/*******************************************************************
 * Cette fct envoi un non delivery failure à l'emetteur qui
 * s'est fait gauler par une formule de filtre
 *******************************************************************/
void NonDeliveryFilter(char *sendto, char *formule, char *origsubject)
{
	STATUS erreur;
	DBHANDLE hMb;
	NOTEHANDLE		hMes;
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	TIMEDATE		tdCurrent;
	char			sujet[3000];
	int				i;
    char			filename[10];



	/* multi by default */
	strcpy(filename,"mail1.box");

	if (CheckMonoMailBox())
		strcpy(filename,"mail.box");


	/* Création du mail */
	if (erreur = NSFDbOpen(filename, &hMb))
	{
		AddInLogMessageText(ERR_OPEN_DB, erreur, NOMAPI, filename);
		return;
	}

	/* On vérifie que c'est une vraie base */
	if (!EstVraiBase(hMb))
	{
		NSFDbClose(hMb);
		AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, filename);
		return;
	}

	if (erreur = NSFNoteCreate(hMb, &hMes))
	{
		AddInLogMessageText(ERR_CREATE_DOC, erreur, NOMAPI, filename);
		NSFDbClose(hMb);
		return;
	}

	/* Masque */
	NSFItemSetText(hMes, "Form", "Memo", 4);

	/* emetteur */
	NSFItemSetText(hMes, "From", ndffrom, (WORD) strlen(ndffrom));

	NSFItemAppendTextList(hMes, "SendTo", sendto, (WORD) strlen(sendto), FALSE);
	NSFItemAppendTextList(hMes, "Recipients", sendto, (WORD) strlen(sendto), FALSE);

	/* date d'envoi */
	OSCurrentTIMEDATE(&tdCurrent);
	NSFItemSetTime(hMes, "PostedDate", &tdCurrent);

	/* Sujet */
	sprintf(sujet, ndfsubject, origsubject);
	NSFItemSetText(hMes, "Subject", sujet, (WORD) strlen(sujet));

  	/* Body */
	if(erreur = CompoundTextCreate(hMes, "Body", &hCompound))
	{
		/* use a TEXT field instead */
		NSFItemSetText(hMes, "Body", formule, (WORD) strlen(formule));
		return;
    }
	else
	{

		/* Loop over Formula_Txt...Append General message */

		CompoundTextInitStyle(&Style);
		if (erreur = CompoundTextDefineStyle( hCompound,
				                          "Normal",
						                  &Style,
								          &dwNormalStyle))
		{
			CompoundTextDiscard(hCompound);
			return;
		}

		/* Add Message from Unipower_API...Politically Correct in the Body rich Text */
		for (i = 0; i < nb_formula_Messagetxt; i++)
		{


#ifdef V4
			if (erreur = CompoundTextAddParagraph
#else
			if (erreur = CompoundTextAddParagraphExt
#endif
									(hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  ndftextmessage[i],
                                  (DWORD)strlen(ndftextmessage[i]),
                                  NULLHANDLE))
			{
				CompoundTextDiscard(hCompound);
				return;
			}
		}



		/* Now add the rules - in clear language ...*/

		for (i = 0; i < nb_formula_txt; i++)
		{


#ifdef V4
			if (erreur = CompoundTextAddParagraph
#else
			if (erreur = CompoundTextAddParagraphExt
#endif
								(hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  Formula_Txt[i],
                                  (DWORD)strlen(Formula_Txt[i]),
                                  NULLHANDLE))
			{
				CompoundTextDiscard(hCompound);
				return;
			}
		}

		/* Close Compound */
		erreur = CompoundTextClose (hCompound, NULL, NULL, NULL, 0);

		/* Free up ressource if necessary */
		if (erreur != NOERROR)
			CompoundTextDiscard(hCompound);
    }

	/* Champ pour savoir si EM bloque ou pas */
	NSFItemSetText(hMes, "NOT_EM", "1", 1);

	NSFNoteUpdate(hMes, UPDATE_FORCE);

	NSFNoteClose(hMes);
	NSFDbClose(hMb);

	return;
}





/***********************************************************************************
* Add a message Text at the end of the e.mail                                      *
* IN : fNote is the filter rule note
* IN : (pointer to hNote)=> is the email note
* IN : fieldName is the fNote field name that contains the TEXT to be appended.
************************************************************************************/
void AddMessageToEmail(NOTEHANDLE fNote, NOTEHANDLE* hNote, char* fieldName, DBHANDLE hDB,
					   int* bNewNote, NOTEHANDLE* hNewNote, NOTEHANDLE hNewNoteCD, int bNewNoteCD)
{

	/***
	fNote is the Filter doc handle
	hNote is the email handle
	hNewNoteCd is the original mail reopen with MIME to CD conversion
	**/

	STATUS			erreur = NOERROR;
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	int				i, nb_Messagetxt,text_len;
	char			szMessageText[MAXTXTMESSAGE];
	char			szMessageTextSubject[MAXRECLEN];
	char			newSubject[MAXRECLEN + MAXRECLEN + 1];
	char			filename[MAXPATH];

	/* init */
	szMessageText[0]='\0';

	/* ---------------------- Attach file for Crypted document ------------------*/
	NSFItemGetText(*hNote, "Encrypt", szMessageText, sizeof(szMessageText)-1);
	if (szMessageText[0] == '1')
	{
	    /* 3.92 */
		/* in order to support Body mail modification  crypted document, the Body item is not modified
		   but a file is attached */

		/* Read the field that contains the full filename & path  do CheckPath on it */
		if (NSFItemGetText(fNote, "BList_Message_Attachment", filename, sizeof(filename)-1 ))
		{
			/** Attachement - do Unix like path if necessary **/
			CheckPath(filename);

			if (erreur = NSFNoteAttachFile(*hNote,"$FILE",(WORD) strlen("$FILE"), filename, filename, COMPRESS_NONE))
				AddInLogMessageText(ERR_FILTER_CANNOT_ATTACH, erreur, NOMAPI, filename);

		}
	} /* message Crypted */
	else
	{
		/*------------------------- RAW MIME in Body field --------------------------*/
		if ( NSFItemIsPresent(*hNote, "$NoteHasNativeMIME", 18) || ( NSFItemIsPresent(*hNote, "MIME_Version", 12)) )
		{
			if ( bNewNoteCD )
			{
				/* remove the current Body fields */
				erreur = NSFItemDelete(*hNote, "Body", (WORD) strlen("Body"));

				/* Append Body fields from the hNewNoteCD if (bnewNoteCD == 1) */
				if ( erreur == NOERROR )
				{
					erreur = CompoundTextCreate(*hNote, "Body", &hCompound);
					if ( erreur == NOERROR )
					{
						/* place here the original body in CD format */
						if ( erreur = CompoundTextAssimilateItem(hCompound, hNewNoteCD, "Body", 0L))
							CompoundTextDiscard(hCompound);

						/* 3.93.38 */
						if ( erreur == NOERROR )
						{
							/* 3.93.22 free ressources */
							if ( erreur = CompoundTextClose (hCompound, 0, 0L, NULL, 0))
								CompoundTextDiscard(hCompound);
						}
					}
				}
				/* if no error then remove the $NoteHasNativeMIME field */
				if ( erreur == NOERROR )
				{
					NSFItemDelete(*hNote, "$NoteHasNativeMIME", 18 );
					NSFItemDelete(*hNote, "MIME_Version", 12 );
					NSFItemDelete(*hNote, "MIMEMailHeaderCharset", 21);
					NSFItemDelete(*hNote, "$MIMETrack", 10);
				}
			}/* make sure the MIME to CD version of the original Notes has been initialized */
		} /* test if "$NoteHasNativeMIME" is there */

		/*-------------------------------------------------------------------------------*/
		/* FULL NOTES FORMAT ??( / DOC NOT CRYPTED )                                     */
		/* to be really sure test $notehasnativeMime again ;)                            */
		/*------------------------- RAW MIME in Body field ------------------------------*/
		if ( !NSFItemIsPresent(*hNote, "$NoteHasNativeMIME", 18) )
		{

			/* ------------- subject prefix ---------------------------------------*/
			/* (3.93) if set : Change Original Subject                            */
			if (NSFItemGetText(fNote, "BList_Message_Subject", szMessageTextSubject, MAXRECLEN-1))
			{
				Trim( szMessageTextSubject, szMessageTextSubject );

				/* append if some subject have been set ! */
				if (NSFItemGetText(*hNote, "subject", szMessageText, MAXRECLEN-5))
				{
					sprintf(newSubject, "%s : %s", szMessageTextSubject, szMessageText);

					/* write back subject field */
					NSFItemSetText(*hNote, "Subject", newSubject, (WORD) strlen(newSubject));
				}
			}


			/* ------------- BODY part parameter is a RICH TEXT field --------------------------------------*/
			/* 3.93.22 : Check if the Argument is RichText - if so use it */
			if ( NSFItemIsPresent(fNote, "BList_Message_RT", (WORD) strlen("BList_Message_RT") ))
			{
				/* create the rich text handle : compound */
				if( erreur = CompoundTextCreate(*hNote, "Body", &hCompound) )
						return;

				/* init contexte */
				 CompoundTextInitStyle(&Style);
				if (erreur = CompoundTextDefineStyle( hCompound,
					                      "Normal",
							              &Style,
									      &dwNormalStyle))
				{
					CompoundTextDiscard(hCompound);
					return;
				}

				/* Concat Body field from filter rules document to the email */
				if ( erreur = CompoundTextAssimilateItem( hCompound, fNote, "BList_Message_RT", 0L ) )
				{
					CompoundTextDiscard(hCompound);
					return;
				}

				/* Close Compound */
				erreur = CompoundTextClose (hCompound, NULL, NULL, NULL, 0);

				/* Free up ressource if necessary */
				if (erreur != NOERROR)
					CompoundTextDiscard(hCompound);
			}
			/* ------------- BODY part parameter is a SIMPLE TEXT field --------------------------------------*/
			else
			{
				/* Add the message at the end in the Body rich Text */
				nb_Messagetxt = NSFItemGetTextListEntries(fNote, fieldName);
				if ( nb_Messagetxt > 0 )
				{
					/* Create a new Body that will be composed by the following text lines */
					if(erreur = CompoundTextCreate(*hNote, "Body", &hCompound))
						return;

					CompoundTextInitStyle(&Style);
					if (erreur = CompoundTextDefineStyle( hCompound,
					                      "Normal",
							              &Style,
									      &dwNormalStyle))
					{
						CompoundTextDiscard(hCompound);
						return;
					}


					/* Read and Write, jump on line */
#ifdef V4
					if (erreur = CompoundTextAddText
#else
					if (erreur = CompoundTextAddTextExt
#endif
									(hCompound,
						             dwNormalStyle,
									 DEFAULT_FONT_ID,
									 "\n",
									 (DWORD) 2,
									 "\r\n",
									 COMP_PRESERVE_LINES,
									 NULLHANDLE))
					{
						CompoundTextDiscard(hCompound);
						return;
					}

					/* read the filter rule parameter */
					for (i = 0; i < nb_Messagetxt; i++)
					{
						text_len = NSFItemGetTextListEntry(fNote, fieldName, (WORD) i,
												szMessageText, (WORD) sizeof(szMessageText)-1);
						szMessageText[text_len] = '\0';

#ifdef V4
						if (erreur = CompoundTextAddParagraph
#else
						if (erreur = CompoundTextAddParagraphExt
#endif
								 (hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  szMessageText,
                                  (DWORD)strlen(szMessageText),
                                  NULLHANDLE))
						{
							CompoundTextDiscard(hCompound);
							return;
						}
					} /* end for */

					/* Close Compound */
					erreur = CompoundTextClose (hCompound, NULL, NULL, NULL, 0);

					/* Free up ressource if necessary */
					if (erreur != NOERROR)
						CompoundTextDiscard(hCompound);
				}/* rules has a field text for Body */

			} /* BList_Message is defined */

		} /* if : "$NoteHasNativeMIME" is still there even after MIME To CD conversion */
	}/* else : not crypted */
}



/*************************************************************************************/
/** For Unix Path, Filename, reverse "\" to "/"                                     **/
void CheckPath(char *Filename)
{

	if (vUnix == TRUE)
	{
		while (*Filename++)

		  if (*Filename == '\\')
		 			   *Filename = '/';

	}
}
/*********************************
* reverse back slash to slash
* remove any drive letter and separator
*******************************************/
void MakeLinkFromPath(char * Filename)
{
	char* loc;

	/* remove ':' */
	loc = strchr(Filename, ':');
	if (loc)
		Filename = loc;

	/* then reverse back slash to slash */
	while (*Filename)
	{
		  if (*Filename == '\\')
 			   *Filename = '/';

		*Filename++;
	}
}


/************************************************************************
* RemoveNotesDomain
* INPUT/OUTPUT : String
*
* 3.95.15 : make sure that the remove @domain are not internet domain
*		  : test in loc2 for "." on the right side.
************************************************************************/
void RemoveNotesDomain(char *n)
{
    /* : xxxxxxx@unipower.net@UNIPOWER_NET => xxxxxx@unipower.net */
    /* : xxxxxxx/FR/Unipower_sys@unipower  => xxxxxx/FR/Unipower_sys */
	char* loc;
	char* loc2;


    /** Look for the last '@' in the address..remove if it is not an internet address **/
		loc = strchr(n, '@');
		if (loc)
		{

			/** Look for "." which if not => Not an internet address **/
			if (!strchr(loc+1,'.'))
				n[strlen(n) - strlen(loc)] = '\0';
			else
			{
				/* Look for other domain */
				loc2 = strchr(loc+1,'@');

				/* 3.95. Remove domain only if non internet domain */
				if ( loc2 )
				{
					/* must not found a "." right position */
					if (!strchr(loc2+1,'.'))
						n[strlen(n) - strlen(loc2)] = '\0';
				}
			}
		}
}



/**************************************************************************************
* SimpleProcCrochet(hNote, hNoteFiltre);
* INPUT		: The message (hNote)
* INPUT		: char the database path
* -------------------------------------------------------------------------------------
* 30/06/03	: new in 3.92 : char* FolderName
* OUTPUT	: None
***************************************************************************************/
void SimpleProcCrochet(NOTEHANDLE hNote, char* basearc, STATUS MW_RAPP_Bool, STATUS MW_RAPP_AGAIN_Bool,
					   char* FolderName, BOOL RemoveReturnReceipt)
{

	/** Local variable **/
	STATUS sErreur;
	DBHANDLE hPirate;
	NOTEHANDLE hCopy;
	OID new_oid;
	int	mwtag = 0;

	/** Beg of prog **/

	/* Ouverture de la base cible */
	CheckPath(basearc); /** For Unix **/

	if (sErreur = NSFDbOpen(basearc, &hPirate))
	{
		AddInLogMessageText(ERR_OPEN_DB, sErreur, NOMAPI, basearc);
		return;
	}

	if (!EstVraiBase(hPirate))
	{
		AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, basearc);
		NSFDbClose(hPirate);
		return;
	}


	/* Copy all fields to the new doc */
	if (sErreur = NSFNoteCopy(hNote, &hCopy))
	{
		NSFDbClose(hPirate);
		AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, basearc);
		return;
	}

	if (sErreur = NSFDbGenerateOID(hPirate, &new_oid))
	{
		NSFNoteClose(hCopy);
		NSFDbClose(hPirate);
		AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, basearc);
		return;
	}

	NSFNoteSetInfo(hCopy, _NOTE_OID, &new_oid);
	NSFNoteSetInfo(hCopy, _NOTE_ID, NULL);
	NSFNoteSetInfo(hCopy, _NOTE_DB, &hPirate);

	/* remove tempo fields */
	NSFItemDelete(hCopy, "MwTemp",             6);
	NSFItemDelete(hCopy, "RecOK",              5);
	NSFItemDelete(hCopy, MW_MAILDUPLICATE, (WORD) strlen(MW_MAILDUPLICATE));	/* work out the hCopy not the hNote fix 3.92.17 */

	/* Purge : Others Special Fields ;)			*/
	if ( RemoveReturnReceipt )
		NSFItemDelete(hCopy,"ReturnReceipt", 13);	/* 3.92.17 */


	/* The mail is going to be posted again in the mail.box */
	if (MW_RAPP_Bool)
	{
		/* Do not set the flag if the Doc needs to be rechecked by MailWatcher */
		if (!MW_RAPP_AGAIN_Bool)
		{
			/* Mark that MWADVT is now finished => Remove the task in the mwtag list  */
			/* but some other task could have been sched.                             */

			mwtag = 0;
			mwtag = NSFItemGetTextListEntries(hCopy, MWTAGFIELD);
			RemoveFromMWTAG(hCopy,MWTAGFIELD, MAILWATCHER_MWADVT);
			mwtag = mwtag - 1;

			/* if no more task => this mail will not be STOP                          */
			if (mwtag == 0)
			{
				/* field to not to stop the mail at (Extension Manager level) : NO TASK */
				NSFItemSetText(hCopy, "NOT_EM", "1", 1);

				/* Delete : Extension Manager STOP Field */
				NSFItemDelete(hCopy, "MWADVSRV"		,	8);
				NSFItemDelete(hCopy, "RoutingState"	,   12);
			}
			/* else : (mwtag => MWADVT as been removed), ADVSRV, RoutingState are kept */

		}
		/* else : mwtag, ADVSRV, RoutingState are kept and unchanged ! */
	}
	else
	{
		/* Delete : Extension Manager STOP Field	*/
		NSFItemDelete(hCopy, "MWADVSRV"		,	8);
		NSFItemDelete(hCopy, "RoutingState"	,   12);

		/* Purge : Rules memory						*/
		NSFItemDelete(hCopy, "MW_BLMemo",          9);
		NSFItemDelete(hCopy, "MW_SKIPRULE",       11);

		/* Purge : Others Special Fields ;)			*/
		if ( RemoveReturnReceipt )
			NSFItemDelete(hCopy,"ReturnReceipt", 13);
	}

	/* Set Readers access */
	if (sErreur = setReaderAccess(hNote, "Recipients", "mw_reader_access", hCopy))
		AddInLogMessageText(ERR_CREATE_DOC_READER, sErreur, NOMAPI, basearc);

	/* save */
	sErreur = NSFNoteUpdate(hCopy, UPDATE_FORCE);

	/* Move to Folder */
	if (strlen( FolderName) )
		sErreur = MoveToFolder(hPirate, basearc, FolderName, hCopy);


	NSFNoteClose(hCopy);
	NSFDbClose(hPirate);

	if (sErreur)
		AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, basearc);

	/* end of prog */
	return;
}






/**************************************************************************************
* SimpleProcCrochet(hNote, hNoteFiltre);
* - This procedure get information to create a DocLink for the copied note.
*
* INPUT		: The message (hNote)
* INPUT		: char the database path
* I/O		: int	: Doclink (boolean) - must be true to use the create the doclink somewhere else
* INPUT		: char*	: View name in "mailcopy" database
* I/O		: TIMEDATE*, UNID*, UNID*	: Doclink details.
*
* OUTPUT	: None
*
* NB : to use DocLink the var 'Doclink' (integer) must equal 1.
***************************************************************************************/
void SimpleProcCrochet_DocLink(NOTEHANDLE hNote, char* basearc, STATUS MW_RAPP_Bool, STATUS MW_RAPP_AGAIN_Bool,
							   int* DocLink, char* ViewName, TIMEDATE* replica_ID, UNID* Note_UNID, UNID* View_UNID,
							   char* FolderName, BOOL RemoveReturnReceipt,
							   char* Original_MessageID)
{

	/** Local variable **/
	STATUS sErreur;
	DBHANDLE hPirate;
	NOTEHANDLE hCopy;
	OID new_oid;
	int	mwtag = 0;

	/** Beg of prog **/
	/* *DocLink = 0; ( 3.94.43 )*/

	/* Ouverture de la base cible */
	CheckPath(basearc); /** For Unix **/

	if (sErreur = NSFDbOpen(basearc, &hPirate))
	{
		AddInLogMessageText(ERR_OPEN_DB, sErreur, NOMAPI, basearc);
		*DocLink = 0;
		return;
	}

	if (!EstVraiBase(hPirate))
	{
		AddInLogMessageText(ERR_OPEN_DB, NOERROR, NOMAPI, basearc);
		NSFDbClose(hPirate);
		*DocLink = 0;
		return;
	}


	/* Copy all fields to the new doc */
	if (sErreur = NSFNoteCopy(hNote, &hCopy))
	{
		NSFDbClose(hPirate);
		AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, basearc);
		*DocLink = 0;
		return;
	}

	if (sErreur = NSFDbGenerateOID(hPirate, &new_oid))
	{
		NSFNoteClose(hCopy);
		NSFDbClose(hPirate);
		AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, basearc);
		*DocLink = 0;
		return;
	}

	NSFNoteSetInfo(hCopy, _NOTE_OID, &new_oid);
	NSFNoteSetInfo(hCopy, _NOTE_ID, NULL);
	NSFNoteSetInfo(hCopy, _NOTE_DB, &hPirate);

	/* remove tempo fields */
	NSFItemDelete(hCopy, "MwTemp",             6);
	NSFItemDelete(hCopy, "RecOK",              5);
	NSFItemDelete(hCopy, MW_MAILDUPLICATE, (WORD) strlen(MW_MAILDUPLICATE));	/* work out the hCopy not the hNote fix 3.92.17 */

	/* 3.95.12 : use by MWSARC like program */
	NSFItemSetText(hCopy, MAIL_ORIGDOCLINK, Original_MessageID, (WORD) strlen(Original_MessageID));


	/* Purge : Others Special Fields ;)		*/
	if ( RemoveReturnReceipt )
		NSFItemDelete(hCopy,"ReturnReceipt", 13);	/* 3.92.17 */


	/* The mail is going to be posted again in the mail.box */
	if (MW_RAPP_Bool)
	{
		/* Do not set the flag if the Doc needs to be rechecked by MailWatcher */
		if (!MW_RAPP_AGAIN_Bool)
		{
			/* Mark that MWADVT is now finished => Remove the task in the mwtag list  */
			/* but some other task could have been sched.                             */

			mwtag = 0;
			mwtag = NSFItemGetTextListEntries(hCopy, MWTAGFIELD);
			RemoveFromMWTAG(hCopy,MWTAGFIELD, MAILWATCHER_MWADVT);
			mwtag = mwtag - 1;

			/* if no more task => this mail will not be STOP                          */
			if (mwtag == 0)
			{
				/* field to not to stop the mail at (Extension Manager level) : NO TASK */
				NSFItemSetText(hCopy, "NOT_EM", "1", 1);

				/* Delete : Extension Manager STOP Field */
				NSFItemDelete(hCopy, "MWADVSRV"		,	8);
				NSFItemDelete(hCopy, "RoutingState"	,   12);
			}
			/* else : (mwtag => MWADVT as been removed), ADVSRV, RoutingState are kept */

		}
		/* else : mwtag, ADVSRV, RoutingState are kept and unchanged ! */
	}
	else
	{
		/* Delete : Extension Manager STOP Field	*/
		NSFItemDelete(hCopy, "MWADVSRV"		,	8);
		NSFItemDelete(hCopy, "RoutingState"	,   12);

		/* Purge : Rules memory						*/
		NSFItemDelete(hCopy, "MW_BLMemo",          9);
		NSFItemDelete(hCopy, "MW_SKIPRULE",       11);

		/* Purge : Others Special Fields ;)			*/
		if ( RemoveReturnReceipt )
			NSFItemDelete(hCopy,"ReturnReceipt", 13);
	}

	/* Save */
	sErreur = NSFNoteUpdate(hCopy, UPDATE_FORCE);

	/* Move to Folder */
	if (strlen( FolderName) )
		sErreur = MoveToFolder(hPirate, basearc, FolderName, hCopy);

	if (sErreur)
		AddInLogMessageText(ERR_CREATE_DOC, sErreur, NOMAPI, basearc);

	if (sErreur == NOERROR)
	{
		/* 3.94.43 : *DocLink = 1; */

		/* Get the Details for DocLink */
		sErreur = MailCopyGetDocLinkDetail(basearc,hPirate, hCopy, replica_ID, ViewName, Note_UNID, View_UNID);

		/* Can not get DocLink info */
		if (sErreur)
		{
			*DocLink = 0;		/* if error reset DocLink function */
			AddInLogMessageText(ERR_CREATE_DOCLINK, sErreur, NOMAPI, basearc);
		}
	}



	/* Set Readers access */
	if (sErreur = setReaderAccess(hNote, "Recipients", "mw_reader_access", hCopy))
		AddInLogMessageText(ERR_CREATE_DOC_READER, sErreur, NOMAPI, basearc);
	else
		NSFNoteUpdate(hCopy, UPDATE_FORCE);



	/* End */
	NSFNoteClose(hCopy);
	NSFDbClose(hPirate);

	/* end of prog */
	return;
}




/***********************************************/
/* b is the mask, a is the key (From&To)       */
/*                                             */
/* sMatch, for String Match                    */
/*                                             */
/***********************************************/
BOOL sMatch(char a[], char b[])
{

	int CondMatch = 1;
	CondMatch = wc_match(b, a);		/* use Wildcard.c <Pattern> <string> */
	return(CondMatch);
}


/******************************************************
 * MetaMatch.
 * Call different match fonction, try to find a rules
 * that match the pattern 'From' & 'To'
 ------------------------------------------------
 ! From			!	To			!	Total		!
 ------------------------------------------------
 User	= 1			User	= 1
 Group	= 2			Group	= 2
 Meta	= 3			Meta	= 3
 User & User						11
 User & Group						12
 User & Meta						13
 Group & User						21
 Group & Group						22
 Group & Meta						23
 Meta & User						31
 Meta & Group						32
 Meta & Meta						33
 -----------------------------------------------------
 * 24/04/01
 + (7) new cases

 User	& Meta Group				14
 Group	& Meta Group				24
 Meta	& Meta Group				34
 Meta Group	& User					41
 Meta Group & Group					42
 Meta Group & Meta					43
 Meta Group & Meta Group			44
 -----------------------------------------------------
 NB : Rules are evaluated starting from the SMALL (w)
 *****************************************************/
int MMatch(char* From, char* To, BLrules CurrentRules)
{
	int result = 0;		/* if no return then result = 0 */
	int PatternMatch;

	/* Loop over the rules; get the pattern then test the match */
	PatternMatch = CurrentRules.w;

	switch(PatternMatch)
	{

		/* case : 2 is a Group (regular user name) */
		case 12: case 32:
			if ( sMatch(From, CurrentRules.From) )
				return(IsInGroup(To, CurrentRules.To) );

/* ( slow ! ) if ( IsInGroup(To, CurrentRules.To) )
				return(sMatch(From, CurrentRules.From)); */
			break;


		case 21: case 23:
			if ( sMatch(To, CurrentRules.To) )
				return( IsInGroup(From, CurrentRules.From) );

/* ( slow ! ) if ( IsInGroup(From, CurrentRules.From) )
				return(sMatch(To, CurrentRules.To));  */
			break;


		case 22:
			if ( IsInGroup(To, CurrentRules.To) )
				return( IsInGroup(From, CurrentRules.From) );
			break;


		/* 7 new cases	: (4) is for a Group containing meta strings */
		/* case value	: left figure  value is FROM */
		/*				: right figure value is TO  */

		case 14 : case 34 :

			if ( sMatch(From, CurrentRules.From) )
				return( IsInGroupWmeta(To, CurrentRules.To) );

/* ( slow !)			if (IsInGroupWmeta(To, CurrentRules.To))
				return(sMatch(From, CurrentRules.From)); */
			break;

		case 41: case 43:

			if ( sMatch(To, CurrentRules.To) )
				return( IsInGroupWmeta(From, CurrentRules.From) );

/* ( slow !)			if (IsInGroupWmeta(From, CurrentRules.From))
				return(sMatch(To, CurrentRules.To)); */
			break;

		/* 3.93.29 : wrong return ! always = 0 */
		case 42 :
			if ( IsInGroupWmeta(From, CurrentRules.From) )
				return(IsInGroup(To, CurrentRules.To) );
			break;

		case 24 :
			if ( IsInGroup(From, CurrentRules.From) )
				return( IsInGroupWmeta(To, CurrentRules.To) );
			break;

		case 44 :
			if ( IsInGroupWmeta(To, CurrentRules.To) )
				return( IsInGroupWmeta(From, CurrentRules.From));
			break;

		/* string x string */
		default:
			if ( sMatch(From, CurrentRules.From) )
				return ( sMatch(To, CurrentRules.To) );
			break;
	}

	/* return */
	return(result);
}





/************************************************************
 * IsInGroup(char * username, char * groupname)
 * INPUT	: username, groupname
 * OUTPUT	: True, False


 The special User List :
 =======================
 This contains all the users found as being part of group used
 in one or more Black List rules.
 * ---------------------------------------------------------------
 * 12/07/04
 * Revision :  3.93.16
 * Support variable form for UserName.
 * if UserName contains '@' then try 2 times if search with '@' fails

 *************************************************************/
int IsInGroup(char* UserName, char* GroupName)
{
	SpUser* SpUserItem;

	/* in the Special user list Get the username */
	/* If found => Try to find the if the GroupName is there !! */
	SpUserItem = SearchUser(SpecialUserListGlobal, UserName);

	/* Second chance ! */
	if (( SpUserItem == NULL ) && (strchr(UserName, '@') != NULL ))
	{
		char	LocalUserName[MAXUSERNAME];

		/* look for the UserName without the domain name */
		strcpy( LocalUserName, UserName);
		RemoveInternetDomain_ShortName( LocalUserName );
		SpUserItem = SearchUser(SpecialUserListGlobal, LocalUserName);
	}

	/* read */
	if(SpUserItem != NULL)
		return( SearchGroup(SpUserItem->GroupList, GroupName) != NULL);
	else
		return(0);

}




/************************************************************
 * IsInGroupWmeta(char * username, char * groupname)
 * INPUT	: username, groupname
 * OUTPUT	: True, False


 The special User List :
 =======================
 This contains all the users found as being part of group used
 in one or more Black List rules.
 These search meta takes METAs in Groups...
 the search is different... the first match is maybe not the good one
 it must continue till the end if it fails

 *************************************************************/
int IsInGroupWmeta(char* UserName, char* GroupName)
{
	SpUser* SpUserItem;
	int cond = FALSE;

	/* in the Special user list Get the username */
	/* If found => Try to find the if the GroupName is there !! */

	SpUserItem = SpecialUserListGlobal;

    while ((SpUserItem !=NULL) && (cond == 0))
	{

		SpUserItem = sSearchUser(SpUserItem, UserName);

		if(SpUserItem != NULL)
		{
			cond = (SearchGroup(SpUserItem->GroupList, GroupName) != NULL);

			/* not found, continue ! */
			if (cond == 0)
			SpUserItem = SpUserItem->Next;
		}

	/* Go and test the While cond */
	}

	return(cond);

}






/*****************************************************************
 * BuildSpecialUserList
 * Link list of username

  Username element
  ================
  char* username
  GRPList* GroupName;

 ****************************************************************/
void BuildSpecialUserList()
{

/* Create */
	SpecialUserListGlobal = (SpUser *) malloc (sizeof(SpUser));
	CreateUserList(SpecialUserListGlobal);

	/* Group List, recursive check up */
	DumpGrpList = (GrpList *) malloc (sizeof(GrpList));
	CreateGroupList(DumpGrpList);
}



/******************************************************************
 * (SearchUser(UserName)
 * Return the NULL the current user element
 ******************************************************************/
SpUser* SearchUser(SpUser* Current, char* TargetUserName)
{


	int cond = 1;

	while (cond && Current->Next != NULL)
	{

		cond = strcmp(Current->UserName, TargetUserName);


		if (cond)
			Current = Current->Next;
	}

	/* Return value */
	if (Current->Next == NULL)
		return(NULL);
	else
		return(Current);


	/**** Recursive version *****/
	/*	if (!strcmp(Current->UserName, TargetUserName))
		return(Current);
	else
		if(Current->Next == NULL)
			return(NULL);
		else
			SearchUser(Current->Next, TargetUserName);  ** end Recursive Version **/

}


/*********************************************************
* Special UserSearch supporting the Meta !!
* Call from IsInGroup (test : 22/04/01).
*
* INPUT :
* TargetUserName   : is the From or To extracted from the hnote
* Current_UserName : is a Meta.. contained in a Group (Parameter)
***********************************************************/
SpUser* sSearchUser(SpUser* Current, char* TargetUserName)
{


	int cond = 0;

	while ((cond==0) && Current->Next != NULL)
	{

		cond = (sMatch(TargetUserName, Current->UserName));

		/* not found, continue ! */
		if (cond == 0)
			Current = Current->Next;
	}

	/* Return value */
	if (Current->Next == NULL)
		return(NULL);
	else
		return(Current);

}







/***********************************************************************
 * SearchGroup(SpUserItem, GroupName);
 *
 * Returns (True, False).
 **********************************************************************/
GrpList* SearchGroup(GrpList* Current, char* GroupName)
{


	int cond = 1;

	while (cond && Current->Next != NULL)
	{
		cond = strcmp(Current->GroupName, GroupName);

		if (cond)
			Current = Current->Next;
	}

	/* Return value */
	if (Current->Next == NULL)
		return(NULL);
	else
		return(Current);




	/************* Recursive Version ********************
	if (!strcmp(Current->GroupName, GroupName))
		return(Current);
	else
		if(Current->Next == NULL)
			return(NULL);
		else
			SearchGroup(Current->Next, GroupName); *********** end of rec ****/

}




/****************************************************************
 * CreateUserList()
 * Create the End of List mark up : NULL, return a pointer on
 * the current item of the list.
 ******************************************************************/
void CreateUserList(SpUser* SpecialUserList)
{
	SpecialUserList->UserName = (char *) malloc(10);
	strcpy(SpecialUserList->UserName, "**INIT**");
	SpecialUserList->GroupList = NULL;
	SpecialUserList->Next = NULL;

	return;
}





/***************************************************************
 * Create the fist item of a list of group                     *
 ***************************************************************/
void CreateGroupList(GrpList* GroupList)
{
	GroupList->GroupName = (char *) malloc(10);
	strcpy(GroupList->GroupName, "**INIT**");
	GroupList->Next = NULL;
	return;
}





/********************************************************************
 * GrpList* InsertGroup(GrpList* Current, char* GroupName)
 * add a new item to the group list for a user
 *
 ********************************************************************/
GrpList* InsertGroup(GrpList* Current, char* GroupName)
{

	GrpList* NewItem;
	GrpList* RefItem;

	/* Search for the user, if not found then ADD, if found that search for group, if not found the ADD */

	RefItem = SearchGroup(Current, GroupName);

	if (RefItem == NULL)
	{
		/* Add a new  group within a SpUser */
		  NewItem = (GrpList*) malloc(sizeof(GrpList));
		  if (NewItem != NULL)
		  {
			 NewItem->GroupName = (char *) malloc( strlen(GroupName) + 1 );

			if (NewItem->GroupName != NULL)
				strcpy(NewItem->GroupName, GroupName);

			/* Mark the next which used to be the first */
			NewItem->Next = Current;
		  }

		/* Return the new first of the list */
		return(NewItem);
	}

	/* Nothing new !! */
	return(Current);

}





/*******************************************************************
 * InsertUser(SpUser* Current)
 * Add a new item
 * and returns a new pointer on the beginning of the list
 *
 *******************************************************************/
SpUser* InsertUser(SpUser* Current, char* UserName, char* GroupName, GrpList* GrpHistory)
{
	SpUser* NewItem;
	SpUser* RefItem;

	/* Search for the user, if not found then ADD, if found that search for group, if not found the ADD */
	RefItem = SearchUser(Current, UserName);

	if (RefItem == NULL)
	{

		/* Add a new user */
		NewItem = (SpUser * ) malloc(sizeof( SpUser ));

		/* Test malloc return */
		if (NewItem != NULL)
		{

			/*** alloc dyn */
			NewItem->UserName = (char *) malloc( strlen(UserName) + 1 ) ;


			if (NewItem->UserName != NULL)
			{
				strcpy(NewItem->UserName, UserName);

				/* Create the user group list structure */
				NewItem->GroupList = (GrpList*) malloc (sizeof(GrpList));

				if(NewItem->GroupList != NULL)
				{

					CreateGroupList(NewItem->GroupList);

					/* Add all the item contained in the GrpHistory to the user's GrpList */
					while(GrpHistory->Next != NULL)
					{
						NewItem->GroupList = InsertGroup(NewItem->GroupList, GrpHistory->GroupName);

						/* Loop */
						GrpHistory = GrpHistory->Next;
					}
				}

				else
				/* debug */ AddInLogMessageText("could not get enough memory for (NewItem->GroupList): %s ", NOERROR, UserName);

			}
			else
			/* debug */ AddInLogMessageText("could not get enough memory for (NewItem->Username) : %s ", NOERROR, UserName);


		/* Mark the next which used to be the first */
		NewItem->Next = Current;

		/* the new Beginning of the list (the fist item) */
		return(NewItem);

		}
		else
		/* debug */ AddInLogMessageText("could not get enough memory for : %s ", NOERROR, UserName);
	}

	else
	{

		/* Loop over GrpHistory */
		/* Add all the item contained in the GrpHistory to the user's GrpList */
		while(GrpHistory->Next != NULL)
		{
			/* NewItem->GroupList = InsertGroup(NewItem->GroupList, GroupName);*/
			RefItem->GroupList = InsertGroup(RefItem->GroupList, GrpHistory->GroupName);

			/* Loop */
			GrpHistory = GrpHistory->Next;
		}

		/* do not change the Beginning of the list (the first item) */
		return(Current);
	}

	/* nothing to do - return Current item */
	return(Current);

}





/***********************************************************************
 *                                                               *******
 * Loop over the Black List rules to get the first match or NULL *******
 *                                                               *******
 * old param : char* from, char * to, char* key)                 *******
 ***********************************************************************/
BOOL GetMatchKey(NOTEHANDLE hNote, char* Key, char* from, char *to, int* i)
{
	int	Match = 0;

	while ((*i < MaxRules) & (!Match))
	{

		Match = MMatch(from, to, BlackRedList[*i]);

		if (Match == TRUE)
		{
			/* if there is no formula the BlackRedList.Formula[*i] = "TRUE"   */
			/* Returns TRUE or FALSE, evaluate the macro formula              */
			if (!(strcmp(BlackRedList[*i].Formula, "TRUE") == 0))
		 	  Match = EvalFormula(hNote, BlackRedList[*i].Formula,BlackRedList[*i].Ref );
		}


		(*i)++;
	}

	/* Returns the key to be read in the Black List view. therefore apply to actions  */
    /* chgt obligatoire : Formule_Key := @If(formule = "";""; "µ"+formule);           */
	/* Formule_Key := @If(formule="";"µTRUE"; "µ"+formule);                           */

	/* 3.91 */
	/* Append the current rules rank to the Key */
	/* this rank is = DocNumber in ($BlackList) view [1,2...MaxRules+1] */
	if (Match == TRUE)
	{
	  	sprintf(Key, "%s|%s|%s|%s", BlackRedList[(*i)-1].From, BlackRedList[(*i)-1].To, BlackRedList[(*i)-1].Formula, BlackRedList[(*i)-1].Ref);

#ifdef VERBOSE
		/* Verbose */
		AddInLogMessageText("key : %s", NOERROR, Key );
#endif
	}

	/* End */
	return (Match);
}





/*************************************************************************/
/* 3.9 (TODO) : Remove the DumpGrpList global var.                       */
/*************************************************************************
 * Read the MWSETUP database, the $BLACKLIST view.
 * Get all the Black/Red List rules
 * INPUT : NotesCollection.
 * ----------------------------------------------------------------------
 * 08/09/02	: Fix in 3.91
 * ----------------------
 * Add the rules rank in the Black&red list structure to avoid
 * bad lookup - if the rules key is the same
 * for example : *|*|TRUE it always the first that matches which is returned
 * so the results looks like the same rules applied more than one time.
 *
 * RulesRank is defined by (i) in the collection document.
 * => for (i= 0; i< (int) NotesFound; i++)
 * ----------------------------------------------------------------------
 * 04/03/03 : Fix in 3.91.
 *			: Upload the argument "This Rules Terminates" in order to apply this
 *			: action even if the Rules is to be skipped because of an Exception match
 *			: => new field in BLrules structure.
 * ----------------------------------------------------------------------
 * 30/05/03	: Release 3.92
 *			: New argument to upload user's all alias as a group entry
 * -----------------------------------------------------------------------
 * 19/06/03	: Replace local variable : MainOID by BL_MainOID
 * -----------------------------------------------------------------------
 * 22/04/06	: add ( char** PredicatList, int pred_nbitem)
 *			: GetAllUserAlias, GetUserInGroup
 *************************************************************************/
STATUS GetBlackRedListRules(HCOLLECTION coll_bl, DBHANDLE hDb)
{
	STATUS				error;
	COLLECTIONPOSITION	CollPosition;
	HANDLE				hBuffer = NULLHANDLE;
	DWORD				NotesFound;
	WORD				SignalFlags;
	NOTEID				*IdList;
	int					i;
	NOTEHANDLE			note_handle;
	char				emetteur[MAXUSERNAME];
	char				emetteurabr[MAXUSERNAME];
	char				recipient[MAXUSERNAME];
	char				recipientabr[MAXUSERNAME];
	char				filter_formula[MAX_BL_FORMULA];
	NUMBER				RulesPriority;
	NUMBER				AfterAllRule;
	int					rulesw;
	int					rulesAA;
	ORIGINATORID		BL_MainOID;
	char				DocUniqueID[DOCID];
	char*				PredicatList[MAXUSERNAME];

	/* The rules */
	BLrules		tmpr;
	int			From_User		 = 1;
	int			To_User			 = 1;
	int			From_Except_User = 1;
	int			To_Except_User	 = 1;
	int			pred_nbitem		 = 0;


	/* ----------- init -------------------*/

	/* Group ListHistory */
	GrpList*	tmpGrpList; 			/* save the Current First Element of DumpGrpList */
	GrpList*	GrpHistory;

	GrpHistory = (GrpList *) malloc (sizeof(GrpList));
	if (GrpHistory == NULL)
	{
		AddInLogMessageText("could not get enough memory in %s", NOERROR, "GetBlackRedListRules");
		return(1);
	}

	CreateGroupList(GrpHistory);

	/* Other local vars */
	RulesWithMacro	= 0;	/* global var. reinit */
	MaxRules		= 0;	/* Global var. reinit */
	BL_AFTERALL		= 0;	/* Global var. reinit */
	rulesw			= 0;
	CollPosition.Level = 0;
	CollPosition.Tumbler[0] = 0;

	/* Get a buffer of all the note IDs for the notes in this collection.
	Check the return code to see if the collection is empty. */

	do
	{
		error = NIFReadEntries(
			    coll_bl,			/* handle to this collection */
				&CollPosition,		/* where to start in collection */
			    NAVIGATE_NEXT,		/* order to skip entries */
				1L,					/* number to skip */
				NAVIGATE_NEXT,		/* order to use after skipping */
				0xFFFF,				/* max return number */
				READ_MASK_NOTEID,	/* info we want */
				&hBuffer,			/* handle to info (return)	*/
				NULL,				/* length of buffer (return) */
				NULL,				/* entries skipped (return) */
				&NotesFound,		/* number of notes (return) */
				&SignalFlags);		/* share warning (return) */

		if (error)
		{
	       if(GrpHistory)
   	       {
  		      if(GrpHistory->GroupName)
        		  free(GrpHistory->GroupName);
	           free(GrpHistory);
	        }
    		if(hBuffer != NULLHANDLE)
              OSMemFree(hBuffer);
	        return(error);
		}

		/* Lock down (freeze the location) of the buffer of notes IDs.
			Cast the resulting pointer to the type we need. */

		if (hBuffer != NULLHANDLE)
		{
  				IdList = (NOTEID far *)OSLockObject(hBuffer);

				/* load the list of all the filtering rules in this collection. */
				if (NotesFound > MAXRULESARRAY)
				  NotesFound = MAXRULESARRAY;


			  	for (i= 0; i< (int) NotesFound; i++)
				{

					if (error = NSFNoteOpen(hDb, IdList[i],0, &note_handle))
					{
				 		AddInLogMessageText(ERR_OPEN_NOTE, error, NOMAPI, IdList[i], "MailWatcher Setup");

					}
					else
					{
						/* Read the filter rules - Build the key (From & To) */

						/* 3.95 : GetPredicatsList -(read argument)- */
						/* a field contains @unique(a:b:c:d); a:bc:d.. list of field that have been set [x] as being "Predicats" */
						/* the result will be passed as 3rd argument to GetUserAllAlias(), and a 4fth is FieldList is not ZERO */
						GetFieldInList(note_handle, BLACKLIST_RULES_PREDICAT_LIST, PredicatList, MAXUSERNAME, MAXUSERNAME, &pred_nbitem);					

						/* Read From */
						if (!NSFItemGetText(note_handle, "Sender", emetteur, sizeof(emetteur)-1))
							strcpy(emetteur, "*");
						DNAbbreviate(0L, NULL, emetteur, emetteurabr, MAXUSERNAME, NULL);

						/* Read To */
						if (!NSFItemGetText(note_handle, "Recipient", recipient, sizeof(recipient)-1))
							strcpy(recipient, "*");
						DNAbbreviate(0L, NULL, recipient, recipientabr, MAXUSERNAME, NULL);

						/* Read Priority */
						if (!NSFItemGetNumber(note_handle, "Priority", &RulesPriority))
							RulesPriority = 0; /* default : */

						#ifdef OS390
						/* Notes gave us an IEEE double which must be converted to native */
							ConvertIEEEToDouble(&RulesPriority, &RulesPriority);
						#endif


						/* Read AfterAll Priority */
						if (!NSFItemGetNumber(note_handle, "AfterAllPriority", &AfterAllRule))
							AfterAllRule = 0; /* default */

						#ifdef OS390
						/* Notes gave us an IEEE double which must be converted to native */
							ConvertIEEEToDouble(&AfterAllRule, &AfterAllRule);
						#endif


						/* Read Formula */
						if (!NSFItemGetText(note_handle, "formule", filter_formula, sizeof(filter_formula)-1))
							strcpy(filter_formula, "TRUE");
						else
						{
							/* returns 1 if one macro is to be executed : this will force ProcBlackList to 
							   compute "Lotus Macro" on fully converted MIME to CD document */
							/* 3.94.34 : do it all the time RulesWithMacro = MacroForCDOnly(filter_formula);	*/
							RulesWithMacro = 1;
						}

						/* if Body are to be appended then RulesWithMacro is set to "1" has the
						hNote will be 2 handle. one with raw mime, the other with MIME to CD conversion */
						if ( NSFItemGetText(note_handle, "BList_MessageAppend", emetteur, MAXUSERNAME -1) )
						{
							if ( emetteur[0] == '1')
								RulesWithMacro = 1;
						}

						/* Read Document unique ID */
						NSFNoteGetInfo(note_handle, _NOTE_OID, &BL_MainOID); /*(return void*) */
						sprintf(DocUniqueID, "%08X%08X%08X%08X",
									 BL_MainOID.File.Innards[1],
									 BL_MainOID.File.Innards[0],
									 BL_MainOID.Note.Innards[1],
									 BL_MainOID.Note.Innards[0]);

						/* reset : user/alias checking */
						From_User		 = 1;
						To_User			 = 1;
						From_Except_User = 1;
						To_Except_User	 = 1;


						/* 04/03/2003 : BlackList Actions (3.91)*/
						GetBlackListAction(&tmpr, note_handle);

						/* fill in */
						UpperCase(emetteurabr);/* From */
						UpperCase(recipientabr); /* To */

						tmpr.From    = (char *) malloc (strlen(emetteurabr) + 1);
						tmpr.To      = (char *) malloc (strlen(recipientabr)+ 1);
						tmpr.Formula = (char *) malloc (strlen(filter_formula) + 1);
						tmpr.Ref     = (char *) malloc (strlen(DocUniqueID) + 1);
						rulesw       = abs((int) RulesPriority);	/* Rev 3.94.62. */
						rulesAA      = (int) AfterAllRule;

						/* Sum up all the AfterAllRule */
						BL_AFTERALL = BL_AFTERALL + rulesAA;

						/* feed the struct */
						strcpy(tmpr.From, emetteurabr);
						strcpy(tmpr.To, recipientabr);
						strcpy(tmpr.Formula, filter_formula);
						strcpy(tmpr.Ref, DocUniqueID);
						tmpr.w = rulesw;
						tmpr.AfterAllRule = rulesAA;

						/* Build up the SpecialUserList */
						switch(rulesw)
						{

							/* ------------------------------------------------------------------------------------------------*/
							/* From is a string or a user with extension to get all user's alias, To is a string or a user     */
							case 11:
								/* -- FROM is a user --*/
								if (From_User)
								{
									/* if error then From_user is ZERO so the RulesW will not be changed so user will remain user and not Group */
									if ( GetUserAllAlias(tmpr.From, GrpHistory, PredicatList, pred_nbitem) )
										From_User = 0;
								}

								/* -- TO is a user ---*/
								if (To_User)
								{
									if ( GetUserAllAlias(tmpr.To, GrpHistory, PredicatList, pred_nbitem) )
										To_User = 0;
								}

								/* reset the rules type */
								ResetRulesw(&tmpr.w, From_User, To_User);

								break;

							case 13:
								/* -- FROM is a user --*/
								if (From_User)
								{
									/* if error then From_user is ZERO so the RulesW will not be changed so user will remain user and not Group */
									if ( GetUserAllAlias(tmpr.From, GrpHistory, PredicatList, pred_nbitem) )
										From_User = 0;

									/* reset the rules type : To_User must be ZERO */
									ResetRulesw(&tmpr.w, From_User, 0);
								}
							break;

							case 31:
								/* -- TO is a user  ---*/
								if (To_User)
								{
									if ( GetUserAllAlias(tmpr.To, GrpHistory, PredicatList, pred_nbitem) )
										To_User = 0;

									/* reset the rules type : From_User must be ZERO */
									ResetRulesw(&tmpr.w, 0, To_User);
								}
								break;

							/* --------------------------------------------------------------------------*/
							/* From is a string or a user with extension to all his alias, To is a Group */
							case 12: case 14:
								/* -- FROM is a user --*/
								if (From_User)
								{
									/* if error then From_user is ZERO so the RulesW will not be changed so user will remain user and not Group */
									if ( GetUserAllAlias(tmpr.From, GrpHistory, PredicatList, pred_nbitem) )
										From_User = 0;

									/* reset the rules type : To_User must be ZERO */
									ResetRulesw(&tmpr.w, From_User, 0);
								}

								/* ------ To is a group or a meta group -------------*/
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.To); /* mark as dump ! */

								/* Dump only if not already dumped */
								/* 3.8a / 4/12/01 : dump anyway because of problems found with group that contains groups already dump ! - the user are not mark as member of the current group */
								/* if (tmpGrpList != DumpGrpList) */
								/* {*/
									GrpHistory = InsertGroup(GrpHistory, tmpr.To); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.To, &GrpHistory, PredicatList, pred_nbitem);  /* modif : 23/4/01 */
								/*}*/
								break;

							/* From is a string, To is a Group */
							case 32:  case 34:
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.To); /* mark as dump ! */

								/* Dump only if not already dumped */
								/* 3.8a / 4/12/01 : dump anyway because of problems found with group that contains groups already dump ! - the user are not mark as member of the current group */
								/* if (tmpGrpList != DumpGrpList) */
								/* {*/
									GrpHistory = InsertGroup(GrpHistory, tmpr.To); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.To, &GrpHistory, PredicatList, pred_nbitem);  /* modif : 23/4/01 */
								/*}*/

								break;

							/* From is a group, To is a string or a user with extension to all user's alias		*/
							case 21: case 41:
								/* --------- TO is a user -------------*/
								if (To_User)
								{
									if ( GetUserAllAlias(tmpr.To, GrpHistory, PredicatList, pred_nbitem) )
										To_User = 0;

									/* reset the rules type : From_User must be ZERO */
									ResetRulesw(&tmpr.w, 0, To_User);
								}

								/* ------ FROM -------------*/
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.From); /* mark as dump ! */

								/* Dump only if not already dumped */
								/* if (tmpGrpList != DumpGrpList) */
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.From); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.From, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/

								break;


							/* From is a group, To is a string */
							case 23: case 43:
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.From); /* mark as dump ! */

								/* Dump only if not already dumped */
								/* if (tmpGrpList != DumpGrpList) */
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.From); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.From, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;


							/* From and To are Groups */
							case 22: case 24: case 42: case 44:
								/* To */
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.To); /* mark as dump ! */
								/* Dump only if not already dumped */
								/* if (tmpGrpList != DumpGrpList) */
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.To); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.To, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/


								/* From */
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.From); /* mark as dump ! */
								/* Dump only if not already dumped */
								/* if (tmpGrpList != DumpGrpList) */
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.From); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.From, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;


							default:
								break;

						} /* end switch */


						/*****************************************************************************************/
						/************************************** EXCEPTIONS ITEM **********************************/
						/*****************************************************************************************/

						/* Read Exception From */
						if (!NSFItemGetText(note_handle, "Except_Sender", emetteur, sizeof(emetteur)-1))
							strcpy(emetteur, "-");
						DNAbbreviate(0L, NULL, emetteur, emetteurabr, MAXUSERNAME, NULL);

						/* Read Exceptions : To */
						if (!NSFItemGetText(note_handle, "Except_Recipient", recipient, sizeof(recipient)-1))
							strcpy(recipient, "-");
						DNAbbreviate(0L, NULL, recipient, recipientabr, MAXUSERNAME, NULL);

						/* Read Exception weight rules : Except_w */
						if (!NSFItemGetNumber(note_handle, "Except_Priority", &RulesPriority))
							RulesPriority = 0; /* default : */

						#ifdef OS390
						/* Notes gave us an IEEE double which must be converted to native */
							ConvertIEEEToDouble(&RulesPriority, &RulesPriority);
						#endif

						/* Convert and Memory alloc */
						UpperCase(emetteurabr);/* Except_From */
						UpperCase(recipientabr); /* Except_To */
						rulesw = (int) RulesPriority;

						tmpr.Except_From = (char *) malloc (strlen(emetteurabr) + 1);
						tmpr.Except_To   = (char *) malloc (strlen(recipientabr) + 1);

						/* feed the struct for Except_[variables] */
						strcpy(tmpr.Except_From, emetteurabr);
						strcpy(tmpr.Except_To, recipientabr );
						tmpr.Except_w = rulesw;


						/* Build up the SpecialUserList : for Exception items */
						switch(rulesw)
						{

							/* ------------------------------------------------------------------------------------------------*/
							/* From is a string or a user with extension to all his alias, To is a string or a user with alias */
							case 11:
								/* -- Except_FROM is a user --*/
								if (From_Except_User)
								{
									/* if error then From_user is ZERO so the RulesW will not be changed so user will remain user and not Group */
									if ( GetUserAllAlias(tmpr.Except_From, GrpHistory, PredicatList, pred_nbitem) )
										From_Except_User = 0;
								}

								/* -- Except_TO is a user ---*/
								if (To_Except_User)
								{
									if ( GetUserAllAlias(tmpr.Except_To, GrpHistory, PredicatList, pred_nbitem) )
										To_Except_User = 0;
								}

								/* reset the rules type */
								ResetRulesw(&tmpr.Except_w, From_Except_User, To_Except_User);
								break;

							case 13:
								/* -- FROM is a user -- */
								if (From_Except_User)
								{
									/* if error then From_user is ZERO so the RulesW will not be changed so user will remain user and not Group */
									if ( GetUserAllAlias(tmpr.Except_From, GrpHistory, PredicatList, pred_nbitem) )
										From_Except_User = 0;

									/* reset the rules type : To Except is ZERO  (Meta = 3)*/
									ResetRulesw(&tmpr.Except_w, From_Except_User, 0);
								}

								/* To is a meta */
								break;

							case 31:
								/* From is a meta */

								/* -- TO is a user --*/
								if (To_Except_User)
								{
									if ( GetUserAllAlias(tmpr.Except_To, GrpHistory, PredicatList, pred_nbitem) )
										To_Except_User = 0;

									/* reset the rules type : FROM Except is ZERO  (Meta = 3)*/
									ResetRulesw(&tmpr.Except_w, 0, To_Except_User );
								}
								break;

							/* ------------------------------------------------------------------------------------------------*/
							case 12: case 14:
								/* -- FROM is a user or just an address --*/
								if (From_Except_User)
								{
									/* if error then From_user is ZERO so the RulesW will not be changed so user will remain user and not Group */
									if ( GetUserAllAlias(tmpr.Except_From, GrpHistory, PredicatList, pred_nbitem) )
											From_Except_User = 0;

									/* reset the rules type : To Except is ZERO  (Meta = 3)*/
									ResetRulesw(&tmpr.Except_w, From_Except_User, 0);
								}


								/*-- TO is a group or a meta group -- */
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.Except_To); /* mark as dump ! */
								/* Dump only if not already dumped */
								/* if (tmpGrpList != DumpGrpList) */
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.Except_To); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.Except_To, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;


							case 32: case 34:
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.Except_To); /* mark as dump ! */
								/* Dump only if not already dumped */
								/* if (tmpGrpList != DumpGrpList) */
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.Except_To); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.Except_To, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;


							case 23: case 43:
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.Except_From); /* mark as dump ! */

								/* Dump only if not already dumped */
								/*if (tmpGrpList != DumpGrpList)*/
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.Except_From); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.Except_From, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;


							case 21: case 41:
								/*-- TO is user or just an address (string) -- */
								if (To_Except_User)
								{
									if ( GetUserAllAlias(tmpr.Except_To, GrpHistory, PredicatList, pred_nbitem) )
										To_Except_User = 0;

									/* reset the rules type : FROM Except is ZERO  (Meta = 3)*/
									ResetRulesw(&tmpr.Except_w, 0, To_Except_User );
								}


								/* -- FROM is a Group --*/
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.Except_From); /* mark as dump ! */

								/* Dump only if not already dumped */
								/*if (tmpGrpList != DumpGrpList)*/
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.Except_From); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.Except_From, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;


							case 22: case 24: case 42: case 44:
								/* To */
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.Except_To); /* mark as dump ! */

								/* Dump only if not already dumped */
								/*if (tmpGrpList != DumpGrpList)*/
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.Except_To); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.Except_To, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/


								/* From */
								tmpGrpList = DumpGrpList;
								DumpGrpList = InsertGroup(DumpGrpList, tmpr.Except_From); /* mark as dump ! */

								/* Dump only if not already dumped */
								/*if (tmpGrpList != DumpGrpList)*/
								/*{*/
									GrpHistory  = InsertGroup(GrpHistory,  tmpr.Except_From); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */
									GetUserInGroup(tmpr.Except_From, &GrpHistory, PredicatList, pred_nbitem);
								/*}*/
								break;

							default:
								break;

						} /* end Exception item switch */


						MaxRules++;				/* 0,1,2 ... */

						/* save in memory */
						tmpr.rank = MaxRules;		/* 3.91: 1,2,n => save this rules 'rank' in the list */
						BlackRedList[i] = tmpr;


						/** Close the NotesDocument **/
						NSFNoteClose(note_handle);


						/* free predicats list */
						if ( pred_nbitem > 0 )
							FreeMemory_GetFieldInList(PredicatList, pred_nbitem);

					}
				} /* for */

				/* Unlock the list of note IDs. */
		  		OSUnlockObject(hBuffer);

				/* Free the memory allocated by NIFReadEntries. */
				OSMemFree(hBuffer);
			 }

		/* repeat loop if more signal flag is set */

	} while (SignalFlags & SIGNAL_MORE_TO_DO);



	/* EVAL Limit */
	#ifdef EVAL
		if (MaxRules > MAXEVAL)
		{
			AddInLogMessageText("Nb of Black-List rules : %d  (NB Max is %d)", NOERROR, MaxRules, MAXEVAL, "MailWatcher Eval Warning");
			MaxRules = MAXEVAL;
		}
	#endif
	/* End of EVAL */


	/* Free Group History */
	if(GrpHistory)
	  {
		if(GrpHistory->GroupName)
		  free(GrpHistory->GroupName);
	    free(GrpHistory);
	  }

	/* sizeof (Black List) */
	return (error);
}

/*************************************************************************/






/********************************************/
/* Open the names.nsf on the current server */
/* Check out for view : $Group              */
/* 3.81   :                                 */
/* INPUT  : none => char *                  */
/* OUTPUT : STATUS                          */
/********************************************/
STATUS OpenNAB(char* dbPath)
{

	STATUS error = NOERROR;
	NOTEID viewmb_id;

    /* Open the Db */
	if (error = NSFDbOpen(dbPath, &hDbNAB))
	{

		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, dbPath);
		AddInLogMessageText(POLSTOP, error);

		/* Disable the advblck operations - 3.8/build2 */
		advblck = FALSE;

		return(error);
	}



	/*************** Vue des black $Group *********************************************/
	if (error = NIFFindDesignNote(hDbNAB, "($VIMGroups)", NOTE_CLASS_VIEW, &viewmb_id))
	  {
		AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, "($VIMGroups)", dbPath);
		AddInLogMessageText(POLSTOP, error);
		NSFDbClose(hDbNAB);

		/* Disable the advblck operations - 3.8/build2 */
		advblck = FALSE;

		return(error);
	  }


	/************* $Group collection **********************/
	 if (error = NIFOpenCollection(hDbNAB, hDbNAB, viewmb_id, 0,
									NULLHANDLE, &coll_grp,
									NULL, NULL, NULL, NULL))
	  {


			AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI, "($VIMGroups)", dbPath);
			AddInLogMessageText(POLSTOP, error);
			NSFDbClose(hDbNAB);

			/* Disable the advblck operations - 3.9 (01/05/02) */
			advblck = FALSE;

			return(error);
	  }

	 /* Continue ? */
	 return(error);
}




STATUS CloseNAB()
{
	STATUS erreur;

	NIFCloseCollection(coll_grp);	/* handle on the names.nsf group view */

	erreur = NSFDbClose(hDbNAB);
	return(erreur);
}




/*********************************************************************************
 *
 * Read a group, create all the final user entry in the SpecialUSerList
 * coll_grp, is global.
 *
 * Fix : 31/10/01.
 * Do (PopGroupHistory) in the group does not exists..!
 * ------------------------------------------------------------------------------
 * Fix : 15/04/02.
 * the global var DumpGroup is not to be used to test if a group has to be dumped
 * the Local group pile is to be used instead.
 * ------------------------------------------------------------------------------
 * 31/05/03 : Release 3.92
 * Add all user's alias
 *********************************************************************************/
void GetUserInGroup(char* GroupName, GrpList** GrpHistory, char** PredicatList, int pred_nbitem)
{

	STATUS error;
	COLLECTIONPOSITION coll_pos;
	DWORD atraiter;
	WORD nbrec, i;
	HANDLE buffer_fhandle = NULLHANDLE;
	DWORD *id_flist;
	NOTEHANDLE hNoteGroup;
	GrpList* tmpGrpList; 			/* Current First Ele of DumpGrpList */


	char GroupItem[MAXUSERNAME];
	char GroupItemAbb[MAXUSERNAME];

	/* Start */
	error = NIFFindByName(coll_grp, GroupName, FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE,
							    &coll_pos, &atraiter);
	if (error)
	{

		AddInLogMessageText(ERR_OPEN_GROUP, error, NOMAPI, GroupName);
		/* Remove the current group so the last (but in the fact the first of the pile) */
		PopGroupHistory(GrpHistory);
		return;
	}

		/* Read the documents then all the MEMBERS */
	if (error = NIFReadEntries(coll_grp, &coll_pos, NAVIGATE_CURRENT, 0,
								NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID,
								&buffer_fhandle, NULL, NULL, &atraiter, NULL))
	{

		if (buffer_fhandle != NULL)
			OSMemFree(buffer_fhandle);
		AddInLogMessageText(ERR_READ_VIEW, error, NOMAPI, "($VIMGroup)", "names.nsf");
		/* Remove the current group so the last (but in the fact the first of the pile) */
		PopGroupHistory(GrpHistory);
		return;
	}



	/* Now let's read the doc field (members) */
	if ((buffer_fhandle != NULLHANDLE) && (atraiter > 0))
	{
		id_flist = (NOTEID *) OSLockObject(buffer_fhandle);

		if (error = NSFNoteOpen(hDbNAB, id_flist[0], 0, &hNoteGroup))
		{
			OSUnlockObject(buffer_fhandle);
			OSMemFree(buffer_fhandle);
			AddInLogMessageText(ERR_OPEN_NOTE, error, NOMAPI, id_flist[0], "names.nsf");

			/* Remove the current group so the last (but in the fact the first of the pile) */
			PopGroupHistory(GrpHistory);

			return;
		}

		/************** Loop over all the members **********************/
		nbrec = NSFItemGetTextListEntries(hNoteGroup, "members");

		for (i=0; i<nbrec; i++)
		{
			NSFItemGetTextListEntry(hNoteGroup, "members", i, GroupItem, sizeof(GroupItem)-1);
			UpperCase(GroupItem);

			/* Recursive call if "GROUPITEM" is also a group and has not been already Dump !!! */
			if (IsaGroupInNab(GroupItem))
			{
						/* This global variable is maintained but is not used anymore as the Local Group Pile is (*GrpHistory) */
				tmpGrpList = DumpGrpList;
				DumpGrpList = InsertGroup(DumpGrpList, GroupItem); /* mark as dump ! */

				/* Fix : 15/04/2002 */
				/* Avoid infinite loop but DO NOT work on the global var DumpGrpList */
				tmpGrpList = (*GrpHistory);
				(*GrpHistory) = InsertGroup((*GrpHistory), GroupItem); /* fix : 23/4/01 : Start the Group history required for group membership inheritance */

				if (tmpGrpList != (*GrpHistory))
				{
					GetUserInGroup(GroupItem, GrpHistory, PredicatList, pred_nbitem); /* Dump now ! */
				}
			}
			/* Standard user */
			else
			{
				DNAbbreviate(0L, NULL, GroupItem, GroupItemAbb, MAXUSERNAME, NULL);
				FirstEle = InsertUser(FirstEle, GroupItemAbb, GroupItem, *GrpHistory);

				/* 3.92 : GroupItem is a member : see if it has some alias to be uploaded too */
				GetUserAllAlias(GroupItem, *GrpHistory, PredicatList, pred_nbitem);		/* 3.95: work to do here !!!!!! */
			}
		} /* end of for */

		/********************* end of members loop ********************/

		/* Close the group doc */
		NSFNoteClose(hNoteGroup);

		/* Remove the current group so the last (but in the fact the first of the pile) */
		PopGroupHistory(GrpHistory);
	}

	/* Free up the NifReadEntries returned buffer */
	OSUnlockObject(buffer_fhandle);
	OSMemFree(buffer_fhandle);

	/* end of prg */
	return;
}





/************************************************************************
 * Check if the GroupItem is a group or Not in the Names (open)
 * Group view Collection, is a global var. (coll_grp)
 *
 *************************************************************************/
BOOL IsaGroupInNab(char* GroupName)
{

	 COLLECTIONPOSITION coll_pos;
	 DWORD	match_size;
	 STATUS	error;

	 /* Select the document where to read the parameters */
	error = NIFFindByName (
			coll_grp,			  		/* collection to look in */
			GroupName,  	    	    /* string to match on */
			FIND_CASE_INSENSITIVE,		/* match rules */
			&coll_pos,        			/* where match begins (return) */
			&match_size);      			/* how many match (return) */


	return(error == NOERROR);
}







/**************************************************************************
*                                                                         *
* FreeMemory (data structure, with .Next as member                        *
*                                                                         *
* ------------------------------------------------------------------------
* 3/08/03	: fix memory leak : in FreeMemory
*			: in freeing memory of global memory :  DumpGrpList
*			: free(GrpFirst->GroupName)
*			: --
*			: Check First : if (FirstEle != NULL)
**************************************************************************/
void FreeMemory()
{

	SpUser* TmpUserDelete;
	SpUser* LocalFirstEle;
	GrpList* TmpGrpDelete;
	GrpList* GrpFirst;
	int i;

	/*** Global variables ***/
	/* SpUser* FirstEle;				list of user and their groups     */
	/* GrpList* DumpGrpList;			List of group, recursive checking */
	/************************/

	/******** USERS *****************/

	if (FirstEle != NULL)
	{
		LocalFirstEle = FirstEle;
		GrpFirst = NULL;

		while (LocalFirstEle->Next !=NULL)
		{
			/* Loop over the grp */
			GrpFirst = LocalFirstEle->GroupList;

			while (GrpFirst->Next != NULL)
			{
				TmpGrpDelete = GrpFirst->Next;

				/* Display what is to be cleared */
				/* AddInLogMessageText("FreeMemory : %s in Group : %s ", NOERROR, LocalFirstEle->UserName, GrpFirst->GroupName ); */

				free(GrpFirst->GroupName);
				free(GrpFirst);

				/* next */
				GrpFirst = TmpGrpDelete;
			}

			/*----------- free user header struct ------------*/
			/* - Group header -*/
			if (GrpFirst->GroupName != NULL)
				free(GrpFirst->GroupName);		/* contains '** init **' */

			if (GrpFirst != NULL)
				free(GrpFirst);					/* user group list header : ** init **/

			/*----------------- Next ---------------------*/
			/* save the next user */
			TmpUserDelete = LocalFirstEle->Next;

			/* Display what is to be cleared */
			/* AddInLogMessageText("FreeMemory  user: %s ", NOERROR, LocalFirstEle->UserName); */

			free(LocalFirstEle->UserName);	/* username */
			free(LocalFirstEle);			/* init size */

			/* -- restore -- **/
			LocalFirstEle = TmpUserDelete;
		}

		/* Kill the last */
		/* AddInLogMessageText("FreeMemory Last user : %s in Group : %s ", NOERROR, LocalFirstEle->UserName, GrpFirst->GroupName ); test */

		/* before doing the free memory on the GroupName list, check if there is any group !!! */
		/* Fixed : 28/12/00 */
		if (LocalFirstEle->GroupList != NULL)
		{
			if (LocalFirstEle->GroupList->GroupName != NULL)
				free(LocalFirstEle->GroupList->GroupName);
		}

		if (LocalFirstEle->UserName != NULL)
			free(LocalFirstEle->UserName);

		if (LocalFirstEle->GroupList != NULL)
			free(LocalFirstEle->GroupList);

		if (LocalFirstEle != NULL)
			free(LocalFirstEle);


		/********* GROUPS, Recursive groups checking list ************/
		GrpFirst = DumpGrpList;

		while (GrpFirst->Next !=NULL)
		{
			/* next user */
			TmpGrpDelete = GrpFirst->Next;

			free(GrpFirst->GroupName);
			free(GrpFirst);

			/* next */
			GrpFirst = TmpGrpDelete;
		}

		/* kill the last */
		if (GrpFirst->GroupName != NULL)
			free(GrpFirst->GroupName);

		if (GrpFirst != NULL)
			free(GrpFirst);


		/** Free up BlackList */
		for (i = 0; i < MaxRules; i++)
		{
			free(BlackRedList[i].From);
			free(BlackRedList[i].To);
			free(BlackRedList[i].Formula);
			free(BlackRedList[i].Ref);
			free(BlackRedList[i].Except_From);
			free(BlackRedList[i].Except_To);
		}
	} /* FirstEle != NULL */
}


/************************************************************************
* DumpBlackList : Create a file containg Sp data
*************************************************************************/
void DumpBlackList(SpUser *FirstEle)
{
	int cond = 1;
	SpUser* LocalFirstEle;
	GrpList* GrpFirst;
	FILE* fpt;

	fpt = fopen("dumpblacklist.txt","w");

	if (!(fpt == NULL))
	{

		LocalFirstEle = FirstEle;

		while (cond & (LocalFirstEle->Next !=NULL))
		{
			fprintf(fpt, "\n [ %s ] \n", LocalFirstEle->UserName);

			/* Loop over the grp */
			GrpFirst = LocalFirstEle->GroupList;

			while (GrpFirst->Next != NULL)
			{
				fprintf(fpt, "%s, ", GrpFirst->GroupName);
				GrpFirst = GrpFirst->Next;
			}

			/* next user */
			fprintf(fpt, "\n");
			LocalFirstEle = LocalFirstEle->Next;
		}

		fclose(fpt);
	}


	return;
}




/******************************************************
 * Exception MetaMatch.
 *
 ------------------------------------------------
 ! Except_From	!	Except_To	!	Total		!
 ------------------------------------------------
 User	= 1			User	= 1
 Group	= 2			Group	= 2
 Meta	= 3			Meta	= 3
 User & User						11
 User & Group						12
 User & Meta						13
 Group & User						21
 Group & Group						22
 Group & Meta						23
 Meta & User						31
 Meta & Group						32
 Meta & Meta						33
 *****************************************************/
int Except_Match(char* From, char* To, BLrules CurrentRules)
{
	int result  = 0;
	int result1 = 0;
	int result2 = 0;
	int PatternMatch;

	/* If one Exception is found then return TRUE          */
	/* (Except_From OR Except_To => DO NOT APPLY THE RULE) */

	PatternMatch = CurrentRules.Except_w;

	/* result1 = Except_From */
	/* result2 = Except_To   */

	switch(PatternMatch)
	{
		case 12: case 32:
			result2 = IsInGroup(To, CurrentRules.Except_To);
			if (!result2)
				result1 = sMatch(From, CurrentRules.Except_From);
			break;


		case 21: case 23:
			result1 = IsInGroup(From, CurrentRules.Except_From);
			if (!result1)
				result2 = sMatch(To, CurrentRules.Except_To);
			break;


		case 22:
			result2 = IsInGroup(To, CurrentRules.Except_To);
			if (!result2)
				result1 = IsInGroup(From, CurrentRules.Except_From);
			break;


		/* 7 new cases	: (4) is for a Group containing meta strings */
		/* case value	: left figure  value is EXCEPT_FROM */
		/*				: right figure value is EXCEPT_TO  */


		case 14 : case 34 :
			result2 = IsInGroupWmeta(To, CurrentRules.Except_To);
			if (!result2)
				result1 = sMatch(From, CurrentRules.Except_From);
			break;


		case 41: case 43:
			result1 = IsInGroupWmeta(From, CurrentRules.Except_From);
			if (!result1)
				result2 = sMatch(To, CurrentRules.Except_To);
			break;


		case 42 :
			result1 = IsInGroupWmeta(From, CurrentRules.Except_From);
			if (!result1)
				result2 = IsInGroup(To, CurrentRules.Except_To);
			break;

		case 24 :
			result1 = IsInGroup(From, CurrentRules.Except_From);
			if (!result1)
				result2 = IsInGroupWmeta(To, CurrentRules.Except_To);
			break;

		case 44 :
			result2 = IsInGroupWmeta(To, CurrentRules.Except_To);
			if (!result2)
				result1 = IsInGroupWmeta(From, CurrentRules.Except_From);
			break;




		default:
			result1 = sMatch(From, CurrentRules.Except_From);
			if (!result1)
				result2 = sMatch(To, CurrentRules.Except_To);
			break;
	}

	/* Return */
	result = result1 ||result2;

#ifdef VERBOSE
	AddInLogMessageText("(%s) : [%s] > %d ", NOERROR, From, CurrentRules.Except_From, result1);
	AddInLogMessageText("(%s) : [%s] > %d ", NOERROR, To, CurrentRules.Except_To, result2);
	AddInLogMessageText(" w : %d   - MATCH > %d ", NOERROR, PatternMatch, result);

#endif

	return(result);
}




/**************************************************************************************************
 * Non delivery message. Based on Back List Rules Document parameter
 *
 * Reads : 'From' field from the e.mail
 * Reads :  The body, the subject is "Delivery failure " & the e.mail subject
 *
 * TODO :
 * Append to the original subject a "string" : Delivery failure or whatsoever
 *
 * (option)
 * Append a Doclink in case of a "Notify" (=> 3.91)
 * Append a URL type link for SMTP mail
 * ------------------------------------------------------------------------------------------------
 * 3.93.5	: change FROM field the 'from' value is FilerDoc is not NULL.
 *			: Append the email Header if 'from' is not null.
 ***************************************************************************************************/
void NonDeliveryBlackList(DBHANDLE hDB, NOTEHANDLE horig_note, char * from, char *sendto, char* sendtobis,
						  char* forwardto, char* SubjectHeader, char *origsubject, NOTEHANDLE hBL_Note,
						  char* Append_Orig_Body, DBHANDLE* hMb, char* Orig_from, char* Orig_To,
						  char * Original_Body_Attach)
{
	STATUS			erreur;
	NOTEHANDLE		hMes;
	HANDLE			phCompound;
	TIMEDATE		tdCurrent;
	char			sujet[15000];
	int				doclink = 0;
	int				nbrec = 0;
	NOTEHANDLE		Doc4Body;			/* hNote used to append Body field */
	NOTEHANDLE		hNewNoteCD;			/* CD foramt email for horig_note  */
	int				bOrigNoteCD = 0;	/* required to track MIME to CD conversion */

	/* Create a document in the current mail(x).box */
	if (erreur = NSFNoteCreate(hDB, &hMes))
	{
		AddInLogMessageText(ERR_CREATE_DOC, erreur, NOMAPI, "mail.box");
		return;
	}

	/* Masque */
	NSFItemSetText(hMes, "Form", "Memo", 4);

	/* 3.93.5.2 : Change From & Principal if from is not empty */
	if ( strlen( from ) )
	{
		/* emetteur  : 3.9 : from is stored the hNoteFiltre				   */
		NSFItemSetText(hMes, "From",      from, (WORD) strlen(from));
		NSFItemSetText(hMes, "Principal", from, (WORD) strlen(from));
	}
	else
	{
		NSFItemSetText(hMes, "From",      Orig_from, (WORD) strlen(Orig_from));
		NSFItemSetText(hMes, "Principal", Orig_from, (WORD) strlen(Orig_from));
	}

	/* Still if sendto is "0" is there then DO NOT use the original From value ! */
	if (strcmp(sendto,"0"))
	{
		NSFItemAppendTextList(hMes, "SendTo", sendto, (WORD) strlen(sendto), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", sendto, (WORD) strlen(sendto), FALSE);
		nbrec = 1;
	}

	/* if sendtobis <> NULL => Append */
	if (strcmp(sendtobis, "NULL"))
	{
		NSFItemAppendTextList(hMes, "SendTo",     sendtobis, (WORD) strlen(sendtobis), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", sendtobis, (WORD) strlen(sendtobis), FALSE);
		nbrec = 1;
	}

	/* if forwardto <> "NULL" => Append */
	if (strcmp(forwardto, "NULL"))
	{
		NSFItemAppendTextList(hMes, "SendTo",     forwardto, (WORD) strlen(forwardto), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", forwardto, (WORD) strlen(forwardto), FALSE);
		nbrec = 1;
	}

	if (nbrec > 0)
	{

		/* date d'envoi */
		OSCurrentTIMEDATE(&tdCurrent);
		NSFItemSetTime(hMes, "PostedDate", &tdCurrent);

		/*- Subject -*/
		/* 3.92 : new : append subject if not null */
		sujet[0]='\0';
		if ( strlen( origsubject) )
			sprintf(sujet, "%s : %s", SubjectHeader, origsubject);
		else
			sprintf(sujet,"%s", SubjectHeader);

		/* subject */
		NSFItemSetText(hMes, "Subject", sujet, (WORD) strlen(sujet));

		/* Body (Copy item from one document to another ) */
		if (erreur = CompoundTextCreate(hMes, "Body", &phCompound))
			return;

		/* Use the orginal note Body  (do it if not == "0") */
		if (strcmp(Append_Orig_Body,"0"))
		{
			/* Convert to CD to append Body + Body */
			bOrigNoteCD = TemporaryhNoteMIME2CD(hDB, horig_note, &hNewNoteCD);
			if (bOrigNoteCD)
				Doc4Body = hNewNoteCD;
			else
				Doc4Body = horig_note;

			/*  */
			if (Append_Orig_Body[0] == '1')
			{
				/* 3.92.5.2 Append new header if the 'from' is different of the original from */
				if ( strlen( from ) )
				{
					/* Original Header */
					if (erreur = AppendMailHeaderToBody(horig_note, &phCompound, Orig_from, Orig_To ,origsubject))
					{
						CompoundTextDiscard(phCompound);
						/* Erase HOLD mail : Doc4Body */
						if (bOrigNoteCD)
						{
							NSFNoteClose( hNewNoteCD );
							/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
						}
						return;
					}
				}

				/* original body -> (TOP == 1) */
				if (erreur = CompoundTextAssimilateItem(phCompound, Doc4Body, "Body", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
					/*	erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}

				/* Filter Body (AFTER) */
				if (erreur = CompoundTextAssimilateItem(phCompound, hBL_Note, "BL_DeliveryFailure", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}
			}
			else
			{
				/* Filter Body (TOP) */
				if (erreur = CompoundTextAssimilateItem(phCompound, hBL_Note, "BL_DeliveryFailure", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
						NSFNoteClose( hNewNoteCD );
					}
					return;
				}

				/* Original Header */
				if ( strlen(from) )
				{
					if (erreur = AppendMailHeaderToBody(horig_note, &phCompound, Orig_from, Orig_To, origsubject))
					{
						CompoundTextDiscard(phCompound);
						/* Erase HOLD mail : Doc4Body */
						if (bOrigNoteCD)
						{
							NSFNoteClose( hNewNoteCD );
							/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
						}
						return;
					}
				}

				/* original body -> (BOTTOM == 2) */
				if (erreur = CompoundTextAssimilateItem(phCompound, Doc4Body, "Body", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}
			}

			/* 3.93.19 DO NOT Purge temporary CD format but close it if it exists */
			if (bOrigNoteCD)
			{
				NSFNoteClose( hNewNoteCD );
				/*	erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
			}
		}
		/* -------------------------------------------------------------------------------- */
		/* -------------- Only the Filter Body  ------------------------------------------- */
		else
		{
			if ( strlen(from) )
			{
				/* Original Header */
				if ( erreur = AppendMailHeaderToBody(horig_note, &phCompound, Orig_from, Orig_To, origsubject) )
				{
					CompoundTextDiscard(phCompound);
					return;
				}
			}

			/* The filter body */
			if ( erreur = CompoundTextAssimilateItem(phCompound, hBL_Note, "BL_DeliveryFailure", 0L) )
			{
				CompoundTextDiscard(phCompound);
				return;
			}
		}

		if (erreur = CompoundTextClose (phCompound, 0, 0L, NULL, 0))
			CompoundTextDiscard(phCompound);

		/* Let the message GO, DO NOT STOP IT WITH THE EXT_MGR */
		NSFItemSetText(hMes, "NOT_EM", "1", 1);

		/* Fix SMLN; Patch 21/02/2003 */
		/* Add MWADVT in the MWTAG list */
		/* NSFItemAppendTextList(hMes, "MWTAG", "MWADVT", 6, FALSE); */

		/* Check if the original Body has been appended and if the $FILE are to be purged ?     */
		if (strcmp(Append_Orig_Body,"0"))
		{
			/* Call PurgeAttachFile now */
			if (strcmp(Original_Body_Attach,"0"))
				DeleteAttachFile(hMes, "$FILE");
		}

		/* save Doc */
		NSFNoteUpdate(hMes, UPDATE_FORCE);
	}

	/* close anyway */
	NSFNoteClose(hMes);

	return;
}





/**************************************************************************************************
 * ------------[ Special Version with DocLink ]----------------------------------------------------
 * ================================================================================================
 * Non delivery message. Based on Back List Rules Document parameter
 *
 * Reads : 'From' field from the e.mail
 * Reads :  The body, the subject is "Delivery failure " & the e.mail subject
 *
 * TODO :
 * Append to the original subject a "string" : Delivery failure or whatsoever
 *
 * (option)
 * Append a Doclink in case of a "Notify" (=> 3.91)
 * Append a URL type link for SMTP mail
 * ------------------------------------------------------------------------------------------------
 * Depending		: mwhotspot.c
 *	3.95.10			: MARK THIS TYPE OF MAIL WITH A SPECIAL FIELD.
 *					: MWDOCLINK=1
 ***************************************************************************************************/
void NonDeliveryBlackList_DocLink(DBHANDLE hDB, NOTEHANDLE horig_note, char * from, char *sendto, char* sendtobis,
								  char* forwardto, char* SubjectHeader,
								  char *origsubject, NOTEHANDLE hBL_Note, int DocLink,
								  char* ViewName, TIMEDATE replica_ID, UNID Note_UNID, UNID View_UNID,
								  char* Append_Orig_Body, DBHANDLE* hMb, char* Orig_from, char* Orig_To,
								  char* Original_Body_Attach,
								  char*	Original_MessageID)
{
	STATUS			erreur;
	NOTEHANDLE		hMes;
	HANDLE			phCompound;
	TIMEDATE		tdCurrent;
	char			sujet[15000];
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	int				nbrec = 0;
	NOTEHANDLE		Doc4Body;			/* hNote used to append Body field */
	NOTEHANDLE		hNewNoteCD;			/* CD foramt email for horig_note  */
	int				bOrigNoteCD = 0;	/* required to track MIME to CD conversion */



	/* called from ProcBlackList : Create in the current mail.box */
	if (erreur = NSFNoteCreate(hDB, &hMes))
	{
		AddInLogMessageText(ERR_CREATE_DOC, erreur, NOMAPI, "mail.box");
		return;
	}

	/* Masque */
	NSFItemSetText(hMes, "Form", "Memo", 4);


	/* 3.93.5.2 : Change From & Principal if from is not NULL */
	if ( strlen(from) )
	{
		/* emetteur  : 3.9 : from is stored the hNoteFiltre				   */
		NSFItemSetText(hMes, "From",      from, (WORD) strlen(from));
		NSFItemSetText(hMes, "Principal", from, (WORD) strlen(from));
	}
	else
	{
		NSFItemSetText(hMes, "From",      Orig_from, (WORD) strlen(Orig_from));
		NSFItemSetText(hMes, "Principal", Orig_from, (WORD) strlen(Orig_from));
	}


	/* Still if sendto is "0" is there then DO NOT use the original From value ! */
	if (strcmp(sendto,"0"))
	{
		NSFItemAppendTextList(hMes, "SendTo",     sendto, (WORD) strlen(sendto), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", sendto, (WORD) strlen(sendto), FALSE);
		nbrec = 1;
	}

	/* if sendtobis <> NULL => Append */
	if (strcmp(sendtobis, "NULL"))
	{
		NSFItemAppendTextList(hMes, "SendTo",     sendtobis, (WORD) strlen(sendtobis), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", sendtobis, (WORD) strlen(sendtobis), FALSE);
		nbrec = 1;
	}

	/* if forwardto <> "NULL" => Append */
	if (strcmp(forwardto, "NULL"))
	{
		NSFItemAppendTextList(hMes, "SendTo",     forwardto, (WORD) strlen(forwardto), FALSE);
		NSFItemAppendTextList(hMes, "Recipients", forwardto, (WORD) strlen(forwardto), FALSE);
		nbrec = 1;
	}


	/* continue if some recipients are there */
	if ( nbrec > 0 )
	{
		/* date d'envoi */
		OSCurrentTIMEDATE(&tdCurrent);
		NSFItemSetTime(hMes, "PostedDate", &tdCurrent);

		/*- Subject -*/
		/* 3.92 : new : append subject if not null */
		sujet[0]='\0';
		if ( strlen( origsubject) )
			sprintf(sujet, "%s : %s", SubjectHeader, origsubject);
		else
			sprintf(sujet,"%s", SubjectHeader);

		/* subject */
		NSFItemSetText(hMes, "Subject", sujet, (WORD) strlen(sujet));

		/* Body (Copy item from one document to another ) */
		if(erreur = CompoundTextCreate(hMes, "Body", &phCompound))
			return;

		CompoundTextInitStyle(&Style);				  /* initializes Style to the defaults */

		if (erreur = CompoundTextDefineStyle( phCompound, "Normal", &Style, &dwNormalStyle))
		{
			CompoundTextDiscard(phCompound);
			return;
		}

		/* Use the orginal note Body  (do it if not == "0") */
		if (strcmp(Append_Orig_Body,"0"))
		{
			/* Convert to CD to append Body + Body */
			bOrigNoteCD = TemporaryhNoteMIME2CD(hDB, horig_note, &hNewNoteCD);
			if (bOrigNoteCD)
				Doc4Body = hNewNoteCD;
			else
				Doc4Body = horig_note;

			/*  */
			if (Append_Orig_Body[0] == '1')
			{
				/* 3.94.24 - 3.92.5.2 Append new header if the 'from' is different of the original from */
				if ( strlen( from ) )
				{
					/* Original Header */
					if (erreur = AppendMailHeaderToBody(horig_note, &phCompound, Orig_from, Orig_To ,origsubject))
					{
						CompoundTextDiscard(phCompound);
						/* Erase HOLD mail : Doc4Body */
						if (bOrigNoteCD)
						{
							NSFNoteClose( hNewNoteCD );
							/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
						}
						return;
					}
				}


				/* original body -> (TOP == 1) */
				if (erreur = CompoundTextAssimilateItem(phCompound, Doc4Body, "Body", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}

				/* Filter Body (AFTER) */
				if (erreur = CompoundTextAssimilateItem(phCompound, hBL_Note, "BL_DeliveryFailure", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}

				/* 3.91 : Create a doclink if mailcopy : 3.94.42 : Doc Link and/or URL*/
				if ( DocLink != 0 )
				{
					sprintf(sujet, "%s : %s  ", "Doc Link", origsubject);
					erreur = SimpleAppendDocLink(hMes, replica_ID, Note_UNID, View_UNID, sujet, phCompound, DocLink);
				}
			}
			else
			{
				/* Filter Body (TOP) */
				if (erreur = CompoundTextAssimilateItem(phCompound, hBL_Note, "BL_DeliveryFailure", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}

				/* 3.91 : Create a doclink if mailcopy */
				if ( DocLink != 0 )
				{
					sprintf(sujet, "%s : %s  ", "Doc Link", origsubject);
					erreur = SimpleAppendDocLink(hMes, replica_ID, Note_UNID, View_UNID, sujet, phCompound, DocLink);

					/* add a blank line */
#ifdef V4
					if (erreur = CompoundTextAddParagraph
#else
					if (erreur = CompoundTextAddParagraphExt
#endif
								  (phCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  " ",
                                  (DWORD) strlen(" "),
                                  NULLHANDLE))
					{
						CompoundTextDiscard(phCompound);
						/* Erase HOLD mail : Doc4Body */
						if (bOrigNoteCD)
						{
							NSFNoteClose( hNewNoteCD );
							/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
						}
						return;
					}
				}


				/* Original Header */
				if ( strlen(from) )
				{
					if (erreur = AppendMailHeaderToBody(horig_note, &phCompound, Orig_from, Orig_To, origsubject))
					{
						CompoundTextDiscard(phCompound);
						/* Erase HOLD mail : Doc4Body */
						if (bOrigNoteCD)
						{
							NSFNoteClose( hNewNoteCD );
							/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
						}
						return;
					}
				}

				/* original body -> (BOTTOM == 2) */
				if (erreur = CompoundTextAssimilateItem(phCompound, Doc4Body, "Body", 0L))
				{
					CompoundTextDiscard(phCompound);
					/* Erase HOLD mail : Doc4Body */
					if (bOrigNoteCD)
					{
						NSFNoteClose( hNewNoteCD );
						/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
					}
					return;
				}
			}

			/* Purge temporary CD format email if it exists */
			if (bOrigNoteCD)
			{
				NSFNoteClose( hNewNoteCD );
				/* erreur = DeleteNoteWith_NoteHandle(hDB, hNewNoteCD); */
			}

		} /* end of : if (strcmp(Append_Orig_Body,"0")) */


		/* ----------------------------------------------------------------------------------------*/
		/* ----------------------------- DO NOT USE THE ORIGINAL EMAIL BODY -----------------------*/
		/* --------- but Only the Filter doc  Body ------------------------------------------------*/
		else
		{
			if (erreur = CompoundTextAssimilateItem(phCompound, hBL_Note, "BL_DeliveryFailure", 0L))
			{
				CompoundTextDiscard(phCompound);
				return;
			}

			/* 3.91 : Create a doclink if mailcopy */
			if ( DocLink != 0 )
			{
				sprintf(sujet, "%s : %s  ", "Doc Link", origsubject);
				erreur = SimpleAppendDocLink(hMes, replica_ID, Note_UNID, View_UNID, sujet, phCompound, DocLink);
			}
		}

		/* Close Compound ! */
		if (CompoundTextClose (phCompound, 0, 0L, NULL, 0))
			CompoundTextDiscard(phCompound);

		/* Let the message GO, DO NOT STOP IT WITH THE EXT_MGR */
		NSFItemSetText(hMes, "NOT_EM", "1", 1);

		/* 3.95.11 DOCLINK SPECIAL MARK */
		NSFItemSetText(hMes, MAIL_DOCLINK, Original_MessageID,(WORD) strlen(Original_MessageID));

		/* Check if the original Body has been appended and if the $FILE are to be purged ?     */
		if (strcmp(Append_Orig_Body,"0"))
		{
			/* Call PurgeAttachFile now */
			if (strcmp(Original_Body_Attach,"0"))
				DeleteAttachFile(hMes, "$FILE");
		}


		/* Update and close the new doc */
		NSFNoteUpdate(hMes, UPDATE_FORCE);

	} /* if nbrec > 0 */

	NSFNoteClose(hMes);

	return;
}






/********************************************************************************************
* CheckMonoMailBox
* Return : STATUS (NOERROR => FALSE => MULTI
* ERROR => mail1.box does not exist => mono mail.box
*
*********************************************************************************************/
STATUS CheckMonoMailBox()
{

	STATUS erreur;
	DBHANDLE hMb;

	/* Check if Mail1.Box exists */
	if (erreur = NSFDbOpen("mail1.box", &hMb))
	{
		/* Error => mono */
		return(erreur);
	}

	/* NOERROR => multi */
	NSFDbClose(hMb);
	return(NOERROR);

}



/*********************************************************************************************
* Set in the current e.mail the From, To, Forward value.. store them in RECOK field
* INPUT :
* hNote : the e.mail
* hNoteFiltre : the filter document
**********************************************************************************************/
void SetRecOK_FromToForward(char* emetteur, char* safe_recipient,
							NOTEHANDLE hNote, NOTEHANDLE hNoteFiltre,
							char* fieldname, char* OrigSubject, int AddMessageToEmail,
							int ApplyAction, char* BListMemo,
							int BL_Rules, int *bRecOK, DBHANDLE hDB,
							char* emetteurabr, char* recipientabr)
{

	/* LV */
	/* char	blacklister[MAXUSERNAME];   - not used in 3.95.6 */
	char	bl_choice[4];
	int		ToVal = 1;


	/* Ge the TO (value) - default value is 1 */
	if (NSFItemGetText(hNoteFiltre, "BList_StopMail_Value", bl_choice, sizeof(bl_choice)-1))
		ToVal = atoi(bl_choice);

	/******************************************************************************
	 * To have a decision on "To". Backlisted or Not use the following table
 	 * Majority (1) : To = 1
	 * Priority (2) : To = MaxRules - Rules Rank
	 * Veto     (3) : To = MaxRules
     * NO = -Yes
	 *******************************************************************************/

	 if (ToVal > 1)
	 {
	 	switch(ToVal)
		{
			/* Priority */
			case 2:
				ToVal = MaxRules - BL_Rules + 1;
			break;

			/* Veto */
			case 3:
				ToVal = MaxRules;
			break;

			/* 3.93.33 Neutral ( No Influence ) */
			case 4:
				ToVal = 0;		/* No influence : NULL ACTION on Routing condition */
			break;

		}
 	 }



	/* Deliver the original e.mail to the original "To" */
	if (NSFItemGetText(hNoteFiltre, "BList_StopMail", bl_choice, sizeof(bl_choice)-1))
	{
		if (bl_choice[0] == '1')
		{
			*bRecOK = *bRecOK + ToVal;
/*			NSFItemAppendTextList(hNote, fieldname, safe_recipient, (WORD) strlen(safe_recipient), FALSE); */
		}

		else
		{	/* Log blacklisted address */
			*bRecOK = *bRecOK - ToVal;
		}
	}


	
	
	/* Forward : Add the Forward field to the Recipients field */
	/* if (NSFItemGetText(hNoteFiltre, "BlackLister", blacklister, sizeof(blacklister)-1)) */
		
	/* ----------- 3.95 ----------------------------------------------------------------------------------------------------------------------*/
	/* 
	GetFilterRulesParameterOrPredicats(hNoteFiltre, "BlackLister", blacklister, sizeof(blacklister), emetteurabr, recipientabr, FirstEle); 
	if ( strlen(blacklister) > 0 )
		NSFItemAppendTextList(hNote, fieldname, blacklister, (WORD) strlen(blacklister), FALSE);
	-------------------------------------------------------------------------------------------------------------------------------------------*/

	/* 3.95.6 : multiple value allow */
	if ( GetFilterRulesParameterOrAllPredicats(hNoteFiltre, "BlackLister", hNote, fieldname, MAXUSERNAME,emetteurabr, recipientabr, FirstEle, MAXUSERNAME, MAX_FILTER_FORWARD_ADDRESS) )
	{
		/* Log error : could read/update FORWARD argt */
	}


	/* 3.95 : Only for Return to Sender */
	if (ApplyAction == 1)
    {

		/* Return to Sender : Add the From to the recipient field and Mail Router is the mail sender !! */
		if (NSFItemGetText(hNoteFiltre, "BList_ReturnToSender", bl_choice, sizeof(bl_choice)-1))
		{
			if (bl_choice[0] == '1')
			{

			   /* create a new mail if we work on the original hNote and the hnote is going to be modified
				if fieldbname != recipients we are actually created a new mail, this function is called from
                NewEmailBlackList */

			/*	if (strcmp(fieldname, "Recipients"))
					NewEmailBlackList(emetteur, emetteur, OrigSubject, hNote, hNoteFiltre, AddMessageToEmail, ApplyAction, BListMemo, BL_Rules, bRecOK, &hDB, 0, NULL);
             */

			/*	else */
				NSFItemAppendTextList(hNote, fieldname, emetteur, (WORD) strlen(emetteur), FALSE);
			}
		}

	}	/*   ApplyAction */


/* end */

}






/****************************************************************************/
/* Remove from the last from the Group History..it is the first of the list */
void PopGroupHistory(GrpList** GrpFirst)
{
	GrpList* Begin;
	Begin = *GrpFirst;

	/* set the new first item */
	*GrpFirst = (*GrpFirst)->Next;

	/* free memory for all allocated item in the struct */
	if (Begin->GroupName != NULL)
		free(Begin->GroupName);

	if (Begin != NULL)
		free(Begin);
}



void SetEmailTempNumFied(NOTEHANDLE hMes,int BL_Rules, int BL_NbRulesApplied)
{
	double num;

	/************************ NUMBERS ****************************************/
	/**** Set the current Rules rank within the while BL_Rules < MAXRULES ****/
		num = BL_Rules; /* current rank */

		#ifdef OS390
			/* Notes gave us an native to IEEE double which must be converted [ back ] */
			ConvertDoubleToIEEE(&num, &num);
		#endif

		/* save in the new e.mail */
		NSFItemSetNumber(hMes, "MW_BLRules", &num);


		/*** Save the number of rules already applied to the original messages ***/
		num = BL_NbRulesApplied; /* current rank */

		#ifdef OS390
			/* Notes gave us an native to IEEE double which must be converted [ back ] */
			ConvertDoubleToIEEE(&num, &num);
		#endif

		/* save in the new e.mail */
		NSFItemSetNumber(hMes, "MW_NbRulesApplied", &num);

}



/***********************************************************/
/* Compute the NbRulesApplied from BL_Memo field (char *)  */
/* Input  : string (bl_memo)                               */
/*        : ","                                            */
/* Output : number of "," => number of token               */
/***********************************************************/
int GetNbRulesApplied(char * string, const char *seps, int a)
{
    int t=0;
	char *token;
	char tmp[MAXMEMORULESIZE];
	char* savetoken;

	/* save */
 	strcpy(tmp, string);

	token     = strtok(tmp, seps );
    savetoken = token;  /* init */

	while( token != NULL )
	{
      /* While there are tokens in "string" */
		t = t+1;

	   /* Save then Get next token: */
	  savetoken = token;
      token = strtok( NULL, seps );
	}

	/* Parsing the returned result           */
	/* if a = 1 : return the number of token */
	/* if a = 0 : return the last token      */
	if (a)
		return(t);
	else
	{
		if (savetoken != NULL)
			return(atoi(savetoken));
		else
			return(0);
	}
}


/* add an item to a Text list item */
void AppendToMWTAG(NOTEHANDLE hNote, char* mwtag, char* token)
{
		NSFItemAppendTextList(hNote, mwtag, token, (WORD) strlen(token), FALSE);
}


/* Remove an item from a Text list item */
void RemoveFromMWTAG(NOTEHANDLE hNote, char* mwtag, char* tag)
{
	int		nbitem, i = 0;
	char*	stritem;
	char*	token;
	char	tagitem[MAXMWTAG+1];


	/* get the field , rewrite it back avoiding the tag item if found ! */
	/* build a string strcat using ",", then write back : token */
	nbitem = NSFItemGetTextListEntries(hNote, mwtag);

	stritem = calloc(nbitem, MAXMWTAG);

	for (i=0; i < nbitem; i++)
	{
		NSFItemGetTextListEntry(hNote, mwtag, (WORD) i ,tagitem, sizeof(tagitem)-1);

		/* concat all item different of tag */
		if (strcmp(tagitem, tag))
			strcat(stritem,strcat(tagitem,","));
	}


	/* Write back */
	NSFItemDelete(hNote, mwtag, (WORD) strlen(mwtag));

	/* Create mwtag back with the correct value */
	token = strtok(stritem, ",");

	while( token != NULL )
	{
		NSFItemAppendTextList(hNote, mwtag, token, (WORD) strlen(token), FALSE);

		/* next token => mwtag item */
		token = strtok( NULL, ",");
	}

	/* Free memory */
	free(stritem);

}




/********************************************************************
 * Fct qui calcule la taille d'un message
 * Inputs : handle du document
 * Ouput : NUMBER = taille en octets
 *				 int pj = nb d'objet (pièces jointes, liens DDE, OLE, etc)
 ********************************************************************/
NUMBER mw_NSFNoteGetSize(DBHANDLE hMb, NOTEHANDLE nh, int *pj)
{
	/* 20/11/01.- LP - */
	/* Change for Non Intel machine use ODS */

	STATUS		error = NOERROR;
	BLOCKID		bidh, bidv;
	WORD		wType, wlen;
	DWORD		dwLen;
	NUMBER		Total=0;
	void		*ptrItemValue;
	FILEOBJECT	pFileObject;
	int			nbpj = 0;
	char		*loc;
	char		*filename;
	size_t		xLen = 0;
	char		s_ExtSize[30];

	/* temp, because of Unix compilation... */
	WORD *ptrItemValueWORD ;

	/* Loop !! */
	error = NSFItemInfo(nh, NULL, 0, &bidh, &wType, &bidv, &dwLen);
	if(error)
		return(Total);


	while(error==NOERROR)
	{
		BLOCKID bidPrev;

		bidPrev = bidh;	/* save the original pointer for the GetNext */
		Total += dwLen;

		/* Si objet ==> on va chercher sa taille */
		if ((wType & TYPE_OBJECT) == TYPE_OBJECT)
		{
			/* Save the original pointer */
			ptrItemValue = OSLockBlock(void, bidv);

			/*  Step over the data type word to point to the actual data */
			ptrItemValueWORD = (WORD *) ptrItemValue ;
			ptrItemValueWORD++ ;
			ptrItemValue = ptrItemValueWORD ;

			/* Use ODSReadMemory */
			ODSReadMemory( &ptrItemValueWORD, _FILEOBJECT, &pFileObject, 1 );

			error = NSFDbGetObjectSize(hMb, pFileObject.Header.RRV, pFileObject.Header.ObjectType, &dwLen, NULL, NULL);

			/* Free memory : no !! */
			OSUnlockBlock(bidv);

			/* Skip the rest if error on NSFDbGetObjectSize !! */
			if (error)
				continue;

			/* Total */
			Total += dwLen;		

			

			/* Extension du fichier */
			if (pFileObject.Header.ObjectType == OBJECT_FILE)
			{
			 	/* Convert compute object size as string */
				sprintf(s_ExtSize, "%lu", pFileObject.FileSize);
				nbpj++;

				wlen = pFileObject.FileNameLength;
				filename = (char *) malloc(wlen+1);

				/* Read the data the comes after the pFileObject */
				ptrItemValue = (char *) ptrItemValue + ODSLength(_FILEOBJECT);

				memcpy(filename, (char *) ptrItemValue, wlen);
				filename[wlen] = '\0';

				/* get File extension : if none log the filename */
				loc = GetWord(filename, ".", 0);
				if (loc == NULL)
					loc = filename;

				/* 3.94.32 : Buffer size with "..." - DO NOT OVERFLOW ! */
				if ( (xLen = strlen(statmw[lgstat].Ext)) + strlen(loc) + 2 < MAXRECLEN )
				{
					if ( xLen > 0 )
					{
					 	strcat(statmw[lgstat].Ext,     ",");
						strcat(statmw[lgstat].ExtSize, ",");
						strcat(statmw[lgstat].Ext, loc);
						strcat(statmw[lgstat].ExtSize, s_ExtSize);
					}
					else
					{
						strcpy(statmw[lgstat].Ext,    loc);
						strcpy(statmw[lgstat].ExtSize, s_ExtSize);
					}
				}

				/* Free memory */
				free(filename);
			}
		}

		error = NSFItemInfoNext(nh, bidPrev, NULL, 0, &bidh, &wType, &bidv, &dwLen);
	}

	*pj = nbpj;
	return(Total);
}



/* DumpStat */
/***************************************************************************************************************************
 * File		: dumpstat.c
 * Project	: MailWatcher 3.9
 * Purpose	: Memory stats dump to a Lotus notes record
 *
 * fix bug : 11/00
 * Reset Index lgstat to ZERO when an error occured => return
 *
 * change	: 01/05/02
 * add test on NSFItem* : close db and reset lgstat.
 * -----------------------------------------------------------------------------------------------------------------------
 * revision 3.92			: char	MW_DocID[DOCID];	Original Notes Document unique ID seen by a MailWatcher server
 *							: char	subject[MAXRECLEN]; mail subject "" if not activated
 * -----------------------------------------------------------------------------------------------------------------------
 * revision 3.95.13			: new field : ExtSize
 *							:
 *************************************************************************************************************************/

void dumpstat(void)
{
	unsigned int	i;
	char			temp[sizeof(struct advt_stat)];
	DBHANDLE		hStatdb;
	NOTEHANDLE		hNoteStat;
	STATUS			error;
	NUMBER			nbchamp;
	long			tailletot;
	long			docsize;
	long			StatLineSize;
	char			fieldtemp[10];

	/* Ouverture de la base des config générales */
	CheckPath(StatFile); /** For Unix **/

	if ( error = NSFDbOpen(StatFile, &hStatdb) )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, StatFile);
		/* reset index */
		lgstat = 0;
		return;
	}

	if ( !EstVraiBase(hStatdb) )
	{
		AddInLogMessageText(ERR_OPEN_DB, error, NOMAPI, StatFile);
		/* reset index */
		lgstat = 0;
		return;
	}

	/* -----------------------------------------------------------------------------------------*/
	/* create empty stat record : close the record handle if and error occured in CreateStatDoc */
	if ( error = CreateStatDoc(hStatdb, &hNoteStat, StatFile ) )
	{
		NSFDbClose(hStatdb);
		/* reset index */
		lgstat = 0;
		return;
	}


	/* Log line onto Domino console & log file */
	AddInLogMessageText(DUMP_STATS, NOERROR, NOMAPI, StatFile);


	/* -----------------------------------------------------------------------------------------*/
	/*                      ********** Start Dump *************                                 */

	nbchamp		= 1;
	tailletot	= 0;
	docsize		= 0;
	strcpy(fieldtemp, "Stat1");

	for (i=0; i<(unsigned int)lgstat; i++)
	{
	  sprintf(temp, "%s;%s;%s;%s;%s;%s;%d;%s;%s;%lu;%d;%s;%d;%d;%d;%d;%d;%d;%lu;%d;%d;%d;%s;%s;%s;%s;%s",
							statmw[i].Date,
							statmw[i].Time,
							statmw[i].PostedDate,
							statmw[i].PostedTime,
							statmw[i].Sender,
							statmw[i].HomeServer,
							statmw[i].nbRec,
							statmw[i].Rec,
							statmw[i].Form,
							statmw[i].Taille,
							statmw[i].Objets,
							statmw[i].Ext,
							statmw[i].Sign,
							statmw[i].Crypt,
							statmw[i].DeliveryConfirm,
							statmw[i].ReturnReceipt,
							statmw[i].DeliveryPriority,
							statmw[i].nbServer,
							statmw[i].DeliverTime,
							statmw[i].nbArch,
							statmw[i].sf,
							statmw[i].nbBl,
							statmw[i].DocID,
							statmw[i].BLNames,
							statmw[i].MW_DocID,
							statmw[i].subject,
							statmw[i].ExtSize );


		/* Stat line size */
	    StatLineSize = (long) strlen(temp);

		/* Document size */
		if ( docsize + StatLineSize > MAX_DOC_FIELD_TEXT_SIZE )
		{
			/* create a new Stat Doc */
			if ( error = SaveStatDoc(hNoteStat, nbchamp, StatFile) )
			{
				NSFDbClose(hStatdb);
				/* reset index */
				lgstat = 0;
				return;
			}

			/* create a new empty stat record */
			if ( error = CreateStatDoc(hStatdb, &hNoteStat, StatFile ) )
			{
				NSFDbClose(hStatdb);
				/* reset index */
				lgstat = 0;
				return;
			}

			/* From now hNoteStat is a brand new record */
			nbchamp		= 1;
			tailletot	= 0;
			docsize		= StatLineSize;			/* init is not ZER0 */
			strcpy(fieldtemp, "Stat1");

		}
		/* ---------------------------------------------------*/
		/*     OTHERWISE : Continue on the same record        */
		/* ---------------------------------------------------*/
		else
			docsize += StatLineSize;


		/* Check Field Max size */
		if ( tailletot + StatLineSize > MAX_FIELD_TEXT_SIZE )
		{
		 	nbchamp++;
			tailletot = StatLineSize;
			sprintf(fieldtemp, "Stat%d", (int) nbchamp);
		}
		/* ------------------------------------*/
		/*   Continue on the same field        */
		/* ------------------------------------*/
		else
			tailletot += StatLineSize;



        /* write StatLine to record */
		if (error = NSFItemAppendTextList(hNoteStat, fieldtemp, temp, (WORD) StatLineSize, TRUE))
		{
	 		AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);
			NSFDbClose(hStatdb);
			/* reset index */
			lgstat = 0;
			return;
		}

	} /* end of loop over stat struct (nb of item = lgstat)*/


	/* Save and close the current open record doc */
	if (error =  SaveStatDoc(hNoteStat, nbchamp, StatFile) )
	{
		NSFDbClose(hStatdb);
		/* reset index */
		lgstat = 0;
		return;
	}

	/* Close notes Database record */
	NSFDbClose(hStatdb);

	/***** Reset var *********/
	for (i=0; i<(unsigned int)lgstat; i++)
	{

		statmw[i].Date[0]          = '\0';
		statmw[i].Time[0]          = '\0';
		statmw[i].PostedDate[0]    = '\0';
		statmw[i].PostedTime[0]    = '\0';
		statmw[i].Sender[0]        = '\0';
		statmw[i].HomeServer[0]    = '\0';
		statmw[i].nbRec            = 0;
		statmw[i].Rec[0]           = '\0';
		statmw[i].Form[0]          = '\0';
		statmw[i].Taille           = 0;
		statmw[i].Objets           = 0;
		statmw[i].Ext[0]           = '\0';
		statmw[i].Sign             = FALSE;
		statmw[i].Crypt            = FALSE;
		statmw[i].DeliveryConfirm  = FALSE;
		statmw[i].ReturnReceipt    = FALSE;
		statmw[i].DeliveryPriority = 0;
		statmw[i].nbServer         = 0;
		statmw[i].DeliverTime      = 0;
		statmw[i].nbArch           = 0;
		statmw[i].sf               = FALSE;
		statmw[i].nbBl             = 0;
		statmw[i].DocID[0]         = '\0';
		statmw[i].BLNames[0]       = '\0';
		statmw[i].MW_DocID[0]      = '\0';
		statmw[i].subject[0]       = '\0';
		statmw[i].ExtSize[0]       = '\0';

	}

	/* reset index */
	lgstat = 0;

}


/***********************************************************
 * Get a Database Replica ID
 * INPUT	: DBHANDLE
 * By Ref	: ReplicaID (TimeDate)
 *
 * OUTPUT	: STATUS
 ***********************************************************/
STATUS GetReplicaID(DBHANDLE hDb, TIMEDATE *replicaID)
{
	STATUS error;
	DBREPLICAINFO replica_info;

	if (error = NSFDbReplicaInfoGet(hDb, &replica_info))
		return(error);

	memcpy(replicaID, &replica_info.ID, sizeof(TIMEDATE));
	return(NOERROR);
}



/*********************************************************************************************
* Append DocLink to a Body
*			Data related to the doc for which the Doclink is being created and append to
*			the hCoupound.
*
* INPUT		: Doc to be linked -> Database handle
*			: Doc to be linked -> View name of Argument 1
*			: Doc to be linked -> NoteHandle
*			: Text display when clicking on the doc link
*			: New Message Body item (hCompound)
*
* OUTPUT	: STATUS
***********************************************************************************************/
STATUS AppendDocLink(DBHANDLE hBck, char* ViewName,
					 NOTEHANDLE hNoteOld, char* DocLinkText,
					 HANDLE* hCompound)
{

	STATUS			error = NOERROR;
	TIMEDATE		replica_ID;

	ORIGINATORID	Note_OID;	/* hNoteOld */
	UNID			Note_UNID;

	NOTEHANDLE		hView;		/* ViewName */
	ORIGINATORID	View_OID;
	NOTEID			View_ID;
	UNID			View_UNID;


	/* Info for Doc link */
	error = GetReplicaID(hBck, &replica_ID);

	if (error = NOERROR)
	{
		NSFNoteGetInfo(hNoteOld, _NOTE_OID, &Note_OID);	/* Note OID */
		Note_UNID.File = Note_OID.File;
		Note_UNID.Note = Note_OID.Note;


		/*****************************/
		/* Get the view -> ViewName  */
		/*---------------------------*/
		error = NIFFindView(hBck, ViewName, &View_ID);


		if (error == NOERROR)
		{
	 		NSFNoteOpen(hBck, View_ID, 0, &hView);
			NSFNoteGetInfo(hView, _NOTE_OID, &View_OID);

			View_UNID.File = View_OID.File;
			View_UNID.Note = View_OID.Note;

			NSFNoteClose(hView);

			/* Create the Doc link */
			if (error = CompoundTextAddDocLink(*hCompound, replica_ID,
									   View_UNID, Note_UNID,
									   DocLinkText, 0L))
			{
				CompoundTextDiscard(*hCompound);
			}
		}
	}

	/* end */
	return(error);

}


/******************************************************************************************** 
* Prog			: SimpleAppendDocLink
* Purpose		: Append a Doc link to an existing Body
* Extension		: Append an HOTPSPOT as within a new Body.
* INPUT			: 
* OUTPUT		: STATUS, Discard RT if error
* -----------------------------------------------------------------------------------------
* Upgrade		: use mwhotspot to append the URL to a new body.
*				: 3.94.43 : pass DocLink to skip URL if == 1;
********************************************************************************************/
STATUS SimpleAppendDocLink(NOTEHANDLE hNote, TIMEDATE replica_ID, UNID Note_UNID, UNID View_UNID, char* DocLinkText, HANDLE hCompound, int DocLink )
{

	STATUS			error;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	char			DocUniqueID[33];
	char			DBUniqueID[33];
	char			ViewUniqueID[33];
	char			mail_server_abbr[MAXPATH];
	char			URLText[450];		/* enough for : 256 + 3*33 + a few */

	/* Replica ID : OK */
	/* Notes UNID : OK */
	/* View UNID  : OK */

	CompoundTextInitStyle(&Style);

	if (error = CompoundTextDefineStyle( hCompound,
                                  "Normal",
                                  &Style,
                                  &dwNormalStyle))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}

	/*********************************************/
	/* Create the Doc link - introduction line - */
	/*-------------------------------------------*/
#ifdef V4
	if (error = CompoundTextAddParagraph
#else
	if (error = CompoundTextAddParagraphExt
#endif
								 (hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  DocLinkText,
                                  (DWORD)strlen(DocLinkText),
                                  NULLHANDLE))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}



	/* ( - Notes -) */
	if (error = CompoundTextAddDocLink(hCompound, replica_ID, View_UNID, Note_UNID, DocLinkText, 0L))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}


	/* add a blank line */
#ifdef V4
	if (error = CompoundTextAddParagraph
#else
	if (error = CompoundTextAddParagraphExt
#endif
				  (hCompound,
                  dwNormalStyle,
                  DEFAULT_FONT_ID,
                  " ",
                  (DWORD) strlen(" "),
                  NULLHANDLE))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}


/** 3.94.43 *********/
if ( DocLink == 2 )
{
	/*********************************************/
	/* ( - Web -)		: URL link */
	/*-------------------------------------------*/
	sprintf(DocUniqueID, "%08X%08X%08X%08X",  Note_UNID.File.Innards[1], Note_UNID.File.Innards[0],
										      Note_UNID.Note.Innards[1], Note_UNID.Note.Innards[0]);
	sprintf(DBUniqueID,  "%08X%08X", replica_ID.Innards[1],replica_ID.Innards[0]);

	sprintf(ViewUniqueID, "%08X%08X%08X%08X", View_UNID.File.Innards[1], View_UNID.File.Innards[0],
										      View_UNID.Note.Innards[1], View_UNID.Note.Innards[0]);

	/* Archive DB URL */
	if ( szServeurHost )
		strcpy(mail_server_abbr, szServeurHost);
	else
		DNAbbreviate(0L, NULL, szServeur, mail_server_abbr, MAXPATH, NULL);

	/* if "/" is found cut the string */
	strtok(mail_server_abbr, "/");

	
	/* 3.94.29 : URL formatting */
	sprintf(URLText, "http://%s/%s/%s/%s?opendocument", mail_server_abbr, DBUniqueID, ViewUniqueID, DocUniqueID );

	if ( error = InsertRichTextWithURL(hNote, URLText, DocLinkText) )
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}


/************* Old fashion : now hide this uggly URL with a HotSpot ************************************************
#ifdef V4
	if (error = CompoundTextAddParagraph
#else
	if (error = CompoundTextAddParagraphExt
#endif
								 (hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  URLText,
                                  (DWORD)strlen(URLText),
                                  NULLHANDLE))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}

*******************************************************************************************************************/
}
/*** DocLink == 2 */

	/* end */
	return(error);

}


/************ Get Details - mailcopy *******************/
STATUS MailCopyGetDocLinkDetail(char* dbName, DBHANDLE hBck, NOTEHANDLE hNoteOld,
								TIMEDATE* replica_ID, char* ViewName,
								UNID* Note_UNID, UNID* View_UNID)
{
	STATUS			error = NOERROR;
	ORIGINATORID	Note_OID;	/* hNoteOld */
	NOTEHANDLE		hView;		/* ViewName */
	ORIGINATORID	View_OID;
	NOTEID			View_ID;


	/* Info for Doc link */
	error = GetReplicaID(hBck, replica_ID);

	if (error == NOERROR)
	{
		NSFNoteGetInfo(hNoteOld, _NOTE_OID, &Note_OID);	/* Note OID */
		Note_UNID->File = Note_OID.File;
		Note_UNID->Note = Note_OID.Note;

		/*****************************/
		/* Get the view -> ViewName  */
		/*---------------------------*/
		error = NIFFindView(hBck, ViewName, &View_ID);

		if (error == NOERROR)
		{
	 		NSFNoteOpen(hBck, View_ID, 0, &hView);
			NSFNoteGetInfo(hView, _NOTE_OID, &View_OID);

			View_UNID->File = View_OID.File;
			View_UNID->Note = View_OID.Note;

			NSFNoteClose(hView);
		}
		/* Log the error */
		else
			AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, ViewName, dbName);

	}

	/* end */
	return(error);
}




/************************************************************************************
* Prog		: DeleteAttachFile
* -----------------------------
* (recursive call - remove all the fields).
*
* INPUT		: current message
* OUTPUT	: STATUS.
*************************************************************************************/
STATUS DeleteAttachFile(NOTEHANDLE note_handle, char* AttachFieldName)
{

 STATUS	  	error = NOERROR;
 BLOCKID  	field_value_blockid;
 BLOCKID	field_item_blockid;
 WORD 		field_type;
 DWORD 		field_length;


 /* Exception required a loop !!!! : While NOERROR ! */

 /* Get item BLOCKID */
 error = NSFItemInfo (note_handle, AttachFieldName,
						(WORD) strlen(AttachFieldName),
 			   	   	     &field_item_blockid,    /* full field (return) */
 			   	   	     &field_type,			 /* field type (return) */
 		      		     &field_value_blockid,   /* field contents (return) */
 	      	     	     &field_length);		 /* field length (return) */


  if (ERR(error) == ERR_ITEM_NOT_FOUND)
		return(error);		/* end recur */

  /* delete */
  error = NSFNoteDetachFile(note_handle, field_item_blockid); /* Delete the attach file (void) function */

  /* some others ? */
  if (error == NOERROR)
	 error = DeleteAttachFile(note_handle, AttachFieldName);

  /* Normal end */
  return(error);

}



/************************************************************************************
* Prog		: DeleteAttachFileWithException
* -------------------------------------------------------------------------------------
* PURPOSE	: iterative version - remove $FILE fields but only if the file
*			: has the specified Extension)
* INPUT		: current message
* INPUT		: List of Extension (char* ExtensionList ; example = "doc,zip,txt,exe"
* OUTPUT	: STATUS.
*************************************************************************************/
STATUS DeleteAttachFileWithException(NOTEHANDLE note_handle, char* AttachFieldName, char* ExtensionList)
{

 STATUS	  		error = NOERROR;
 char			FileName[MAXPATH];
 char			FileNameExtension[MAXPATH];
 BLOCKID		item_bid, value_bid, prev_bid, wdc_bid;
 WORD			type, flags, name_len, wdc_wd;
 DWORD			length, wdc_dwd;
 char			field_name[MAXFIELDVALUE];
 int			i = 0;
 BLOCKID		TobeDeleted[MAXDELETEATTACH];	/* max */
 int			NbTobeDeleted = 0;
 int			Except_Flag = 1;

 /* Uppercase for string comparaison */
UpperCase(ExtensionList);

 /* Loop over all ITEM */
 for(i=0;;prev_bid = item_bid,i++)
 {
	 /*----------------------------------------------*/
	 /*[---------- Extract ITEM info ---------------]*/

     if(i==0) /* => first one */
     {
       error = NSFItemInfo(note_handle, NULL, 0, &item_bid, &type, &value_bid, &length);
       if(error)
           return(0);
     }
     else    /* => now get next item */
     {
		error = NSFItemInfoNext(note_handle, prev_bid, NULL, 0, &item_bid, &type, &value_bid, &length);
        if(ERR(error) == ERR_ITEM_NOT_FOUND)
            break;
        else if(error)
          return(0);
	 }

	 /*-----------------------------------------------------------*/
	 /*[---------- Extract data : Get NAME & FLAG ---------------]*/

	 NSFItemQuery(note_handle, item_bid, field_name, MAXFIELDVALUE-1, &name_len, &flags, &wdc_wd, &wdc_bid, &wdc_dwd);
     field_name[name_len] = '\0';

     /** ATTACHMENT ? **/
     if(type == TYPE_OBJECT)
     {
		/* Work out */
		NSFGetAttachmentFileName(value_bid, FileName, FileNameExtension);

		/* Uppercase for string comparaison */
		UpperCase(FileNameExtension);

		/* -------
		* [ Except_Flag ]
		* Except_Flag = 1 => Delete the $FILE if the filename's extension is part of the reference string
        * Except_Flag = 0 => Delete the $FILE if the filename's extension IS NOT part of the reference string
		------ */
		if ( IsInString(ExtensionList, FileNameExtension) == Except_Flag )
/*		if (strstr(ExtensionList, FileNameExtension)) */
		{
			/* need to save the item_bid that is to be destroyed to not to break the 'Get Next Item loop' */
			if ( NbTobeDeleted + 1 < MAXDELETEATTACH )
				TobeDeleted[NbTobeDeleted++] = item_bid;

			/* printf("deleted : %s => %s \n", FileName, FileNameExtension); */
		}
	 }

 } /* end of loop */


 /* ----------- now Delete $FILE for Good --------------*/
if ( NbTobeDeleted > 0 )
{
	for (i=0; i < NbTobeDeleted; i++)
		error = NSFNoteDetachFile(note_handle, TobeDeleted[i]); /* Delete the attach file (void) function */

}

 /* Normal end */
 return(error);

}



/*********************************************************************************************
* > DeleteFieldInNote <
* ---------------------
* Prog		: DeleteFieldInNote(NOTEHANDLE, char*, char*)
*
*			: delete all the fields defined in the (fieldNameList)
*
* INPUT		: current Rules (note_handle_rules) , current message (note_handle),
* OUTPUT	: STATUS.
**********************************************************************************************/
STATUS DeleteFieldInNote(NOTEHANDLE note_handle_rules, NOTEHANDLE note_handle, char* fieldNameList, char* RulesRef)
{

	/* Get the field list */
	STATUS	error;
	char	field_to_delete[MAXUSERNAME]; /* max size of a field item name */
	WORD	typechamp;
	int		i;
	WORD	nbrec;


	error = NOERROR;
	error = NSFItemInfo(note_handle_rules, fieldNameList, (WORD) strlen(fieldNameList), NULL, &typechamp, NULL, NULL);
	if (error)
	{
		AddInLogMessageText(ERR_READ_FIELD, error, NOMAPI, fieldNameList, RulesRef);
		return(error);
	}

	/*********** Start *******************/
	if (typechamp == TYPE_TEXT)
	{
		NSFItemGetText(note_handle_rules, fieldNameList, field_to_delete, sizeof(field_to_delete)-1);

		if (strlen(field_to_delete))
			NSFItemDelete(note_handle, field_to_delete, (WORD) strlen(field_to_delete));
	}
	else
	{
		if (typechamp == TYPE_TEXT_LIST)
		{
			/* Loop over all the item */
			nbrec = NSFItemGetTextListEntries(note_handle_rules, fieldNameList);

			for (i=0; i<nbrec; i++)
			{
				NSFItemGetTextListEntry(note_handle_rules, fieldNameList, (WORD) i, field_to_delete, sizeof(field_to_delete)-1);

				if (strlen(field_to_delete))
					NSFItemDelete(note_handle, field_to_delete, (WORD) strlen(field_to_delete));
			}
		}
	}

	/* end */
	return(error);
}








/***********************************************************************
* Send a mail back to the sender with the current subject + sufixe
* and a new Body field  + a list of Address for which the mail has not
* been delivered
*
* Create a new NOTE using the current email.
* ----------------------------------------------------------------------
* This procedure is called if BLIST_NOSEND_NOTIFY
*
* INPUT		: a mail.box (mail1.box or mail.box)
*			: NOTEHANDLE (hNote)
*
* OUTPUT	: STATUS
* ----------------------------------------------------------------------
* Global	: BlackList_Address_Subject ->(Subject suffix)
*			: nb_BlackList_Address_Message ->(nb of lines)
*			: BlackList_Address_Message ->(Body message)
* ---------------------------------------------------------------------
* 3.93.9	: Save only if recipients > 0 and not equal to ""
*************************************************************************/
STATUS	BlackListedAddressNotification(NOTEHANDLE hNote, DBHANDLE *hMb)
{

	STATUS			error;
	char			emetteur[MAXUSERNAME];
	char			OrigSubject[2500];
	char			NewSubject[5000];
	int				i=0;
	int				nbrec =0;
	NOTEHANDLE		hMes;
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	TIMEDATE		tdCurrent;
	char			AddressText[MAXUSERNAME];
	int				DocToSave = 0;


	/*******************************************************/
	/*         Get current email data                      */
	/*-----------------------------------------------------*/

	/* Message Sender */
	if (!NSFItemGetText(hNote, "From", emetteur, sizeof(emetteur)-1))
		strcpy(emetteur, "Mail Router");


	/*******************************************************/
	/*         Create a new email                          */
	/*-----------------------------------------------------*/
	if (error = NSFNoteCreate(*hMb, &hMes))
	{
		AddInLogMessageText(ERR_CREATE_DOC, error, NOMAPI, "mail.box");
		return(error);
	}

	/* Form */
	NSFItemSetText(hMes, "Form", "Memo", 4);

	/* Init From with the original sender */
	NSFItemSetText(hMes, "From",       "MailWatcher", (WORD) strlen("MailWatcher") );
	NSFItemSetText(hMes, "Recipients", emetteur,      (WORD) strlen(emetteur) );

	/* Date */
	OSCurrentTIMEDATE(&tdCurrent);
	NSFItemSetTime(hMes, "PostedDate", &tdCurrent);

	/* 3.93.22 : Do not catch this update by advsrv extension manager */
	NSFItemSetText(hMes, "NOT_EM", "1", 1);

	/*---------------------------------------------------------------------------------*/
	/*                 >>>>>>>>>>>>     Subject  <<<<<<<<<<<<<<                        */

	if (!NSFItemGetText(hNote, "subject", OrigSubject, sizeof(OrigSubject)-1))
	strcpy(OrigSubject, "");

	sprintf(NewSubject, "%s : %s", BlackList_Address_Subject, OrigSubject);	/* 3.92.18, set Black... as prefix, remove parantheses */

/*	sprintf(NewSubject, "%s : ( %s )", OrigSubject, BlackList_Address_Subject); */

	NSFItemSetText(hMes, "Subject", NewSubject, (WORD) strlen(NewSubject));


	/*---------------------------------------------------------------------------------*/
	/* >>>>>>>>>>>>    Dump the 'BlackList_Address' Body lines  <<<<<<<<<<<<<<         */

	if (error = CompoundTextCreate(hMes, "Body", &hCompound))
		return(error);

	CompoundTextInitStyle(&Style);				  /* initializes Style to the defaults */

	if (error = CompoundTextDefineStyle(hCompound, "Normal", &Style, &dwNormalStyle))
	{
		CompoundTextDiscard(hCompound);
		NSFNoteClose(hMes);
		return(error);
	}


	for (i = 0; i < nb_BlackList_Address_Message; i++)
	{

#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  BlackList_Address_Message[i],
                                  (DWORD) strlen(BlackList_Address_Message[i]),
                                  NULLHANDLE))
			{
				CompoundTextDiscard(hCompound);
				NSFNoteClose(hMes);
				return(error);
			}
	}


	/*-------------------------------------------------------------------*/
	/*                >>>>>>>>>> Nb RecNotOk <<<<<<<<<<<<                */

	nbrec = NSFItemGetTextListEntries(hNote, "RecNotOk");

	for (i=0; i<nbrec; i++)
	{
			NSFItemGetTextListEntry(hNote, "RecNotOk", (WORD) i, AddressText, sizeof(AddressText)-1);
			if (strlen(AddressText))
			{
#ifdef V4
				if (error = CompoundTextAddParagraph
#else
				if (error = CompoundTextAddParagraphExt
#endif
									  (hCompound,
				                      dwNormalStyle,
					                  DEFAULT_FONT_ID,
						              AddressText,
							          (DWORD) strlen(AddressText),
								      NULLHANDLE))
				{
					CompoundTextDiscard(hCompound);
					NSFNoteClose(hMes);
					return(error);
				}
				/* to be saved ! */
				DocToSave = 1;
			}
	}

	/* if no error */
	if (error = CompoundTextClose (hCompound, 0, 0L, NULL, 0))
		CompoundTextDiscard(hCompound);

	if (DocToSave == 1)
		error = NSFNoteUpdate(hMes, 0);

	NSFNoteClose(hMes);

	/* return */
	return(error);
}



/*********************************************************************
* DUPLICATE MAIL.
*
* IN	: current DBHANDLE of hnote.
* IN	: current hnote
* OUT	: address of the new hNote in the current DBHANDLE
* -------------------------------------------------------------------
* Fix 3.92	: Remove temporary fields.
* new 3.92	: convert MIME to CD add int clone argument to not to remove
*			: temporay fields
*********************************************************************/
STATUS DuplicateMail(DBHANDLE* hMB, NOTEHANDLE hNote, NOTEHANDLE* hCopy, int clone)
{

	STATUS error = NOERROR;
	OID new_oid;

	/* copy */
	if (error = NSFNoteCopy(hNote, hCopy))
			return(error);



	if (error = NSFDbGenerateOID(*hMB, &new_oid))
	{
			NSFNoteClose(*hCopy);
			return(error);
	}

	/* Set Notes internal IDs */
	NSFNoteSetInfo(*hCopy, _NOTE_OID, &new_oid);
	NSFNoteSetInfo(*hCopy, _NOTE_ID, NULL);
	NSFNoteSetInfo(*hCopy, _NOTE_DB, hMB);


	/* Purge MailWatcher temporary working field (RecOK, RecNotOK) fields */
	if (clone == 0)
	{
		NSFItemDelete(*hCopy, "MwTemp",             6);
		NSFItemDelete(*hCopy, "RecOK",              5);
		NSFItemDelete(*hCopy, "RecNotOK",           8);
	}

	/* 3.94.65 : AntiVirus that can not exclude MWADVT */
	if ( AntiVirus_MailWatcherTask_NotExclude )
		NSFItemSetText(*hCopy, SKIPEXTENSIONMGR, "1", 1);		/* #define SKIPEXTENSIONMGR				"NOT_EM"	*/



	/* debug
	sprintf(LogString, "Duplicate - hNote : %i  \thCopy %i \n", (int) hNote, (int) (*hCopy) );
	LogLine("mwadvt.log", LogString, 1);
    */

	return(error);
}



/*********************************************************************************
 Close & Update a mail that has been created within the ProcBlackList
the mail is duplicated if it is modified 'physically'..
> removing fields
> removing Attachments
> Add new attachments
> Change Body field.

**********************************************************************************/
STATUS UpdateNewMailNote(NOTEHANDLE hNote)
{
	STATUS	sErreur = NOERROR;
	int		mwtag;
	NUMBER	num;


	/* Add MWNUM field as if this new note was a real original email */

	MarkNoteNum(hNote, &num);


	/* 3.9 : Read if the MWTAG field new task have been set within the hNote */
	mwtag = 0; /* re-init*/
	mwtag = NSFItemGetTextListEntries(hNote, MWTAGFIELD);

	/* Remove now the task "MailWatcher : MWADVT" from the task list */
	if (mwtag > 0)
	{
		RemoveFromMWTAG(hNote,MWTAGFIELD, MAILWATCHER_MWADVT);
		mwtag = mwtag - 1; /* one Tag has been removed ! */
	}

	/* remove tempo fields */
	NSFItemDelete(hNote, "MwTemp",             6);
	NSFItemDelete(hNote, "RecOK",              5);
	NSFItemDelete(hNote, "RecNotOk",		   8);
	NSFItemDelete(hNote, "MW_BLMemo",          9);
	NSFItemDelete(hNote, "MW_SKIPRULE",       11);
	NSFItemDelete(hNote, MW_MAILDUPLICATE, (WORD) strlen(MW_MAILDUPLICATE));
	/* 3.94.56 : do not remove 	NSFItemDelete(hNote, MW_MACRO_FIELD_NB_RECIPIENTS,(WORD) strlen(MW_MACRO_FIELD_NB_RECIPIENTS)); */ /* 3.94.46 */


	/* the mwtag is there to signal that some other mailwatcher task have been scheduled ! */
	/* so do not release the 'RoutingState' + 'Extension manager mark up'	*/
	if (mwtag == 0)
	{
		if (sErreur = NSFItemDelete(hNote, "RoutingState", 12))
		{
			AddInLogMessageText(ERR_DEL_RS, sErreur, NOMAPI, NULL, 1);
			NSFNoteClose(hNote);
			return(sErreur);
		}
	} /* mwtag */


	/* Update and close anyway */
	if (sErreur = NSFNoteUpdate(hNote, UPDATE_FORCE))
	{
		AddInLogMessageText(ERR_DEL_RS, sErreur, NOMAPI, NULL, 2);
		NSFNoteClose(hNote);
		return(sErreur);
	}

	/* Close the document now */
	NSFNoteClose(hNote);
	return(sErreur);

}





/******************************************************************************************
* Update the Recipients list with the temporary var : RecOK stored in the Doc itself
* and returns the number of address that have been stored in recipients
* IN	: hnote
* OUT	: TRUE (>=1) : 0   ( !nb of recipients)
------------------------------------------------------------------------------------------
* 3.93.9	: recipients can be "" NULL. because this field might not exist
*			: but MW is creating artifically to enter the main rules loop in order
*			: to apply actions even on these type of documents.
*****************************************************************************************/
int UpdateRecipientsList(NOTEHANDLE hNote)
{
	int		nbrec = 0;
	WORD	i;
	char	recipient[MAXUSERNAME];
	int		DocToDelete = 1;			/*3.93.9 : recipients can be "" (empty) : Not to be save by default ( return ) */


	/****  Write back the recipients field   *****/
	NSFItemDelete(hNote, "Recipients", 10);

	/* Append the new RecOK */
	nbrec = NSFItemGetTextListEntries(hNote, "RecOK");

	for (i=0; i<nbrec; i++)
	{
		NSFItemGetTextListEntry(hNote, "RecOK", i, recipient, sizeof(recipient)-1);

		if (strlen( recipient) )
		{
			NSFItemAppendTextList(hNote, "Recipients", recipient, (WORD) strlen(recipient), FALSE);
			DocToDelete = 0;
		}

		/* verbose */
#ifdef VERBOSE
		AddInLogMessageText("Add in Recipients : %s", NOERROR, recipient);
#endif

	}

	/* Return the Number of Recipients :Delete if nbrec = 0 or if all recipients were "" */
	return(DocToDelete);

	/* return(!nbrec); */

}


/*****************************************************************************************
* PROG		: TATUS	MoveFieldFromNote2Note()
* PURPOSE	: Transfert item from hNote to another
* OUTPUT	: status
******************************************************************************************/
STATUS	MoveFieldFromNote2Note(NOTEHANDLE FromNote, NOTEHANDLE ToNote, char* FieldName )
{
	STATUS		error = NOERROR;
	BLOCKID		item_blockid;
    WORD		FieldNameLength = 0;

	FieldNameLength = (WORD) strlen(FieldName);

	/* Get field from : FromNote */
	if ( NSFItemIsPresent(FromNote, FieldName, FieldNameLength ) )
	{
		/* Remove it from ToNote */
		NSFItemDelete(ToNote, FieldName,FieldNameLength );

		/* Copy */
		error = NSFItemInfo(FromNote, FieldName, FieldNameLength, &item_blockid, NULL, NULL, NULL);
		
		if ( error == NOERROR )
			error = NSFItemCopy(ToNote, item_blockid);
	}	

	return(error);
}

/********************************************************************************************
* SetReaderAccess
* ---------------
*
* IN	: Source HANDLE
*		: Source FieldName
* OUT	: Target HANDLE
**********************************************************************************************/
STATUS setReaderAccess(NOTEHANDLE hnote, char* fieldname, char* readersField, NOTEHANDLE tnote)
{
	STATUS		error = NOERROR;
	char		fullname[MAXUSERNAME];
	int			i, nbrec = 0;


	/* Append the FROM (sender) */
	if ( NSFItemGetText(hnote, MAILNOTE_FROM_FIELD, fullname, sizeof(fullname) -1) )
		AppendReaderItem (tnote, readersField, fullname, 1);


	/* Append all the entry found the fieldname */
	nbrec = NSFItemGetTextListEntries(hnote, fieldname);

	for (i=0; i < nbrec; i++)
	{
		/* read */
		if (NSFItemGetTextListEntry(hnote, fieldname,(WORD) i, fullname, sizeof(fullname)-1))
		{
			/* write - exit if error */
			if (error = AppendReaderItem (tnote, readersField, fullname, 1))
				i = nbrec;
		}
	}

	/*-- Append the Admin Role --*/
	if (error == NOERROR)
		AppendReaderItem (tnote, readersField, "[Admin]", 0);


	/* -- Append the SERVER as reader --*/
	if (error == NOERROR)
		AppendReaderItem (tnote, readersField, szServeur, 0);

	return(error);
}


/**********************************************************
* AppendMailHeadertoBody
* INPUT/OUT		: compound (RichText handler)
* INPUT			: Document handler
* ----------------------------------------------------------
* 3.93.31		: add Header_cc;
*				: FYI : "CC" is harded coded for now !
**********************************************************/
STATUS AppendMailHeaderToBody(NOTEHANDLE hNote, HANDLE*	hCompound,
							  char* From, char* To, char* OrigSubject)
{

	STATUS			error = NOERROR;
	char			Header_Subject[3000];
	char			Header_From[MAXUSERNAME];
	char			Header_To[1100];
	char			Header_cc[1100];
	char			tmp_Header_To[1000];
	DWORD			dwNormalStyle;
	COMPOUNDSTYLE	Style;


	/* init the Rich Text context to append text line */
	CompoundTextInitStyle(&Style);				  /* initializes Style to the defaults */

	if (error = CompoundTextDefineStyle(*hCompound, "Normal", &Style, &dwNormalStyle))
		return(error);

	/* Create the Header lines */
	sprintf(Header_From,		"%s\t\t: %s", BlackList_Notification_From_Header,	From);

	if ( MakeStringFromNotesAddressList(hNote, "SendTo", tmp_Header_To, sizeof(tmp_Header_To) ) )
		sprintf(Header_To,      "%s\t\t: %s", BlackList_Notification_To_Header,		To);
	else
		sprintf(Header_To,      "%s\t\t: %s", BlackList_Notification_To_Header,		tmp_Header_To);

	/* 3.93.31 : Display CC as well */
	if ( MakeStringFromNotesAddressList(hNote, "CopyTo", tmp_Header_To, sizeof(tmp_Header_To) ) == NOERROR )
		sprintf(Header_cc,      "cc\t\t: %s", tmp_Header_To);
	else
		Header_cc[0]='\0';

	/* subject */
	sprintf(Header_Subject,		"%s\t\t: %s", BlackList_Notification_Subject_Header, OrigSubject);

	/*--------------------------*/
	/*      Line One            */
	/*--------------------------*/

#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
								  HEADER_SEP,
                                  (DWORD) strlen(HEADER_SEP),
                                  NULLHANDLE))
				return(error);



	/* => From */
#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  Header_From,
                                  (DWORD) strlen(Header_From),
                                  NULLHANDLE))
				return(error);

	/* => To */
#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  Header_To,
                                  (DWORD) strlen(Header_To),
                                  NULLHANDLE))
				return(error);

	/* 3.93.31 => CC */
if ( Header_cc[0]!= NULL )
{
#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  Header_cc,
                                  (DWORD) strlen(Header_cc),
                                  NULLHANDLE))
				return(error);
}


	/* => Subject */
#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  Header_Subject,
                                  (DWORD) strlen(Header_Subject),
                                  NULLHANDLE))
				return(error);



	/*--------------------------*/
	/*      Line One            */
	/*--------------------------*/

#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
								  HEADER_SEP,
                                  (DWORD) strlen(HEADER_SEP),
                                  NULLHANDLE))
				return(error);

	/*-------------------------*/
	/* Blank line              */
	/*-------------------------*/

#ifdef V4
			if (error = CompoundTextAddParagraph
#else
			if (error = CompoundTextAddParagraphExt
#endif
								  (*hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  " ",
                                  (DWORD) strlen(" "),
                                  NULLHANDLE))
				return(error);


	/*------------*/
	/* normal end */
	return(error);

}







/***************************************************************************************
* PROCBLACKLIST_sub:
* Test Date from the Filter document
* Field Date #1 : see header file. 'mwadvt.h'
* Field Date #2 :
* call the  : int CheckRulesPeriod(TIMEDATE FirstUse_td, TIMEDATE LastUse_td)
* if returns (1) = expired or not started
***************************************************************************************/
int CheckBlackListRulesPeriod(NOTEHANDLE hNote)
{
	TIME		StartDate;
	TIME		EndDate;
	TIMEDATE	today_td;
	int			Expired_Cond = 0;
	int			Started_Cond = 0;

	/*debug
	char		ligne[300];
	*/
	/*----------------------------------*/
	/* if error then the rules is OK :) */
	/*----------------------------------*/

    /* Get current TimeDate */
    OSCurrentTIMEDATE(&today_td);


	/* Field #1 : Expired */
	if (NSFItemGetTime(hNote, BLACKLIST_EXPIRED_DATE, &EndDate.GM) )
	{
	   /* Obtain the difference in SECONDS between two TIMEDATEs
		this difference > 0 when Today is smaller than Expired Date */

		/*debug
		TimeGMToLocal(&EndDate);
		sprintf(ligne, "%02d/%02d/%02d", EndDate.day, EndDate.month, EndDate.year);
		debug*/


		Expired_Cond = (TimeDateDifference(&today_td, &EndDate.GM) > 0) ;
		if (Expired_Cond)
			return(1);
	}


	/* Field #2 : Start Date */
	if (NSFItemGetTime(hNote, BLACKLIST_START_DATE,   &StartDate.GM) )
	{

		/*debug
		TimeGMToLocal(&StartDate);
		sprintf(ligne, "%02d/%02d/%02d", StartDate.day, StartDate.month, StartDate.year);
		debug*/

		Started_Cond = (TimeDateDifference(&today_td, &StartDate.GM) < 0 );
	}

	return(Started_Cond | Expired_Cond);

}



/********************************************************************
 * PURPOSE	: Detach all attached files to disk
 *
 * INPUT	: current email handle
 * INPUT	: Directory.
 *
 * Called from ProcBlackList : based on 'BList_DetachToDisk' field
 ********************************************************************/
STATUS DetachAllFileToDisk(NOTEHANDLE note_handle, char* FileOnDisk)
{
	STATUS error = NOERROR;

	/* Loop over the OBJECT item */
	/* Detach all in FileOnDisk (prefix) keeping the same filename */

	return(error);
}


/************************************************************************
 * SplitGroup - Eclate les groupes en recipients (routage)
 * Inputs : NOTEHANDLE
 * Ouput : void
 * ---------------------------------------------------------------------
 * Build 3.8ab6.
 * Copy temp => temp_noDomain +  RemoveNotesDomain(temp_noDomain)
 * still work with temp in the rest of the routine.
 * ----------------------------------------------------------------------
 * Fix : 14/3/03.
 * 2 fixes
 * Split group if the GroupType = "1".
 * Do not add to the Recipients list the "From".
 ************************************************************************/
void SplitGroup(NOTEHANDLE hNote, char* FromValue)
{
	STATUS sErreur;
	HANDLE hLookup = NULLHANDLE;
	char *pLookup, *pName, *pMatch;
	WORD numret, nbrec, j, k;
	char temp[MAXUSERNAME];
	unsigned int i;
	char fromfield[12];
	char tofield[12];
	BOOL recurs = FALSE;
	char temp2[MAXUSERNAME];
	char temp2_upper[MAXUSERNAME];
	char temp2_upper_abbr[MAXUSERNAME];
	char temp_noDomain[MAXUSERNAME]; /* since 3.8ab6 */

	recurs = TRUE;
	for (i=0; i<6; i++) /* Max is 6 */
	{
		/* Si plus de groupe trouvé ET liste dans Recipients ==> on gicle */
		if ((!(i%2)) && (!recurs))
			break;
		recurs = FALSE;
		if (i%2)
		{
			strcpy(fromfield, "MwTmp");
			strcpy(tofield, "Recipients");
		}
		else
		{
			strcpy(fromfield, "Recipients");
			strcpy(tofield, "MwTmp");
		}

		nbrec = NSFItemGetTextListEntries(hNote, fromfield);
		for (j=0; j<nbrec; j++)
		{
			NSFItemGetTextListEntry(hNote, fromfield, j, temp, sizeof(temp)-1);

			/* Remove the @Domain if is at the end of 'temp' : 3.8ab6 */
			strcpy(temp_noDomain, temp);
			RemoveNotesDomain(temp_noDomain);

			/* Look for "Members" field */
			/* Fix : 13/3/03 : Router is using "($Users)"  */
			if (sErreur = NAMELookup(NULL, 0, 1, "($Users)", 1, temp_noDomain, 1, "Members", &hLookup))
			{
				if (NULLHANDLE != hLookup)
					OSMemFree(hLookup);

				NSFItemAppendTextList(hNote, tofield, temp, (WORD) strlen(temp), TRUE);
				continue;
			}

			pLookup = OSLockObject(hLookup);
			pName = NAMELocateNextName(pLookup, NULL, &numret);
			if (!numret)
			{
				OSUnlockObject(hLookup);
				OSMemFree(hLookup);

				NSFItemAppendTextList(hNote, tofield, temp, (WORD) strlen(temp), TRUE);
				continue;
			}

			recurs = TRUE;
			pMatch = NAMELocateNextMatch(pLookup, pName, NULL);

			k=0;
			do
			{
				if (sErreur = NAMEGetTextItem(pMatch, 0, k, temp2, sizeof(temp2)-1))
				{
					/* Error when k = 0 means there is NO "Member field" => user */
					if (k == 0)
					{
						NSFItemAppendTextList(hNote, tofield, temp, (WORD) strlen(temp), TRUE);
						recurs = FALSE;
					}
					break;
				}

				/* fix : 13/03/03 : Check if the 'address' == 'from' */
				strcpy(temp2_upper, temp2);
				UpperCase(temp2_upper);
				DNAbbreviate(0L, NULL, temp2_upper, temp2_upper_abbr, MAXUSERNAME, NULL);

				if (strcmp(FromValue, temp2_upper_abbr))
					NSFItemAppendTextList(hNote, tofield, temp2, (WORD) strlen(temp2), TRUE);
				k++;
			}       while (!sErreur);

			OSUnlockObject(hLookup);
			OSMemFree(hLookup);
		}       /* fin itération sur liste destinataire */

		NSFItemDelete(hNote, fromfield, (WORD) strlen(fromfield));

	} /* fin de récursivité */
}


/**************************************************************************************
* PURPOSE	: read and store BlackList argument.
*
* INPUT		: tmpr : reference to a BlackListRules structure
*			: note_handle : reference to a BlackList document
*
* OUTPUT	: STATUS
* released	: 04/04/2003. Fix 3.91
*******************************************************************************************/
STATUS GetBlackListAction(BLrules *tmpr, NOTEHANDLE note_handle)
{
	STATUS	error = NOERROR;

	/* --------------- This Rules Terminates Argt ------------------*/
	tmpr->Argt_ThisRulesTermintes = GetBlackListAction_BOOL(note_handle, "BList_MW_LAST_Flag");

	/*--- Normal end ---*/
	return(error);
}

/***********************************************************************************
* PURPOSE	: Read Boolean argument stored in the BlackList
* ---------------------------------------------------------------------------------
* INPUT		: NOTEHANDLE	: BlackList document
*			: char*			: BlackList Argument field name
* OUTPUT	: int			: 0 or 1*
************************************************************************************/
int GetBlackListAction_BOOL(NOTEHANDLE note_handle, char* fieldname)
{
	char MW_RAPP_Flag[4];
	int  action = 0;

	/* Check if the rules is the LAST that will be checked !!! */
	if (NSFItemGetText(note_handle, fieldname, MW_RAPP_Flag, sizeof(MW_RAPP_Flag)-1))
		action = 1;

	return(action);
}


/******************************************************************************
* mark this note with MailWatcher task flag
*******************************************************************************/
void MarkNoteNum(NOTEHANDLE hNote, NUMBER*	num)
{

		/* set the number of MailWatcher process */
		*num = 0;
		NSFItemGetNumber(hNote, "mwnum", num);

		/* Convert to native format if necessary as Notes is IEEE but not OS/390 */
		/* Notes gave us an IEEE double which must be converted to [ native ] */
		#ifdef OS390
		ConvertIEEEToDouble(num, num);
		#endif

		*num += 1;


		#ifdef OS390
		/* Notes gave us an native to IEEE double which must be converted [ back ] */
		ConvertDoubleToIEEE(num, num);
		#endif

		/* save the number */
		NSFItemSetNumber(hNote, "mwnum", num);

		/* restore to native format */
		#ifdef OS390
		/* Notes gave us an IEEE double which must be converted to [ native ] */
		ConvertIEEEToDouble(num, num);
		#endif
}


/****************************
* Remove leading blank
****************************/
void Trim(char* in, char* out)
{
	/* go one by one until no more blank or end of string */
	while ((*in == ' ') && (*in != NULL) )
		in++;

	/* no more blank or nothing ;) */
	strcpy(out, in);
}



/*****************************************************************************
* MakeStringFromNotesAddressList
* INPUT		: hnote
*			: notes field
* 			: char[ ] / in/out
*			: Max length
* OUTPUT	: STATUS
*********************************************************************************/
STATUS MakeStringFromNotesAddressList(NOTEHANDLE hNote, char* fieldname, char* out_string, int max_len)
{
	STATUS	error = NOERROR;
	int		wnb   = 0;
	int		wbcl  = 0;
	int		wlen  = 0;
	char	qui[MAXUSERNAME];
	char	quiabr[MAXUSERNAME];

	/* init out_string : end string */
	out_string[0] = '\0';

	/* Get number of entries */
	wnb = NSFItemGetTextListEntries(hNote, fieldname);

	for (wbcl=0; wbcl<wnb; wbcl++)
	{
		wlen = NSFItemGetTextListEntry(hNote, fieldname, wbcl, qui, sizeof(qui)-1);

		/* break as the current item is empty */
		if (!strlen(qui))
			break;

		/* reduce size / abbreviate */
		DNAbbreviate(0L, NULL, qui, quiabr, MAXUSERNAME, NULL);

		/* over flow => stop */
		if ( (int) ( strlen(out_string) + strlen(quiabr) + 2 ) > max_len - 3)
		{
			strcat(out_string, "...");
			break;
		}
		else
		{
			/* add a new item */
			if ( strlen(out_string) )
			{
		 		strcat(out_string, ", ");
				strcat(out_string, quiabr);
			}
			/* first item */
			else
				strcpy(out_string, quiabr);
		}

	}	/* end for */


	/* normal end */
	return(error);

} /* end : MakeStringFromNotesAddressList */



/********************************************************************
* new in release 3.92                                               
* AddUserAlias                                                      
* consider user as a group using $user                              
* INPUT	: username (upper, abbreviated)                             
*                                                                   
* OUTPUT : Status                                                   
* if error then alias have not been found or uploaded correctly     
* so the user address will stay as "1" = user                       
* ------------------------------------------------------------------
* Field containing user's different possible addresses
* ------------------------------------------------------------------
* ShortName (text list)
* Fullname (text)
* AltFullName (text list)
* InternetAddress (text)
*********************************************************************/
STATUS GetUserAllAlias(char* username, GrpList* GrpHistory, char** FieldPredicatList, int pred_nbitem)
{
	int			AliasFound = 0;
	GrpList*	PredicatList = NULL;			/* 3.95.1 */
	int			PredicatsFound = 0;

	/* create */
	if ( pred_nbitem > 0 )
	{
		PredicatList = (GrpList *) malloc ( sizeof(GrpList) );
	
		if ( PredicatList == NULL )
			AddInLogMessageText("not enough memory in %s - no predicats for %s", NOERROR, "GetUserAllAlias", username);
		else
		{
			PredicatList->GroupName = NULL;
			PredicatList->Next      = NULL;
		}
	}

	/* convert user name */
	UpperCase(username);

	if ( PredicatList != NULL )
	{
		while (*FieldPredicatList != NULL )
		{
			PredicatsFound = PredicatsFound + GetUserPredicat(username, *FieldPredicatList, &PredicatList);
			*FieldPredicatList++;
		}
	}

	if ( PredicatsFound > 0 )
		FirstEle = InsertUser(FirstEle, username, "**PREDICATS**", PredicatList);

	/* create the group list = username, all the alias will be part of this group */
	GrpHistory = InsertGroup(GrpHistory, username);

	if ( GrpHistory != NULL )
	{
		/* call for "ShortName", "FullName", "InternetAddress", "AltFullName" */
		AliasFound =              GetUserAlias(username, "shortname",       GrpHistory, PredicatsFound, PredicatList);

		AliasFound = AliasFound + GetUserAlias(username, "FullName",        GrpHistory, PredicatsFound, PredicatList);
		AliasFound = AliasFound + GetUserAlias(username, "InternetAddress", GrpHistory, PredicatsFound, PredicatList);
	
		/* 3.93.27 : use Internet Forward address too */
		AliasFound = AliasFound + GetUserAlias(username, "MailAddress",     GrpHistory, PredicatsFound, PredicatList);
		AliasFound = AliasFound + GetUserAlias(username, "ccMailLocation",  GrpHistory, PredicatsFound, PredicatList);
		AliasFound = AliasFound + GetUserAlias(username, "AltFullName",     GrpHistory, PredicatsFound, PredicatList);


		/* Remove this group (username) from the group history as all the "alias" have been added to */
		PopGroupHistory(&GrpHistory);
	}


	/* free Predicat list */
	if (PredicatList != NULL )
		FreeGrpList(PredicatList);

	/* return and error code when nothing has been uploaded */
	if (AliasFound)
		return(NOERROR);
	else
		return(1);

} /* end GetAllUserAlias */



/* NAB Query for a specific field */
int GetUserAlias(char* username, char* fieldname, GrpList* GrpHistory, int PredicatsFound, GrpList* PredicatList)
{
	STATUS	sErreur = NOERROR;
	HANDLE	hLookup = NULLHANDLE;
	char	*pLookup, *pName, *pMatch;
	WORD	numret = 0;
	WORD	k = 0;
	int		check = 0;
	char	user_alias_abbr[MAXUSERNAME];
	char	user_alias[MAXUSERNAME];

	/* Create the user as a group */
	UpperCase(username);

	/* Lookup the alias by looking up in the $users view */
	if (sErreur = NAMELookup(NULL, 0, 1, "($Users)", 1, username, 1, fieldname, &hLookup))
	{
		if (NULLHANDLE != hLookup)
			OSMemFree(hLookup);

		return(0);
	}

	/* debug
	sprintf(LogString, "GetUserAlias : handle %i \n",  (int) hLookup);
	LogLine("mwadvt.log", LogString, 1);
   */

	/* found ! : new get the values */
	pLookup = OSLockObject(hLookup);
	pName = NAMELocateNextName(pLookup, NULL, &numret);
	if (!numret)
	{
		OSUnlockObject(hLookup);
		OSMemFree(hLookup);
		/* nothing has been uploaded = $user found but no alias are to be uploaded */
		return(0);
	}

	/* loop over the lookup buffer */
	pMatch = NAMELocateNextMatch(pLookup, pName, NULL);
	k = 0;

	do
	{
		if (sErreur = NAMEGetTextItem(pMatch, 0, k, user_alias, sizeof(user_alias)-1))
		{
			/* Error when k = 0 means there is NO "alias field" */
			if (k == 0)
			{
				OSUnlockObject(hLookup);
				OSMemFree(hLookup);
				/* nothing has been uploaded user remain a user */
				return(0);
			}
			break;
		}

		/* upload field values when not "" */
		if ( strlen(user_alias) > 0 )
		{
			UpperCase(user_alias);
			DNAbbreviate(0L, NULL, user_alias, user_alias_abbr, MAXUSERNAME, NULL);
			FirstEle = InsertUser(FirstEle, user_alias_abbr, username, GrpHistory);
			
			/* 3.95 Insert Predicat history */
			if ( PredicatsFound > 0 )
				FirstEle = InsertUser(FirstEle, user_alias_abbr, "**PREDICATS**", PredicatList);


			check = 1;
		}
		k++;

	} while (!sErreur);

	/* free memory */
	OSUnlockObject(hLookup);
	OSMemFree(hLookup);

	/* ok some alias have been uploaded (if all of them where not blank ;)so => user (1) will be changed to (2 = Group) */
	return(check);

} /* end GetUserAlias */




/* Recal the rules weight */
void ResetRulesw(int* w, int From_User, int To_User)
{
   /* 11 : User => User, stay 11 when From_user = 0 and To_User = 0
	*  11 : User => User, become : 21 when From_To = 1 and To_User = 0
	*  21 is Group => User where Group is the user's all alias
	*  22 : When From_User = 1 and To_User = 1 so Group => Group
    * From_User & To_User can be 1 or something else if it stays at 1 it means that user's alias
	* have been correctly uploaded so the User (type) become a Group (type) so 1 becomes 2
	*/

	if (From_User == 1)
		*w = ( *w ) + 10;

	if (To_User == 1)
		*w = ( *w ) + 1;

} /* end of : ResetRulesw */


/*****************************************************************************
* GetWord(string, sep, pos)
* INPUT			: string, Separator
* OUTPUT		: (char*)string or token if found.
* ----------------- argt3 : a -------------------------------------------------
* if pos = 0 => last word
* if pos = 1 => first word and so on pos = 2 => second word...
*******************************************************************************/
char* GetWord(char * string, const char *seps, int a)
{
    int t		= 0;
	int	cond	= 1;
	char *token;
	char* savetoken = NULL;

	/* first token */
	token = strtok(string, seps );

	while( ( token != NULL ) & ( cond ) )
	{
      /* inc nb of tokens in the "string" */
		t = t+1;

	   /* Save then Get next token: */
	  savetoken = token;

	  /* test cond : counter (pos) */
	  if (a)
		  cond = t < a;

      token = strtok( NULL, seps );
	}


	return(savetoken);
} /* end of GetWord */


/*******************************************************************************************
* STATUS NoteHandleNativeConvertMIME2CD(NOTEHANDLE in-out)
*
* PURPOSE	: close and reopen with MIME to CD conversion using the notes native method
*			: This procedure DOES NOT UPDATE the NOTEHANDLE so all the temporary data will
*			: be lost, the caller has to do the update before.
*			:
* INPUT		: NOTEHANDLE address
*			: Current DB (mail.box)
*			: note_id to reopen.
*			:
* OUTPUT	: Status (success or not)
* -----------------------------------------------------------------------------------------
* not documented nnotes.dll ( MCConvertBodyParts )
********************************************************************************************/
STATUS NoteHandleNativeConvertMIME2CD(NOTEHANDLE* hNote, DBHANDLE hDB, NOTEID note_id)
{
	STATUS	error = NOERROR;

#ifndef V4

	/* retreive the note_id if not supplied */
	if (note_id == 0)
		NSFNoteGetInfo(*hNote, _NOTE_ID, &note_id);


	/* close */
 	if ( error = NSFNoteClose( *hNote ) )
		return(error);

	/* Open with conversion */
	error = NSFNoteOpen(hDB, note_id, 0, hNote);

	/* try to reopen with the original method and return and ERROR */
	if (error)
	{
		/* this prog is only available in V5 */
		error = NSFNoteOpenExt(hDB, note_id, OPEN_RAW_MIME, hNote);
	}

#endif

	return(error);

} /* end of NoteHandleNativeConvertMIME2CD */

/*************************************************************************
* DeleteNote
* PURPOSE : easy delete notehandle if note_id not known.
* INPUT		: database handle     : hMN
*			: note memory handler : hNote
*			: note_id or ZERO
*
* OUTPUT	: STATUS
*
* Delete a hNote if note_id is not defined then get it
********************************************************************/
STATUS DeleteNote(DBHANDLE hMB, NOTEHANDLE hNote, NOTEID note_id)
{
	STATUS	error = NOERROR;

	/* retreive the note_id if not supplied */
	if (note_id == 0)
		NSFNoteGetInfo(hNote, _NOTE_ID, &note_id);

	/* delete : FORCE */
	error = NSFNoteDelete(hMB, note_id, UPDATE_FORCE);

	return(error);
} /* end of DeleteNote */


/****************************************************************
* Move a doc to a Folder                                        *
* INPUT		: DBHandle (open)									*
*			: char * : mailfile									*
*			: char * : the folder name.							*
*			: NOTEHANDLE (open)									*
* OUTPUT	: error /STATUS                                     *
*****************************************************************/
STATUS	MoveToFolder(DBHANDLE mail_handle, char* mailfile, char* FolderName, NOTEHANDLE notehandle)
{
	STATUS		erreur;
	char		ligne[300];
	NOTEID		foldid;		/* folder id */
	NOTEID		retID;		/* notehandle id */
	HANDLE		hIDTable;


	/* check if it exists - otherwise creates it or return an error */
	if (erreur = NIFFindView(mail_handle, FolderName, &foldid))
	{
		/* Create if it does not exists */
		erreur = CreateFolder(mail_handle, FolderName, &foldid);

		if (erreur)
		{
			sprintf(ligne, FOLDER_NOT_FOUND, FolderName, mailfile);
			LogEventText(ligne, NULLHANDLE, erreur);
			return(erreur);
		}
	}


	/* Get the NOTEID associated to the current NOTEHANDLE */
	NSFNoteGetInfo(notehandle, _NOTE_ID, &retID);


	/* Moving a NOTEID to the specified folder, (option : can mark the date of this action) */
	erreur = IDCreateTable(sizeof(NOTEID), &hIDTable);

	if (erreur == NOERROR)
	{
			/* Move to Folder now */
			IDInsert(hIDTable, retID, NULL);

			if (erreur = FolderDocAdd(mail_handle, NULLHANDLE, foldid, hIDTable, 0L))
			{
	 		 	sprintf(ligne, ERR_CREATE_FOLDER, FolderName, mailfile);
				LogEventText(ligne, NULLHANDLE, erreur);
				return(erreur);
			}
			IDDestroyTable(hIDTable);
/*			OSMemFree(hIDTable);*/
	}
	else
	{
 		 	sprintf(ligne, ERR_MOVE_TO_FOLDER, FolderName, mailfile);
			LogEventText(ligne, NULLHANDLE, erreur);
	}


	return(erreur);
	/*end of prog */
}


/*************************************************************
* Create a Folder
* INPUT		: DBHANDLE -> Database handle
*			: char * -> Folder name
*
* OUTPUT	: Status (Error/NoError)
***************************************************************/
STATUS CreateFolder(DBHANDLE db, char* FolderName, NOTEID* foldid)
{
	STATUS		error = NOERROR;

	/* CreateFolder */
	error = FolderCreate (db, NULLHANDLE,			/* dbases */
							  0L, NULLHANDLE,		/* use the default folder */
                              FolderName, MAXWORD, /* MAXWORD is used as FolderName is null terminated */
							  DESIGN_TYPE_SHARED,
							  0L,
							  foldid);
	/* end */
	return(error);
}



/*********************************************************************
* DUPLICATE MAIL.
*
* IN	: current DBHANDLE of hnote.
* IN	: current hnote
* OUT	: address of the new hNote in the current DBHANDLE
* -------------------------------------------------------------------
* Fix 3.92	: Remove temporary fields.
* new 3.92	: convert MIME to CD add int clone argument to not to remove
*			: temporay fields
*********************************************************************/
STATUS SplitMail(DBHANDLE* hMB, NOTEHANDLE hNote, NOTEHANDLE* hCopy)
{

	STATUS error = NOERROR;
	OID new_oid;

	/* copy */
	if (error = NSFNoteCopy(hNote, hCopy))
			return(error);



	if (error = NSFDbGenerateOID(*hMB, &new_oid))
	{
			NSFNoteClose(*hCopy);
			return(error);
	}

	/* Set Notes internal IDs */
	NSFNoteSetInfo(*hCopy, _NOTE_OID, &new_oid);
	NSFNoteSetInfo(*hCopy, _NOTE_ID, NULL);
	NSFNoteSetInfo(*hCopy, _NOTE_DB, hMB);

	/* Purge MailWatcher temporary working field (RecOK, RecNotOK) fields */
	NSFItemDelete(*hCopy, MAILNOTE_RECIPIENTS_FIELD, (WORD) strlen(MAILNOTE_RECIPIENTS_FIELD));

	/* add a field that will mark this message as a duplicate */
	NSFItemSetText(*hCopy, MW_MAILDUPLICATE, "1", (WORD) strlen("1"));

	/* debug
	sprintf(LogString, "hNote : %i  \thCopy %i \n", (int) hNote, (int) (*hCopy) );
	LogLine("mwadvt.log", LogString, 1);
    */

	return(error);
}


/*********************************************
 Check if the email address is a user
 return TRUE or FALSE

**********************************************/
int	isUserAddress( char* temp_noDomain )
{
	STATUS	sErreur = NOERROR;
	HANDLE			hLookup = NULLHANDLE;
	char			*pLookup, *pName;
	WORD			numret;


	/* Look for "Members" field */
	if (sErreur = NAMELookup(NULL, 0, 1, "($Users)", 1, temp_noDomain, 1, "Members", &hLookup))
	{
		if (NULLHANDLE != hLookup)
			OSMemFree(hLookup);

		return(1);
	}

	/* debug
	sprintf(LogString, "isUserAddress : handle %d \n",  (int) hLookup);
	LogLine("mwadvt.log", LogString, 1);
    */

	pLookup = OSLockObject(hLookup);
	pName = NAMELocateNextName(pLookup, NULL, &numret);
	if (!numret)
	{
		OSUnlockObject(hLookup);
		OSMemFree(hLookup);
		return(1);
	}
	else
	{
		/* free memory */
		OSUnlockObject(hLookup);
		OSMemFree(hLookup);
		return(0);
	}

}




/**************************************************************************************
* SaveStatDoc
*
* PURPOSE	: Write to db the Stat record
* INPUT		: Current handle to the record
*			: Nb of field
*			: Db field name
*
* OUTPUT	: STATUS (error code)
***************************************************************************************/
STATUS SaveStatDoc(NOTEHANDLE hNoteStat, NUMBER nbchamp, char* StatFile)
{
	STATUS	error = NOERROR;

	#ifdef OS390
	/* Notes gave us an native to IEEE double which must be converted [ back ] */
		ConvertDoubleToIEEE(&nbchamp, &nbchamp);
	#endif


	if (error = NSFItemSetNumber(hNoteStat, "NbStat", &nbchamp))
	{
	 	AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);
		return(error);
    }


	#ifdef OS390
	/* Notes gave us an IEEE double which must be converted to native */
		ConvertIEEEToDouble(&nbchamp, &nbchamp);
	#endif


	/* Display the error : do not return, continue => close and reset data */
	if ( error = NSFNoteUpdate(hNoteStat, UPDATE_FORCE))
	 	AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);

	/* close the open handle */
	NSFNoteClose(hNoteStat);

	return(error);
}


/*******************************************************************************
* CreateStatDoc( hStatdb, &hNoteStat )
* PURPOSE		: init stat record
* INPUT			: Database handle
* INPUT.OUTPUT	: Ref to the new record handle
*
* OUTPUT		: Error status
**********************************************************************************/
STATUS	CreateStatDoc(DBHANDLE hStatdb, NOTEHANDLE* hNoteStat, char* StatFile )
{
	STATUS		error = NOERROR;
	TIMEDATE	actuel;

	/* Creation */
	if (error = NSFNoteCreate(hStatdb, hNoteStat))
	{
	 	AddInLogMessageText(ERR_CREATE_DOC, error, NOMAPI, StatFile);
		return(error);
	}


	/* Form name */
	if ( error = NSFItemSetText(*hNoteStat, "Form", "Stat_Col", 8) )
	{
	 	AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);
		NSFNoteClose(*hNoteStat);
		return(error);
    }


	/* Server Name */
	if ( error = NSFItemSetText(*hNoteStat, "Server", szServeur, (WORD) strlen(szServeur)))
	{
	 	AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);
		NSFNoteClose(*hNoteStat);
		return(error);
    }


	/* Domain */
	if ( error = NSFItemSetText(*hNoteStat, "Domain", szDomain, (WORD) strlen(szDomain)))
	{
	 	AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);
		NSFNoteClose(*hNoteStat);
		return(error);
    }


	/* Date */
	OSCurrentTIMEDATE(&actuel);
	if ( error = NSFItemSetTime(*hNoteStat, "Date", &actuel))
	{
	 	AddInLogMessageText(ERR_UPDATE_DOC, error, NOMAPI, StatFile);
		NSFNoteClose(*hNoteStat);
		return(error);
    }

	return(error);
}


/***********************************************************/
/*   LogLine() : Write to Log file                         */
/***********************************************************/
void LogLine(char *file_name, char *line, int display_time)
{
  FILE			*stream;
  struct tm		*time_date;
  time_t		clock;
  char			buffer[256];

  /* Ouverture du fichier */
  if(file_name == NULL)
    return;
  stream = fopen(file_name,"a+");

  if(stream != NULL)
  {
    /* Ecriture de la date courante */
     if(display_time)
     {
        time(&clock);
        time_date = localtime(&clock);

		sprintf(buffer,"\n%02d/%02d/%4d %02d:%02d:%02d ",
                  time_date->tm_mday,time_date->tm_mon+1,1900+time_date->tm_year,
                  time_date->tm_hour,time_date->tm_min,time_date->tm_sec);
          fwrite(buffer,strlen(buffer),1,stream);
     }
     else
       fwrite("\n",1,1,stream);

    /* Ecriture du message */
    fwrite(line,strlen(line),1,stream);
    fclose(stream);
  }
}

/**************************************************************
* MacroForCDOnly(filter_formula)
* Check out if the MACRO formula used requires or not to have
* MIME to CD conversion
* INPUT		: char* : formula (by value)
* OUTPUT	: 1 or 0.
**************************************************************/
int MacroForCDOnly(char * filter_formula)
{
	int		CDformula = 0;
	char*	filter_formula_upper;

	/* Convert uppercase before test */
	filter_formula_upper = (char *) malloc ( strlen(filter_formula) + 1 );

	/* short memory ? */
	if ( filter_formula_upper == NULL )
		return(CDformula);

	/* is there some "special" @Macro (see mwadvt.h) in this string ? */
	strcpy(filter_formula_upper, filter_formula);
	UpperCase(filter_formula_upper);

	if ( strstr (filter_formula_upper, MACRO_REQUIRING_MIME2CD) )
		CDformula = 1;

	/* end */
	free(filter_formula_upper);
	return(CDformula);
}


/*****************************************************************
* ConvertToMIME-Temporary doc
*
* to convert safely to MIME
*  duplicate memory by a memory copy
*  update + reopen the new copy as a new note hNote
*  if No Error the hNote is save but also will be destroyed
*
* INPUT		: Original notes format (MIME or Not)
* OUTPUT	: New note in CD format (confirmed by OUTPUT = 1)
* OUTPUT	: routing success = 1
*******************************************************************/
int	TemporaryhNoteMIME2CD_Old(DBHANDLE hDB, NOTEHANDLE hNote, NOTEHANDLE* hNewNoteCD)
{
	/* lv */
    int		bNewNoteCD = 0;
	STATUS	erreur	= NOERROR;

	/*------------------------- RAW MIME in Body field --------------------------*/
	if ( NSFItemIsPresent(hNote, "$NoteHasNativeMIME", 18) || ( NSFItemIsPresent(hNote, "MIME_Version", 12)) )
	{
	   /* to convert safely to MIME
		*  duplicate memory by a memory copy
		*  update + reopen the new copy as a new note hNote
		*  if No Error the hNote is save but also will be destroyed
		*/
		erreur = DuplicateMail(&hDB, hNote, hNewNoteCD, 1); /* argt 4 = 1 we want a real copy will all the fields */

		/* before saving on disk : remove the MWTAG field : this document is not a REAL mail */
		NSFItemDelete(*hNewNoteCD, MWTAGFIELD,(WORD) strlen(MWTAGFIELD));

		if (erreur == NOERROR)
			erreur = NSFNoteUpdate(*hNewNoteCD, UPDATE_FORCE);

		/* reopen to get MIME => CD conversion */
		if (erreur == NOERROR)
		{
			erreur = NoteHandleNativeConvertMIME2CD(hNewNoteCD, hDB, 0);
		}

		if (erreur == NOERROR)
		{
			/* this document is to be deleted */
			bNewNoteCD = 1;
		}
	}

	/* end */
	return(bNewNoteCD);
}




/*****************************************************************
* ConvertToMIME-Temporary doc
* 15/09/04	: Revision : 3.93.19 =>> Release 3.94.
*
*
* INPUT		: Original notes format (MIME or Not)
* OUTPUT	: New note in CD format (confirmed by OUTPUT = 1)
* OUTPUT	: routing success = 1
*******************************************************************/
int	TemporaryhNoteMIME2CD(DBHANDLE hDB, NOTEHANDLE hNote, NOTEHANDLE* hNewNoteCD)
{
	/* lv */
    int		bNewNoteCD = 0;
	NOTEID	note_id;
	STATUS	erreur	= NOERROR;

	/*------------------------- RAW MIME in Body field --------------------------*/
	if ( NSFItemIsPresent(hNote, "$NoteHasNativeMIME", 18) || ( NSFItemIsPresent(hNote, "MIME_Version", 12)) )
	{

		/* Get the note_id of the hNote  */
		NSFNoteGetInfo(hNote, _NOTE_ID, &note_id);

		/* Reopen with MIME to CD conversion using hNewNoteCD handle */
		erreur = NSFNoteOpen(hDB, note_id, 0, hNewNoteCD);

		if (erreur == NOERROR)
		{
			/* this document is to be deleted */
			bNewNoteCD = 1;
		}
	}

	/* end */
	return(bNewNoteCD);
}




/****************************************************
* DeleteNoteWith_NoteHandle( hNote )
*
* INPUT		: NOTEHANDLE
* OUTPUT	: STATUS
***********************************************************/
STATUS DeleteNoteWith_NoteHandle(DBHANDLE hDB, NOTEHANDLE hNewNoteCD)
{
	STATUS		error = NOERROR;
	NOTEID		note_id;

	/* Get the note_id of the hNote "MIME=>CD" */
	NSFNoteGetInfo(hNewNoteCD, _NOTE_ID, &note_id);

	error = NSFNoteClose(hNewNoteCD);

	if (error == NOERROR)
		error = NSFNoteDelete(hDB, note_id, UPDATE_FORCE);

	return(error);
}


/**********************************************************
* NSFGetAttachmentFileName()
* INPUT		: BLOCKID
* I/O		: Filename
* I/O		: Filename extension
* OUTPUT	: STATUS
************************************************************/
void NSFGetAttachmentFileName(BLOCKID bidv, char* FileName, char* FileNameExtension)
{
	STATUS		error = NOERROR;
	void		*ptrItemValue;
	WORD		*ptrItemValueWORD;
	FILEOBJECT	pFileObject;
	WORD		wlen = 0;
	char*		loc;

	/* Save the original pointer */
	ptrItemValue = OSLockBlock(void, bidv);

	/*  Step over the data type word to point to the actual data */
	ptrItemValueWORD = (WORD *) ptrItemValue ;
	ptrItemValueWORD++ ;
	ptrItemValue = ptrItemValueWORD ;

	/* Use ODSReadMemory */
	ODSReadMemory( &ptrItemValueWORD, _FILEOBJECT, &pFileObject, 1 );

	/* Free memory  */
	OSUnlockBlock(bidv);

	/* FileName and its Extension */
	if (pFileObject.Header.ObjectType == OBJECT_FILE)
	{
		wlen = pFileObject.FileNameLength;

		/* Read the data the comes after the pFileObject */
		ptrItemValue = (char *) ptrItemValue + ODSLength(_FILEOBJECT);

		/* get File extension : if none log the filename */
		strncpy(FileName, (char *) ptrItemValue, wlen);
		FileName[wlen]='\0';

		/* get Extension */
		strcpy(FileNameExtension, FileName);
		loc = GetWord(FileNameExtension, ".", 0);
		if (loc)
			strcpy(FileNameExtension, loc);
	}

} /* end NSFGetAttachmentFileName */


/*****************************************************************
* PURPOSE		: get the proper internet address
*				: remove "" and <> char
* INPUT/OUTPUT	: char* (the address)
* OUTPUT		: strlen (OUTPUT)
*****************************************************************/
int CleanUpAddress(char * useraddress)
{
	char* pos_start;
	char* pos_end;

	/* check if this address match *<*@*.*>*    */
	if (wc_match (MIME_INTERNET_ADDRESS_FORMAT, useraddress))
	{
		/* start at the first '<' */
		pos_start = strchr(useraddress, '<');

		if (pos_start != NULL)
		{
			/* cut after the last '>' */
			pos_end = strchr(pos_start, '>');
			if (pos_end != NULL)
				pos_end = '\0';
		}
	}

	/* end */
	if (useraddress != NULL)
		return( (int) strlen(useraddress));
	else
		return(0);
}/* end CleanUpAddress */







/************************************************************************************
* Prog		: SaveOnDiskAttachFileWithException
* -------------------------------------------------------------------------------------
* PURPOSE	: iterative version - detach $FILE fields but only if the file
*			: has the specified Extension)
* INPUT		: current message
* INPUT		: List of Extension (char* ExtensionList ; example = "doc,zip,txt,exe"
* INPUT		: char* filepath : where to dump the attachments
* INPUT		: New Notes status (int = 1/O)
* INPUT		: current mail.box handle (hDB)
* OUTPUT	: STATUS.
* -----------------------------------------------------------------------------------
* Extension :
*           : if HostAttachmentLink is not empty and the current note_handle is still
*           : the original mail we need to duplicate and work out on an new copy
*           : before appending 'HostAttachmentLink+filepath' to the body field
*           : mind that we might also need to close/reopen the duplicated mail if body
*           : field is in native MIME format
* -----------------------------------------------------------------------------------
* 27/05/04	: revision 3.93.11
*			: do not use the filepath ( ) for the link
*************************************************************************************/
STATUS SaveOnDiskAttachFileWithException(NOTEHANDLE* note_handle, char* AttachFieldName, char* ExtensionList,
										 char* filepath, char* HostAttachmentLink, int* bNewNote, DBHANDLE hDB,
										 int nbrec, int current_rec, char* mail_ID)
{

 STATUS	  		error = NOERROR;
 char			FileName[MAXPATH];
 char			DetachFileName[MAXPATH*2];
 char**			FileNameList;
 char			FileNameExtension[MAXPATH];
 BLOCKID		item_bid, value_bid, prev_bid, wdc_bid;
 WORD			type, flags, name_len, wdc_wd;
 DWORD			length, wdc_dwd;
 char			field_name[MAXFIELDVALUE];
 int			i = 0;
 BLOCKID		TobeDetached[MAXDELETEATTACH];	/* max */
 char			DetachFileNameHostLink[4*MAXPATH];
 int			NbTobeDetach = 0;
 int			HostLink = 0;
 int			Except_Flag = 1;

 /* alloc mem */
 FileNameList = (char**) malloc( MAXDELETEATTACH * sizeof(char) );
 if (FileNameList == NULL)
	 return(0);

 /* default path value */
 if ( strlen(filepath) == 0 )
 {
	if (vUnix)
		strcpy(filepath, "/tmp");
	else
		strcpy(filepath, "c:\temp");	/* to do : use the Notes Directory */
 }

 /* init */
 HostLink = (int) strlen(HostAttachmentLink);

 /* Uppercase for string comparaison */
 UpperCase(ExtensionList);

 /* Loop over all ITEM */
 for(i=0;;prev_bid = item_bid,i++)
 {
	 /*----------------------------------------------*/
	 /*[---------- Extract ITEM info ---------------]*/

     if(i==0) /* => first one */
     {
       error = NSFItemInfo(*note_handle, NULL, 0, &item_bid, &type, &value_bid, &length);
       if(error)
           return(0);
     }
     else    /* => now get next item */
     {
		error = NSFItemInfoNext(*note_handle, prev_bid, NULL, 0, &item_bid, &type, &value_bid, &length);
        if(ERR(error) == ERR_ITEM_NOT_FOUND)
            break;
        else if(error)
          return(0);
	 }

	 /* --------------------------------------------------------- */
	 /*[---------- Extract data : Get NAME & FLAG ---------------]*/

	 NSFItemQuery(*note_handle, item_bid, field_name, MAXFIELDVALUE-1, &name_len, &flags, &wdc_wd, &wdc_bid, &wdc_dwd);
     field_name[name_len] = '\0';

     /** ATTACHMENT ? **/
     if(type == TYPE_OBJECT)
     {
		/* Work out */
		NSFGetAttachmentFileName(value_bid, FileName, FileNameExtension);

		/* Uppercase for string comparaison */
		UpperCase(FileNameExtension);

		/* ------------------------------
		* [ Except_Flag ]
		* Except_Flag = 1 => Detach the $FILE if the filename's extension is part of the reference string
        * Except_Flag = 0 => Detach the $FILE if the filename's extension IS NOT part of the reference string
		* ------------------------------ */
		if ( IsInString(ExtensionList, FileNameExtension) == Except_Flag  || (!strlen(ExtensionList)) )
		/* if ( (strstr(ExtensionList, FileNameExtension)) || (!strlen(ExtensionList)) ) */
		{
			/* need to save the item_bid and filename of the object that
			is to be detached to not to break the 'Get Next Item loop' */
			if ( NbTobeDetach + 1 < MAXDELETEATTACH )
			{
				TobeDetached[NbTobeDetach] = item_bid;

				/*-- check that this filename is unique within the same mail, append
				the mail_id not to overwrite over mail file attachment that could have
				been stored on the same drive --*/
				SaveOnDiskAttach_MakeUniqueName(FileNameList, NbTobeDetach, FileName);

				/* strcpy(FileNameList[NbTobeDetach], FileName); */

				NbTobeDetach++;
			}
		}
	 }

 } /* end of loop */


 /* ----------- now Save on Disk the $FILE for Good --------------*/
if ( NbTobeDetach > 0 )
{
	for (i=0; i < NbTobeDetach; i++)
	{
		sprintf(DetachFileName, "%s\\%s_%s", filepath, FileNameList[i], mail_ID);
		CheckPath(DetachFileName);

		/* TODO : a procedure that create & test the filename before dump to hard drive */
		if (error = DetachFileToDisk(*note_handle, TobeDetached[i], DetachFileName))
				AddInLogMessageText( ERR_DETACH_FILE, error, NOMAPI, DetachFileName );
		else
		{
			/* Create the link for the detached item */
			if ( HostLink )
			{
				/* duplicate if we are working on the original email */
				/* TODO : test if it is necessary to create a record - for MIME to CD conversion */
				if ( (nbrec - (current_rec - 1) >= 1) && (*bNewNote == 0) )
				{
					if (error = DuplicateEmailForBodyChange(note_handle, bNewNote, hDB)   )
						AddInLogMessageText( ERR_DETACH_FILE_LINK, error, NOMAPI, HostAttachmentLink, DetachFileName );
				}

				if (error == NOERROR)
				{
					/* Append Link to body field */
					MakeLinkFromPath(DetachFileName);
					sprintf(DetachFileNameHostLink, "%s%s", HostAttachmentLink, DetachFileName); /* remove '/' : 3.93.11 */

					/* Append a blank Line to Body */
					if ( i == 0)
					{
						if ( error = AppendBlankLineToBodyField(*note_handle) )
							AddInLogMessageText( ERR_DETACH_FILE_LINK, error, NOMAPI, HostAttachmentLink, DetachFileName );

						if (error == NOERROR)
							if (error = AppendLineToBodyField(*note_handle, "---------------------------------------------------------------------------------------" ) )
								AddInLogMessageText( ERR_DETACH_FILE_LINK, error, NOMAPI, HostAttachmentLink, DetachFileName );

					}

					if (error == NOERROR)
						if (error = AppendLineToBodyField(*note_handle, DetachFileNameHostLink ) )
							AddInLogMessageText( ERR_DETACH_FILE_LINK, error, NOMAPI, HostAttachmentLink, DetachFileName );
				}
			}
		}

		/* free memory */
		if (FileNameList[i] != NULL)
			free(FileNameList[i]);

	}/* end : for NbTodeDetach */
}

 /* free */
if(FileNameList !=NULL)
	free(FileNameList);

 /* Normal end */
 return(error);

}

/************************************************************************
* RemoveInternetDomain
* INPUT/OUTPUT : String
*
************************************************************************/
void RemoveInternetDomain_ShortName(char *n)
{
    /* : xxxxxxx@unipower.net => xxxxxx */
	char* loc;

    /** Look for the first '@' in the address..and cut **/
	loc = strchr(n, '@');
	if (loc)
	{
		n[strlen(n) - strlen(loc)] = '\0';

		/** now if any '.' in the address then replace by ' ' **/
		while (loc = strchr(n,'.'))
		{
			if (loc)
				*loc = 0x20;
		}
	}
}


/********************************************************************
* DuplicateEmailForBodyChange
* Create a copy, and convert Body to RichText if necessary
* INPUT			: hDB, the current mail.box
* INPUT/OUTPUT	: hNote, boolean (bNewNote)
*******************************************************************/
STATUS DuplicateEmailForBodyChange(NOTEHANDLE *hNote, int* bNewNote, DBHANDLE hDB)
{
	STATUS		error	= NOERROR;
	NOTEHANDLE	hNewNote;

	if ( error = DuplicateMail(&hDB, *hNote, &hNewNote, 1) )
		return(error);

	if ( error = NSFNoteUpdate(hNewNote, UPDATE_FORCE) )
		return(error);

	/* -----------------------------------------------------------------------------------------
	 * Check if Body field is Mime or not
	 * to convert safely to MIME
	 *  duplicate memory by a memory copy
	 *  update + reopen the new copy as a new note hNote if No Error the hNote is replaced
	 * ----------------------------------------------------------------------------------------*/
	if ( NSFItemIsPresent(hNewNote, "$NoteHasNativeMIME", 18) || ( NSFItemIsPresent(hNewNote, "MIME_Version", 12)) )
		error = NoteHandleNativeConvertMIME2CD(&hNewNote, hDB, 0);

	/* hNote then get the new version : hNewNote */
	if (error == NOERROR)
	{
		*hNote    = hNewNote;
		*bNewNote = 1;
	}

	return(error);

} /* end : DuplicateEmailForBodyChange */

/********************************************
* AppendBlankLineToBodyField
* INPUT		: NOTEHANDLE
* OUTPUT	: STATUS
****************************************************/
STATUS AppendBlankLineToBodyField(NOTEHANDLE hNote)
{
	STATUS			error = NOERROR;
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;

	/* Get the current Body item */
	if(error = CompoundTextCreate(hNote, "Body", &hCompound))
		return(error);

	CompoundTextInitStyle(&Style);

	if (error = CompoundTextDefineStyle( hCompound,
					                      "Normal",
							              &Style,
									      &dwNormalStyle))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}

#ifdef V4
	if (error = CompoundTextAddText
#else
	if (error = CompoundTextAddTextExt
#endif
					(hCompound, dwNormalStyle,DEFAULT_FONT_ID,
									 "\n",
									 (DWORD) 2,
									 "\r\n",
									 COMP_PRESERVE_LINES,
									 NULLHANDLE))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}

	/* Close Compound */
	if (error = CompoundTextClose (hCompound, NULL, NULL, NULL, 0))
		/* Free up ressource if necessary */
		CompoundTextDiscard(hCompound);

	return(error);
}

/********************************************
* AppendLineToBodyField
* INPUT		: NOTEHANDLE
* INPUT		: char* text to be appended
* OUTPUT	: STATUS
****************************************************/
STATUS AppendLineToBodyField(NOTEHANDLE hNote, char* szMessageText)
{
	STATUS			error = NOERROR;
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;

	/* Get the current Body item */
	if(error = CompoundTextCreate(hNote, "Body", &hCompound) )
		return(error);

	CompoundTextInitStyle(&Style);

	if (error = CompoundTextDefineStyle( hCompound,
					                      "Normal",
							              &Style,
									      &dwNormalStyle))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}

#ifdef V4
	if (error = CompoundTextAddParagraph
#else
	if (error = CompoundTextAddParagraphExt
#endif
								 (hCompound,
                                  dwNormalStyle,
                                  DEFAULT_FONT_ID,
                                  szMessageText,
                                  (DWORD)strlen(szMessageText),
                                  NULLHANDLE))
	{
		CompoundTextDiscard(hCompound);
		return(error);
	}

	/* Close Compound */
	if (error = CompoundTextClose (hCompound, NULL, 0L, NULL, 0))
			CompoundTextDiscard(hCompound);

	return(error);
}


/**************************************************************
* DetachFileToDisk
* PURPOSE		: Detach an attachment to disk
* INPUT			: NOTEHANDLE, the current email
*				: Reference to file
* OUTPUT		: DetachFileName and STATUS
****************************************************************/
STATUS DetachFileToDisk(NOTEHANDLE note_handle, BLOCKID TobeDetached, char * DetachFileName)
{
	STATUS	error = NOERROR;
	FILE*	f;

	/* check that the file is not already there (TODO : MD5 signature : unique key) */
	f = fopen(DetachFileName, "r");
	if (f == NULL)
	{
		/* Physically detach the file to disk */
		error = NSFNoteExtractFile(note_handle, TobeDetached, DetachFileName, NULL);
	}
	else
		fclose(f);

	return(error);
} /* end of: DetachFileToDisk */


/******************************************************************************************************
* PURPOSE	: Create a unique name before detaching the file to disk
*			: 1 : unique name within the same mail
*			: 2 : unique by appending the mail unique ID
* INPUT		: filename + docid, array [filename], current index of the array => nb of filename stored
* OUTPUT	: filename, array
* OUTPUT	: error = 1 :overflow, memory or more than a 100 of same file name.
*******************************************************************************************************/
int SaveOnDiskAttach_MakeUniqueName(char** FileNameList, int NbTobeDetach, char *FileName)
{
	int		i = 0;
	char*	index_f;


	/* mem alloc */
	index_f = (char*) malloc(NbTobeDetach);		/* more than enough :) */
	if (index_f == NULL)
		return(1);


	( FileNameList[NbTobeDetach] ) = (char*) malloc(strlen(FileName) + 1 + 33 + NbTobeDetach);  /*_DOC_ID */
	if ( FileNameList[NbTobeDetach] == NULL)
		return(1);

	/* check out if this name is already in use */
	if ( NbTobeDetach > 0)
	{
		/* Loop over the array with filename */
		for (i = 0; i < NbTobeDetach; i++)
		{
			/* check if this filename already exists ! */
#ifdef DEBUG
			printf("%s ? %s \n", FileNameList[i], FileName);
#endif
			if ( strcmp( FileNameList[i], FileName ) == 0)
			{
				sprintf(index_f, "_%d",i);
				strcat(FileNameList[i], index_f);
			}
		}
	}

	/* save into array */
	strcpy(FileNameList[NbTobeDetach], FileName);

	if (index_f != NULL)
		free(index_f);
	return(0);

}/* end of : SaveOnDiskAttach_MakeUniqueName */



/*******************************************************************************
* int IsInString(char*, char*)
* NB : this function is case sensitive.
* PURPOSE : make boolean the response
* char_target : is the long string ; checking if char_search is a substring of it
*********************************************************************************/
int IsInString(char* char_target, char* char_search)
{
	/* nothing to compare */
	if ( (!strlen(char_target)) || (!strlen(char_search)) )
		return(0);

	/* found ? */
	if ( strstr(char_target, char_search) )
		return(1);

	return(0);
} /* end of : ISInString() */



/************************************************************************************************
* GetUserAddressFullName
* PURPOSE	: Query address book with a potential address (internet address)
*			: read "fullname".
* INPUT		: user's address - used by a router
* OUTPUT	: Error/No Error
*			: user_fullname[MAXUSERNAME]
*************************************************************************************************/
int GetUserAddressFullName(char* user_address, char* user_fullname, char* fieldname, BOOL Abbreviate_it)
{
	STATUS	sErreur = NOERROR;
	HANDLE	hLookup = NULLHANDLE;
	char	*pLookup, *pName, *pMatch;
	WORD	numret = 0;
	WORD	k = 0;
	int		check = 0;
	char	user_alias[MAXUSERNAME];


	/* Lookup the alias by looking up in the $users view */
	if (sErreur = NAMELookup(NULL, 0, 1, "($Users)", 1, user_address, 1, fieldname, &hLookup))
	{
		if (NULLHANDLE != hLookup)
			OSMemFree(hLookup);

		return(0);
	}


	/* found ! : new get the values */
	pLookup = OSLockObject(hLookup);
	pName = NAMELocateNextName(pLookup, NULL, &numret);
	if (!numret)
	{
		OSUnlockObject(hLookup);
		OSMemFree(hLookup);
		/* nothing has been uploaded = $user found but no alias are to be uploaded */
		return(0);
	}

	/* loop over the lookup buffer */
	pMatch = NAMELocateNextMatch(pLookup, pName, NULL);
	k = 0;

	do
	{
		if (sErreur = NAMEGetTextItem(pMatch, 0, k, user_alias, sizeof(user_alias)-1))
		{
			/* Error when k = 0 means there is NO "alias field" */
			if (k == 0)
			{
				OSUnlockObject(hLookup);
				OSMemFree(hLookup);
				/* nothing has been uploaded user remain a user */
				return(0);
			}
			break;
		}

		/* upload field values when not "" */
		/* 3.94.33 : Append the first none empty entry ; ( && check == 0)  */
		/* 3.94.39 : New argt : Abbreviate = FALSE/TRUE */
		if ( strlen(user_alias) > 0 && ( check == 0 ) )
		{
				UpperCase(user_alias);

				if ( Abbreviate_it )
					DNAbbreviate(0L, NULL, user_alias, user_fullname, MAXUSERNAME, NULL);
				else
					strcpy(user_fullname, user_alias);

				check = 1;
		}
		/* next value */
		k++;

	} while (!sErreur);

	/* free memory */
	OSUnlockObject(hLookup);
	OSMemFree(hLookup);

	/* ok some alias have been uploaded (if all of them where not blank ;)so => user (1) will be changed to (2 = Group) */
	return(check);

} /* end GetUserAddressFullName */


/**********************************************************************************
*    FUNCTION :  AppendAuthorItem
*    PURPOSE  :   Append item named "Reader" to the open note.
*----------------------------------------------------------------------------------
* Fix 3.92	: AppendReaderItem
* 26/06/03	: Make sure that the READER values do not contains @Domain
* 08/12/03	: Get Fullname to make sure the reader value is correct for notes
***********************************************************************************/
STATUS  AppendReaderItem (NOTEHANDLE  hNote, char* fieldname, char* szReader, int check)
{
    STATUS  error = NOERROR;

	if (check)
	{
		/* Remove any email domain */
		/* RemoveAllDomain(szReader); */
		RemoveNotesDomain(szReader);

		/* query ($users) view get fullname for reader access */
		if ( GetUserAddressFullName(szReader, szReader, "fullname", FALSE) )
		{
			error = NSFItemAppend ( hNote, ITEM_SUMMARY | ITEM_READERS | ITEM_NAMES,
                                fieldname,
                                (WORD) strlen(fieldname),
                                TYPE_TEXT,
                                szReader,
                                (WORD) strlen(szReader));
		}
		else
		{
			/* try again :) : with a short name computed */
			RemoveAllDomain_ShortName(szReader);

			/* query ($users) view get fullname for reader access */
			if ( GetUserAddressFullName(szReader, szReader, "fullname", FALSE) )
			{
				error = NSFItemAppend ( hNote, ITEM_SUMMARY | ITEM_READERS | ITEM_NAMES,
                                fieldname,
                                (WORD) strlen(fieldname),
                                TYPE_TEXT,
                                szReader,
                                (WORD) strlen(szReader));
			}

		}
	}
	/* Append the szReader as is */
	else
	{
		if ( strlen(szReader) )
		{
			error = NSFItemAppend ( hNote, ITEM_SUMMARY | ITEM_READERS | ITEM_NAMES,
                                fieldname,
                                (WORD) strlen(fieldname),
                                TYPE_TEXT,
                                szReader,
                                (WORD) strlen(szReader));
		}
	}

    return (error);
}

/************************************************************************
* RemoveInternetDomain
* INPUT/OUTPUT : String
*
************************************************************************/
void RemoveAllDomain(char *n)
{
	/* : xxxx/Org@Domain@DomainB => xxxx */
	/* : xxxxxxx@unipower.net => xxxxxx  */

	char* loc;

    /** Look for the first '@' in the address and cut **/
	loc = strchr(n, '@');
	if (loc)
	{
		n[strlen(n) - strlen(loc)] = '\0';

	}
}

void RemoveAllDomain_ShortName(char *n)
{

	/* : xxxx/Org@Domain@DomainB => xxxx */
	/* : abc.efg@unipower.net => abc efg */

	char* loc;

    /** Look for the first '@' in the address and cut **/
	loc = strchr(n, '@');
	if (loc)
	{
		n[strlen(n) - strlen(loc)] = '\0';

		/* remplace all the '.' by blank */
		while ( loc = strchr(n, '.') )
			loc[0]= ' ';
	}
}





/**************************************************************************************
* Purpose	: Create a linked list of all recipients based on original "recipients" field
* INPUT		: NOTEHANDLE (memory access to data)
* INPUT		: Char* (Field name)
* OUTPUT	: Nb of User found (int *)
* OUTPUT	: List* point to all addresses found
***************************************************************************************/
STATUS	CheckRecipientField(DBHANDLE hmb, NOTEHANDLE hNote, char* FieldName, WORD* nbUserFound, char * FromValue)
{
	STATUS		error = NOERROR;
	List		L;	/* list.h */
	Position	P;	/* list.h */
	int			newnote;
	WORD		nbrec, j, CurrentSize, UserSize;
	char		username[MAXUSERNAME];
	NOTEHANDLE	hNoteSafe, hNewNote;
	char		MailDuplicate[4];
	NUMBER		SafeNbRecipients = 0;

	/* init */
	L = MakeEmpty( NULL );
	if (L == NULL)
	{
		AddInLogMessageText("could not get enough memory for recipients item %s ", NOERROR, "CheckRecipientField");
		return(error);
	}

	P = Header( L );
	*nbUserFound	= 0;
	newnote			= 0;
	CurrentSize		= 2;  /* minimum size for a field */


	/* Work out only not MailWatcher forced splited mail */
	if ( ! NSFItemGetText(hNote, MW_MAILDUPLICATE, MailDuplicate, sizeof(MailDuplicate)-1) )
		MailDuplicate[0]='\0';

	if (MailDuplicate[0] == '1')
		return(NOERROR);

	/* Read : MAIL_NOTE_RECIPIENTS :  Loop over field values */
	nbrec = NSFItemGetTextListEntries(hNote, FieldName);

	for (j=0; j<nbrec; j++)
	{
		NSFItemGetTextListEntry(hNote, FieldName, j, username, sizeof(username)-1);

		/* get all user addresses */
		error =	GetAllAddressesFromField(hNote, username, FromValue, nbUserFound, L, P, 0);
	}

	/* Loop over List write back to "recipients", free memory */
	/* PrintList( L );  debug */

	if ( *nbUserFound > 0)
	{
		/* 3.94.45 : Save the MAILWATCHER Recipients original numbers */
		SafeNbRecipients = *nbUserFound;
		
		/* 3.94.56 : update if does not	already exists */
		if (!NSFItemIsPresent(hNote, MW_MACRO_FIELD_NB_RECIPIENTS, (WORD) strlen(MW_MACRO_FIELD_NB_RECIPIENTS)) )
			NSFItemSetNumber(hNote, MW_MACRO_FIELD_NB_RECIPIENTS, &SafeNbRecipients);

		/* Remove the original Recipient Field */
		NSFItemDelete(hNote, MAILNOTE_RECIPIENTS_FIELD, (WORD) strlen(MAILNOTE_RECIPIENTS_FIELD));

		/* Loop over List write back to "recipients", free memory */
		do
		{
            P = Advance( P );

			if ( P != NULL )	/* 3.93.8 */
			{
				strcpy( username, Retrieve ( P ) );

				if ( username != NULL )	/* 3.93.8 */
				{
					UserSize = (WORD) strlen( username ) + 2;

					/* if Max string for Recipients List has been reached then create a new "copy" */
					if (CurrentSize + UserSize > MAX_RECIPIENTS_FIELD_SIZE)
					{
						/* log : splitting */
						if (newnote == 1)
						{
							NSFNoteUpdate(hNote, UPDATE_FORCE);
							NSFNoteClose(hNote);
							hNote = hNoteSafe;
							newnote = 0;
						}

						error = SplitMail(&hmb, hNote, &hNewNote);
						if (error == NOERROR)
						{
							hNoteSafe   = hNote;
							hNote       = hNewNote;
							newnote     = 1;
							CurrentSize = 2; /* see declaration */
						}
					}

					/* Append to the current "Recipients Field" */
					/* fix : 13/03/03 : Check if the 'address' == 'from'
					strcpy(username_upper, username);
					UpperCase(username_upper);
					DNAbbreviate(0L, NULL, username_upper, username_upper_abbr, MAXUSERNAME, NULL);
					*/

					/* Fix 3.92.12/20/01/04/LP  :  to be done when appending 'members' of group into P list */
					/* if (strcmp(FromValue, username_upper_abbr)) */

					/* 5/4/04 : decode SMTP first or return as is (notes format) */
					parseSMTPaddress(username, username);

					NSFItemAppendTextList(hNote, MAILNOTE_RECIPIENTS_FIELD, username, (WORD) strlen(username), TRUE);

					CurrentSize = CurrentSize + UserSize;

				} /* username not NULL */
			} /* P is not NULL */
		} while( !IsLast( P, L ) );

	}
	/* free memory : Pass L as P has reach the last element (end of while) */
	DeleteList( L );


	/* save current open "new" mail if exists : restore and return */
	if (newnote == 1)
	{
		NSFNoteUpdate(hNote, UPDATE_FORCE);
		NSFNoteClose(hNote);
		hNote = hNoteSafe;
		newnote = 0;
	}

	return(error);
}

/*************************
*
*************************/
STATUS GetAllAddressesFromField(NOTEHANDLE hNote, char* temp, char* FromValue, WORD* nbUserFound, List UserAddresses, Position P, int recurse)
{
	STATUS		error = NOERROR;
	HANDLE		hLookup = NULLHANDLE;
	char		*pLookup, *pName, *pMatch;
	WORD		numret, k;
	char		temp2[MAXUSERNAME];
	char		temp_noDomain[MAXUSERNAME];
	char		username_upper_abbr[MAXUSERNAME];
	char		username_upper[MAXUSERNAME];

	/* Remove the @Domain if is at the end of 'temp' : 3.8ab6 */
	strcpy(temp_noDomain, temp);
	RemoveNotesDomain(temp_noDomain);
	/* ----------------------------------------------------------------------------------------
	  Appending a 'member' field into P but do not append this entry if FROM == Recipients
	  if we are uploading a member field
	  -----------------------------------------------------------------------------------------*/
	strcpy(username_upper, temp_noDomain);
	UpperCase(username_upper);
	DNAbbreviate(0L, NULL, username_upper, username_upper_abbr, MAXUSERNAME, NULL);

	/* Look for "Members" field */
	/* Fix : 13/3/03 : Router is using "($Users)"  */
	if (error = NAMELookup(NULL, 0, 1, "($Users)", 1, temp_noDomain, 1, "Members", &hLookup))
	{
		if (NULLHANDLE != hLookup)
			OSMemFree(hLookup);

		if ( (strcmp(FromValue, username_upper_abbr)) || (recurse == 0) )
		{
			/* insert user */
			if (Insert(temp, UserAddresses, P))
			{
				AddInLogMessageText("could not get enough memory for recipients item : %s ", NOERROR, temp);
				return(error);
			}
			(*nbUserFound)++;
		}
		return(error);
	}

	/* debug
	sprintf(LogString, "GetAllAddressFromField : handle %d \n",  (int) hLookup);
	LogLine("mwadvt.log", LogString, 1);
   */

	pLookup = OSLockObject(hLookup);
	pName = NAMELocateNextName(pLookup, NULL, &numret);

	if (!numret)
	{
		OSUnlockObject(hLookup);
		OSMemFree(hLookup);

		if ( (strcmp(FromValue, username_upper_abbr)) || (recurse == 0) )
		{
			if (Insert(temp, UserAddresses, P))
			{
				AddInLogMessageText("could not get enough memory for recipients item : %s ", NOERROR, temp);
				return(error);
			}
			(*nbUserFound)++;
		}
		return(error);
	}


	/* Go thru the buffer */
	pMatch = NAMELocateNextMatch(pLookup, pName, NULL);
	k=0;

	do
	{
		if (error = NAMEGetTextItem(pMatch, 0, k, temp2, sizeof(temp2)-1))
		{
			/* Error when k = 0 means there is NO "Member field" => user */
			if (k == 0)
			{
				if ( (strcmp(FromValue, username_upper_abbr)) || (recurse == 0) )
				{
					if ( Insert(temp, UserAddresses, P) )
						AddInLogMessageText("could not get enough memory for recipients item : %s ", NOERROR, temp);
					else
						(*nbUserFound)++;
				}
			}
			break;
		}


		/* member field has been found */
		recurse = 1;

		/* Recursive call */
		if (! isUserAddress(temp2) )
			GetAllAddressesFromField(hNote, temp2, FromValue, nbUserFound, UserAddresses, P, recurse);

		else
		{
			/* Appending a 'member' field into P but do not append this entry if FROM == Recipients */
			strcpy(username_upper, temp2);
			UpperCase(username_upper);
			DNAbbreviate(0L, NULL, username_upper, username_upper_abbr, MAXUSERNAME, NULL);

			if ( (strcmp(FromValue, username_upper_abbr)) || (recurse == 0) )
			{
				if ( Insert(temp2, UserAddresses, P) )
					AddInLogMessageText("could not get enough memory for recipients item : %s ", NOERROR, temp2);
				else
					(*nbUserFound)++;
			}
		}

		k++;
	}
	while (!error);

	OSUnlockObject(hLookup);
	OSMemFree(hLookup);

	return(error);

} /* end of GetAllRecipients */


/*******************************************************************************
 * Purpose	: Decode encoded address
 * author	: Laurent Prat
 *
 * For example, the encoded addresses
 *     <God@heaven.af.mil>
 *    <\God@heaven.af.mil>
 *    <"God"@heaven.af.mil>
 *    <@gateway.af.mil,@uucp.local:"\G\o\d"@heaven.af.mil>
 *
 *  all give : god@heaven.af.mil
 * -------------------------------------------------------------------------------
 * case of "<" and that's it : Must stop the process if (p++) is NULL
 * Check if FullAddress is not NULL
 * use char c - test p[0] not ZERO
 *******************************************************************************/
int parseSMTPaddress(char* FullAddress, char* ShortAddress)
{
	char *	p;
	char	c;
	int		inQuote = 0;
	int		k = 0;
	int		cond = 1;

	/* init */
	if ( FullAddress != NULL )
		strcpy(ShortAddress, FullAddress);
	else
		return(1);

	/* Get the first '<' if in there */
	p = strchr(FullAddress, '<');
	if ( p == NULL )
		return(1);

	/*  If the next character is @, skip all characters through the next colon. */
	if ( p[1] )
		p++;
	else
		return(1);	/* 3.93.7 end of string */

	if ( p[0] ==  '@' )
	{
		p = strchr(p, ':' );
		if ( p == NULL)
			return(1);
	}

	/* remove bad characters */
	while ( (c = p[0]) && cond )
	{
		switch ( c )
		{
			case '<' :
				break;

			case '\\':
				break;

			case '\"':
				inQuote = !(inQuote);
				break;

			case ':':
				break;

			case '>':
				if (!inQuote)
					cond = 0;
				break;

			default:
				ShortAddress[k++] = p[0];
				break;
		}

		/* next char */
		p++;
	}

    /* end the string if necessary */
	if ( k > 0 )
		ShortAddress[k] = '\0';
	return(0);
} /* end of parseSMTPaddress */


/********************************************************************************************
* Read the server document : get field : HostnameFullQualified
* GetDominoServerDocument
* INPUT			: dbnab; is already open ; do not close it in case of error !
* INPUT			: ServerName, get from the server.id (mwadvt launcher !) in principle
* INPUT/OUTPUT	: ServerHostName : Fully qualified Internet host name
* OUTPUT		: STATUS (error / NOERROR).
*********************************************************************************************/
STATUS GetDominoHostName(DBHANDLE module_dbnab, char * capath, char* ServerName, char* Serverhostname)
{
	STATUS				error = NOERROR;
	NOTEID				nab_server_view_id;
	HCOLLECTION			server_view_coll;
	COLLECTIONPOSITION	coll_capos;
	DWORD				atraiter;
	HANDLE				buffer_cahandle = NULLHANDLE;
	DWORD *				id_calist;
	NOTEHANDLE			hConfNote;
	DBHANDLE			dbnab;
	char				NamesPath[MAXPATH];
	int					CloseNab = 0;


	/* First of all set the $MailWatcherServerName variable */
	OSSetEnvironmentVariable("$MailWatcherServerName", ServerName);

	/* default it */
	strcpy(Serverhostname, ServerName);

	/* Open the Proper names.nsf if capath is different */
	strcpy(NamesPath, capath);
	UpperCase(NamesPath);

	if ( strcmp("NAMES.NSF", NamesPath) == 0 )
		dbnab = module_dbnab;
	else
	{
		if ( error = GetDatabaseHandle("LOCAL", "names.nsf", &dbnab, vUnix) )
		{
			strcpy(Serverhostname, ServerName);
			return(error);
		}
		CloseNab = 1;
		strcpy(NamesPath, "names.nsf");
	}


	/* check the server view */
	if (error = NIFFindDesignNote(dbnab,
								  NAB_SERVER_VIEW,
								  NOTE_CLASS_VIEW,
								  &nab_server_view_id))
	{
		AddInLogMessageText(ERR_FIND_VIEW, error, NOMAPI, NAB_SERVER_VIEW, NamesPath );
		if ( CloseNab )
			NSFDbClose(dbnab);
		return(error);
	}

	/* Open view  */
	if (error = NIFOpenCollection(dbnab, dbnab, nab_server_view_id, 0 /* 3.95.14 OPEN_REBUILD_INDEX */,
								  NULLHANDLE,
								  &server_view_coll,
								  NULL, NULL, NULL, NULL))
	{
	    AddInLogMessageText(ERR_OPEN_VIEW, error, NOMAPI,  NAB_SERVER_VIEW, NamesPath );
		if ( CloseNab )
			NSFDbClose(dbnab);
		return(error);
	}

	/* lookup */
	if (error = NIFFindByName(server_view_coll,
							  ServerName,
							  FIND_CASE_INSENSITIVE | FIND_ACCENT_INSENSITIVE,
					          &coll_capos,
							  &atraiter))
	{
		NIFCloseCollection(server_view_coll);
		AddInLogMessageText(ERR_FIND_CONFIG, error, NOMAPI, ServerName, NAB_SERVER_VIEW, NamesPath );

		if ( CloseNab )
			NSFDbClose(dbnab);

		return(error);
	}

	/* get and read NOTE_ID */
	if (error = NIFReadEntries(server_view_coll,
							   &coll_capos, NAVIGATE_CURRENT, 0,
							   NAVIGATE_NEXT, 0x01, READ_MASK_NOTEID,
							   &buffer_cahandle, NULL, NULL, &atraiter, NULL))
	{
		if (buffer_cahandle != NULL)
				OSMemFree(buffer_cahandle);

		NIFCloseCollection(server_view_coll);
		AddInLogMessageText(ERR_READ_CONFIG, error, NOMAPI, ServerName,  NAB_SERVER_VIEW, NamesPath );
		if ( CloseNab )
			NSFDbClose(dbnab);
		return(error);
	}

	/* Something to read ? */
	if ((buffer_cahandle == NULLHANDLE) || (atraiter == 0))
	{
		NIFCloseCollection(server_view_coll);
		AddInLogMessageText(ERR_FIND_CONFIG, error, NOMAPI, ServerName,  NAB_SERVER_VIEW, NamesPath );
		if ( CloseNab )
			NSFDbClose(dbnab);
		return(error);
	}

	/* lock it and open */
	id_calist = (NOTEID *)OSLockObject(buffer_cahandle);
	if (error = NSFNoteOpen(dbnab, id_calist[0], 0, &hConfNote))
	{
		OSUnlockObject(buffer_cahandle);
		OSMemFree(buffer_cahandle);
		NIFCloseCollection(server_view_coll);

		AddInLogMessageText(ERR_OPEN_CONFIG, error, NOMAPI, NamesPath );
		if ( CloseNab )
			NSFDbClose(dbnab);

		return(error);
	}

	/* hopefully now read the field */
	error = NSFItemGetText(hConfNote, NAB_SERVER_HOST_FULLNAME_FIELD, Serverhostname, MAXUSERNAME -1);

	/* release & close objects : buffer, hnote, doc_collection. */
	OSUnlockObject(buffer_cahandle);
	OSMemFree(buffer_cahandle);
	NSFNoteClose(hConfNote);
	NIFCloseCollection(server_view_coll);

	if ( CloseNab )
		NSFDbClose(dbnab);

	/* (re-default it if null */
	if (strlen(Serverhostname) == 0 )
		strcpy(Serverhostname, ServerName);

	return(error);
}


/*******************************************************************************
* Copy all the values from one text field to another
* INPUT		: notehandle, field_in (from), field_out (receiver)
* OUPUT		: STATUS
**********************************************************************************/
STATUS	NSFItemDuplicateTextList(NOTEHANDLE hNote, char* Field_in, char* Field_out, BOOL AllowDuplicateItem)
{
	STATUS	error = NOERROR;

	/* Get Field_in */
	if ( NSFItemIsPresent(hNote, Field_in, (WORD) strlen(Field_in) ))
	{
		/* TODO : Type checking (waste of time in for mwadvt.c usage ) */

		/* How many value ? */
		char	LocalRec[MAXUSERNAME];
		int		nbval = 0;
		int		i = 0;

		nbval = NSFItemGetTextListEntries(hNote, Field_in);

		/* if anyvalue then remove field_out, copy over all the entries  */
		if ( nbval > 0 )
		{
			NSFItemDelete(hNote, Field_out, (WORD) strlen( Field_out) );
			for ( i = 0; i < nbval; i++)
			{
				if ( NSFItemGetTextListEntry(hNote, Field_in, i, LocalRec, MAXUSERNAME-1))
					if (error = NSFItemAppendTextList( hNote, Field_out, LocalRec, (WORD) strlen(LocalRec), AllowDuplicateItem ))
						 i = nbval; /* stop */
			}
		}
	}

	return(error);
} /* end of NSFItemDuplicateTextList */


/**********************************************************
* int CheckBlackListRulesMemoryValidation(NOTEHANDLE)
* PURPOSE	: Evaluation if a rules memory condition has been set
* INPUT		: hNote (the filter rule document)
* OUTPUT	: returns 1 if the rules has to be SKIPPED, otherwise O to apply it
* I/O		: RulesRef (for memory rules) saved in the filter document
***********************************************************************************/
int	CheckBlackListRulesMemoryValidation(NOTEHANDLE hNote, RulesList* L_rules, char* RulesRef )
{
	STATUS	error = NOERROR;
	double	result;
	char	RulesCmd[BLACKLIST_RULES_MEMORY_MAX_CMD];

	/* Field : reference of the rules that counts */
	NSFItemGetText(hNote, BLACKLIST_RULES_MEMORY_REF, RulesRef, MAXMEMORYRULESREF -1 );

	/* Field : Rules command line to evaluate */
	if ( NSFItemGetText(hNote, BLACKLIST_RULES_MEMORY_REF_CMD, RulesCmd, BLACKLIST_RULES_MEMORY_MAX_CMD -1 ) )
	{
		if ( evalrules(L_rules, RulesCmd, &result) )
			return( 0 );

		if ( result == 1)
			return( 0 );
		else
			return( 1 );
	}

	/* rule will be applied */
	return( 0 );

}/* end of CheckBlackListRulesMemoryValidation */



/*****************************************************
* RulesMemoryCount
* PURPOSE	: Add ++ to the Hasrun item of a rules memory reference
*/
int RulesMemoryCount(char* RulesMemoryRef, char* TmpBLName, RulesList* L_rules, RulesList ** StartL )
{
	RulesList*	Header;

	/*-- 3.94 : remember this ! . the rules has been activated */
	if ( advblck_rm )
	{
		if ( strlen( RulesMemoryRef ) )
			Header = RulesInsertInList(L_rules, RulesMemoryRef, StartL );
		else
		{
			if ( strlen( TmpBLName) > 0 )
				Header = RulesInsertInList(L_rules, TmpBLName, StartL );
			else
			{
				strcpy(TmpBLName, DEFAULT_RULES_MEMORY_REF);
				Header = RulesInsertInList(L_rules, TmpBLName, StartL );
			}
		}

		/* reset the starting point of the list */
		if ( Header != NULL )
			L_rules = *StartL;
		else
			return(1);
	}

	return(0);
} /* end of RulesMemoryCount */



/********************************************************************
* WriteToDebugLog
* PURPOSE		: Dump items form debugging.
* INPUT			: filename
* INPUT			: hnote, field name
* OUTPUT		: int ( 1 for error, 0 = noerror )
* depend		: LogLine();
*************************************************************************/
int	WriteToDebugLog(char* filename, NOTEHANDLE hnote, char* fieldname)
{
	char	TextVal[MAXDEBUGLINE];
	NUMBER	NumVal;
	char	DebugStr[MAXDEBUGLINE];
	WORD	typechamp;

	/* check if hnote is not null */
	if ( hnote == NULL )
		return(1);

	/* get value - data type - */
	NSFItemInfo(hnote, fieldname, (WORD) strlen(filename), NULL, &typechamp, NULL, NULL);

	switch ( typechamp )
	{
		case TYPE_TEXT:
			if ( !NSFItemGetText(hnote, fieldname, TextVal, MAXDEBUGLINE -1) )
				strcpy(TextVal, "N/A");
			sprintf(DebugStr,"%s = %s", fieldname, TextVal);
		break;

		case TYPE_NUMBER:
			if ( NSFItemGetNumber(hnote, fieldname, &NumVal) )
				sprintf(DebugStr, "%s = %d", fieldname, NumVal);
			else
				sprintf(DebugStr,"%s = N/A", fieldname);
		break;

		default:
			sprintf(DebugStr, "%s - can not be dumped -", fieldname);
		break;
	}

	/* Format string */
	LogLine(filename, DebugStr, 1);

	return(0);
}


/******************************************************************************************
* Proc		: DeleteInfectedAttachment( )
* PURPOSE	: Based on field (text list) which contains all the filenames -
*			: delete the attachments -
* INPUT		: NOTEHANDLE
* INPUT		: AttachedFileName : Files names.
* OUTPUT	: STATUS.
*************************************************************************************/
STATUS DeleteAttachFileByName(NOTEHANDLE note_handle, char* AttachedFileName)
{

 STATUS	  		error = NOERROR;
 BLOCKID		item_bid, value_bid, prev_bid, wdc_bid;
 BLOCKID		TobeDeleted[MAXDELETEATTACH];	/* max */
 WORD			type, flags, name_len, wdc_wd;
 DWORD			length, wdc_dwd;
 char			field_name[MAXFIELDVALUE];
 char*			FileNameList[MAXDELETEATTACH];
 int			NbTobeDeleted = 0;
 int			nbitem = 0;
 int			i = 0;
 char			FileName[MAXPATH];


 /* Get all the filenames to be deleted */
 if ( error = GetFieldInList(note_handle, AttachedFileName, FileNameList, MAXPATH, MAXDELETEATTACH, &nbitem) )
 {
	return( error );
 }


 /* Loop over all ITEM */
 for(i=0;;prev_bid = item_bid,i++)
 {
	 /*[---------- Extract ITEM info ---------------]*/

     if(i==0) /* => first one */
     {
		error = NSFItemInfo(note_handle, NULL, 0, &item_bid, &type, &value_bid, &length);
		if(error)
			return(0);
     }
     else    /* => now get next item */
     {
		error = NSFItemInfoNext( note_handle, prev_bid, NULL, 0, &item_bid, &type, &value_bid, &length );
		if(ERR(error) == ERR_ITEM_NOT_FOUND)
			break;
        else if(error)
			return(0);
	 }

	 /* [---------- Extract data : Get NAME & FLAG ---------------] */

	 NSFItemQuery(note_handle, item_bid, field_name, MAXFIELDVALUE-1, &name_len, &flags, &wdc_wd, &wdc_bid, &wdc_dwd);
     field_name[name_len] = '\0';

     /** ATTACHMENT **/
     if(type == TYPE_OBJECT)
     {
		/* Work out */
		if ( error = NSFGetAttachmentFullFileName(value_bid, FileName, MAXPATH -1) )
		{

		}
		else
		{
			if ( IsInFieldValueList(FileNameList, FileName) )
			{
				/* need to save the item_bid that is to be destroyed to not to break the 'Get Next Item loop' */
				if ( NbTobeDeleted + 1 < MAXDELETEATTACH )
					TobeDeleted[NbTobeDeleted++] = item_bid;

				/* printf("deleted : %s => %s \n", FileName, FileNameExtension); */
			}
		}
	 }

 } /* end of loop */


 /* ----------- now Delete $FILE for Good --------------*/
if ( NbTobeDeleted > 0 )
{
	for (i=0; i < NbTobeDeleted; i++)
		error = NSFNoteDetachFile(note_handle, TobeDeleted[i]);

}

/* Free ressources */
FreeMemory_GetFieldInList(FileNameList, nbitem);

/* Normal end */
 return(error);

}



/**********************************************
* Release memory
*/
void FreeMemory_GetFieldInList( char** field_List, int nbitem )
{
	int i = 0;

	for (i = 0; i < nbitem; i++)
	{
		if ( field_List[i] != NULL )
			free(field_List[i]);
	}
}



/*****************************************************************************************************
* PROC		: IsInFieldValueList
* INPUT		: char*		Field value.
*			: char**	field_list.
*			: >> Case sensitive <<
*			:
* OUTPUT	: Int			-> 0 if one of the fields is set !
*			:				-> 1 if none of the fields is set
******************************************************************************************************/
int IsInFieldValueList(char** field_List, char* FieldValue)
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



/**********************************************************
* NSFGetAttachmentFileName()
* INPUT		: BLOCKID
* I/O		: Filename
* OUTPUT	: STATUS
************************************************************/
STATUS NSFGetAttachmentFullFileName(BLOCKID bidv, char* FileName, int MaxLength)
{
	STATUS		error = NOERROR;
	void		*ptrItemValue;
	WORD		*ptrItemValueWORD;
	FILEOBJECT	pFileObject;
	WORD		wlen = 0;
	WORD		wCheckedLength = 0;

	/* Save the original pointer */
	ptrItemValue = OSLockBlock(void, bidv);

	/*  Step over the data type word to point to the actual data */
	ptrItemValueWORD = (WORD *) ptrItemValue ;
	ptrItemValueWORD++ ;
	ptrItemValue = ptrItemValueWORD ;

	/* Use ODSReadMemory */
	ODSReadMemory( &ptrItemValueWORD, _FILEOBJECT, &pFileObject, 1 );

	/* Free memory  */
	OSUnlockBlock(bidv);

	/* FileName and its Extension */
	if ( pFileObject.Header.ObjectType == OBJECT_FILE )
	{
		wlen = pFileObject.FileNameLength;
		wCheckedLength = mw_MIN(wlen, MaxLength);

		/* Read the data the comes after the pFileObject */
		ptrItemValue = (char *) ptrItemValue + ODSLength(_FILEOBJECT);

		/* get File extension : if none log the filename */
		strncpy(FileName, (char *) ptrItemValue, wCheckedLength);
		FileName[wCheckedLength]='\0';
	}
	else	/* wrong type */
		error = ERR_ITEM_NOT_FOUND;

	return(error);

} /* end NSFGetAttachmentFileName */




/********************************************
* AppendLineToDefinedBodyField
* INPUT		: HANDLE ( a new temporary RICH TEXT ITEM )
* INPUT		: char* text to be appended
* OUTPUT	: STATUS
****************************************************/
STATUS AppendLineToDefinedBodyField(HANDLE* hCompound, DWORD dwNormalStyle, char* szMessageText )
{
	STATUS			error = NOERROR;

	/* remove blank */
	Trim(szMessageText, szMessageText);

	if ( strlen(szMessageText) )
	{
#ifdef V4
		if ( error = CompoundTextAddParagraph
#else
		if ( error = CompoundTextAddParagraphExt
#endif
								 ( *hCompound, dwNormalStyle, SMALL_FOREIGN_FONT_ID,
                                  szMessageText, (DWORD)strlen(szMessageText), NULLHANDLE) )
		/* release memory : kill RT item */
			CompoundTextDiscard(*hCompound);
	}

	return(error);
}




/********************************************************************************
* AppendTextListToBodyUsingFieldArgument( )
* INPUT		: NOTEHANDLE 1 -
*			: char* 'FIELD_LIST_DEF', field name that contains 'field' names to be displayed - 	append to a NOTEHANDLE
*			: COUMPOUND ( Body field )
*			:
****************************************************************************************************/
STATUS	AppendTextListToBodyUsingFieldArgument(NOTEHANDLE hNote,
										HANDLE* hCompound,
										DWORD dwNormalStyle,
										char* sFieldNameRef,
										char* sTitle1,char* sTitle2,char* sTitle3,
										int iUnderline, char sep)
{
	STATUS			error	= NOERROR;
	int				nbitem	= 0;
	char*			sFieldValue1[MAXDELETEATTACH];
	char			sHeaderTitle[(3*MAXPATH) + 1];

	/* Get the FieldNameRef items */
	if (! NSFItemIsPresent(hNote, sFieldNameRef, (WORD) strlen( sFieldNameRef)) )
			return(ERR_ITEM_NOT_FOUND);

	/* Get TEXT, TEXT LIST - FIELD 1 */
	if ( error = GetFieldInList(hNote, sFieldNameRef, sFieldValue1, MAXPATH, MAXDELETEATTACH, &nbitem) )
		return(error);

	/* -- Call up the proper functions -- */
	switch ( nbitem )
	{

		case 1:
			sprintf(sHeaderTitle,"! %40s !", sTitle1);
			error = AppendTextFieldToBodyWithHeader(hNote,hCompound,dwNormalStyle,
										sFieldValue1[0],
										sHeaderTitle, iUnderline, sep);
			break;

		case 2:
			sprintf(sHeaderTitle,"! %40s ! %-40s !", sTitle1, sTitle2);
			error = AppendTwoTextFieldToBodyWithHeader(hNote,hCompound,dwNormalStyle,
										sFieldValue1[0],
										sFieldValue1[1],
										sHeaderTitle, iUnderline, sep);
			break;

		case 3:
			sprintf(sHeaderTitle,"! %33s ! %-33s ! %-33s !", sTitle1, sTitle2, sTitle3);
			error = AppendThreeTextFieldToBodyWithHeader(hNote,hCompound,dwNormalStyle,
										sFieldValue1[0],
										sFieldValue1[1],
										sFieldValue1[2],
										sHeaderTitle, iUnderline, sep);
			break;


		default:
			return(ERR_ITEM_NOT_FOUND);
	}

	return(error);
} /* end of AppendTextListToBodyUsingFieldArgument */


/*****************************************************************************************************************
* AppendThreeTextFieldToBodyWithHeader
* Version	: 1.1
* Purpose	: Append 2 TEXT field ( TEXT or TEXT LIST) to the supplied BODY ITEM ( Rich TEXT)
*			: if not null start with the header,
*           : underlined or not
*			: Write all the TEXT item
*			: use separator ( new line ) or ( ,; 'char').: TODO - V1.0 => newline :
*			:
* INPUT		: HNOTE ( current note )
*			: COMPOUND handle ( BODY field )
*			: TEXT FIELD name, char* header, int 'underline (1/0), Separator ( NULL => newline)
* OUTPUT	: STATUS
*******************************************************************************************************************/
STATUS	AppendThreeTextFieldToBodyWithHeader(NOTEHANDLE hNote,
										HANDLE* hCompound,
										DWORD dwNormalStyle,
										char* sTextItemFieldName1,
										char* sTextItemFieldName2,
										char* sTextItemFieldName3,
										char* HeaderTitle,
										int iUnderline, char sep)
{
	STATUS			error	= NOERROR;
	int				nbitem	= 0;
	int				nbitem1 = 0;
	int				nbitem2 = 0;
	int				nbitem3 = 0;
	char*			sFieldValue1[MAXDELETEATTACH];
	char*			sFieldValue2[MAXDELETEATTACH];
	char*			sFieldValue3[MAXDELETEATTACH];
	int				HeaderLength = 0;
	char*			HeaderTitle_UnderLine;
	int				i = 0;
	char*			LongLine;
	int				ToDiscard = 1;


	/* Get Fields - if not present => error ( ERR_ITEM_NOT_FOUND) */
	if (! NSFItemIsPresent(hNote, sTextItemFieldName1, (WORD) strlen(sTextItemFieldName1)) )
				return(ERR_ITEM_NOT_FOUND);

	if (! NSFItemIsPresent(hNote, sTextItemFieldName2, (WORD) strlen(sTextItemFieldName2)) )
				return(ERR_ITEM_NOT_FOUND);

	if (! NSFItemIsPresent(hNote, sTextItemFieldName3, (WORD) strlen(sTextItemFieldName3)) )
				return(ERR_ITEM_NOT_FOUND);

	/* --------------------------------------------------------------*/
	/* Get TEXT, TEXT LIST - FIELD 1 */
	if ( error = GetFieldInList(hNote, sTextItemFieldName1, sFieldValue1, MAXPATH, MAXDELETEATTACH, &nbitem1) )
		return(error);

	/* Get TEXT, TEXT LIST - FIELD 2 */
	if ( error = GetFieldInList(hNote, sTextItemFieldName2, sFieldValue2, MAXPATH, MAXDELETEATTACH, &nbitem2) )
	{
		FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
		return(error);
	}

	/* Get TEXT, TEXT LIST - FIELD 3 */
	if ( error = GetFieldInList(hNote, sTextItemFieldName3, sFieldValue3, MAXPATH, MAXDELETEATTACH, &nbitem3) )
	{
		FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
		FreeMemory_GetFieldInList(sFieldValue2, nbitem2);
		return(error);
	}

	/* --------------------------------------------------------------*/

	nbitem = mw_MIN( nbitem1, nbitem2);
	nbitem = mw_MIN( nbitem,  nbitem3);

	if ( nbitem > 0 )
	{
		/* Append Header if not NULL */
		if ( ( HeaderLength = (int) strlen(HeaderTitle) ) > 0 )
		{
			/* over flow */
			if ( HeaderLength > (MAXPATH - 1) )
			{
				HeaderTitle[MAXPATH -1] = '\0';
				HeaderLength = MAXPATH - 1;
			}
		}

		if ( iUnderline != 0 )
		{
			/* create header underline */
			HeaderTitle_UnderLine = (char*) calloc( HeaderLength + 1, sizeof(char) );

			if ( HeaderTitle_UnderLine != NULL )
			{
				/* create the header top line */
				HeaderTitle_UnderLine = (char*) memset(HeaderTitle_UnderLine, '-', HeaderLength); /* HeaderLength + 1 = '\0' => calloc */

				if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
				{
					CompoundTextDiscard(*hCompound);
					free(HeaderTitle_UnderLine);
					FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
					FreeMemory_GetFieldInList(sFieldValue2, nbitem2);
					FreeMemory_GetFieldInList(sFieldValue3, nbitem3);
					return(error);
				}
			}
			else
				iUnderline = 0;
		}

		/* Append first line : HEADER */
		if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle) )
		{
			CompoundTextDiscard(*hCompound);
			if  ( HeaderTitle_UnderLine != NULL )
				free(HeaderTitle_UnderLine);
			FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
			FreeMemory_GetFieldInList(sFieldValue2, nbitem2);
			FreeMemory_GetFieldInList(sFieldValue3, nbitem3);
			return(error);
		}

		if ( iUnderline != 0)
		{
			/* header bottom line */
			if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
			{
				CompoundTextDiscard(*hCompound);
				free(HeaderTitle_UnderLine);
				FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
				FreeMemory_GetFieldInList(sFieldValue2, nbitem2);
				FreeMemory_GetFieldInList(sFieldValue3, nbitem3);
				return(error);
			}
		}


		/* -------------------- Append the TEXT item values -----------------------------*/
		for (i = 0; i < nbitem; i++)
		{
			if ( sFieldValue1[i] != NULL )
			{
				/* run only if NOERROR; otherwise do not break to finish up the free() */
				if ( error == NOERROR )
				{
					LongLine = (char*) calloc( 105 + strlen(sFieldValue1[i]) + strlen(sFieldValue2[i]), sizeof(char));
					if ( LongLine != NULL )
					{
						sprintf(LongLine,"! %33s ! %-33s ! %-33s !", sFieldValue1[i], sFieldValue2[i], sFieldValue3[i]);
						error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, LongLine);
						free(LongLine);
					}
				}
				else
				{
					if ( ToDiscard )
						CompoundTextDiscard(*hCompound);
					else
						ToDiscard = 0;
				}
			}
		} /* end for */

		/* close the box '!-!' */
		if ( iUnderline != 0 )
		{
			if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
				CompoundTextDiscard(*hCompound);

			if ( HeaderTitle_UnderLine != NULL )
				free(HeaderTitle_UnderLine);
		}

	} /* nbitem > 0 */

	/* free memory */
	FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
	FreeMemory_GetFieldInList(sFieldValue2, nbitem2);
	FreeMemory_GetFieldInList(sFieldValue3, nbitem3);

	return(error);

} /* end of [ AppendThreeeTextFieldToBodyWithHeader ] */


/*****************************************************************************************************************
* AppendTwoTextFieldToBodyWithHeader
* Version	: 1.1
* Purpose	: Append 2 TEXT field ( TEXT or TEXT LIST) to the supplied BODY ITEM ( Rich TEXT)
*			: if not null start with the header,
*           : underlined or not
*			: Write all the TEXT item
*			: use separator ( new line ) or ( ,; 'char').: TODO - V1.0 => newline :
*			:
* INPUT		: HNOTE ( current note )
*			: COMPOUND handle ( BODY field )
*			: TEXT FIELD name, char* header, int 'underline (1/0), Separator ( NULL => newline)
* OUTPUT	: STATUS
*******************************************************************************************************************/
STATUS	AppendTwoTextFieldToBodyWithHeader(NOTEHANDLE hNote,
										HANDLE* hCompound,
										DWORD dwNormalStyle,
										char* sTextItemFieldName1,
										char* sTextItemFieldName2,
										char* HeaderTitle,
										int iUnderline, char sep)
{
	STATUS			error	= NOERROR;
	int				nbitem	= 0;
	int				nbitem1 = 0;
	int				nbitem2 = 0;
	char*			sFieldValue1[MAXDELETEATTACH];
	char*			sFieldValue2[MAXDELETEATTACH];
	int				HeaderLength = 0;
	char*			HeaderTitle_UnderLine;
	int				i = 0;
	char*			LongLine;
	int				ToDiscard = 1;


	/* Get Fields - if not present => error ( ERR_ITEM_NOT_FOUND) */
	if (! NSFItemIsPresent(hNote, sTextItemFieldName1, (WORD) strlen(sTextItemFieldName1)) )
				return(ERR_ITEM_NOT_FOUND);

	if (! NSFItemIsPresent(hNote, sTextItemFieldName2, (WORD) strlen(sTextItemFieldName2)) )
				return(ERR_ITEM_NOT_FOUND);

	/* Get TEXT, TEXT LIST - FIELD 1 */
	if ( error = GetFieldInList(hNote, sTextItemFieldName1, sFieldValue1, MAXPATH, MAXDELETEATTACH, &nbitem1) )
		return(error);

	/* Get TEXT, TEXT LIST - FIELD 2 */
	if ( error = GetFieldInList(hNote, sTextItemFieldName2, sFieldValue2, MAXPATH, MAXDELETEATTACH, &nbitem2) )
	{
		FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
		return(error);
	}


	/* force field length synchro */
	nbitem = mw_MIN(nbitem1, nbitem2);

	if ( nbitem > 0 )
	{
		/* Append Header if not NULL */
		if ( ( HeaderLength = (int) strlen(HeaderTitle) ) > 0 )
		{
			/* over flow */
			if ( HeaderLength > (MAXPATH - 1) )
			{
				HeaderTitle[MAXPATH -1] = '\0';
				HeaderLength = MAXPATH - 1;
			}

			if ( iUnderline != 0 )
			{
				/* create header underline */
				HeaderTitle_UnderLine = (char*) calloc( HeaderLength + 1, sizeof(char) );

				if ( HeaderTitle_UnderLine != NULL )
				{
					/* create the underliner */
					HeaderTitle_UnderLine = (char*) memset(HeaderTitle_UnderLine, '-', HeaderLength); /* HeaderLength + 1 = '\0' => calloc */

					if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
					{
						CompoundTextDiscard(*hCompound);
						free(HeaderTitle_UnderLine);
						FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
						FreeMemory_GetFieldInList(sFieldValue2, nbitem2);
						return(error);
					}
				}
				else
					iUnderline = 0;

			}/* end if : iUnderline */

			/* Append first line : HEADER */
			if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle) )
			{
				CompoundTextDiscard(*hCompound);

				if ( HeaderTitle_UnderLine != NULL )
					free(HeaderTitle_UnderLine);

				FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
				FreeMemory_GetFieldInList(sFieldValue2, nbitem2);

				return(error);
			}


			if ( iUnderline != 0 )
			{
				if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
				{
					CompoundTextDiscard(*hCompound);

					if ( HeaderTitle_UnderLine != NULL )
						free(HeaderTitle_UnderLine);

					FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
					FreeMemory_GetFieldInList(sFieldValue2, nbitem2);

					return(error);
				}
			}


		} /* end if :  HeaderTitle is not empty */


		/* -------------------- Append the TEXT item values -----------------------------*/
		for (i = 0; i < nbitem; i++)
		{
			if ( sFieldValue1[i] != NULL )
			{
				/* run only if NOERROR; otherwise do not break to finish up the free() */
				if ( error == NOERROR )
				{
					LongLine = (char*) calloc(90 + strlen(sFieldValue1[i]) + strlen(sFieldValue2[i]), sizeof(char));
					if ( LongLine != NULL )
					{
						sprintf(LongLine,"! %40s ! %-40s !", sFieldValue1[i], sFieldValue2[i]);
						error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, LongLine);
						free(LongLine);
					}
				}
				else
				{
					if ( ToDiscard )
						CompoundTextDiscard(*hCompound);
					else
						ToDiscard = 0;
				}
			}
		}


		/* close the box '!-!' */
		if ( iUnderline != 0 )
		{
			if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
				CompoundTextDiscard(*hCompound);

				if ( HeaderTitle_UnderLine != NULL )
					free(HeaderTitle_UnderLine);
		}

	}/* nbitem > 0 */

	/* free memory */
	FreeMemory_GetFieldInList(sFieldValue1, nbitem1);
	FreeMemory_GetFieldInList(sFieldValue2, nbitem2);

	return(error);
}



/*****************************************************************************************************************
* AppendTextFieldToBodyWithHeader
* Version	: 1.1
* Purpose	: Append a field ( TEXT or TEXT LIST) to the supplied BODY ITEM ( Rich TEXT)
*			: if not null start with the header,
*           : underlined or not
*			: Write all the TEXT item
*			: use separator ( new line ) or ( ,; 'char').: TODO - V1.0 => newline :
*			:
* INPUT		: HNOTE ( current note )
*			: COMPOUND handle ( BODY field )
*			: TEXT FIELD name, char* header, int 'underline (1/0), Separator ( NULL => newline)
* OUTPUT	: STATUS
*******************************************************************************************************************/
STATUS	AppendTextFieldToBodyWithHeader(NOTEHANDLE hNote,
										HANDLE* hCompound,
										DWORD dwNormalStyle,
										char* sTextItemFieldName,
										char* HeaderTitle, int iUnderline, char sep)
{
	STATUS			error	= NOERROR;
	int				nbitem	= 0;
	char*			sFieldValue[MAXDELETEATTACH];
	int				HeaderLength = 0;
	char*			HeaderTitle_UnderLine;
	int				i = 0;
	int				ToDiscard = 1;

	/* Get Fields - if not present => error ( ERR_ITEM_NOT_FOUND) */
	if (! NSFItemIsPresent(hNote, sTextItemFieldName, (WORD) strlen(sTextItemFieldName)) )
				return(ERR_ITEM_NOT_FOUND);

	/* Get TEXT, TEXT LIST */
	if ( error = GetFieldInList(hNote, sTextItemFieldName, sFieldValue, MAXPATH, MAXDELETEATTACH, &nbitem) )
		return(error);

	if ( nbitem > 0 )
	{
		/* Append Header if not NULL */
		if ( ( HeaderLength = (int) strlen(HeaderTitle) ) > 0 )
		{
			/* over flow */
			if ( HeaderLength > (MAXPATH - 1) )
			{
				HeaderTitle[MAXPATH -1] = '\0';
				HeaderLength = MAXPATH - 1;
			}

			/* Append first line : HEADER */
			if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle) )
			{
				CompoundTextDiscard(*hCompound);
				return(error);
			}

			if ( iUnderline != 0 )
			{
				/* create header underline */
				HeaderTitle_UnderLine = (char*) calloc( HeaderLength + 1, sizeof(char) );

				if ( HeaderTitle_UnderLine != NULL )
				{
					/* create the underliner */
					HeaderTitle_UnderLine = (char*) memset(HeaderTitle_UnderLine, '-', HeaderLength); /* HeaderLength + 1 = '\0' => calloc */

					if ( error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, HeaderTitle_UnderLine) )
					{
						CompoundTextDiscard(*hCompound);
						free(HeaderTitle_UnderLine);
						return(error);
					}
					free(HeaderTitle_UnderLine);
				}
			}/* end if : iUnderline */
		} /* end if :  HeaderTitle is not empty */


		/* -------------------- Append the TEXT item values -----------------------------*/
		for (i = 0; i < nbitem; i++)
		{
			if ( sFieldValue[i] != NULL )
			{
				/* run only if NOERROR; otherwise do not break to finish up the free() */
				if ( error == NOERROR )
					error = AppendLineToDefinedBodyField(hCompound, dwNormalStyle, sFieldValue[i]);
				else
				{
					if ( ToDiscard )
						CompoundTextDiscard(*hCompound);
					else
						ToDiscard = 0;
				}

				/* free anyway - till the end of for */
				free(sFieldValue[i]);
			}
		}

	}

	return(error);
}

/***************************************************************************************
* ProcBl_AppendFieldDetailsToEmailBody
* PURPOSE		: use in the ProcBlackList, can append field list to Body.
* INPUT			: filter document with field definition
* INPUT			: email document (hnote) - with a BODY field
* OUTPUT		: STATUS
**************************************************************************************************/
STATUS ProcBl_AppendFieldDetailsToEmailBody(NOTEHANDLE hNoteFiltre, NOTEHANDLE hNote,
											char* sHeaderTitle,
											char* sFieldRef)
{
	STATUS			error = NOERROR;
	HANDLE			hCompound;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	char*			sFieldValue[MAXDELETEATTACH];
	char*			sTitleValue[MAXDELETEATTACH];
	int				Go = 0;
	int				i = 0;
	int				j = 0;
	int				nbitem = 0;
	int				nbitem_title = 0;

	/* Check if the field are presents */
	if (! NSFItemIsPresent(hNoteFiltre, sFieldRef, (WORD) strlen(sFieldRef)) )
				return(ERR_ITEM_NOT_FOUND);

	/* Get List */
	if ( error = GetFieldInList(hNoteFiltre, sFieldRef, sFieldValue, MAXPATH, MAXDELETEATTACH, &nbitem) )
		return(error);


	if ( nbitem > 0 )
	{
		/* init title array */
		for ( j = 0; j < MAXDELETEATTACH; j++)
			sTitleValue[j]=NULL;

		/* Get Title TEXT List */
		error = GetFieldInList(hNoteFiltre, sHeaderTitle, sTitleValue, MAXPATH, MAXDELETEATTACH, &nbitem_title);

		/* error or wrong number.. reset */
		if ( (error != NOERROR) || (nbitem_title != nbitem) )
		{
			for (j = 0; j < nbitem; j++ )
			{
				if ( sTitleValue[j] == NULL )
				{
					sTitleValue[j] = (char*) malloc(2);

					if ( sTitleValue[j] != NULL )
						strcpy(sTitleValue[j]," ");
				}
			}
			/* synchronize the Header List & Value List */
			nbitem_title = nbitem;
		}


		/* get hNoteEmail Body ( ! or create it ? ) */
		if( error = CompoundTextCreate(hNote, "Body", &hCompound) )
		{
			FreeMemory_GetFieldInList(sFieldValue, nbitem);
			FreeMemory_GetFieldInList(sTitleValue, nbitem_title);
			return(error);
		}

		CompoundTextInitStyle(&Style);

		if (error = CompoundTextDefineStyle( hCompound, "Normal", &Style, &dwNormalStyle) )
		{
			FreeMemory_GetFieldInList(sFieldValue, nbitem);
			FreeMemory_GetFieldInList(sTitleValue, nbitem_title);
			CompoundTextDiscard(hCompound);
			return(error);
		}

		/* Call up the Notifications proc */
		Go = 0;
		for (i = 0; i < nbitem; i++)
		{
			if ( sFieldValue[i] != NULL )
			{
				NSFItemAppendTextList(hNote, "MW_temp_Field_to_Print", sFieldValue[i], (WORD) strlen(sFieldValue[i]), FALSE);
				Go = 1;
			}
		}

		if ( Go == 1)
		{
			if ( error = AppendTextListToBodyUsingFieldArgument(hNote, &hCompound, dwNormalStyle, "MW_temp_Field_to_Print", sTitleValue[0], sTitleValue[1], sTitleValue[2],1, '-') )
			{
				FreeMemory_GetFieldInList(sFieldValue, nbitem);
				FreeMemory_GetFieldInList(sTitleValue, nbitem_title);
				CompoundTextDiscard(hCompound);
				return(error);
			}

			/* remove temporary field */
			NSFItemDelete(hNote, "MW_temp_Field_to_Print", (WORD) strlen("MW_temp_Field_to_Print"));
		}

		/* Close Compound */
		FreeMemory_GetFieldInList(sFieldValue, nbitem);
		FreeMemory_GetFieldInList(sTitleValue, nbitem_title);

		if (error = CompoundTextClose (hCompound, NULL, NULL, NULL, 0))
			CompoundTextDiscard(hCompound); 		/* Free up ressource if necessary */
	}

	return(error);

}/* end of ProcBl_AppendFieldDetailsToEmailBody */


/*************************************************************************
* PROC		: GetDatabaseHandle
* INPUT		: filename (on the local server)
* OUTPUT	: dbhandle.
*			: STATUS
**************************************************************************/
STATUS GetDatabaseHandle(char* servername, char * mailfile, DBHANDLE * mail_handle, BOOL vUnix)
{
	STATUS		erreur = NOERROR;
	char		mailfilepath[MAXPATH];
	char		servername_upper[MAXUSERNAME];


	/* Copy local */
	strcpy(servername_upper, servername);
	UpperCase(servername_upper);

	/* Open : Get an dbhandle */
	if ( strlen(servername) == 0 || (strcmp(servername_upper, "LOCAL") == 0) )
	{
		/* make Unix path if necessary */
		if ( strlen( mailfile) )
			CheckPath(mailfile);

		/* Get the DB on "local" server */
        erreur = NSFDbOpen(mailfile, mail_handle);
	}

	/* ----- Server name has been specified ------ */
	else
	{
		/* make.unmake 'Unix' path if necessary */
		CheckPath(mailfile);

		/* build full path */
		if (erreur = OSPathNetConstruct(NULL, servername, mailfile, mailfilepath))
			return(erreur);

		erreur = NSFDbOpen(mailfilepath, mail_handle);
	}

	/* end */
	return(erreur);

}

/**************************************************************
* PURPOSE		: readjust time to - 5 years
*/
void InitMailBoxOpeningTime( TIMEDATE* retoldtime, int NbMailBox )
{
	int	ctr = 0;

	/***** Loop over tha MAXMAILBOX time array 	***/
	for (ctr = 0; ctr < NbMailBox; ctr++)
	{
		OSCurrentTIMEDATE(&retoldtime[ctr]);
		TimeDateAdjust(&retoldtime[ctr], 0, 0, 0, 0, 0, -5);
	}
}


/*******************************************************************************************************
* PROC		: GetUserPredicat
* PURPOSE	: names & address book special Query for a specific field	
*			: Save the value in a list : fieldname | value						
* INPUT		: query key : fullname
*           : fielname : field to read
*           : Linked list (char*) 
* OUTPUT	: n - number of value found / 0 if none.
*******************************************************************************************************/
int GetUserPredicat(char* username, char* fieldname, GrpList** PredicatList)
{
	STATUS		sErreur = NOERROR;
	HANDLE		hLookup = NULLHANDLE;
	char		*pLookup, *pName, *pMatch;
	WORD		numret = 0;
	WORD		k = 0;
	int			check = 0;
	char		user_predicat[MAXUSERNAME];
	char		user_predicat_abbr[MAXUSERNAME];
	char		user_predicat_full_definition[MAXRECLEN];
    
	/* Create the user as a group */
	UpperCase(username);

	/* Lookup the alias by looking up in the $users view */
	if ( sErreur = NAMELookup(NULL, 0, 1, "($Users)", 1, username, 1, fieldname, &hLookup) )
	{
		if (NULLHANDLE != hLookup)
			OSMemFree(hLookup);

		return(0);
	}

	/* found ! : new get the values */
	pLookup = OSLockObject(hLookup);
	pName = NAMELocateNextName(pLookup, NULL, &numret);
	
	if (!numret)
	{
		OSUnlockObject(hLookup);
		OSMemFree(hLookup);
		/* nothing has been uploaded = $user found but no alias are to be uploaded */
		return(0);
	}

	/* loop over the lookup buffer */
	pMatch = NAMELocateNextMatch(pLookup, pName, NULL);
	k = 0;

	do
	{
		if ( sErreur = NAMEGetTextItem(pMatch, 0, k, user_predicat, sizeof(user_predicat)-1) )
		{
			/* Error when k = 0 means there is NO "field" */
			if ( k == 0 )
			{
				OSUnlockObject(hLookup);
				OSMemFree(hLookup);
				/* nothing has been uploaded */
				return(0);
			}
			break;
		}

		/* upload field values when not "" */
		if ( strlen(user_predicat) > 0 )
		{
			UpperCase(user_predicat);
			UpperCase(fieldname);

			/* if field is a NAME */
			DNAbbreviate(0L, NULL, user_predicat, user_predicat_abbr, MAXUSERNAME, NULL);

			/* the predicats is code : fieldname|value */
			sprintf(user_predicat_full_definition , "%s%c%s", fieldname, PREDICAT_ITEM_SEP, user_predicat_abbr);			
			*PredicatList = InsertGroup(*PredicatList, user_predicat_full_definition);

			check = 1;
		}

		k++;

	} while (!sErreur);

	/* free memory */
	OSUnlockObject(hLookup);
	OSMemFree(hLookup);

	/* ok some alias have been uploaded (if all of them where not blank ;)so => user (1) will be changed to (2 = Group) */
	return(check);

} /* end - GetUserPredicat - */

/* FreeMemory of GrpList */
void FreeGrpList(GrpList* L )
{
	GrpList* Lnext;

	while ( L != NULL )
	{
		Lnext = L->Next;
		
		/* free item */
		if ( L->GroupName != NULL )
			free(L->GroupName);

		/* free node */ 
		if ( L != NULL )
			free(L);

		/* continue */
		L = Lnext;
	}

}




/**********************************************************************************************************
* PROG			: ReadFirstUserPredicat( )
* PURPOSE		: Get a user specific predicats
*				: Read the user list then look for GroupName that start with the predicats.
*				: Predicats have the following syntax : Predicat|Value
* INPUT			: Username ( user user key for search )
*				: Predicats name ( user group key for search - look for strstr( ) and not strcmp ). 
* OUTPUT		: int : number of predicats found ( 1 / O )
*				: char* the predicats value as string
* ----------------------------------------------------------------------------------------------------------
* prevent overflow.
* char* PredicatValue  - size : [ MAXRECLEN ];
************************************************************************************************************/
int ReadFirstUserPredicat(char* UserName, char* Predicat, SpUser* SpecialUserListGlobal, char* PredicatValue)
{
	SpUser*		SpUserItem = NULL;
	int			PredicatFound = 0;
	char		MatchPredicats[MAXRECLEN];
	GrpList*	PredicatItem;
	char*		Pos_PredicatSep = NULL;

	/* Get user record */
	if ( ( SpUserItem = SearchUser(SpecialUserListGlobal, UserName) ) != NULL )
	{
		/* Search for Predicats Name in the GroupList */
		sprintf( MatchPredicats, "%s%c*", Predicat, PREDICAT_ITEM_SEP);

		if ( (PredicatItem = SearchGroupWithMeta(SpUserItem->GroupList,  MatchPredicats)) != NULL )
		{
			/* Extract the value */
			Pos_PredicatSep = strchr( PredicatItem->GroupName, PREDICAT_ITEM_SEP );
			if ( Pos_PredicatSep )
			{
				if ( strlen(Pos_PredicatSep) > 1 )
				{
					Pos_PredicatSep++;		/* skip one char the separator */
					PredicatFound++;
					strcpy(PredicatValue, Pos_PredicatSep); 
				}
			}
		}
	}

	return(PredicatFound);

} /* end of ReadFirstUserPredicat( ) */


/**********************************************************************************************************
* PROG			: ReadAllUserPredicats( )
* PURPOSE		: Get all the user specific predicats
*				: Read the user list then look for GroupName that start with the predicats.
*				: Predicats have the following syntax : Predicat|Value1, Predicat|Value2
* INPUT			: Username ( user user key for search )
*				: Predicats name ( user group key for search - look for strstr( ) and not strcmp ).
*				: result MAX item.
* OUTPUT		: int : number of predicats found ( n / O )
*				: char* the predicats value as string with PREDICAT_ITEM_SEP between them
* ----------------------------------------------------------------------------------------------------------
*				: memory managment.
*				: char* PredicatValue  - size : [ MaxPredicatSize ] * MaxPredicatList;
* NB			: Caller must free the memory !!
************************************************************************************************************/
int ReadAllUserPredicats(char* UserName, char* Predicat, SpUser* SpecialUserListGlobal, char** PredicatValue, int MaxPredicatSize, int MaxPredicatList)
{
	SpUser*		SpUserItem = NULL;
	int			PredicatFound = 0;
	GrpList*	PredicatItem = NULL;
	char*		MatchPredicats = NULL;
	char*		Pos_PredicatSep = NULL;
	int			cond = 1;


	/* Allocate memory  */
	if ( MaxPredicatList < 0 )
		MaxPredicatList = 1;

	if ( MaxPredicatSize > MAXRECLEN )
		MaxPredicatSize = MAXRECLEN;

	/* Global result */
	 *PredicatValue  = calloc( (MaxPredicatSize + 1) * MaxPredicatList, sizeof(char) );

	 /* not enough memo - get out */
	 if ( *PredicatValue == NULL ) 
		 return(0);

     /* Search string */	
	 MatchPredicats = calloc( (MaxPredicatSize + 5), sizeof(char) );
	
	 /* not enough memo - get out */
	 if ( MatchPredicats == NULL )
	 {
		if ( *PredicatValue != NULL )
			free(*PredicatValue);

		/* make sure the call will not free this one again */
		PredicatValue = NULL;
		return(0);
	 }


	/* Get user record */
	if ( ( SpUserItem = SearchUser(SpecialUserListGlobal, UserName) ) != NULL )
	{
		/* Search for Predicats Name in the GroupList */
		sprintf( MatchPredicats, "%s%c*", Predicat, PREDICAT_ITEM_SEP);

		PredicatItem = SearchGroupWithMeta(SpUserItem->GroupList,  MatchPredicats);

		while ( ( PredicatItem != NULL ) && ( cond == 1) )
		{
			/* Loop over the value */
			Pos_PredicatSep = strchr( PredicatItem->GroupName, PREDICAT_ITEM_SEP );
			
			if ( Pos_PredicatSep )
			{
				if ( strlen(Pos_PredicatSep) > 1 )
				{
					if ( PredicatFound == 0 )
						Pos_PredicatSep++;							/* Skip first separator */
					PredicatFound++;
					strcat(*PredicatValue, Pos_PredicatSep);			/* otherwise keep the original separator : PREDICAT_ITEM_SEP */
				}
			}

			/* next  */
			PredicatItem = PredicatItem->Next;
			cond = wc_match(MatchPredicats, PredicatItem->GroupName);	/* argt1 : wildcard, argt2 : target */

		} /* end while */
	}

	/* Free local */
	if ( MatchPredicats != NULL )
		free(MatchPredicats);

	/* free memory here if there nothing has been found */
	if ( PredicatFound == 0 )
	{
		if ( *PredicatValue != NULL )
			free(*PredicatValue);

		/* make sure the call will not free this one again */
		*PredicatValue = NULL;
	}

	return(PredicatFound);

} /* end of ReadAllUserPredicats( ) */




/*****************************************************************************************************
PROG	: GetFilterRulesParameterOrPredicats( )
Release	: 3.95
PURPOSE	: Read the string argument, replace it by the predicats (FROM or TO ) or leave it as is
		: if predicats is not valid.
INPUT	: note_handle, fieldname.
OUPUT	: fields value.
*********************************************************************************************************/
STATUS	GetFilterRulesParameterOrPredicats(NOTEHANDLE hNote, char* fieldname, char* field_value, int MaxArgtSize, char* FROM, char* TO, SpUser* HeadofList)
{
	STATUS		error = NOERROR;
	char		PredicatFieldName[MAXUSERNAME + 10];		/* "Pred_{fieldname} */
	char		PredicatsValue[MAXUSERNAME];					/* "0":"1":"2":..."n"  or PredicatValue*/

	/* Read the filter argument */
	NSFItemGetText(hNote, fieldname, field_value, MaxArgtSize -1);

	if ( field_value[0] != NULL )
	{
		/* Required to query the memory list */
		UpperCase(field_value);

		/* Check Predicat Field associated */
		sprintf(PredicatFieldName,"%s%s", BLACKLIST_PREDICAT_FIELD_PREFIX, fieldname);

		/* Get the predicats value (0,1,2....,n) */
		NSFItemGetText(hNote, PredicatFieldName, PredicatsValue, MAXUSERNAME -1);

		/* field_value has been set but if the argument PredicatValues tells that it is a predicate it will be updated or erase if not OK */
		switch ( PredicatsValue[0] )
		{
			/* FROM predicats */
			case '1':
				/* Replace 'field_value' by FROM-PREDICATS if not NULL */
				if ( ReadFirstUserPredicat(FROM, field_value, HeadofList, PredicatsValue) )
					strcpy(field_value, PredicatsValue);
				else
					field_value[0]='\0';		/* 3.95.5 : reset if not found */
				break;

			/* TO predicats */
			case '2':
				/* Replace 'field_value' by TO-PREDICATS if not NULL */
				if ( ReadFirstUserPredicat(TO, field_value, HeadofList, PredicatsValue) )
					strcpy(field_value, PredicatsValue);
				else
					field_value[0]='\0';		/* 3.95.5 : reset if not found */
				break;

		}
	}

	return(error);
}





/*****************************************************************************************************
PROG	: GetFilterRulesParameterOrAllPredicats( )
Release	: 3.95.6
PURPOSE	: Read the string argument, replace it by the predicats (FROM or TO ).
INPUT	: note_handle, fieldname ( containing the predicat value)
		: field_value, the fields where to write the predicats ( FIELD is TEXT LIST ).
OUPUT	: fields value.
*********************************************************************************************************/
STATUS	GetFilterRulesParameterOrAllPredicats(NOTEHANDLE hNote, char* fieldname, NOTEHANDLE target_hNote, char* targetfield_value, 
											  int MaxArgtSize, char* FROM, char* TO, SpUser* HeadofList, int MaxPredicatSize, int MaxPredicatList)
{
	STATUS		error = NOERROR;
	char		PredicatFieldName[MAXUSERNAME + 10];		/* "Pred_{fieldname} */
	char		PredicatsArgt[MAXUSERNAME];
	char*		PredicatsValue = NULL;
	char		filter_value[MAXRECLEN];

	/* Read the filter argument */
	NSFItemGetText(hNote, fieldname, filter_value, MaxArgtSize -1);

	if ( filter_value[0] != NULL )
	{
		/* Required to query the memory list */
		UpperCase(filter_value);

		/* Check Predicat Field associated */
		sprintf(PredicatFieldName,"%s%s", BLACKLIST_PREDICAT_FIELD_PREFIX, fieldname);

		/* Get the predicats value (0,1,2....,n) */
		NSFItemGetText(hNote, PredicatFieldName, PredicatsArgt, MAXUSERNAME - 1);

		/* field_value has been set but if the argument PredicatValues tells that it is a predicate it will be updated or erase if not OK */
		switch ( PredicatsArgt[0] )
		{
			/* FROM predicats */
			case '1':
				/* Replace 'field_value' by FROM-PREDICATS if not NULL */
				if ( ReadAllUserPredicats(FROM, filter_value, HeadofList, &PredicatsValue, MaxPredicatSize, MaxPredicatList) )
					MakeTextListFieldStringWithSep(target_hNote, targetfield_value, PredicatsValue, PREDICAT_ITEM_SEP_CHAR );
				break;

			/* TO predicats */
			case '2':
				/* Replace 'field_value' by TO-PREDICATS if not NULL */
				if ( ReadAllUserPredicats(TO, filter_value, HeadofList, &PredicatsValue, MaxPredicatSize, MaxPredicatList) )
					MakeTextListFieldStringWithSep(target_hNote, targetfield_value, PredicatsValue, PREDICAT_ITEM_SEP_CHAR );
				break;

		}
	}

	return(error);
}







/***********************************************************************
 * PROG		: SearchGroupWithMeta(SpUserItem, GroupName);
 * PURPOSE	: same as SearchGroup but comparison uses wildcard pattern
 * OUTPUT	: Groupitem found or NULL
  ----------------------------------------------------------------------
  wc_match:
 
 * This is the real wildcard matching routine. It returns 1 for a
 * successful match, 0 for an unsuccessful match, and <0 for a
 * syntax error in the wildcard.
 
 **********************************************************************/
GrpList* SearchGroupWithMeta(GrpList* Current, char* GroupName)
{
	int cond = 1;

	while ( cond && (Current->Next != NULL) )
	{
		if ( cond = ( ! wc_match(GroupName, Current->GroupName) ) )		/* cond = (not) wc_match */
			Current = Current->Next;
	}

	/* Return value */
	if ( Current->Next == NULL )
		return(NULL);
	else
		return(Current);

} /* end of SearchGroupWithMeta */


/***********************************************************
 *	LogErr() : log Notes d'une erreur
 *	Inputs : char * = message à afficher
 *					 STATUS = code erreur interne Notes
 ***********************************************************/
void LogErr(char *message, STATUS err)
{
 	char meserr[150];

	if (err)
	{
		OSLoadString(NULLHANDLE, ERR(err), meserr, sizeof(meserr)-1);
		LogEventText("%s : <%s> - <%s>", NULLHANDLE, NOERROR, NOMAPI, message, meserr);
	}
	else
		LogEventText("%s : %s", NULLHANDLE, NOERROR, NOMAPI, message);
	return;
}




/*************************************************************************
* PROG		:
* PURPOSE	: Create TEXT LIST FIELD based on a string
* INPUT		: hnote, fieldname, string to parse, separator
* OUTPUT	: STATUS
*****************************************************************************/
STATUS MakeTextListFieldStringWithSep(NOTEHANDLE hNote, char* fieldname, char* stritem, char* sep )
{
	STATUS	error = NOERROR;
	char*	token;

	/* get first */
	token = strtok(stritem, sep);

	/* loop & update */
	while ( ( token != NULL ) && ( error == NOERROR ) )
	{
		error = NSFItemAppendTextList(hNote, fieldname, token, (WORD) strlen(token), FALSE);

		/* next token => mwtag item */
		if ( error == NOERROR )
			token = strtok( NULL, sep);
	}

	return(error);
}




/* --------------------------------------------------*/
/* 3.94.51 : use GetFieldInList - new argt : MAXList */

/***************************************************************************************
* GetFieldInList
* INPUT		: NOTEHANDLE	-> Current Doc
*			: char*			-> Field Name
*
* OUTPUT	: STATUS		-> error
*			: char**		-> field_List
*			: int			-> nb item
*****************************************************************************************/
STATUS GetFieldInList(NOTEHANDLE note_handle, char* FieldName, char** field_List, int MAXItemSize, int MAXList, int* nbitem)
{
	STATUS	error		= NOERROR;
	int		i			= 0;
	int		nb_field	= 0;
	WORD	typechamp;

	/* init */
	*nbitem = 0;

	/* Type ? */
	if (! NSFItemIsPresent(note_handle, FieldName, (WORD) strlen(FieldName) ) )
		return(ERR_ITEM_NOT_FOUND);

	/* Get the Data */
	NSFItemInfo(note_handle, FieldName, (WORD) strlen(FieldName), NULL, &typechamp, NULL, NULL);

	if (typechamp == TYPE_TEXT)
	{
		field_List[i] = (char*) malloc(MAXItemSize);
		if ( field_List[i] != NULL )
		{
			if(! NSFItemGetText(note_handle, FieldName, field_List[i], MAXItemSize -1) )
				field_List[i] = NULL;
			else
			{
				field_List[i];
				(*nbitem)++;
			}
		}
		field_List[i+1]= NULL;
	}
	else if (typechamp == TYPE_TEXT_LIST)
	{
		nb_field = NSFItemGetTextListEntries(note_handle, FieldName);

		/* : fix ( 7/12/2005 - LP ): control the size of the list - malloc */
		if ( MAXList != 0)
			nb_field = mw_MIN( MAXList, nb_field);

		for (i = 0; i < nb_field; i++)
		{
			field_List[i] = (char*) malloc(MAXItemSize);

			if ( field_List[i] != NULL )
			{
				if ( ! NSFItemGetTextListEntry(note_handle, FieldName, (WORD) i, field_List[i], (WORD) MAXItemSize -1) )
					i = nb_field;	/* break */
				else
					(*nbitem)++;
			}
		}
		/* end of list */
		field_List[i]= NULL;
	}
	else
		return(1);	/* wrong type */

	/* end */
	return(error);
} /* end of : GetFieldInList */





/**********************************/
/*     end of source file         */
/**********************************/




