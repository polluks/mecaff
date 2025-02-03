/*
** EELIST.C    - MECAFF file-list and file-viewer screen
**
** This file is part of the MECAFF based fullscreen tools of MECAFF
** for VM/370 R6 "SixPack".
**
** This module implements the fullscreen file lister and file viewer screens
** including the dialog transitions between these screens and the EE main
** screen.
**
**
** This software is provided "as is" in the hope that it will be useful, with
** no promise, commitment or even warranty (explicit or implicit) to be
** suited or usable for any particular purpose.
** Using this software is at your own risk!
**
** Written by Dr. Hans-Walter Latz, Berlin (Germany), 2011,2012,2013
** Released to the public domain.
*/

#include "glblpre.h"

#include <stdio.h>
#include <cmssys.h>
#include <string.h>

#include "errhndlg.h"

#include "ee_first.h"
#include "eecore.h"
#include "eeutil.h"
#include "eescrn.h"
#include "eemain.h"
#include "fsio.h"
#include "ee_pgm.h"   /* Process Global Memory */

#include "glblpost.h"

/*
static char *HEAD_PATTERN_FSLIST
    = "%s: %s %s %s\t\tLines %d-%d/%d  %s " VERSION;
static char *HEAD_PATTERN_SHOWF
    = "FSVIEW: %s %s %s\t\tLines %d-%d/%d %c%d[%d-%d]  FSVIEW " VERSION;



static char FOOT_FSLIST[90];
static char FOOT_SHOWF[90];

static ScreenPtr PGMB_loc->fslistScreen = NULL;
static ScreenPtr PGMB_loc->browseScreen = NULL;

static bool fslistPrefixOn = false;

static char listPfCmds[25][CMDLINELENGTH+1]; / * the FSLIST PF key commands * /
static char viewPfCmds[25][CMDLINELENGTH+1]; / * the FSVIEW PF key commands * /

static bool fslisterSearchUp;
static char fslisterSearchBuffer[CMDLINELENGTH + 1];

static bool browserSearchUp;
static char browserSearchBuffer[CMDLINELENGTH + 1];

static SortItem PGMB_loc->sortSpecs[12]; / * max 9 columns + TS + FORMAT + last * /
int PGMB_loc->sortSpecCount = 0;
*/

void setFSLInfoLine(char *infoLine) {
  t_PGMB *PGMB_loc = CMSGetPG();
  memset(PGMB_loc->FOOT_FSLIST, '\0', sizeof(PGMB_loc->FOOT_FSLIST));
  if (!infoLine || !*infoLine) { infoLine = " "; }
  int len = minInt(strlen(infoLine), sizeof(PGMB_loc->FOOT_FSLIST)-1);
  if (len > 77) {
    memcpy(PGMB_loc->FOOT_FSLIST, infoLine, len);
  } else {
    sprintf(PGMB_loc->FOOT_FSLIST, "\t%s\t", infoLine);
  }
}

void setFSVInfoLine(char *infoLine) {
  t_PGMB *PGMB_loc = CMSGetPG();
  memset(PGMB_loc->FOOT_SHOWF, '\0', sizeof(PGMB_loc->FOOT_SHOWF));
  if (!infoLine || !*infoLine) { infoLine = " "; }
  int len = minInt(strlen(infoLine), sizeof(PGMB_loc->FOOT_SHOWF)-1);
  if (len > 77) {
    memcpy(PGMB_loc->FOOT_SHOWF, infoLine, len);
  } else {
    sprintf(PGMB_loc->FOOT_SHOWF, "\t%s\t", infoLine);
  }
}

void setFSLPFKey(int key, char *cmd) {
  t_PGMB *PGMB_loc = CMSGetPG();
  if (key < 1 || key > 24) { return; }
  memset(PGMB_loc->listPfCmds[key], '\0', CMDLINELENGTH + 1);
  if (cmd && *cmd) {
    int len = minInt(strlen(cmd), CMDLINELENGTH);
    memcpy(PGMB_loc->listPfCmds[key], cmd, len);
  }
}

void setFSVPFKey(int key, char *cmd) {
  t_PGMB *PGMB_loc = CMSGetPG();
  if (key < 1 || key > 24) { return; }
  memset(PGMB_loc->viewPfCmds[key], '\0', CMDLINELENGTH + 1);
  if (cmd && *cmd) {
    int len = minInt(strlen(cmd), CMDLINELENGTH);
    memcpy(PGMB_loc->viewPfCmds[key], cmd, len);
  }
}

void setFSLPrefix(bool on) {
  t_PGMB *PGMB_loc = CMSGetPG();
  PGMB_loc->fslistPrefixOn = on;
  if (!PGMB_loc->fslistScreen) { return; }
/* temporarily disabled */
/*
  if (on) {
    PGMB_loc->fslistScreen->prefixMode = 1;
    PGMB_loc->fslistScreen->prefixChar = ' ';
    PGMB_loc->fslistScreen->prefixLen = 1;
  } else {
    PGMB_loc->fslistScreen->prefixMode = 0;
  }
*/
}

void initFSPFKeys() {
  setFSLPFKey(1, "CENTER");
  setFSLPFKey(2, "EE");
  setFSLPFKey(3, "QUIT");
  setFSLPFKey(4, "/");
  setFSLPFKey(5, "TOP");
  setFSLPFKey(6, "PGUP");
  setFSLPFKey(7, "PGUP SHORT");
  setFSLPFKey(8, "PGDOWN SHORT");
  setFSLPFKey(9, "PGDOWN");
  setFSLPFKey(10, "BOTTOM");
  setFSLPFKey(11, "MARK");
  setFSLPFKey(12, "FSVIEW");
  setFSLPFKey(13, NULL);
  setFSLPFKey(14, NULL);
  setFSLPFKey(15, "QQUIT");
  setFSLPFKey(16, "-/");
  setFSLPFKey(17, NULL);
  setFSLPFKey(18, NULL);
  setFSLPFKey(19, NULL);
  setFSLPFKey(20, NULL);
  setFSLPFKey(21, NULL);
  setFSLPFKey(22, NULL);
  setFSLPFKey(23, NULL);
  setFSLPFKey(24, NULL);
  setFSLInfoLine(
    "02=EE 03=Quit 04=Srch "
    "05=Top 06=PgUp 07=Up 08=Down 09=PgDown 10=Bot "
    "12=View");

  setFSVPFKey(1, "CENTER");
  setFSVPFKey(2, "EE");
  setFSVPFKey(3, "QUIT");
  setFSVPFKey(4, "/");
  setFSVPFKey(5, "TOP");
  setFSVPFKey(6, "PGUP");
  setFSVPFKey(7, "PGUP SHORT");
  setFSVPFKey(8, "PGDOWN SHORT");
  setFSVPFKey(9, "PGDOWN");
  setFSVPFKey(10, "BOTTOM");
  setFSVPFKey(11, "LEFT");
  setFSVPFKey(12, "RIGHT");
  setFSVPFKey(13, NULL);
  setFSVPFKey(14, NULL);
  setFSVPFKey(15, "QUIT");
  setFSVPFKey(16, "-/");
  setFSVPFKey(17, "");
  setFSVPFKey(18, "");
  setFSVPFKey(19, "");
  setFSVPFKey(20, "");
  setFSVPFKey(21, "");
  setFSVPFKey(22, "");
  setFSVPFKey(23, "LEFT SHORT");
  setFSVPFKey(24, "RIGHT SHORT");
  setFSVInfoLine(
    "02=EE 03=Quit 04=Srch "
    "05=Top 06=PgUp 07=Up 08=Dwn 09=PgDwn 10=Bot 11=SL "
    "12=SR");
}

static ScreenPtr initScreen(ScreenPtr tmpl, char *msg) {
  ScreenPtr scr = allocateScreen(msg);
  if (scr == NULL) { return NULL; }

  scr->attrFilearea = tmpl->attrFilearea;
  scr->attrCmd = tmpl->attrCmd;
  scr->attrArrow = tmpl->attrArrow;
  scr->attrMsg = tmpl->attrMsg;
  scr->attrHeadLine = tmpl->attrHeadLine;
  scr->attrFootLine = tmpl->attrFootLine;
  scr->attrSelectedLine = tmpl->attrCurLine;

  scr->attrCurLine = scr->attrFilearea;
  scr->readOnly = true;
  scr->wrapOverflow = false;
  scr->yyy_cmdLinePos = 1; /* at bottom */
  scr->msgLinePos = 1; /* at bottom */
  /* scr->prefixMode = 0; */ /* off */
  scr->yyy_currLinePos = 0; /* first avail line */
  scr->yyy_scaleLinePos = 0; /* off */
  scr->yyy_showTofBof = false;
  scr->infoLinesPos = -1; /* top */
  scr->attrInfoLines = scr->attrHeadLine;

  return scr;
}

static bool isShortParam(char *cmd, char *msg) {
  char *params = getCmdParam(cmd);
  if (!params || !*params) { return false; }
  if (!isAbbrev(params, "SHORT")) {
    strcpy(msg, "Invalid parameter given");
    return false;
  }
  params = getCmdParam(params);
  if (params) {
    while(*params) {
      if (*params != ' ') {
        strcpy(msg, "Extra parameters ignored");
        return true;
      }
      params++;
    }
  }
  return true;
}

void initFSList(ScreenPtr tmpl, char *msg) {
  t_PGMB *PGMB_loc = CMSGetPG();
  if (PGMB_loc->fslistScreen) { freeScreen(PGMB_loc->fslistScreen); PGMB_loc->fslistScreen = NULL; }
  if (PGMB_loc->browseScreen) { freeScreen(PGMB_loc->browseScreen); PGMB_loc->browseScreen = NULL; }

  if (tmpl == NULL) { return; }

  memset(PGMB_loc->sortSpecs, '\0', sizeof(PGMB_loc->sortSpecs));

  PGMB_loc->fslisterSearchUp = false;
  PGMB_loc->browserSearchUp = false;

  memset(PGMB_loc->fslisterSearchBuffer, '\0', sizeof(PGMB_loc->fslisterSearchBuffer));
  memset(PGMB_loc->browserSearchBuffer, '\0', sizeof(PGMB_loc->browserSearchBuffer));

  PGMB_loc->fslistScreen = initScreen(tmpl, msg);
  PGMB_loc->browseScreen = initScreen(tmpl, msg);
}

/* get the cms filename from a line in the filelist list */
static void extractFilename(char *line, char *fn, char *ft, char *fm) {
  char *src = line;
  char *trg = fn;
  while(*src != ' ') { *trg++ = *src++; }
  *trg = '\0';

  src = &line[9];
  trg = ft;
  while(*src != ' ') { *trg++ = *src++; }
  *trg = '\0';

  src = &line[18];
  trg = fm;
  while(*src != ' ') { *trg++ = *src++; }
  *trg = '\0';
}

static deltaHShift(ScreenPtr scr, short by) {
  short newHSHift = scr->hShift + by;
  short lineOverhead
    = (scr->ed->view->prefixMode == 0)
    ? 1 /* field start */
    : scr->ed->view->prefixLen + 2; /* prefix + 2xfieldstart */
  scr->hShift
    = maxShort(
        0,
        minShort(
            newHSHift,
            getFileLrecl(scr->ed) + lineOverhead - scr->screenColumns));
}

typedef enum _scrollCmd {
  CENTER,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  TOP,
  BOTTOM
  } ScrollCmd;

static bool handleScrolling(ScreenPtr scr, ScrollCmd cmd, bool shortScroll) {
  t_PGMB *PGMB_loc = CMSGetPG();
  EditorPtr ed = scr->ed;
  int middleLine = scr->visibleEdLines / 2;
  int middleCol = scr->screenColumns / 2;

  if (scr->cElemType == 2) {
    if (cmd == CENTER || cmd == LEFT || cmd == RIGHT) {
      deltaHShift(
        scr,
        scr->cColAbs - middleCol);
    }
    if (cmd == CENTER || cmd == UP || cmd == DOWN) {
      if (scr->cElemLineNo > middleLine) {
        moveToLineNo(ed, scr->cElemLineNo - middleLine);
      }
    }
    scr->cursorPlacement = 2;
    scr->cursorLine = scr->cElem;
    scr->cursorOffset = scr->cElemOffset;
  } else if (cmd == CENTER) {
    LinePtr targetLine = getCurrentLine(ed);
    LinePtr nextLine = getNextLine(ed, targetLine);
    int lineOffset = 0;
    while (lineOffset < middleLine && nextLine != NULL) {
      targetLine = nextLine;
      lineOffset++;
      nextLine = getNextLine(ed, targetLine);
    }
    scr->cursorPlacement = 2;
    scr->cursorLine = targetLine;
    scr->cursorOffset = scr->hShift + middleCol;
  } else if (cmd == TOP) {
    moveToBOF(ed);
  } else if (cmd == UP && !shortScroll) {
    moveUp(ed, scr->visibleEdLines - 1);
  } else if (cmd == UP) {
    moveUp(ed, (scr->visibleEdLines * 2) / 3);
  } else if (cmd == DOWN && !shortScroll) {
    moveDown(ed, scr->visibleEdLines - 1);
  } else if (cmd == DOWN) {
    moveDown(ed, (scr->visibleEdLines * 2) / 3);
  } else if (cmd == BOTTOM) {
    moveToLastLine(ed);
  } else if (cmd == LEFT && !shortScroll) {
    deltaHShift(PGMB_loc->browseScreen, -20);
  } else if (cmd == RIGHT && !shortScroll) {
    deltaHShift(PGMB_loc->browseScreen, 20);
  } else if (cmd == LEFT) {
    deltaHShift(PGMB_loc->browseScreen, -10);
  } else if (RIGHT == RIGHT) {
    deltaHShift(PGMB_loc->browseScreen, 10);
  } else {
    return false;
  }

  unsigned int lineCount;
  unsigned int currLine;
  getLineInfo(scr->ed, &lineCount, &currLine);
  if (lineCount < (currLine + scr->visibleEdLines - 1)) {
    moveToLineNo(scr->ed, maxInt(1, (int)lineCount - scr->visibleEdLines + 1));
  } else if (currLine == 0) {
    moveToLineNo(scr->ed, 1);
  }

  return true;
}

static void loadSingleFile(char *line, void *cbdata) {
  EditorPtr ed = (EditorPtr)cbdata;
  insertLine(ed, line);
}

/* load a new file list and return a new editor */
static EditorPtr loadList(char *fn, char *ft, char *fm, int *rc, char *msg) {
  t_PGMB *PGMB_loc = CMSGetPG();
  EditorPtr ed = createEditor(NULL, 72, 'V');
  if (!ed) { return NULL; }
  setWorkLrecl(ed, 71);
  char *m = NULL;
  _try {
  getFileList(&loadSingleFile, ed, fn, ft, fm);
  } _catchall() {
    m = getLastEmergencyMessage();
    if (!m || !strlen(m)) {
      m = "Unable to load file list (OUT OF MEMORY?)";
    }
  } _endtry;
  if (m) {
    freeEditor(ed);
    strcpy(msg, m);
    msg[0] = '\0';
    strcat(msg, "**\n** ");
    strcat(msg, m);
    strcat(msg, "\n**\n** ");
    *rc = 4;
    return NULL;
  }
  if (getLineCount(ed) == 0) {
    sprintf(msg, "File or pattern not found: %s %s %s", fn, ft, fm);
    freeEditor(ed);
    *rc = 24;
    return NULL;
  }

  *msg = '\0';
  if (PGMB_loc->sortSpecCount > 0) {
     sort(ed, PGMB_loc->sortSpecs);
  }
  moveToLineNo(ed, 1);
  return ed;
}

static void doFind(EditorPtr ed, bool upwards, char *pattern, char *msg) {
  LinePtr oldCurrentLine = getCurrentLine(ed);
  if (!findString(ed, pattern, upwards, NULL)) {
    sprintf(msg,
        (upwards)
           ? "Pattern \"%s\" not found (upwards)"
           : "Pattern \"%s\" not found (downwards)",
        pattern);
    moveToLine(ed, oldCurrentLine);
  }
}

/* load a file into a new editor and display/interact in 'PGMB_loc->browseScreen' */
int doBrowse(char *fn, char *ft, char *fm, char *msg) {
  t_PGMB *PGMB_loc = CMSGetPG();
  if (!PGMB_loc->browseScreen) { return -1; }

  int rc;
  EditorPtr fEd = createEditorForFile(NULL, fn, ft, fm, 80, 'V', &rc, msg);
  if (!fEd || rc != 0) {
    if (fEd != NULL) { freeEditor(fEd); }
    if (rc == 1) {
      /* for FSVIEW : new file => file not found */
      sprintf(msg, "File not found: %s %s %s", fn, ft, fm);
      rc = 28;
    }
    return rc;
  }
  moveToLineNo(fEd, 1);

  PGMB_loc->browseScreen->ed = fEd;
  PGMB_loc->browseScreen->hShift = 0;
  PGMB_loc->browseScreen->cElemType = 0;
  PGMB_loc->browseScreen->cElemOffset = 0;

  char headline[80];
  PGMB_loc->browseScreen->headLine = headline;

  PGMB_loc->browseScreen->footLine = PGMB_loc->FOOT_SHOWF;

  PGMB_loc->browseScreen->aidCode = Aid_NoAID;
  PGMB_loc->browseScreen->cmdLinePrefill = NULL;
  while (rc == 0 && PGMB_loc->browseScreen->aidCode != Aid_PF03) {
    PGMB_loc->browseScreen->cursorPlacement = 0;
    PGMB_loc->browseScreen->cursorOffset = 0;
    PGMB_loc->browseScreen->msgText = msg;
    *msg = '\0';

    char *cmd = NULL;
    int aidIdx = aidPfIndex(PGMB_loc->browseScreen->aidCode);
    if (aidIdx == 0 && *PGMB_loc->browseScreen->cmdLine) {
      cmd = PGMB_loc->browseScreen->cmdLine;
    } else if (aidIdx > 0 && aidIdx < 25) {
      cmd = PGMB_loc->viewPfCmds[aidIdx];
    }

    if (cmd && *cmd) {
      if (isAbbrev(cmd, "Quit") || isAbbrev(cmd, "RETurn")) {
        break;
      } else if (isAbbrev(cmd, "Help")) {
        doHelp("FSVIEW", msg);
      } else if (cmd[0] == '/' && cmd[1] == '\0') {
        if (*PGMB_loc->browserSearchBuffer) {
          doFind(fEd, PGMB_loc->browserSearchUp, PGMB_loc->browserSearchBuffer, msg);
        }
      } else if (cmd[0] == '-' && cmd[1] == '/' && cmd[2] == '\0') {
        PGMB_loc->browserSearchUp = !PGMB_loc->browserSearchUp;
        if (*PGMB_loc->browserSearchBuffer) {
          doFind(fEd, PGMB_loc->browserSearchUp, PGMB_loc->browserSearchBuffer, msg);
        }
      } else if (cmd[0] == '/' || (cmd[0] == '-' && cmd[1] == '/')) {
        int val;
        char *param = cmd;
        int locType = parseLocation(&param, &val, PGMB_loc->browserSearchBuffer);
        if (locType == LOC_PATTERN) {
          PGMB_loc->browserSearchUp = false;
          doFind(fEd, PGMB_loc->browserSearchUp, PGMB_loc->browserSearchBuffer, msg);
        } else if (locType == LOC_PATTERNUP) {
          PGMB_loc->browserSearchUp = true;
          doFind(fEd, PGMB_loc->browserSearchUp, PGMB_loc->browserSearchBuffer, msg);
        } else {
          sprintf(msg, "No valid locate command");
        }
      } else if (isAbbrev(cmd, "TOp")) {
        handleScrolling(PGMB_loc->browseScreen, TOP, false);
      } else if (isAbbrev(cmd, "BOTtom")) {
        handleScrolling(PGMB_loc->browseScreen, BOTTOM, false);
      } else if (isAbbrev(cmd, "CENTer")) {
        handleScrolling(PGMB_loc->browseScreen, CENTER, false);
      } else if (isAbbrev(cmd, "LEft")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(PGMB_loc->browseScreen, LEFT, isShort);
      } else if (isAbbrev(cmd, "RIght")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(PGMB_loc->browseScreen, RIGHT, isShort);
      } else if (isAbbrev(cmd, "PGUP")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(PGMB_loc->browseScreen, UP, isShort);
      } else if (isAbbrev(cmd, "PGDOwn")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(PGMB_loc->browseScreen, DOWN, isShort);
      } else if (isAbbrev(cmd, "Ee")) {
        rc = RC_SWITCHTOEDIT;
        break;
      } else {
        sprintf(msg, "Invalid command: %s", cmd);
      }
    }

    unsigned int lineCount;
    unsigned int currLineNo;
    getLineInfo(fEd, &lineCount, &currLineNo);
    sprintf(headline, PGMB_loc->HEAD_PATTERN_SHOWF,
      fn, ft, fm,
      currLineNo,
      minInt(lineCount, currLineNo + PGMB_loc->browseScreen->screenRows - 5),
      lineCount,
      getRecfm(fEd), getFileLrecl(fEd),
      PGMB_loc->browseScreen->hShift + 1,
      minShort(
        PGMB_loc->browseScreen->hShift + PGMB_loc->browseScreen->screenColumns - 1,
        getFileLrecl(fEd))
      );
    rc = writeReadScreen(PGMB_loc->browseScreen);
  }

  *msg = '\0';
  PGMB_loc->browseScreen->ed = NULL;
  freeEditor(fEd);

  return rc;
}

static int addSortSpec(int count, bool desc, int offset, int length) {
  t_PGMB *PGMB_loc = CMSGetPG();
  int i = 0;
  while(i < count) {
    if (PGMB_loc->sortSpecs[i].offset == offset && PGMB_loc->sortSpecs[i].length == length) {
      return count; /* already to sort, but with higher priority ! */
    }
    i++;
  }
  PGMB_loc->sortSpecs[count].sortDescending = desc;
  PGMB_loc->sortSpecs[count].offset = offset;
  PGMB_loc->sortSpecs[count].length = length;
  return count + 1;
}

static bool isSortCommand(char *cmd, char *msg) {
  t_PGMB *PGMB_loc = CMSGetPG();
  if (!isAbbrev(cmd, "Sort")) { return false; }

  char *param = getCmdParam(cmd);
  if (!param || !*param) {
    strcpy(msg, "Missing parameter for sort");
    return true;
  }

  memset(PGMB_loc->sortSpecs, '\0', sizeof(PGMB_loc->sortSpecs));
  PGMB_loc->sortSpecCount = 0;

  if (isAbbrev(param, "OFf")) {
    return true;
  }

  while(param && *param) {
    bool sortDescending = false;
    if (*param == '-') {
      sortDescending = true;
      param++;
    } else if (*param == '+') {
      param++;
    }

    while(*param == ' ') { param++; }
    if (!*param) {
      if (PGMB_loc->sortSpecCount == 0) {
        strcpy(msg, "No or no valid parameter given for sort");
      }
      return true;
    }

    if (isAbbrev(param, "NAme")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 0, 8);
    } else if (isAbbrev(param, "TYpe")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 9, 8);
    } else if (isAbbrev(param, "MOde")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 18, 2);
    } else if (isAbbrev(param, "RECFm")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 22, 1);
    } else if (isAbbrev(param, "LRecl")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 24, 5);
    } else if (isAbbrev(param, "Format")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 22, 7);
    } else if (isAbbrev(param, "RECS")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 30, 6);
    } else if (isAbbrev(param, "BLocks")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 37, 6);
    } else if (isAbbrev(param, "DAte")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 45, 10);
    } else if (isAbbrev(param, "TIme")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 56, 5);
    } else if (isAbbrev(param, "TS")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 45, 16);
    } else if (isAbbrev(param, "LAbel")) {
      PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, sortDescending, 63, 6);
    } else {
      sprintf(msg, "Invalid sort parameter at: %s", param);
      return true;
    }

    param = getCmdParam(param);
  }

  return true;
}

static void diskLineCallback(char *line, void *cbdata) {
  tmpInfAppend(line);
}

static int xlistSaveActions(
    ScreenPtr scr,
    char *pfn, char *pft, char *pfm,
    char *command,
    char *exfn, char *exft, char *exfm,
    char *msg,
    int *selCount,
    bool displayOnly,
    bool collectReturncodes) {
  t_PGMB *PGMB_loc = CMSGetPG();
  EditorPtr ed = scr->ed;
  char line[256];
  char cmdline[256];
  int targetCount = 0;
  bool hadCmdParm = false;

  int i;

  *selCount = 0;

  memset(cmdline, '\0', sizeof(cmdline));
  char *s = command;
  char *t = cmdline;
  char *l = &cmdline[sizeof(cmdline)-9];
  while(*s && t < l) {
    if (*s == '/') {
      s++;
      if (*s == ' ' || *s == '\0') {
        *t++ = '&'; *t++ = '1'; *t++ = ' ';
        *t++ = '&'; *t++ = '2'; *t++ = ' ';
        *t++ = '&'; *t++ = '3';
        *t++ = *s;
        if (*s) { s++; }
        hadCmdParm = true;
      } else if (*s == 'n' || *s == 'N') {
        *t++ = '&'; *t++ = '1';
        s++;
        hadCmdParm = true;
      } else if (*s == 't' || *s == 'T') {
        *t++ = '&'; *t++ = '2';
        s++;
        hadCmdParm = true;
      } else if (*s == 'm' || *s == 'M') {
        *t++ = '&'; *t++ = '3';
        s++;
        hadCmdParm = true;
      } else {
        *t++ = '/';
        *t++ = c_upper(*s++);
      }
    } else {
      *t++ = c_upper(*s++);
    }
  }
  *t = '\0';
  if (!hadCmdParm) { strcat(t, " &1 &2 &3 "); }

  tmpInfClear();

  tmpInfAppend("&CONTROL OFF NOMSG");
  sprintf(line, "STATE %s XLISTRES %s", exfn, exfm);
  tmpInfAppend(line);
  sprintf(line, "&IF &RETCODE EQ 0 ERASE %s XLISTRES %s", exfn, exfm);
  tmpInfAppend(line);
  tmpInfAppend("*");

  for (i = 0; i < PGMB_loc->sortSpecCount; i++) {
    sprintf(line, "*#SORT %d %02d %02d",
      (PGMB_loc->sortSpecs[i].sortDescending) ? 1 : 0,
      PGMB_loc->sortSpecs[i].offset,
      PGMB_loc->sortSpecs[i].length);
    tmpInfAppend(line);
  }
  sprintf(line, "*#LIST %-8s %-8s %-2s", pfn, pft, pfm);
  tmpInfAppend(line);
  tmpInfAppend("*");

  if (collectReturncodes) {
    s_upper(command, command);
    sprintf(line,
      "EXECUTIL WRITE %s XLISTRES %s * 1 V 80 ( Results for command: %s )",
      exfn, exfm, command);
    tmpInfAppend(line);
    tmpInfAppend("&STACK LIFO");
    sprintf(line, "EXECUTIL WRITE %s XLISTRES %s * 1 V 80", exfn, exfm);
    tmpInfAppend(line);
  }

  _try {
    LinePtr lastUnselected = NULL;
    LinePtr currLine = getCurrentLine(ed);
    LinePtr f = getFirstLine(ed);
    while (f) {
      if (f->text[scr->selectionColumn] == scr->selectionMark) {
        f->text[20] = '\0';
        tmpInfAppend("*");
        sprintf(line, "&ARGS  %s", f->text);
        tmpInfAppend(line);
        tmpInfAppend(cmdline);
        if (collectReturncodes) {
          tmpInfAppend("&STACK LIFO 1 +9 +9 +5 +3 +3 +2");
          sprintf(line,
           "EXECUTIL WRITE %s XLISTRES %s * 1 V 80 "
           "( &1 &2 &3 => RC : &RETCODE ) TAB READ",
           exfn, exfm);
          tmpInfAppend(line);
        }
        targetCount++;
        if (f == currLine) {
          sprintf(line, "*#CURR %s", f->text);
          tmpInfAppend(line);
          if (lastUnselected) {
            lastUnselected->text[20] = '\0';
            sprintf(line, "*#CURR %s", lastUnselected->text);
            tmpInfAppend(line);
            lastUnselected->text[20] = ' ';
          }
        }
        f->text[20] = ' ';
      } else {
        lastUnselected = f;
        if (f == currLine) {
          f->text[20] = '\0';
          sprintf(line, "*#CURR %s", f->text);
          tmpInfAppend(line);
          f->text[20] = ' ';
        }
      }
      f = getNextLine(ed, f);
    }

    tmpInfAppend("*");
    tmpInfAppend("EMIT Press ENTER to continue and return to XLIST");
    if (!connectedtoMecaffConsole()) {
      tmpInfAppend("&READ VARS &DUMMY");
    }
    tmpInfAppend("&EXIT 0");
  } _catchall() {
    char *m = getLastEmergencyMessage();
    if (!m || !strlen(m)) {
      m = "Unable to intermediary EXEC file (OUT OF MEMORY?)";
    }
    msg[0] = '\0';
    strcat(msg, "**\n** ");
    strcat(msg, m);
    strcat(msg, "\n**\n** ");
    return -2;
  } _endtry;

  *selCount = targetCount;

  if (displayOnly && targetCount > 0) {
    char *fLine
      = "01=RUN\t03=Quit 05=Top 06=PgUp 07=Up 08=Dwn 09=PgDwn 10=Bot\t";
    tmpInfShow(scr, msg, "\tCMS command list for XLIST\t", "", NULL);
    return 0;
  } else if (targetCount > 0) {
    int wrRc = tmpInfWrite(exfn, exft, exfm, true, msg);
    if (wrRc != 0) { return wrRc; }
    return 2044;
  } else {
    strcpy(msg, "No files selected");
    return -1;
  }
}

static void rtrim(char *s) {
  while(*s) {
    if (*s == ' ') {
      *s = '\0';
      return;
    }
    s++;
  }
}

static void removeFileEntry(EditorPtr ed, char *fn, char *ft, char *fm) {
  char pattern[32];
  sprintf(pattern, "%-8s %-8s %-2s", fn, ft, fm);
  rtrim(pattern);
  moveToBOF(ed);
  if (findString(ed, pattern, false, NULL)) {
    LinePtr line = getCurrentLine(ed);
    deleteLine(ed, line);
  }
}

static EditorPtr xlistRestart(
    ScreenPtr scr,
    char *exfn, char *exft, char *exfm,
    int  *rc, char *msg, int *selCount) {

  t_PGMB *PGMB_loc = CMSGetPG();
  EditorPtr ed = NULL;
  LinePtr currentLine = NULL;
  char fid[20];
  CMSFILE cmsfile;
  CMSFILE *f = &cmsfile;
  char buffer[81];
  int selLines = 0;

  PGMB_loc->sortSpecCount = 0;
  *selCount = 0;

  if (tmpInfLoad(exfn, "XLISTRES", exfm)) {
    tmpInfShow(scr, msg, "\tReturncodes for commands applied\t", "", NULL);
  }

  *rc = 0;

  sprintf(fid, "%-8s%-8s%-2s", exfn, exft, exfm);
  int cmsrc = CMSfileOpen(fid, buffer, 80, 'V', 1, 1, f);
  if (cmsrc == 0) {
    int bytesRead;
    cmsrc = CMSfileRead(f, 0, &bytesRead);
    while(cmsrc == 0) {
      buffer[bytesRead] = '\0';

      buffer[6] = '\0';
      if (strcmp(buffer, "*#LIST") == 0 && ed == NULL) {
        char *pfn = &buffer[7];
        char *pft = &buffer[16];
        char *pfm = &buffer[25];
        buffer[15] = '\0';
        buffer[24] = '\0';
        buffer[27] = '\0';
        rtrim(pfn);
        rtrim(pft);
        rtrim(pfm);
        ed = loadList(pfn, pft, pfm, rc, msg); /* also sorts... */
        if (*rc != 0 || ed == NULL) {
          if (ed) { freeEditor(ed); ed = NULL; }
          ed = createEditor(NULL, 72, 'V');
          setWorkLrecl(ed, 71);
          *rc = 0;
          cmsrc = 12; /* simulate eof */
          break;
        }
        removeFileEntry(ed, exfn, exft, exfm);       /* hide ... */
        removeFileEntry(ed, exfn, "XLISTRES", exfm); /* ... ourself */
        moveToBOF(ed);
      } else if (strcmp(buffer, "*#SORT") == 0 && ed == NULL) {
        buffer[11] = '\0';
        buffer[14] = '\0';
        bool descending = (buffer[7] != '0');
        int offset = atoi(&buffer[9]);
        int length = atoi(&buffer[12]);
        PGMB_loc->sortSpecCount = addSortSpec(PGMB_loc->sortSpecCount, descending, offset, length);
      } else if (strcmp(buffer, "&ARGS ") == 0 && ed != NULL) {
        char *target = &buffer[7];
        if (findString(ed, target, false, NULL)) {
          LinePtr curr = getCurrentLine(ed);
          curr->text[scr->selectionColumn] = scr->selectionMark;
          selLines++;
        }
      } else if (strcmp(buffer, "*#CURR") == 0 && currentLine == NULL) {
        char *target = &buffer[7];
        if (findStringInLine(ed, target, getCurrentLine(ed), 0) >= 0) {
          currentLine = getCurrentLine(ed);
        } else if (findString(ed, target, false, NULL)) {
          currentLine = getCurrentLine(ed);
        }
      }
      cmsrc = CMSfileRead(f, 0, &bytesRead);
    }
    if (cmsrc != 12) {
      sprintf(msg, "Error reading file %s : rc = %d", fid, cmsrc);
    }
    CMSfileClose(f);
  }

  if (ed == NULL && *rc == 0) {
    *rc = 28;
    strcpy(msg, "XLIST internal error, command & exchange EXEC not available");
  }
  if (*rc != 0) {
    return ed;
  }

  *selCount = selLines;

  if (currentLine != NULL) {
    moveToLine(ed, currentLine);
  } else {
    moveToLineNo(ed, 1);
  }
  return ed;
}

/* to: < 0 = unmark if marked, > 0 mark if unmarked, == 0 = toggle */
static void toggleSelected(ScreenPtr scr, LinePtr line, int to) {
  char currMark = line->text[scr->selectionColumn];
  if (currMark == scr->selectionMark && to <= 0) {
    line->text[scr->selectionColumn] = '\0';
  } else if (currMark != scr->selectionMark && to >= 0) {
    line->text[scr->selectionColumn] = scr->selectionMark;
  }
}

static bool deselectAll(ScreenPtr scr) {
  EditorPtr ed = scr->ed;
  bool hadSelected = false;
  LinePtr f = getFirstLine(ed);
  while (f) {
    if (f->text[scr->selectionColumn] == scr->selectionMark) {
      f->text[scr->selectionColumn] = '\0';
      hadSelected = true;
    }
    f = getNextLine(ed, f);
  }
  return hadSelected;
}

static void applyPatternSelection(
    ScreenPtr scr,
    char *pattern,
    bool isSelect,
    char *msg) {
  char *lastCharRead = NULL;
  int consumed = 0;
  char tfn[9];
  char tft[9];
  char tfm[3];
  EditorPtr ed = scr->ed;

  int rc = parse_fileid(
          &pattern, 0, 1,
          tfn, tft, tfm, &consumed,
          NULL, NULL, NULL,
          &lastCharRead, msg);
  if (rc == PARSEFID_OK) {
    char *m = compileFidPattern(tfn, tft, tfm);
    if (!m) {
      LinePtr f = getFirstLine(ed);
      while (f) {
        if (f->text[scr->selectionColumn] == scr->selectionMark
           && !isSelect
           && isFidPatternMatch(f->text, &f->text[9], &f->text[18])) {
          f->text[scr->selectionColumn] = '\0';
        } else if (f->text[scr->selectionColumn] != scr->selectionMark
           && isSelect
           && isFidPatternMatch(f->text, &f->text[9], &f->text[18])) {
          f->text[scr->selectionColumn] = scr->selectionMark;
        }
        f = getNextLine(ed, f);
      }
    }
  }
}

int doFSList(
  char *fn, char *ft, char *fm,
  char *fnout, char*ftout, char *fmout,
  char *msg,
  unsigned short xlistMode) {

  /* printf("doFSList, pattern: '%s %s %s'\n", fn, ft, fm); */

  t_PGMB *PGMB_loc = CMSGetPG();
  int i;

  if (!PGMB_loc->fslistScreen) { return -1; }
/*
  if (PGMB_loc->fslistPrefixOn) {
    PGMB_loc->fslistScreen->prefixMode = 1;
    PGMB_loc->fslistScreen->prefixChar = ' ';
    PGMB_loc->fslistScreen->prefixLen = 1;
  } else {
    PGMB_loc->fslistScreen->prefixMode = 0;
  }
*/
  char fnDefault[9];
  char ftDefault[9];
  char fmDefault[3];
  strcpy(fnDefault, fn);
  strcpy(ftDefault, ft);
  strcpy(fmDefault, fm);

  bool isFileChooser = (fnout && ftout && fmout && xlistMode == 0);
  int rc = 0;
  int selCount = 0;

  ScreenPtr scr = PGMB_loc->fslistScreen;
  scr->selectionColumn = (xlistMode > 0) ? 71 : 0;
  scr->selectionMark = '*';
  scr->attrPrefix = DA_WhiteIntens;

  *msg = '\0';

  EditorPtr ed = (xlistMode != 2)
               ? loadList(fn, ft, fm, &rc, msg)
               : xlistRestart(scr, fnout, ftout, fmout, &rc, msg, &selCount);
  if (rc != 0) {
    if (ed) { freeEditor(ed); }
    return rc;
  }
  scr->ed = ed;

  char cmdPrefill[CMDLINELENGTH + 1];
  memset(cmdPrefill, '\0', sizeof(cmdPrefill));

  char *headToolname = (xlistMode) ? "XLIST" : "FSLIST";
  char headline[80];
  scr->headLine = headline;

  scr->footLine = PGMB_loc->FOOT_FSLIST;

  char listHeader[81];
  memset(listHeader, '\0', sizeof(listHeader));
  strcpy(listHeader, "  ");
  strncat(listHeader, getFileListHeader(), sizeof(listHeader)-3);
  char *lhp0 = &listHeader[2];
  char *lhp1 = listHeader;
#define setFileListHeader() \
  { scr->infoLines_p_EELIST[0] = (scr->ed->view->prefixMode) ? lhp1 : lhp0; }

  setFileListHeader();

  rc = 0;
  scr->aidCode = Aid_NoAID;
  scr->cmdLinePrefill = NULL;
  while(rc == 0) {

    scr->cursorPlacement = 0;
    scr->cursorOffset = 0;
    scr->msgText = msg;
    scr->cmdLinePrefill = NULL;

    if (xlistMode) {
      for (i = 0; i < scr->cmdPrefixesAvail; i++) {
        PrefixInput *pi = &scr->cmdPrefixes[i];
        int prefixLen = strlen(pi->prefixCmd);

        if (pi->prefixCmd[0] == '\0' || pi->prefixCmd[0] == ' ') {
          toggleSelected(scr, pi->line, -1);
        } else {
          toggleSelected(scr, pi->line, 1);
        }
      }
      if (scr->cmdPrefixesAvail > 0) {
        scr->cursorPlacement = scr->cElemType;
        scr->cursorLine = scr->cElem;
        scr->cursorOffset = scr->cElemOffset;
      }
    }

    char *cmd = NULL;
    bool tryKeepCommand = true;
    int aidIdx = aidPfIndex(scr->aidCode);
    if (aidIdx == 0 && *scr->cmdLine) {
      cmd = scr->cmdLine;
      tryKeepCommand = (!cmd || !*cmd);
    } else if (aidIdx > 0 && aidIdx < 25) {
      cmd = PGMB_loc->listPfCmds[aidIdx];
    }

    if (cmd && *cmd) {
      while(*cmd == ' ') { cmd++; }
      if (isAbbrev(cmd, "Listfile")) {
        char *param = getCmdParam(cmd);
        if (!param) { param = ""; }
        int lrc;
        char *lastCharRead = NULL;
        if (*param) {
          int consumed = 0;
          char tfn[9];
          char tft[9];
          char tfm[3];

          lrc = parse_fileid(
                  &param, 0, 1,
                  tfn, tft, tfm, &consumed,
                  fnDefault, ftDefault, fmDefault,
                  &lastCharRead, msg);
          if (lrc == PARSEFID_NONE) {
            strcpy(fn, "*");
            strcpy(ft, "*");
            strcpy(fm, "A");
            lrc = PARSEFID_OK;
          } else if (lrc == PARSEFID_OK) {
            strcpy(fn, tfn);
            strcpy(ft, tft);
            strcpy(fm, tfm);
          }
        }
        EditorPtr led = NULL;
        if (lrc == PARSEFID_OK) {
          led = loadList(fn, ft, fm, &lrc, msg);
        }
        if (led == NULL || lrc != 0) {
          if (led != NULL) { freeEditor(led); }
          scr->msgText = msg;
        } else {
          freeEditor(ed);
          ed = led;
          scr->ed = ed;
          strcpy(fnDefault, fn);
          strcpy(ftDefault, ft);
          strcpy(fmDefault, fm);
        }
      } else if (isSortCommand(cmd, msg)) {
        sort(ed, PGMB_loc->sortSpecs);
        moveToLineNo(ed, 1);
      } else if (cmd[0] == '/' && cmd[1] == '\0') {
        if (*PGMB_loc->fslisterSearchBuffer) {
          doFind(ed, PGMB_loc->fslisterSearchUp, PGMB_loc->fslisterSearchBuffer, msg);
        }
      } else if (cmd[0] == '-' && cmd[1] == '/' && cmd[2] == '\0') {
        PGMB_loc->fslisterSearchUp = !PGMB_loc->fslisterSearchUp;
        if (*PGMB_loc->fslisterSearchBuffer) {
          doFind(ed, PGMB_loc->fslisterSearchUp, PGMB_loc->fslisterSearchBuffer, msg);
        }
      } else if (cmd[0] == '/' || (cmd[0] == '-' && cmd[1] == '/')) {
        int val;
        char *param = cmd;
        int locType = parseLocation(&param, &val, PGMB_loc->fslisterSearchBuffer);
        if (locType == LOC_PATTERN) {
          PGMB_loc->fslisterSearchUp = false;
          doFind(ed, PGMB_loc->fslisterSearchUp, PGMB_loc->fslisterSearchBuffer, msg);
        } else if (locType == LOC_PATTERNUP) {
          PGMB_loc->fslisterSearchUp = true;
          doFind(ed, PGMB_loc->fslisterSearchUp, PGMB_loc->fslisterSearchBuffer, msg);
        } else {
          sprintf(msg, "No valid locate command");
        }
      } else if (isAbbrev(cmd, "Quit")) {
        if (cmdPrefill[0]) {
          cmdPrefill[0] = '\0';
        } else if (!deselectAll(scr)) {
          break;
        }
      } else if (isAbbrev(cmd, "QQuit")) {
        break;
      } else if (isAbbrev(cmd, "Help")) {
        doHelp("FSLIST", msg);
      } else if (isAbbrev(cmd, "TOp")) {
        handleScrolling(scr, TOP, false);
      } else if (isAbbrev(cmd, "BOTtom")) {
        handleScrolling(scr, BOTTOM, false);
      } else if (isAbbrev(cmd, "CENTer")) {
        handleScrolling(scr, CENTER, false);
      } else if (isAbbrev(cmd, "LEft")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(scr, LEFT, isShort);
      } else if (isAbbrev(cmd, "RIght")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(scr, RIGHT, isShort);
      } else if (isAbbrev(cmd, "PGUP")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(scr, UP, isShort);
      } else if (isAbbrev(cmd, "PGDOwn")) {
        bool isShort = isShortParam(cmd, msg);
        handleScrolling(scr, DOWN, isShort);
      } else if ((isAbbrev(cmd, "EE") || isAbbrev(cmd, "FSView"))
                 && (scr->cElemType == 2 || scr->cElemType == 1)) {
        char fn[16];
        char ft[16];
        char fm[16];
        extractFilename(scr->cElem->text, fn, ft, fm);
        if (isAbbrev(cmd, "EE")) {
          if (isFileChooser) {
            strcpy(fnout, fn);
            strcpy(ftout, ft);
            strcpy(fmout, fm);
            rc = RC_FILESELECTED;
            break;
          } else {
            rc = doEdit(fn, ft, fm, msg);
            scr->cursorPlacement = scr->cElemType;
            scr->cursorLine = scr->cElem;
            scr->cursorOffset = scr->cElemOffset;
          }
        } else {
          rc = doBrowse(fn, ft, fm, msg);
          scr->cursorPlacement = scr->cElemType;
          scr->cursorLine = scr->cElem;
          scr->cursorOffset = scr->cElemOffset;
          if (rc == RC_SWITCHTOEDIT) {
            if (isFileChooser) {
              strcpy(fnout, fn);
              strcpy(ftout, ft);
              strcpy(fmout, fm);
              rc = RC_FILESELECTED;
              break;
            } else {
              rc = doEdit(fn, ft, fm, msg);
            }
          }
        }
        scr->ed = ed; /* global scr => scr->ed is lost by FSLIST from EE */
      } else if (isAbbrev(cmd, "EE") || isAbbrev(cmd, "FSView")) {
        sprintf(msg, "Cursor not in list area for command %s", cmd);
      } else if (isAbbrev(cmd, "PREFIX")) {
        char *param = getCmdParam(cmd);
        if (isAbbrev(param, "ON")) {
          scr->ed->view->prefixMode = 1;
          scr->ed->view->prefixChar = ' ';
          scr->ed->view->prefixLen = 1;
          param = getCmdParam(param);
        } else if (isAbbrev(param, "OFf")) {
          scr->ed->view->prefixMode = 0;
          param = getCmdParam(param);
        } else if (!param || !*param) {
          strcpy(msg, "Missing parameter ON or OFF for PREFIX command");
        }
        if (param && *param) {
          strcpy(msg, "invalid or extra parameter ignored");
        }
        setFileListHeader();
      } else if (isAbbrev(cmd, "DIsks")) {
        _try {
          tmpInfClear();
          getDiskList(diskLineCallback, NULL);
          tmpInfShow(
            scr,
            msg,
            "FSLIST\tDisk overview\t" VERSION,
            getDiskListHeader(),
            NULL);
        } _catchall() {
          char *m = getLastEmergencyMessage();
          if (!m || !strlen(m)) {
            m = "Unable to load disk list (OUT OF MEMORY?)";
          }
          msg[0] = '\0';
          strcat(msg, "**\n** ");
          strcat(msg, m);
          strcat(msg, "\n**\n** ");
        } _endtry;
      } else if (xlistMode) {
        tryKeepCommand = true;
        if (isAbbrev(cmd, "Mark") && xlistMode > 0
                   && (scr->cElemType == 2 || scr->cElemType == 1)) {
          toggleSelected(scr, scr->cElem, 0);
          scr->cursorPlacement = scr->cElemType;
          scr->cursorLine = scr->cElem;
          scr->cursorOffset = scr->cElemOffset;
        } else if (isAbbrev(cmd, "Mark")) {
          /* ignored in all other cases */
        } else if (*cmd == '!' || *cmd == '?' || *cmd == '*') {
          char *command = &cmd[1];
          while(*command && *command == ' ') { command++; }
          if (!*command) {
            strcpy(msg, "Missing CMS command for apply on selected files");
          } else {
            rc = xlistSaveActions(
                       scr,
                       fn, ft, fm,
                       command,
                       fnout, ftout, fmout,
                       msg, &selCount, (*cmd == '?'), (*cmd == '*'));
            if (rc == 2044) {
              break;
            } else if (rc >= 0) {
              strcpy(cmdPrefill, cmd);
              cmdPrefill[0] = '#';
            }
          }
        } else if (*cmd == '#') {
          strcpy(cmdPrefill, cmd);
        } else if (isAbbrev(cmd, "SElect") || isAbbrev(cmd, "DESelect")) {
          bool isSelect = isAbbrev(cmd, "SElect");
          char *param = getCmdParam(cmd);
          if (param && *param) {
            applyPatternSelection(scr, param, isSelect, msg);
          } else {
            sprintf(msg, "Missing file pattern for (DE)SELECT");
          }
        } else if (isAbbrev(cmd, "CLear")) {
          deselectAll(scr);
        } else {
          sprintf(msg, "Invalid command: %s", cmd);
        }
      } else if (isAbbrev(cmd, "Mark")) {
        /* ignored in non-XLIST-mode as assigned to PF11 per default */
      } else {
        sprintf(msg, "Invalid command: %s", cmd);
      }
    }


    unsigned int lineCount;
    unsigned int currLineNo;
    getLineInfo(ed, &lineCount, &currLineNo);
    sprintf(headline, PGMB_loc->HEAD_PATTERN_FSLIST,
            headToolname,
            fn, ft, fm,
            currLineNo,
            minInt(lineCount, currLineNo + scr->screenRows - 6),
            lineCount,
            headToolname);
    scr->headLine = headline;
    if (msg[0]) { scr->msgText = msg; }
    /* extend message lines with potential common messages */
    char *emergencyMessage = getLastEmergencyMessage();
    if (emergencyMessage) {
      scr->msgText[0] = '\0';
      strcat(scr->msgText, "**\n** ");
      strcat(scr->msgText, emergencyMessage);
      strcat(scr->msgText, "\n**\n** ");
    }
    if (cmdPrefill[0] && tryKeepCommand) { scr->cmdLinePrefill = cmdPrefill; }
    rc = writeReadScreen(scr);
    *msg = '\0';
  }

  scr->msgText = NULL;
  *msg = '\0';
  freeEditor(ed);
  return rc;
}
