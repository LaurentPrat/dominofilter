/**************************************************************************
* File		: rulesmem.c
* Version	: 1.0
* PURPOSE	: Rules memory managment
* DATE		: 12/04
* BY		: LP
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>	/* malloc	*/
#include <string.h>	/* strcpy	*/
#include <time.h>	/* time		*/
#include <math.h>	/* floor	*/


/* local includes */
#include "rulesmem.h"
#include "mwstrjob.h"

/*********************************** AS400 Patch *******************************/
#ifdef OS400
#define MW_EBCDIC
#define __STRING_CODE_SET__ 850
#pragma convert(850)
/* no more include with QADRT package from IBM */
#endif



#ifdef TEST

/******** TEST prg *****************/
int main(int argc, char** argv)
{
	RulesList*		L_rules;
	RulesList*		Header;
	RulesList**		StartL;
	char			srules[10];
	/* test eval */
	char*			sexpression;
	double			result;
	char			save_expr[100];



	/* Make the list */
	L_rules = MakeEmptyRulesList();
	if ( L_rules )
		StartL = &L_rules;

	if (( argc == 2 ) 	& ( argv[1] != NULL ))
	{
		printf("reloading rules from %s \n", argv[1]);
		RulesMemoryRestoreFromFile(L_rules, argv[1], StartL );
	}

	while ( 1 )
	{
		printf("input rules ref :");
		scanf("%s", srules );

		if ( strcmp(srules,"end") )
		{
			/* todo : keep track of the header of the list.. x->prev == NULL is the one !, update if necessary */
			Header = RulesInsertInList(L_rules, srules, StartL );
			if ( Header == NULL )
				printf("** error while creating rules '%s'\n", srules);
			else
				DisplaySimpleRulesList(*StartL);

		}
		else
			break;

		/* reset start of the List */
		L_rules = *StartL;
	}

	/* eval loop */
	printf(" -- starting eval expression  (type end to quit) \n");
	while (!feof(stdin))
	{
			printf("input eval expression :");
			sexpression = readLine(stdin);

			if ( strcmp ( sexpression , "end" ) == 0)
				break;
			/* save for display */
			strcpy(save_expr, sexpression);
			if ( evalrules(L_rules, sexpression, &result) )
				printf("** could not evaluate %s \n", save_expr );
			else
				printf(" %s -> %d \n", save_expr, (int) result);

			free(sexpression);
	}

	/* --------------- real end now -------------------*/
	/* Dump the full List */
	printf("--------------- [Dump ] --------------------- \n");
	Header = *StartL;
	DisplayRulesList(Header, 60);

	printf(" -----------[ save to file ] ---------------\n");
	Header = *StartL;
	if ( RulesMemoryDumpToFile(Header, "dumprulesmemory.txt") )
		printf("error while dumping to file \n");


	/* destroy the list */
	printf(" ---------- [ Clear memory ] ---------------\n");
	Header = *StartL;
	if ( DestroyRulesMemoryList( Header) )
		printf("error while releasing memory \n");


	/* end */
	return(0);
}

/******** TEST *************/
#endif



/* ------------------------------------------------------------*/
/* Init Rules Item                                             */
RulesMemory* MakeEmptyRules()
{
	RulesMemory* rules1;

	rules1 = ( RulesMemory *) malloc ( sizeof (RulesMemory) );
	if ( rules1 )
	{
		rules1->RulesRef = NULL;	/* will be allocated later */
		rules1->start_time = 0;
		rules1->has_run = 0;
	}

	return(rules1);
}



/* ------------------------------------------------------------*/
/* init rules list root                                        */
RulesList* MakeEmptyRulesList()
{
	RulesList* rl;

	rl = ( RulesList*) malloc ( sizeof (RulesList) );
	if ( rl )
	{
		rl->r = NULL;
		rl->next = NULL;
		rl->prev = NULL;
	}

	return(rl);
}



/* ------------------------------------------------------------*/
int	initrules(char* rulesref, RulesMemory* rules1 )
{
	/* init */
	rules1->RulesRef = (char*) malloc( strlen(rulesref) + 1 );
	if ( rules1->RulesRef == NULL )
		return(0);

	/* set values */
	strcpy( rules1->RulesRef, rulesref );

	if ( rules1->start_time == 0 )
		rules1->start_time = time(NULL);		/* number of secondes since 01/01/1970 */

	rules1->has_run++;

	/*return TRUE for sucess */
	return(1);
}


RulesList* RulesMemoryInList(RulesList* L, char* sRules)
{
	int	x = 0;

	while ( L->next != NULL)
	{
		if ( L->r != NULL )
		{
			/* List is sorted to limit the search */
			x = strcmp(sRules, L->r->RulesRef );

			/* smaller than */
			if ( x <= 0 )
				return( L );
		}
		/* go ahead */
		L = L->next;
	}

	/* end of loop, return the last */
	return( L );
}


/******
* Get the specified rules
* or NULL
**/
RulesList* GetRulesFromMemoryList(RulesList* L, char* sRules)
{
	int	x = 0;

	while ( L != NULL )
	{
		if ( L->r != NULL )
		{
			/* List is sorted to limit the search */
			x = strcmp(sRules, L->r->RulesRef );

			/* smaller than */
			if ( x == 0 )
				return( L );

			/* break */
			if ( x < 0 )
				return( NULL );
		}
		/* go ahead */
		L = L->next;
	}

	/* end of loop : return NULL */
	return( L );
}



/* Search & set */
RulesList* RulesInsertInList(RulesList* L, char* sRules, RulesList** ListHeader)
{
	RulesList* rListItem;

	/* rules ref are not case sensitive .. */
	UpperCase(sRules);

	rListItem = RulesMemoryInList(L, sRules);

	/* perfect match : update */
	if ( rListItem->r != NULL )
	{
		if ( rListItem->r->RulesRef != NULL )
		{
			if ( strcmp( rListItem->r->RulesRef, sRules) == 0 )
				rListItem->r->has_run++;
			else
			{
				/* create a new item */
				RulesMemory*	r1;
				RulesList*		rL;

				/* save rules data, reference, Top (t0), init counter */
				r1 = MakeEmptyRules();
				if ( initrules(sRules, r1) )
				{
					/* new list item */
					rL    = MakeEmptyRulesList();
					rL->r = r1;		/* rules */

					/* attach it before  ?*/
					if ( strcmp( rListItem->r->RulesRef, sRules) > 0 )
					{
						rL->next         = rListItem;			/* -> next is the found  */
						rL->prev         = rListItem->prev;		/* new item prev is the Liste prev : as new item become the header */
						rListItem->prev->next = rL;				/* item before is now linked to the item */
						rListItem->prev  = rL;					/* item prev is now linked to the new item */
						/* no change for rListItem->next */
					}
					else
					{
						/* insert after */
						rListItem->next = rL;	 	/* rL is the last one			*/
						rL->prev = rListItem;		/* rl is back track one step back 	*/
					}

					/* reset head of the list */
					if ( rL->prev->r == NULL)
						*ListHeader = rL;

					rListItem = rL;	/* set the returned value */
				} /* make sure memory has been allocated for r1 */
			}/* create a new item */
		}/* make sure memory has been allocated */
	} /* make sure memory has been allocated */
	else
	{
		/* first element :  create a new item */
		RulesMemory*	r1;
		RulesList*		rL;

		/* save rules data, reference, Top (t0), init counter */
		r1 = MakeEmptyRules();
		if ( initrules(sRules, r1) )
		{
			/* new list item */
			rL    = MakeEmptyRulesList();
			rL->r = r1;		/* rules */

			/* insert after */
			rListItem->next = rL;	 	/* rL is the last one				*/
			rL->prev = rListItem;		/* rl is back track one step back 	*/
			rListItem = rL;				/* set the returned value */
		}
	}

	return(rListItem);
}


/*************************************************************************************
* function	: GetRulesFrequency ( return double )
* PURPOSE	: Return the Rules Frequency based on the interval - in second -
*			: call sub ( RulesFrequency).
* Interval = 1 => 1 second
* Interval = 60 => 1 minutes
* Interval = 3600 => 1 hour
* and so ...
******************************************************************************************/
double GetRulesFrequency(RulesList* L, char* sRules, double Interval)
{
	RulesList*		rItem;

	/* search for the rules by key */
	rItem = GetRulesFromMemoryList(L, sRules);

	/* Get the rules */
	if ( rItem != NULL )
		return(RulesFrequency(rItem->r, Interval ));

	return(0);
}


/*************************************************************************************
* function	: (sub) for GetRulesFrequency ( return long )
******************************************************************************************/
double	RulesFrequency(RulesMemory* r, double Interval )
{
	double	nb_interval = 0;

	/* prevent DIV by 0 */
	if ( Interval <= 0 )
		Interval = 1;

	/* elaspe in interval */
	nb_interval = ( time(NULL) - (r->start_time) ) / Interval;

	/* not even a complete one : return all */
	if ( nb_interval < 1 )
		nb_interval = 1;

	/* debug */
#ifdef VERBOSE
	printf("'%s' [t0 :%lu] [nbtime:%d] [time(null):%lu] [nb_interval:%f] [Interval:%f] [ret:%f]\n", 
		    r->RulesRef, r->start_time, r->has_run, time(NULL), nb_interval, Interval, floor((double) r->has_run / nb_interval));
#endif
	return( floor((double) r->has_run / nb_interval) );
}


/**************************************************************************
* PURPOSE		: Save Rules to File
* File format	: Dump format : "%s / %d / %i \n"
*				: (%s) Rules Ref
*				: (%d) Number of second since C ANSI T0 (1970)
*				: (%i) Number of time (int)
*******************************************************************************/
int RulesMemoryDumpToFile(RulesList* L, char* filename)
{
	FILE*	f;
	char	buf[80];

	/* open a new file */
	if ( (f = fopen(filename,"w")) == NULL )
		return(1);

	/* f is now open :  loop & write */
	while ( L != NULL)
	{
		if ( L->r != NULL )
		{
			sprintf(buf,"%s / %d / %i \n", L->r->RulesRef, L->r->start_time, L->r->has_run);
			fprintf(f, buf);
		}

		L = L->next;
	}

	/* normal end */
	fclose(f);
	return(0);

}


/***
* DestroyRulesMemoryList()
*/

int DestroyRulesMemoryList(RulesList* Header)
{
	RulesList*		t = Header;

	while ( Header != NULL )
	{
		if (Header->r != NULL)
		{
			/* free (char*) malloc */
			/* printf("freeing : %s ( %d ) [ %i ]\n", Header->r->RulesRef, Header->r->start_time, Header->r->has_run ); */
			free(Header->r->RulesRef);
		}

		t = Header->next;

		/* struct item */
		if (Header != NULL )
		{
			free(Header);
			Header = t;
		}
	}

	return(0);
}

/*
* Dump to Screen
*/
int DisplayRulesList(RulesList* Header, long Interval)
{
	while ( Header != NULL)
	{
		if ( Header->r != NULL )
			printf("Ref: %s ( %i ) [ %i ] -> %i \n", Header->r->RulesRef,
			                                  Header->r->start_time,
											  Header->r->has_run,
											  RulesFrequency(Header->r, Interval) );
		Header = Header->next;
	}

	return(0);
}

/*
* Dump to Screen - single line ( debug )
*/
int DisplaySimpleRulesList(RulesList* Header)
{
	printf("_rules__\n");
	while ( Header != NULL)
	{
		if ( Header->r != NULL )
			printf("%s [%i],",     Header->r->RulesRef, Header->r->has_run );
		Header = Header->next;
	}
	printf("-\n");
	return(0);
}


/*************************************************************************************
* PURPOSE		: * Restore Rules From File
* File format	: Dump format : "%s / %d / %i \n"
*
*				: (%s) Rules Ref
*				: (%d) Number of second since C ANSI T0 (1970)
*				: (%i) Number of time (int)
*
* INPUT			: initialized RulesList and valid header
* OUTPUT		: 0 if no error
*				: 1 if error
***************************************************************************************/
int RulesMemoryRestoreFromFile(RulesList* L, char* filename, RulesList** header )
{
	FILE*			f;
	RulesList*		r1;
	char			file_ref[50];
	time_t			file_start_time;
	int				file_has_run;

	/* open a new file */
	if ( (f = fopen(filename,"r")) == NULL )
		return(1);

	while ( fscanf(f,"%s / %d / %i ", file_ref, &file_start_time, &file_has_run) != EOF )
	{
		/* insert */
		r1 = RulesInsertInList(L, file_ref, header);
		if ( r1 != NULL )
		{
			/* reset values */
			r1->r->has_run		= file_has_run;
			r1->r->start_time	= file_start_time;

			/* new header */
			L = *header;
		}
	}

	/* normal end */
	fclose(f);
	return(0);

}

/****************************************************************************
* Function based on rulesmemory
* Function	: MODULO, 2 argt (double, double)
*			: Argt1 : ( Number of time the rules has run, argt2 : X )
*
*/
int rModulo(RulesList* L, char* sRules, double x, double* result)
{
	/* search */
	RulesList*	rItem;
	*result		= -1; /* init with a fake value */

	/* skip if modulo is not what we expect */
	if ( x <= 0 )
		return(1);

	/* search for the rules by key */
	if ( ( rItem = GetRulesFromMemoryList(L, sRules) ) == NULL )
	{
		*result = 0;	
		return(0);
	}

	/* normal end */
	*result = fmod(rItem->r->has_run, x);
	return(0);
}

/******************************************
* Return Number of time a rule has run
* or ZERO
*/
double GetRulesTotal(RulesList* L, char* sRules )
{
	RulesList*		rItem;

	/* search for the rules by key */
	rItem = GetRulesFromMemoryList(L, sRules);

	/* Got the rules ? */
	if ( rItem != NULL )
		return(rItem->r->has_run);

	return(0);
}



/******************************************************************************
* Function	: EvalRules
* INPUT		: string ( cmd )
* OUTPUT	: 1 for ERROR, O for NOERROR
* I/O		: results ( double ) the result or 1/O for TRUE,FALSE
* TEST		: cmd, Syntax/examples
* -------------------------------------------------------------------------------
*	where R is the Rules Reference
*	-- OPERATION --
*	M, MOD	: Modula(R x) with x > 0
*	F, FRQ	: Frequency(R x) x is a nb of secondes
*	T, TOT	: Total(R)
*    -- FUNCTION --
*	GET : return the result of the operation
*	IS : GET + TEST
*
*	------ Mod / Modulo ------------------------------------------------------------
*    	is Mod ru1 10 = 3 (returns 1 for TRUE or 0 for FALSE)
*		get Mod ru1 10    ( returns the result :Mod(ru1->has_run, 10) )
*
*	------ Frq / Frequency ---------------------------------------------------------
*		is Frq ru1 60 = 10	( returns 1 if the frequency of ru1 is 10 per minute)
*		get Frq rul 60		( returns the frequency of ru1 per minute)
*
*	------ Tot / Total --------------------------------------------------------------
*		is Tot ru1 > 5		( returns 1 for TRUE if r->has_run > 5)
*		get Tot ru1			( returns r->has_run)
*
*************************************************************************************/
int evalrules(RulesList* L, char* cmd, double* result)
{

	char	f_KW[10];
	char	f_cmd[10];
	char	f_rulesref[10];
	char	f_operator[10];
	double	f_val = 0;
	double	f_testvalue = 0;

	/* temporary result */
	double	result1 = 0;

	/* parse */
	UpperCase(cmd);

	if ( ParseCmdEvalRules(cmd, f_KW, f_cmd, f_rulesref, &f_val, f_operator, &f_testvalue) )
		return(1);

	switch ( f_cmd[0] )
	{
		case 'M':
			if ( rModulo(L, f_rulesref, f_val, &result1) )
				return(1);
		break;

		case 'F':
			result1 =  GetRulesFrequency(L, f_rulesref, f_val);
		break;

		case 'T':
			result1 = GetRulesTotal( L, f_rulesref );
		break;
	}

	/* KW is GET or IS or nothing, then consider KW as GET */
	if ( strcmp( f_KW, "IS") == 0 )
	{
		switch ( f_operator[0] )
		{
			case '=':
				*result =  ( f_testvalue == result1 ? 1 : 0 );
			break;

			case '>':
				if ( f_operator[1] != '=' )
					*result =  ( result1 > f_testvalue ? 1 : 0 );
				else
					*result =  ( result1 >= f_testvalue ? 1 : 0 );
			break;

			case '<':
				if ( f_operator[1] != '=' )
					*result =  ( result1 < f_testvalue ? 1 : 0 );
				else
				{
					if ( f_operator[1] != '>' )
						*result =  ( result1 <= f_testvalue ? 1 : 0 );
					else
						*result =  ( f_testvalue == result1 ? 0 : 1 ); /* <> */
				}
			break;

			/* default is different : <> or != */
			case '!':
				*result =  ( f_testvalue == result1 ? 0 : 1 );
				break;
		}
	}
	else
		*result = result1;

	/* normal end */
	return(0);
}



/************************************************************
* Extract token from evarules string
* INPUT			:
* cmd			: full string
*				:
* OUTPUT		:
* f_KW			: word1 (IS,GET,SET, ...)
* f_cmd			: word2 ( TOT, FRQ, MOD, ...)
* f_rulesref	: word3 ( variable - rules reference )
* f_val			: word4 ( double, value )
* f_operator	: word5 (=,!=,>,>...)
* f_testvalue	: word6 ( double, value)
*				:
* ON ERROR		: return(1)
*************************************************/
int ParseCmdEvalRules(char* cmd, char* f_KW, char* f_cmd, char* f_rulesref, double* f_val, char* f_operator, double* f_testvalue)
{
	int		tok	= 0;
	char*	token;
	char	f_tmp[11];
	int		tok_inc = 0;

	/* token ( blank ) */
	token = strtok(cmd, " ");
	if ( token == NULL)
		return(1);

	while ( token )
	{
		switch ( tok )
		{

			case 0:
				strcpy( f_KW, token );
			break;

			case 1:
				strcpy( f_cmd, token );
				tok_inc = isCmdEvalRulesWithNoArgt( f_cmd );
			break;

			case 2:
				strcpy( f_rulesref, token );
				tok = tok + tok_inc;		/* used to skip if the function have no argument */
			break;

			case 3:
				strcpy( f_tmp, token );
				if (strlen(f_tmp))
					*f_val = atof(f_tmp);
			break;

			case 4:
				strcpy( f_operator, token );
			break;

			case 5:
				strcpy( f_tmp, token );
				if (strlen(f_tmp))
					*f_testvalue = atof(f_tmp);
			break;
		}

		/* next item */
		tok++;
		token = strtok(NULL, " ");
	}
	return(0);
}

/****************************************************************************
* isCmdEvalRulesWithNoArgt
* PURPOSE	: Return 1 if the function gets no argument
* so the returned value will increment the token position in order to skip
* the following (missing) argument
* OUPUT		: 1 or 0, error ( 0 ).
*******************************************************************************/
int isCmdEvalRulesWithNoArgt( char * f_cmd )
{
	switch ( f_cmd[0] )
	{
		case 'M':
			return(0);
		break;

		case 'F':
			return(0);
		break;

		case 'T':
			return(1);
		break;
	}

	/* default */
	return(0);

} /* end of isCmdEvalRulesWithNoArgt */



#ifdef TEST

/*-- readLine() ------------------------------------------------------------*/

/* Read str of letters up to newline or EOF and return ptr to newly
 * malloc-ed memory containing it.
 */
static uchar *readLine(FILE *handle) {
  static uchar *pBuf = NULL; /* Buffer area preserved between calls. */
  static ulong bufSize = 0;

  int ch;
  uchar *str;
  ulong len;


  for (len = 0; ; ++len) {
    if (len == bufSize) { /* Need (more) space for buffer. */
      uchar *pBuf2;

      /* NB: Allocate first time, otherwise double size. */
      bufSize = bufSize ? bufSize<<1 : START_BUF_SIZE;
      pBuf2 = (uchar *)realloc(pBuf, bufSize*sizeof(uchar));
      if (!pBuf2) {
	fprintf(stderr, "Error: Out of memory in readLine().\n");
	exit(MEMORY_ERROR);
      } /* end if */
      pBuf = pBuf2;
    } /* end if */

    ch = getc(handle);
    if (ch == EOF && ferror(handle)) {
      fprintf(stderr, "Error: getc() failed in readLine().\n");
      exit(FILE_ERROR);
    } else if (ch == EOF) break;
    else if (ch == '\n') break;
    else pBuf[len] = ch;
  } /* end for */


  /* TO DO: Reduce code duplication here. */
  if (len == bufSize) { /* Need (more) space for buffer. */
    uchar *pBuf2;

    /* NB: Allocate first time, otherwise double size. */
    bufSize = bufSize ? bufSize<<1 : START_BUF_SIZE;
    pBuf2 = (uchar *)realloc(pBuf, bufSize*sizeof(uchar));
    if (!pBuf2) {
      fprintf(stderr, "Error: Out of memory in readLine().\n");
      exit(MEMORY_ERROR);
    } /* end if */
    pBuf = pBuf2;
  } /* end if */
  pBuf[len] = '\0';

  str = (uchar *)malloc((len+1)*sizeof(uchar)); /* Allocate just enough. */
  if (!str) {
    fprintf(stderr, "Error: Out of memory in readLine().\n");
    exit(MEMORY_ERROR);
  } /* end if */
  strcpy(str, pBuf);

  return str;
} /* end function readLine() */

#endif

