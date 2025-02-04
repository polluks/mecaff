/*
** EECORE.H    - MECAFF EE editor core header file
**
** This file is part of the MECAFF based fullscreen tools of MECAFF
** for VM/370 R6 "SixPack".
**
** This module defines the interface to the EE editor core used by the
** fullscreen tools to handle line organized text data.
**
** The EE core basically manipulates a list of lines with a maximum length of
** 255 characters which may or not be associated to a CMS file.
** It supports basic editing functions of such lines like inserting, removing,
** replacing, moving or copying lines.
** Additionally, reading / writing CMS files into/from the line sequence is
** supported.
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

#ifndef _EECOREimported
#define _EECOREimported

#include "bool.h"

#define MIN_LRECL 1      /* minimal record length */
#define MAX_LRECL 255    /* maximal record length */

#define MAX_TAB_COUNT 16 /* max. number of tab positions */

/*
** definition of
**   the editor-handle which represents exactly one line-list
** and
**   the line-handle holding exactly one line of the editor.
*/

typedef struct _publicView *ViewPtr;

#ifdef _eecore_implementation

/* when included into the implementation, the complete internal structures are
   visible
*/

typedef struct _editor *EditorPtr;
typedef struct _line *LinePtr;

#else

/*
** public interface to EditorPtr and LinePtr
*/

/* the EditorPtr is mainly an opaque pointer hiding the internals.
   However, the client can associate up to 4 pointers for its own purposes,
   these pointers are ignored by EECORE.
*/
typedef struct _editor_public {
    void *clientdata1;
    void *clientdata2;
    void *clientdata3;
/*  void *clientdata4; */
    ViewPtr view;      /* pointer to first view */
} *EditorPtr;

/* the LinePtr is not fully opaque to allow faster access to the string in
   the line, BUT:
   - all fields must be handled as read-only fields!
   - if the length of the line is the LRECL of the editor,
     then there is no null-character at the end!
     (use the lineLength() function to get the current length of the line)
*/
typedef struct _publicLine {
  long privData[3];
  long selectionLevel;
  char text[0];
} *LinePtr;

#endif

typedef struct _publicView {
  struct _publicView *prevView;        /* previous view to editor/file */
  struct _publicView *nextView;        /* next view to editor/file */
  EditorPtr backEditor;                /* back link to editor */
/* ScreenPtr */ void *backLScreen;     /* back link to logical screen */
  bool prefixNumbered;
  char prefixMode; /* 0 = off, 1 = left, >1 right */
  char prefixChar; /* standard prefix filler, default: = */
  char fileToPrefixFiller; /* fill char after file line if prefixMode > 1 */
  short prefixLen; /* 1..5, will be forced to this range in _scrio() !! */



  /*
  char infoline0[LINES_LEN + 1];
  char infoline1[LINES_LEN + 1];
  char infoline2[LINES_LEN + 1];
  char infoline3[LINES_LEN + 1];
  */
  char infoLines[INFOLINES_MAX][LINES_LEN + 1];
  char *infoLines_p[INFOLINES_MAX];


  long flscreen1; /* EXTRTACT /FLSCREEN/ line numbers of the first/last lines */
  long flscreen2; /*                      of the file displayed on the screen */
  bool showTofBof;    /* show "Top of file" / "Bottom of file" ? */
  short currLinePos;  /* <= 0: first avail. line for content else middle */
  short cmdLinePos;   /* <=0 : top, > 0 bottom */
  short scaleLinePos; /* 0=off, <0 top, 1=before curr, >1 below curr */


    /* display attributes for screen elements, must be a DisplayAttr value */
    unsigned char  attrArrow          ;
    unsigned char HiLitArrow          ;
    unsigned char  attrBlock          ;
    unsigned char HiLitBlock          ;
    unsigned char  attrCBlock         ;
    unsigned char HiLitCBlock         ;
    unsigned char  attrCHighLight     ;
    unsigned char HiLitCHighLight     ;
    unsigned char  attrCmd            ;
    unsigned char HiLitCmd            ;
    unsigned char  attrCPrefix        ;
    unsigned char HiLitCPrefix        ;
    unsigned char  attrCTofeof        ;
    unsigned char HiLitCTofeof        ;
    unsigned char  attrCurLine        ;
    unsigned char HiLitCurLine        ;
    unsigned char  attrEMPTY          ;
    unsigned char HiLitEMPTY          ;
    unsigned char  attrFilearea       ;
    unsigned char HiLitFilearea       ;
    unsigned char  attrFileToPrefix   ;
    unsigned char HiLitFileToPrefix   ;
    unsigned char  attrFootLine       ;
    unsigned char HiLitFootLine       ;
    unsigned char  attrHeadLine       ;
    unsigned char HiLitHeadLine       ;
    unsigned char  attrHighLight      ;
    unsigned char HiLitHighLight      ;
    unsigned char  attrInfoLines      ;
    unsigned char HiLitInfoLines      ;
    unsigned char  attrMsg            ;
    unsigned char HiLitMsg            ;
    unsigned char  attrPending        ;
    unsigned char HiLitPending        ;
    unsigned char  attrPrefix         ;
    unsigned char HiLitPrefix         ;
    unsigned char  attrScaleLine      ;
    unsigned char HiLitScaleLine      ;
    unsigned char  attrSelectedLine   ;
    unsigned char HiLitSelectedLine   ;
    unsigned char  attrShadow         ;
    unsigned char HiLitShadow         ;
    unsigned char  attrTabline        ;
    unsigned char HiLitTabline        ;
    unsigned char  attrTofeof         ;
    unsigned char HiLitTofeof         ;

  /****************** PF key definitions at end of 'view' structure ! ******************/
  int pfMode[25];
  char pfCmds[25][CMDLINELENGTH+1];     /* additional PF key definitions on VIEW level */
  /****************** PF key definitions at end of 'view' structure ! ******************/

} *ViewPtr2;


/*
** WARNING: depending on the compilation mode for EECORE (paranoic mode), all
**          routines having 'LinePtr's as parameter(s) will check these
**          lines for plausibility (i.e. part of the editor claimed to belong
**          to?) and if this fails, will write a corresponding message to the
**          console and return immediately, resulting in a not performed or
**          aborted editor operation.
*/

/* get the last emergency message (mainly out-of-memory) generated by EECORE.
   The returned string points to a static memory block and will be NULL if
   no message is available.
   Getting the message also resets it. If present, the message has already
   been written to the console.
*/
extern char* glstemsg();
#define getLastEmergencyMessage() \
  glstemsg()


/*
**
** ############################ managament of editors
**
*/


/* create a new (empty) editor for the given LRECL and RECFM.
   It will be placed in the ring of 'prevEd' if specified or will create the
   new editor without a ring (resp. creating a new one). The current line
   will be NULL, i.e. Begin-Of-File (BOF).
*/
extern EditorPtr mkEd(EditorPtr prevEd, int lrecl, char recfm);
#define createEditor(prevEd, lrecl, recfm) \
  mkEd(prevEd, lrecl, recfm)


/* create an editor for the file 'fn ft fm', loading its content if the file
   exists (-> state = 0 = OK), making the first line the current line.
   If the file does not exists, the editor will be empty (-> state = 1) and
   the current line will be NULL (BOF = Begin-Of-File).
   The internal properties LRECL and RECFM are taken from the file if it
   exists or defaulted from 'defaultLrecl' resp. 'defaultRecfm'.
   If case of an error (state > 1), 'prevEd' will be returned.
   'msg' will be set to a meaningful message depending of the outcome
   of creating the editor.
*/
extern EditorPtr mkEdFil(
    EditorPtr prevEd,
    char *fn,
    char *ft,
    char *fm,
    int defaultLrecl,
    char defaultRecfm,
    int *state, /* 0=OK, 1=FileNotFound, 2=ReadError, 3=OtherError */
    char *msg);
#define createEditorForFile( \
    prevEd, fn, ft, fm, defLrecl, defRecfm, state, msg) \
  mkEdFil(prevEd, fn, ft, fm, defLrecl, defRecfm, state, msg)


/* remove 'ed' from its ring an free all resources associated to ot without
   writing the file to disk.
*/
extern void frEd(EditorPtr ed);
#define freeEditor(ed) \
  frEd(ed)


/* get the previous editor if the ring of 'ed' (resulting in 'ed' if the ring
   only consists of 'ed').
*/
extern EditorPtr _prevEd(EditorPtr ed);
#define getNextEd(ed) \
  _nextEd(ed)


/* get the next editor if the ring of 'ed' (resulting in 'ed' if the ring only
   consists of 'ed').
*/
extern EditorPtr _nextEd(EditorPtr ed);
#define getPrevEd(ed) \
  _prevEd(ed)


/*
**
** ############################ properties of editors
**
*/


/* get the current number of lines in 'ed'.
*/
extern int gilcnt(EditorPtr ed);
#define getLineCount(ed) \
  gilcnt(ed)


/* get line counter informations for 'ed', giving the current number of lines
   in 'lineCount' and the line number of the current line in 'currLineNo'.
*/
extern void glinfo(
   EditorPtr ed,
   unsigned int *lineCount,
   unsigned int *currLineNo);
#define getLineInfo(ed, lineCount, currLineNo) \
  glinfo(ed, lineCount, currLineNo)


/* get all or selected components of the fileid associated with 'ed'.
*/
extern void giftm(EditorPtr ed, char *fn, char *ft, char *fm);
#define getFnFtFm(ed, fn, ft, fm) \
  giftm(ed, fn, ft, fm)
#define getFn(ed, fn) \
  giftm(ed, fn, NULL, NULL)
#define getFt(ed, ft) \
  giftm(ed, NULL, ft, NULL)
#define getFm(ed, fm) \
  giftm(ed, NULL, NULL, fm)


/* the logical record length (LRECL) (i.e. the maximal line length of the
   records and the LRECL set to the file when saved).
*/
extern int gilrecl(EditorPtr ed, bool fileLrecl);
#define getLrecl(ed) \
  gilrecl(ed)
#define getFileLrecl(ed) \
  gilrecl(ed,true)
#define getWorkLrecl(ed) \
  gilrecl(ed,false)

/* set the working line width (1..LRECL) all line changing operations
   work with.
   If a line is changed, content beyond the working line length will
   be lost.
*/
void siwrecl(EditorPtr ed, int workLrecl);
#define setWorkLrecl(ed, workLrecl) \
  siwrecl(ed, workLrecl)

/* set the LRECL of the editor, this internally involves copying all lines
   of the editor, possibly truncating the lines if the new LRECL is smaller
   as before.
   Returns if one or more line(s) were truncated.
*/
extern bool silrecl(EditorPtr ed, int newLrecl);
#define setLrecl(ed, newLrecl) \
  silrecl(ed, newLrecl)


/* get the record format (RECFM) from the file loaded or specified for the
   editor.
*/
extern char girecfm(EditorPtr ed);
#define getRecfm(ed) \
  girecfm(ed)


/* set the record format (RECFM) for the editor, this will used when saving the
   file to disk.
   Valid values for 'recfm' are 'V' (variable) and 'F' (fixed).
*/
extern void sirecfm(EditorPtr ed, char recfm);
#define setRecfm(ed, recfm) \
  sirecfm(ed, recfm)


/* set uppercase conversion: if set, all subsequent updates to existing lines
   or new lines added will be converted to uppercase if 'uppercase' is true.
*/
extern void edScase(EditorPtr ed, bool uppercase);
#define setCaseMode(ed, uppercase) \
  edScase(ed, uppercase)


/* get the current uppercase conversion setting.
*/
extern bool edGcase(EditorPtr ed);
#define getCaseMode(ed) \
  edGcase(ed)


/* set case respect mode for 'ed', all subsequent find-operations will do a
   case-insensitive search if 'respect' is false.
*/
extern void edScasR(EditorPtr ed, bool respect);
#define setCaseRespect(ed, respect) \
  edScasR(ed, respect)


/* get the current case respect mode.
*/
extern bool edGcasR(EditorPtr ed);
#define getCaseRespect(ed) \
  edGcasR(ed)


/* get the modified flag of 'ed'. This flag is set to true by any modifying
   operation after creating the editor (with or without loading a file) and
   can explicitely set with 'setModified'.
*/
extern bool gmdfd(EditorPtr ed);
#define getModified(ed) \
  gmdfd(ed)


/* (re)set the modified flag of 'ed'.
*/
extern void smdfd(EditorPtr ed, bool modified);
#define setModified(ed, modified) \
  smdfd(ed, modified)


/* get the 'binary' flag of 'ed'. This flag is automatically set when initially
   loading the file or reading in a file if at least one character is not
   displayable (i.e. EBCDIC codes 0x00..0x3F or 0xFF, replaced with '.').
   The files content cannot be written to disk if the binary flag is set.
*/
extern bool gibin(EditorPtr ed);
#define isBinary(ed) \
  gibin(ed)


/* reset the binary flag of 'ed', allowing to write the files content to disk.
*/
extern bool ribin(EditorPtr ed);
#define resetIsBinary(ed) \
  ribin(ed)


/* hidden from ring ?
*/
extern bool gihid(EditorPtr ed);
#define isHidden(ed) \
  gihid(ed)

/* hide from ring
*/
extern bool sihid(EditorPtr ed);
#define setIsHidden(ed) \
  sihid(ed)

/* unhide
*/
extern bool rihid(EditorPtr ed);
#define resetIsHidden(ed) \
  rihid(ed)













/* set the tab positions for 'ed'. Only the first MAX_TAB_COUNT elements of
   'tabs'.
   The tab positions a 0-based offsets to the first character position in the
   record, so only positions in the valid range 1..(lrecl-1) will be taken
   into account as position 0 is considered an implicit tab position.
   So for setting 3 tabs, the first 3 elements of 'tabs' should be set and
   the remaining MAX_TAB_COUNT-3 elements must be 0.
   Remarks:
     - this only stores the tab positions behind 'ed', EECORE does not provide
       any tabulating functionality (at lest currently);
     - this replaces the current tab positions.
*/
extern void _stabs(EditorPtr ed, int *tabs);
#define setTabs(ed, tabs) \
  _stabs(ed, tabs)


/* copy the tab positions of 'ed' into 'tabs' (must be at least MAX_TAB_COUNT
   long) and return the number of tabs set.
*/
extern int _gtabs(EditorPtr ed, int *tabs);
#define getTabs(ed, tabs) \
  _gtabs(ed, tabs)


/* SET SCOPE ALL       -> true
   SET SCOPE DISPLAY   -> false
*/
extern bool gParadox(EditorPtr ed);
extern void sParadox(EditorPtr ed, bool scope);

extern bool getScope(EditorPtr ed);
extern void setScope(EditorPtr ed, bool scope);

extern bool gtShadow(EditorPtr ed);
extern void stShadow(EditorPtr ed, bool scope);

#define SET_SELECT_MAX 2147483647   /* z/VM 6.4: (2**31)-1 */

extern long getDisp1(EditorPtr ed);
extern long getDisp2(EditorPtr ed);

extern void _setDisp(EditorPtr ed, long display1, long display2);
#define setDisplay(ed, display1, display2) \
  _setDisp(ed, display1, display2)

extern bool _isInDpR(LinePtr line);
#define isInDisplayRange(line) \
  _isInDpR(line)

extern bool _isInScp(LinePtr line);
#define isInScope(line) \
  _isInScp(line)



/*
**
** ############################ file i/o operations
**
*/


/* insert the file 'fn ft fm' into 'ed' after the current line of 'ed' (resp.
    before the first line - if any - if the current line is NULL = BOF).
*/
extern int edRdFil(
    EditorPtr ed,
    char *fn,
    char *ft,
    char *fm,
    char *msg);
#define readFile(ed, fn, ft, fm, msg) \
  edRdFil(ed, fn, ft, fm, msg)


/* write all lines of 'ed' into the file associated with 'ed' (i.e. the
   fileid of 'ed' must be set). The file is automatically overwritten if it
   exists.
*/
extern int edSave(EditorPtr ed, char *msg);
#define saveFile(ed, msg) \
  edSave(ed, msg)


/* write all lines of 'ed' to the file 'fn ft fm' and set the fileid 'ed' is
   associated to accordingly if successful. If the file exists, it is only
   replaced if 'forceOverwrite' is passed as true.
   The returned value has the following meanings:
   0  => OK, file written resp. replaced
   1  => not written, file exists and 'forceOverwrite' is false
   2  => not written, as a rename operation (orig->temp, newtemp->orig) failed
   >2 => not written due to some other problem
   A meaningful message will be copied to 'msg'.
*/
extern int edWrFil(
    EditorPtr ed,
    char *fn,
    char *ft,
    char *fm,
    char forceOverwrite,
    char selectiveEditing,
    char *msg);
#define writeFile(ed, fn, ft, fm, forceOverwrite, selectiveEditing, msg) \
  edWrFil(ed, fn, ft, fm, forceOverwrite, selectiveEditing, msg)


/* write the range of lines 'firstLine' .. 'lastLine' (order not relevant) to
   to the file 'fn ft fm'. If the file exists, it is replaced if
   'forceOverwrite' is true.
   The returned value has the following meanings:
   0  => OK, file written resp. replaced
   1  => not written, file exists and 'forceOverwrite' is false
   2  => not written, as a rename operation (orig->temp, newtemp->orig) failed
   >2 => not written due to some other problem
   A meaningful message will be copied to 'msg'.
*/
extern int edWrRng(
    EditorPtr ed,
    char *fn,
    char *ft,
    char *fm,
    char forceOverwrite,
    char selectiveEditing,
    LinePtr firstLine,
    LinePtr lastLine,
    char *msg);
#define writeFileRange(ed, fn, ft, fm, force, selective, firstLine, lastLine, msg) \
  edWrRng(ed, fn, ft, fm, force, selective, firstLine, lastLine, msg)


/*
**
** ############################ basic line access and edit operations
**
*/


/* get the line length of 'line'.
*/
extern int edll(EditorPtr ed, LinePtr line);
#define lineLength(ed, line) \
  edll(ed, line)
extern int edllF(EditorPtr ed, LinePtr line); /* added 2024-12-21 */
#define fileLineLength(ed, line) \
  edllF(ed, line)


/* get the line number of the current line of 'ed'.
*/
extern int gcno(EditorPtr ed);
#define getCurrLineNo(ed) \
  gcno(ed)


/* get the line with the absolute line number 'lineNo'.
*/
extern LinePtr glno(EditorPtr ed, int lineNo);
#define getLineAbsNo(ed, lineNo) \
  glno(ed, lineNo)


/* getlinum :
   get the line number from given LinePtr
   getLineNumber(line)
*/
extern int getlinum(LinePtr line);
#define getLineNumber(line) \
  getlinum(line)



/* get the indentation (offset of the first non-blank character) of the first
   non-empty line preceding 'forLine'.
*/
extern int gllindt(EditorPtr ed, LinePtr forLine);
#define getLastLineIndent(ed, forLine) \
  gllindt(ed, forLine)


/* get the indentation (offset of the first non-blank character) of 'forLine'.
*/
extern int gclindt(EditorPtr ed, LinePtr forLine);
#define getCurrLineIndent(ed, forLine) \
  gclindt(ed, forLine)


/* insert a new line after 'edLine' with the content 'lineText', returning
   the new line.
*/
extern LinePtr inslina(EditorPtr ed, LinePtr edLine, char *lineText);
#define insertLineAfter(ed, edLine, lineText) \
  inslina(ed, edLine, lineText)


/* insert a new line before 'edLine' with the content 'lineText', returning
   the new line.
*/
extern LinePtr inslinb(EditorPtr ed, LinePtr edLine, char *lineText);
#define insertLineBefore(ed, edLine, lineText) \
  inslinb(ed, edLine, lineText)


/* insert a new line after the current line with the content 'lineText',
   returning the new line.
   The line text is
   - converted to uppercase if uppercase was set on
   - trimmed at the line end (white-space is removed)
   - truncated if the line length exceeds the LRECL of 'ed'.
*/
extern LinePtr insline(EditorPtr ed, char *lineText);
#define insertLine(ed, lineText) \
  insline(ed, lineText)


/* replace the content of 'line' with the new content 'txt' having the length
   'txtLen'.
   The line text is
   - converted to uppercase if uppercase was set on
   - trimmed at the line end (white-space is removed)
   - truncated if the line length exceeds the LRECL of 'ed'.
*/
extern void updline(EditorPtr ed, LinePtr line, char *txt, unsigned int txtLen);
#define updateLine(ed, line, txt, txtLen) \
  updline(ed, line, txt, txtLen)


/* delete the line 'edLine'.
*/
extern void delline(EditorPtr ed, LinePtr edLine);
#define deleteLine(ed, edLine) \
  delline(ed, edLine)


/*
**
** ############################ moving through an editor's content
**
*/


/* move the current line before the first content line of 'ed', returning NULL.
*/
extern LinePtr m2bof(EditorPtr ed);
#define moveToBOF(ed) m2bof(ed)


/* move the current line to the last line of 'ed', returning this line or NULL
  if the file is empty.
*/
extern LinePtr m2lstl(EditorPtr ed);
#define moveToLastLine(ed) \
  m2lstl(ed)


/* move the current line to the line with the given line number. Lines start
   counting with 1.
   Moving to a 'lineNo' < 1 will move to BOF, moving to a 'lineNo' after the
   last line (getLineCount(...)) moves to the last line (if any).
   Returns the line effectively moved to (NULL if outside valid lines range).
*/
extern LinePtr m2lno(EditorPtr ed, int lineNo);
#define moveToLineNo(ed, lineNo) \
  m2lno(ed, lineNo)


/* set 'line' the new current line of 'ed'.
*/
extern LinePtr m2line(EditorPtr ed, LinePtr line);
#define moveToLine(ed, line) \
  m2line(ed, line)


/* move the current line of 'ed' by 'by' lines towards the file start.
   Returns the resulting new current line, possibly NULL if moving beyond the
   first line.
*/
extern LinePtr moveUp(EditorPtr ed, unsigned int by);


/* move the current line of 'ed' by 'by' lines towards the file end, bounded
   by the last line.
   Returns the new current line.
*/
extern LinePtr moveDown(EditorPtr ed, unsigned int by);


/* manipulate the line marks table of 'ed':
    - if 'mark' is a string of length 1 with a letter, assoiates this line
      mark with 'line'
    - if 'mark' is the string "*", then alle marks are cleared.
   A meaningful message for the outcome of the operation is copied to 'msg'.
*/
extern bool edSMark(EditorPtr ed, LinePtr line, char *mark, char *msg);
#define setLineMark(ed, line, mark, msg) \
  edSMark(ed, line, mark, msg)


/* get the line associated with 'mark' or NULL if the mark is undefined.
   A meaningful message for the outcome of the operation is copied to 'msg'.
*/
extern LinePtr edGMark(EditorPtr ed, char *mark, char *msg);
#define getLineMark(ed, mark, msg) \
  edGMark(ed, mark, msg)


/* move the current line to the line associated with 'mark', returning true
   if 'mark' is a valig line mark. The current line will not be moved if 'mark'
   is currently undefined.
   A meaningful message for the outcome of the operation is copied to 'msg'.
*/
extern bool m2Mark(EditorPtr ed, char *mark, char *msg);
#define moveToLineMark(ed, mark, msg) \
  m2Mark(ed, mark, msg)


/* find the offset of 'what' in 'line' based on the case respect setting of
   'ed', starting to search the lines content at 'offset'.
   Returns -1 if 'what' was not found.
*/
extern int edFsil(
    EditorPtr ed,
    char *what,
    LinePtr line,
    int offset);
#define findStringInLine(ed, what, line, offset) \
  edFsil(ed, what, line, offset)


/* find the first line of 'ed' containing 'what' (based on the case respect
   setting) starting from the current line in 'upwards' direction (or not)
   bounded to 'toLine'.
   If 'toLine' is NULL, the search is bounded by BOF or EOF, depending on
   the value of 'upwards'.
   If found, this line is made the current line.
   Returns if the current line has changed (i.e. 'what' was found).
*/
extern bool edFind(EditorPtr ed, char *what, bool upwards, LinePtr toLine);
#define findString(ed, what, upwards, toLine) \
  edFind(ed, what, upwards, toLine)


/*
**
** ############################ accessing specific lines of an editor
**
*/


/* get a frame of lines around the current line of 'ed' with 'upLinesReq' lines
   before and 'downLinesReq' lines after the current line.
   The frame is specified in the out parameters:
   - 'currLine' and 'currLineNo' give the current line, with 'currLine' being
      NULL if 'ed' is empty or the current line is at BOF.
   - 'upLines' and 'upLinesCount' give the above frame half, with 'upLines'
     holding the first line in the frame
   - 'downLines' and 'downLinesCount' give the below frame half, with
     'downLines' being the last line of the frame.
*/
extern void glframe(
  EditorPtr ed,
  unsigned int upLinesReq,
  LinePtr *upLines,
  unsigned int *upLinesCount,
  LinePtr *currLine, /* will be NULL if BOF */
  unsigned int *currLineNo,
  unsigned int downLinesReq,
  LinePtr *downLines,
  unsigned int *downLinesCount);
#define getLineFrame(ed, ulreq, uls, ulscnt, cl, clno, dlreq, dls, dlscnt) \
  glframe(ed, ulreq, uls, ulscnt, cl, clno, dlreq, dls, dlscnt)


/* get the first line of 'ed' or NULL if 'ed' is empty.
*/
extern LinePtr glfirst(EditorPtr ed);
#define getFirstLine(ed) \
  glfirst(ed)


/* get the last line of 'ed' or NULL if 'ed is empty.
*/
extern LinePtr gllast(EditorPtr ed);
#define getLastLine(ed) \
  gllast(ed)


/* get the current line of 'ed' or NULL if the current line is BOF or 'ed' is
   empty.
*/
extern LinePtr glcurr(EditorPtr ed);
#define getCurrentLine(ed) \
  glcurr(ed)


/* Get the line following 'from', returning the first line if 'from' is NULL.
   Returns NULL if from == NULL and the file is empty OR from is last line.
*/
extern LinePtr glnext(EditorPtr ed, LinePtr from);
#define getNextLine(ed, from) \
  glnext(ed, from)


/* Get the line preceding 'from'.
   Returns NULL if 'from' == NULL OR 'from' is first line.
*/
extern LinePtr glprev(EditorPtr ed, LinePtr from);
#define getPrevLine(ed, from) \
  glprev(ed, from)


/*
**
** ############################ operations on line ranges
**
*/


/* check if 'first'..'last' is a valid range,i.e. if 'first' comes before
   'last'.
   If not, the line pointers are swapped.
   Returns true if the 2 lines are valid at all (not NULL).
*/
extern bool ordrlns(EditorPtr ed, LinePtr *first, LinePtr *last);
#define orderLines(ed, firstLine, lastLine) \
  ordrlns(ed, firstLine, lastLine)


/* check if 'checkLine' is contained in the range ['rangeFirst'..'rangeLast'),
   returning true if so.
*/
extern bool isinrng(
    EditorPtr ed,
    LinePtr checkLine,
    LinePtr rangeFirst,
    LinePtr rangeLast,
    const char *src, int srcLine);
#define isInLineRange(ed, checkLine, rangeFirst, rangeLast) \
  isinrng(ed, checkLine, rangeFirst, rangeLast, _FILE_NAME_, __LINE__)


/* delete a range of lines of 'ed'.
   If 'fromLine' or 'toLine' are invalid, the operation is not performed and
   false is returned.
*/
extern bool delrng(EditorPtr ed, LinePtr fromLine, LinePtr toLine);
#define deleteLineRange(ed, fromLine, toLine) \
  delrng(ed, fromLine, toLine)


/* copy the range 'srcFromLine'..'srcToLine' of 'srcEd' to ' 'trgEd', placing
   the copied lines before/after 'trgLine' of 'trgEd' depending on
   'insertBefore'.
   Returns false if any of the parameters is invalid (and no copying was done)
*/
extern bool cprng(
    EditorPtr srcEd, LinePtr srcFromLine, LinePtr srcToLine,
    EditorPtr trgEd, LinePtr trgLine, bool insertBefore);
#define copyLineRange(srcEd, srcFromLn, srcToLn, trgEd, trgLn, insBefore) \
  cprng(srcEd, srcFromLn, srcToLn, trgEd, trgLn, insBefore)


/* move the range 'srcFromLine'..'srcToLine' of 'srcEd' to ' 'trgEd', placing
   the moved lines before/after 'trgLine' of 'trgEd' depending on
   'insertBefore'.
   Returns false if any of the parameters is invalid (and no moving was done)
*/
extern bool mvrng(
    EditorPtr srcEd, LinePtr srcFromLine, LinePtr srcToLine,
    EditorPtr trgEd, LinePtr trgLine, bool insertBefore);
#define moveLineRange(srcEd, srcFromLn, srcToLn, trgEd, trgLn, insBefore) \
  mvrng(srcEd, srcFromLn, srcToLn, trgEd, trgLn, insBefore)


/*
**
** ############################ high-level operations: replace, join/split, ...
**
*/

/* update 'line' by replacing the substring 'from' by 'to', with beginnning
   to look for 'from' at offset 'startOffset' in the line content.
   The out parameters 'found' and 'truncated' inform if 'from' was found resp.
   if the line was truncated by removing 'from'and inserting 'to'.
   Returns the character position (offset) in the line directly after the
   'to' string if the replacement took place or 'startOffset' if not.
*/
extern int edRepl(
    EditorPtr ed,
    char *from,
    char *to,
    LinePtr line,
    int startOffset,
    bool *found,
    bool *truncated);
#define changeString(ed, from, to, line, startOffset, found, truncated) \
  edRepl(ed, from, to, line, startOffset, found, truncated)


/* join the line 'line' of 'ed' with its following line. If 'atPos' is beyond
   the lines end (and below LRECL), blanks will be appended up to the
   position 'atPos' before joining.
   Leading blanks on the second line are removed before joining.
   Joining will not take place if the new line length would be larger than
   LRECL unless 'force' is passed with true, in which case the resulting line
   may be truncated.
   Returns the following values:
    0 = not joined
    1 = joined
    2 = joined but truncated
*/
extern int edJoin(EditorPtr ed, LinePtr line, unsigned int atPos, bool force);


/* split the line 'line' of 'ed' at offset 'atPos', with the character at
   'atPos' being the first character on the new next line.
   Returns the new line.
*/
extern LinePtr edSplit(EditorPtr ed, LinePtr line, unsigned int atPos);


/* SortItem: a single zone of character columns to compare lines for sorting */
typedef struct _sort_item {
  bool sortDescending;
  unsigned char offset;
  unsigned char length;
} SortItem;

/* sort all lines in 'ed' with the sort specification 'sortItems'.
   The list of sort items ends with the first item having length == 0
*/
extern void sort(EditorPtr ed, SortItem *sortItems);


#define SHIFTMODE_IFALL 0 /* shift lines by 'by' only if no truncation */
#define SHIFTMODE_MIN   1 /* shift all lines up to 'by' w/out truncating */
#define SHIFTMODE_LIMIT 2 /* shift each line up to 'by' w/out truncating */
#define SHIFTMODE_TRUNC 3 /* shift all lines by 'by', possibly truncating*/

/* shift the text in the block 'fromLine'..'toLine' to the left by 'by' places
   according to the 'mode' (one of the constants SHIFTMODE_*).

  returns:
    0 = OK (lines shifted according 'mode' and 'by'),
    1 = (mode = IFALL) not shifted, as would truncate some line(s)
    2 = (mode = TRUNC) some lines truncated
   > 100 = (mode = LIMIT): shifted by (rc - 100), i.e. by less than 'by'
   -1 = fromLine or toLine not valid (null, other editor, ...)
*/
extern int edShftl(
    EditorPtr ed,
    LinePtr fromLine,
    LinePtr toLine,
    unsigned int by,
    int mode);
#define shiftLeft(ed, fromLine, toLine, by, mode) \
  edShftl(ed, fromLine, toLine, by, mode)


/* shift the text in the block 'fromLine'..'toLine' to the right by 'by' places
   according to the 'mode' (one of the constants SHIFTMODE_*).

  returns:
    0 = OK (lines shifted according 'mode' and 'by'),
    1 = (mode = IFALL) not shifted, as would truncate some line(s)
    2 = (mode = TRUNC) some lines truncated
   > 100 = (mode = LIMIT): shifted by (rc - 100), i.e. by less than 'by'
   -1 = fromLine or toLine not valid (null, other editor, ...)
*/
extern int edShftr(
    EditorPtr ed,
    LinePtr fromLine,
    LinePtr toLine,
    unsigned int by,
    int mode);
#define shiftRight(ed, fromLine, toLine, by, mode) \
  edShftr(ed, fromLine, toLine, by, mode)

#endif
