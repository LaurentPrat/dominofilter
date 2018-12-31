//
//  main.c
//  MatchTimeInterval
//
//  Created by Laurent PRAT on 07/10/2014.
//  Copyright (c) 2014 Laurent PRAT. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <string.h>

/* local */
#include "timeslot.h"




#ifdef TIMESLOTDEBUG
int main(int argc, const char * argv[]) {

    time_t  now;
/* init with pattern */
    char *TimeIntervalPatterns[] = {"000,001,002,003",                        /* jour 0 : dimanche, minuit à 3h59 = 4H */
                                    "120,121,122,123,124",                    /* jour 1 : lundi de 20h à 24h */
                                    "210,211,212,213,214,215,216,218",        /* jour 2 : mardi entre 10h et 19h, sauf de 17h à 18h */
                                    "37,38,39,310,311,312",                   /* jour 3 : mercredi de 7h à 13h(12h59) */
                                    NULL};
    
    char singleTimeIntervalPatterns[] = {"000,001,002,003,120,121,122,123,124,210,211,212,213,214,215,216,218,37,38,39,310,311,312"};
    
    
    /* Match ? */
    if ( NowMatchTimeInterval(TimeIntervalPatterns) )
        printf("Yes we have a match !!");
    else
        printf("No Match..");
    
    /* single string */
    if ( nowDateMatchTimeIntervalString(singleTimeIntervalPatterns) )
        printf("[single string] Yes we have a match !!");
    else
        printf("[single string] No Match..");

    
    /* Test with special time slot   : Y,M,D,H */
    now = makeTimeDate(2014, 10, 5, 0);
    DateMatchTimeInterval(now, TimeIntervalPatterns);
    
    
}
#endif

time_t makeTimeDate(int year, int month, int day, int hour)
{
    struct tm date;
    
    /* init */
    memset(&date, 0, sizeof(date));
    
    /* fill in */
    date.tm_year = year - 1900;
    date.tm_mon  = month -1;        /* jan = 0 */
    date.tm_mday = day;
    date.tm_hour = hour;
    date.tm_isdst = -1;
    
    return(mktime(&date));
}


int NowMatchTimeInterval(char**TimeIntervalPatterns)
{
    int         match = 0;
    int         i = 0;
    time_t      now;
    char        sDatePattern[20];
    
    now = time(NULL);
    
    /* Build string to be compared */
    if ( BuildDatePattern(now, sDatePattern, sizeof(sDatePattern)) )
        return(0);
    
    /* Loop over TimeIntervalPatterns */
    while(TimeIntervalPatterns[i]!= NULL && match == 0)
    {
        if ( strstr(TimeIntervalPatterns[i], sDatePattern) )
            match = 1;

        /* Debug */
        /* printf("matching = %s with pattern: %s => match : %d\n", sDatePattern, TimeIntervalPatterns[i], match); */
        
        i++;
    }
    
    return(match);
}


int DateMatchTimeInterval(time_t now, char**TimeIntervalPatterns)
{
    int         match = 0;
    int         i = 0;
    char        sDatePattern[20];
    
    /* Build string to be compared */
    if ( BuildDatePattern(now, sDatePattern, sizeof(sDatePattern)) )
        return(0);
    
    /* Loop over TimeIntervalPatterns */
    while(TimeIntervalPatterns[i]!= NULL && match == 0)
    {
        if ( strstr(TimeIntervalPatterns[i], sDatePattern) )
            match = 1;

        /* debug */
        printf("matching = %s with pattern: %s => match : %d\n", sDatePattern, TimeIntervalPatterns[i], match);
        
        i++;
    }
    
    return(match);
}


int nowDateMatchTimeIntervalString(char*TimeIntervalPatterns)
{
    int         match = 0;
    char        sDatePattern[20];
    time_t      now;
    
    
    now = time(NULL);
    
    /* Build string to be compared */
    if ( BuildDatePattern(now, sDatePattern, sizeof(sDatePattern)) )
        return(0);
    
    /* look for substring (WeekDay+Hours) into a signgle global list with all the TimeSlot */
    if ( strstr(TimeIntervalPatterns, sDatePattern) )
        match = 1;
        
    /* debug */
    /* printf("[single string] matching = %s with pattern: %s => match : %d\n", sDatePattern, TimeIntervalPatterns, match); */
        
    
    return(match);
}


int BuildDatePattern(time_t t, char* DatePattern, int MaxSize)
{
    struct tm   *tmp;
    
    tmp = localtime(&t);
    if (tmp == NULL)
        return(1);
    
    /* debug */
    strftime(DatePattern, MaxSize, "[%d/%m/%y]", tmp);
    /* printf("\nDate : %s\n", DatePattern); */
    
    /* Build string to be compared */
    if ( strftime(DatePattern, MaxSize, "%w%H", tmp) == 0 )
        return(1);
    
    return(0);
}




