/* timeslot.h */

/* local prototypes */
int     BuildDatePattern(time_t t, char* DatePattern, int MaxSize);
int     NowMatchTimeInterval(char**TimeIntervalPatterns);
int     DateMatchTimeInterval(time_t now, char**TimeIntervalPatterns);
int     nowDateMatchTimeIntervalString(char*TimeIntervalPatterns);
time_t  makeTimeDate(int year, int month, int day, int hour);