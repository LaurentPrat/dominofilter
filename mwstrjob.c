#include <string.h>
#include <stdlib.h>

#include "mwstrjob.h"


/*****************************************************************
* convert UpperCase
******************************************************************/
void UpperCase(char *n)
{
	while (*n)
	{
	  *n = toupper(*n);
	  n++;
	}
}
