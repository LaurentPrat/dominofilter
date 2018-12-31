/****** Quick version with all the basic information *****/
STATUS SimpleAppendDocLink(TIMEDATE replica_ID, UNID Note_UNID, UNID View_UNID,
						   char* DocLinkText, HANDLE hCompound)
{

	STATUS			error;
	COMPOUNDSTYLE	Style;
	DWORD			dwNormalStyle;
	char			DocUniqueID[33];
	char			DBUniqueID[33];
	char			ViewUniqueID[33];
	char			mail_server_abbr[MAXPATH];
	char			URLText[350];

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
	sprintf(URLText, "<a href=\"http://%s/%s/%s/%s?opendocument\"> web : %s </a>", mail_server_abbr, DBUniqueID, ViewUniqueID, DocUniqueID, DocLinkText );

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



	/* end */
	return(error);

}
