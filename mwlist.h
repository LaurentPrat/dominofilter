/***********************************************************************
* List Processing 
* Version 1.0
***********************************************************************/

/* List of ... */
        typedef char* ElementType;

/* Prototypes */
        #ifndef _List_H
        #define _List_H

        struct Node;
        typedef struct Node *PtrToNode;
        typedef PtrToNode List;
        typedef PtrToNode Position;

        List MakeEmpty( List L );
        int IsEmpty( List L );
        int IsLast( Position P, List L );
        Position Find( ElementType X, List L );
        void Delete( ElementType X, List L );
        Position FindPrevious( ElementType X, List L );
        int Insert( ElementType X, List L, Position P );
        void DeleteList( List L );
        Position Header( List L );
        Position First( List L );
        Position Advance( Position P );
        ElementType Retrieve( Position P );

		/* Extra */
		void PrintList( const List L );
		ElementType Retrieve_Value( Position P );


        #endif    /* _List_H */

