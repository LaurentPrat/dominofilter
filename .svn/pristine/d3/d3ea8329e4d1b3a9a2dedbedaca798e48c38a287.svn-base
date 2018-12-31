/*************************************************************************************
* List processing
* Version 2.0
* -----------
* String processsing
*
*************************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "fatal.h"
#include "mwlist.h"

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


/* Place in the interface file */
struct Node
{
     ElementType Element;		/* key */
     Position    Next;
};

List
MakeEmpty( List L )
        {
            if( L != NULL )
                DeleteList( L );

			L = malloc( sizeof( struct Node ) );
            if( L == NULL )
				return L;
/*                FatalError( "Out of memory!" ); */

			/* element */
			L->Element = malloc (sizeof (ElementType));
			if( L->Element == NULL )
				return (NULL);
/*                FatalError( "Out of memory!" ); */

			/* init */
			L->Element = NULL;
			L->Next = NULL;

            return L;
        }


/* Return true if L is empty */

int
IsEmpty( List L )
{
            return L->Next == NULL;
}


/* Return true if P is the last position in list L */
/* Parameter L is unused in this implementation */

int IsLast( Position P, List L )
{
            return P->Next == NULL;
}


/* Return Position of X in L; NULL if not found */
Position
Find( ElementType X, List L )
{
            Position P;

/* 1*/      P = L->Next;
/* 2*/      while( P != NULL && ( strcmp(P->Element, X) ) )				/* string */
/* 3*/          P = P->Next;

/* 4*/      return P;
}


/* Delete from a list */
/* Cell pointed to by P->Next is wiped out */
/* Assume that the position is legal */
/* Assume use of a header node */

void
Delete( ElementType X, List L )
{
            Position P, TmpCell;

            P = FindPrevious( X, L );

            if( !IsLast( P, L ) )  /* Assumption of header use */
            {                      /* X is found; delete it */
                TmpCell = P->Next;
                P->Next = TmpCell->Next;  /* Bypass deleted cell */
                free( TmpCell );
            }
}


/* If X is not found, then Next field of returned value is NULL */
/* Assumes a header */

Position
FindPrevious( ElementType X, List L )
{
            Position P;

/* 1*/      P = L;
/* 2*/      while( P->Next != NULL && P->Next->Element != X )
/* 3*/          P = P->Next;

/* 4*/      return P;
}


/* Insert (after legal position P) */
/* Header implementation assumed */
/* Parameter L is unused in this implementation */

int
Insert( ElementType X, List L, Position P )
{
	Position TmpCell;

/* 1*/      TmpCell = malloc( sizeof( struct Node ) );
/* 2*/      if( TmpCell == NULL )
/* 3*/          return(1);		/* out of memory */

			TmpCell->Element = malloc (strlen( X ) + 1);
			if (TmpCell->Element == NULL)
				return(1);	    /* out of memory */
			else
				strcpy(TmpCell->Element, X);

/* 5*/      TmpCell->Next = P->Next;
/* 6*/      P->Next = TmpCell;

			/* normal end */
			return(0);
}

/*  DeleteList */

void
DeleteList( List L )
{
            Position P, Tmp;

/* 1*/      P = L->Next;  /* Header assumed */
/* 2*/      L->Next = NULL;
/* 3*/      while( P != NULL )
            {
/* 4*/          Tmp = P->Next;

				free ( P->Element);
/* 5*/          free ( P );
/* 6*/          P = Tmp;
            }
}


Position
Header( List L )
{
            return L;
}

Position
First( List L )
{
            return L->Next;
}

Position
Advance( Position P )
{
            return P->Next;
}

ElementType
Retrieve( Position P )
{
            return P->Element;
}


/* Display all Item - rank -> value */
/* Rank starts at 1..n */
void PrintList( const List L )
{
	int i = 1;
    Position P = Header( L );

    if( IsEmpty( L ) )
        printf( "Empty list\n" );
    else
    {
        do
        {
            P = Advance( P );
            printf( "%d -> %s \n", i++, Retrieve( P ) );
        } while( !IsLast( P, L ) );

        printf( "\n" );
    }
}
