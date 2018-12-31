/* Wildcard.h */

const char *wc_error(int value);
static int wc_match_fragment(const char **fragment, const char **target);
int wc_match(const char *wildcard, const char *target);
int wc_unescape(char *output, const char *wildcard);



