

/* the structure holding blockmode operation data between screen roundtrips */

typedef struct _blockops {
  EditorPtr srcEd;
  LinePtr blockPos1;
  LinePtr blockPos2;
  short blockEndsAvail; /* will be 2 if single line ops M / C were given */
  char op; /* m , c , M , C , D , " , < , > , / (with: / for no op) */
  int shiftBy;
  int shiftMode;
} BlockOps, *BlockOpsPtr;


typedef struct t_PGMB {
  unsigned long GPR_SUBCOM[16] ;
  unsigned long cmscrab        ;
  unsigned long sc_block       ;

  /* EEMAIN.C */
  ScreenPtr scr                ;
  char headline[LINES_LEN + 1] ;
  char footline[LINES_LEN + 1] ;
/*
  char infoline0[LINES_LEN + 1];
  char infoline1[LINES_LEN + 1];
  char infoline2[LINES_LEN + 1];
  char infoline3[LINES_LEN + 1];
*/
 /** / char infolines[INFOLINES_MAX][LINES_LEN + 1] ; / **/    /* do not use on global level */

  char identify[LINES_LEN + 1] ;
  char *progName               ;

  /* EECMDS.C */
  EditorPtr commandHistory     ;
  EditorPtr filetypeDefaults   ;
  EditorPtr filetypeTabs       ;
  EditorPtr macroLibrary       ;
  int pfMode[25]               ;
  char pfCmds[25][CMDLINELENGTH+1];
  int fileCount                ;
  char searchPattern[CMDLINELENGTH + 1];
  bool searchUp                ;
  /* debugging SUBCOM */
  char *saveMsgPtr             ;
  ScreenPtr saveScreenPtr      ;
  /* t_PGMB *savePGMB_loc; --- REMOVED */
  long versionCount            ;

  /* EELIST.C  */
  char *HEAD_PATTERN_FSLIST    ;
  char *HEAD_PATTERN_SHOWF     ;
  char FOOT_FSLIST[90]         ;
  char FOOT_SHOWF[90]          ;
  ScreenPtr fslistScreen       ;
  ScreenPtr browseScreen       ;

  bool fslistPrefixOn          ;

  char listPfCmds[25][CMDLINELENGTH+1]; /* the FSLIST PF key commands */
  char viewPfCmds[25][CMDLINELENGTH+1]; /* the FSVIEW PF key commands */

  bool fslisterSearchUp;
  char fslisterSearchBuffer[CMDLINELENGTH + 1];

  bool browserSearchUp;
  char browserSearchBuffer[CMDLINELENGTH + 1] ;

  SortItem sortSpecs[12]; /* max 9 columns + TS + FORMAT + last */
  int sortSpecCount ;


  /* EEHELP.C  */
  char *headTemplate           ; /* [sic!] */
  char* ExtraAllowed           ; /* [sic!] */

  /* EEPREFIX.C  */
  char *SingleCharPrefixes                    ;
  BlockOps blockOpsData                       ;
  BlockOpsPtr blockOps                        ;

  /* EECORE.C  */
  char *emergencyMessage                      ;

  /* EESCRN.C  */
  char termName[TERM_NAME_LENGTH + 1] ;
  int numAltRows ;
  int numAltCols ;
  bool canAltScreenSize ;
  bool canExtHighLight  ;
  bool canColors ;
  int sessionId ;
  int sessionMode ;

  unsigned int rows ;
  unsigned int cols ;

  unsigned int lastRow ;
  unsigned int lastCol ;

  char colorsFor3270[16] ;
  char *cmdArrow         ;
  char *topOfFileText    ;
  char *bottomOfFileText ;
  char *prefixLocked     ;

  /* new stuff */
  struct _publicView global_view;

} t_PGMB;



