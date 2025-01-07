#define SUBCOM_DELETE -1
#define SUBCOM_QUERY   0
#define SUBCOM_SET     1

#define CMD_HISTORY_LEN 1024
#define CMD_HISTORY_DUPE_CHECK 32

#define LINES_LEN 255

#define PGMB_size 32768; /* 2**15 2024-12-29 */   /* 16384; 2**14 */

/* max. length of the command line text the user can enter */
#define CMDLINELENGTH 255     /* moved from EESCRN.H to EE_FIRST.H */

#define PFSCOPE_AUTO   0
#define PFSCOPE_PERM   1
#define PFSCOPE_GLOBAL 2
#define PFSCOPE_SCREEN 3
#define PFSCOPE_FILE   4
#define PFSCOPE_VIEW   5

#define PFMODE_CLEAR   0
#define PFMODE_BEFORE  1
#define PFMODE_AFTER   2
#define PFMODE_ONLY    3
#define PFMODE_IGNORE  4
#define PFMODE_BOTH    5
#define PFMODE_TWICE   6

#define PF_PHASE_BEFORE  1
#define PF_PHASE_COMMAND 2
#define PF_PHASE_AFTER   3

#define _rc_success    0
#define _rc_limit      1
#define _rc_tof_eof    1
#define _rc_error      2
#define _rc_not_found  2
#define _rc_unspecific 3
#define _rc_invalid    5
#define _rc_failure   -1
#define _rc_unknown   -3
#define _rc_NYI       -5 /* not yet implemented */
#define _rc_ABORT      7777 /* ABORT subcommand */

