/* rulesmem.h */

/* Const */
#define NB_SECOND_BY_HOUR		3600
#define NB_SECOND_BY_MIN		60
#define	NB_SECOND_BY_DAY		86400

#define RULES_MEMORY_DEFAULT_FILE	"dumprulesmemory.txt"

#ifdef TEST
	#define START_BUF_SIZE			100
	typedef unsigned char uchar;
	typedef unsigned long ulong;
	typedef enum { NO_ERROR, SYNTAX_ERROR, FILE_ERROR, MEMORY_ERROR } ErrType;
#endif


typedef struct
{
	char*				RulesRef;
	time_t				start_time;
	time_t				last_time;
	long				has_run;
} RulesMemory;


/* Link list */
struct RulesItem
{
	RulesMemory*		r;
	struct RulesItem*	next;
	struct RulesItem*	prev;
} RulesItem;

typedef struct RulesItem RulesList;



/* prototypes */
RulesMemory* 	MakeEmptyRules(void);
int				initrules(char* rulesref, RulesMemory* rules1 );
RulesList*		MakeEmptyList(void);
int 			DestroyRulesMemoryList(RulesList* Header);
RulesList*		MakeEmptyRulesList();
RulesList*		RulesMemoryInList(RulesList* L, char* sRules);
RulesList*		RulesInsertInList(RulesList* L, char* sRules, RulesList** head);
int				RulesMemoryDumpToFile(RulesList* L, char* filename);
int				DisplayRulesList(RulesList* Header, long Interval);
int				DisplaySimpleRulesList(RulesList* Header);
int				RulesMemoryRestoreFromFile(RulesList* L, char* filename, RulesList** header);
RulesList*		GetRulesFromMemoryList(RulesList* L, char* sRules);


/* string utilities */
#ifdef TEST
static			uchar* readLine(FILE *handle);
#endif


/* Evaluator */
int				rModulo(RulesList* L, char* sRules, double x, double* result);
double			GetRulesTotal(RulesList* L, char* sRules );
double			GetRulesFrequency(RulesList* L, char* sRules, double interval, int MWBLACKLISTDEBUG );
double			RulesFrequency(RulesMemory* r, double interval, int MWBLACKLISTDEBUG );
int				evalrules(RulesList* L, char* cmd, double* result, int MWBLACKLISTDEBUG );
int				ParseCmdEvalRules(char* cmd, char* f_KW, char* f_cmd, char* f_rulesref, double* f_val, char* f_operator, double* f_testvalue);
int				isCmdEvalRulesWithNoArgt( char * f_cmd );


