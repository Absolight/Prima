/*-
 * Copyright (c) 1997-1999 The Protein Laboratory, University of Copenhagen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _APRICOT_H_
#define _APRICOT_H_

#define PRIMA_CORE 1
#ifdef PRIMA_CORE
#define POLLUTE_NAME_SPACE 1
#endif

#if (PERL_PATCHLEVEL < 4)
#error "Prima require at least perl 5.004, better 5.005"
#endif

/*  #define PARANOID_MALLOC */

#define DOLBUG debug_write

#ifdef _MSC_VER
   #define BROKEN_COMPILER       1
   #define BROKEN_PERL_PLATFORM  1
   #define BOOLEAN_DEFINED
   #define __INLINE__            __inline
   #define snprintf              _snprintf
   extern double                 NAN;
#else
   #define __INLINE__            __inline__
#endif

#ifdef __unix	/* This is wrong, not every unix */
   extern double NAN;
#endif

#ifdef WORD
#error "Reconsider the order in which you #include files"
#endif
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#if defined(WORD) && (WORD==257)
#undef WORD
#endif
#include <stdlib.h>
#ifdef USE_DBMALLOC
#include "dbmalloc.h"
#endif
#if ( PERL_PATCHLEVEL == 4) && (PERL_SUBVERSION <= 4)
#undef sv_2mortal
#define sv_2mortal readonly_clean_sv_2mortal
extern SV* readonly_clean_sv_2mortal( SV* sv);
#endif

#ifdef BROKEN_PERL_PLATFORM
   #ifdef close
      #undef close
      #undef dup
      #define close win32_close
      #define dup   win32_dup
   #endif
#endif

#define PERL_CALL_SV_DIE_BUG_AWARE 1

#ifdef PERL_CALL_SV_DIE_BUG_AWARE
#define PERL_CALL_METHOD   clean_perl_call_method
#define PERL_CALL_PV       clean_perl_call_pv
#else
#define PERL_CALL_METHOD   perl_call_method
#define PERL_CALL_PV       perl_call_pv
#endif

typedef I32 Bool;
typedef UV Handle;
typedef Handle ApiHandle;
typedef long Color;

#include "Types.h"

typedef U8                     Byte;
typedef I16                    Short;
typedef I32                    Long;

typedef struct _DelegatedMessages_ {
   unsigned dmCreate             : 1;
   unsigned dmDestroy            : 1;
   unsigned dmPostMessage        : 1;
   unsigned dmTick               : 1;
   unsigned dmChange             : 1;
   unsigned dmClick              : 1;
   unsigned dmClose              : 1;
   unsigned dmColorChanged       : 1;
   unsigned dmDisable            : 1;
   unsigned dmDragDrop           : 1;
   unsigned dmDragOver           : 1;
   unsigned dmEnable             : 1;
   unsigned dmEndDrag            : 1;
   unsigned dmEnter              : 1;
   unsigned dmFontChanged        : 1;
   unsigned dmHide               : 1;
   unsigned dmHint               : 1;
   unsigned dmKeyDown            : 1;
   unsigned dmKeyUp              : 1;
   unsigned dmLeave              : 1;
   unsigned dmMenu               : 1;
   unsigned dmMouseClick         : 1;
   unsigned dmMouseDown          : 1;
   unsigned dmMouseUp            : 1;
   unsigned dmMouseMove          : 1;
   unsigned dmMouseWheel         : 1;
   unsigned dmMouseEnter         : 1;
   unsigned dmMouseLeave         : 1;
   unsigned dmMove               : 1;
   unsigned dmPaint              : 1;
   unsigned dmPopup              : 1;
   unsigned dmSetup              : 1;
   unsigned dmShow               : 1;
   unsigned dmSize               : 1;
   unsigned dmTranslateAccel     : 1;
   unsigned dmZOrderChanged      : 1;
   unsigned dmActivate           : 1;
   unsigned dmDeactivate         : 1;
   unsigned dmExecute            : 1;
   unsigned dmEndModal           : 1;
   unsigned dmWindowState        : 1;
   unsigned dmHelp               : 1;
} DelegatedMessages;

typedef struct _RGBColor
{
   unsigned char b;
   unsigned char g;
   unsigned char r;
} RGBColor, *PRGBColor;

typedef struct _Complex      { float  re, im; } Complex;
typedef struct _DComplex     { double re, im; } DComplex;
typedef struct _TrigComplex  { float  r,  ph; } TrigComplex;
typedef struct _TrigDComplex { double r,  ph; } TrigDComplex;

#define nil Null(void*)
#define nilHandle Null(Handle)
#define nilSV     &sv_undef
#define true TRUE
#define false FALSE

/* Event structures */

typedef struct _KeyEvent {
   int    cmd;
   int    subcmd;
   Handle source;
   int    code;
   int    key;
   int    mod;
   int    repeat;
} KeyEvent, *PKeyEvent;

typedef struct _PositionalEvent {
   int    cmd;
   int    subcmd;
   Handle source;
   Point  where;
   int    button;
   int    mod;
   Bool   dblclk;
} PositionalEvent, *PPositionalEvent;

typedef struct _GenericEvent {
   int    cmd;
   int    subcmd;
   Handle source;
   int    i;
   long   l;
   Bool   B;
   Point  P;
   Rect   R;
   void*  p;
   Handle H;
} GenericEvent, *PGenericEvent;

typedef union _Event {
  int             cmd;
  GenericEvent    gen;
  PositionalEvent pos;
  KeyEvent        key;
} Event, *PEvent;

typedef struct _PostMsg {
   int     msgId;
   Handle  h;
   SV   *  info1;
   SV   *  info2;
} PostMsg, *PPostMsg;

/* Object life stages */
#define csConstructing  -1         /* before create() finished */
#define csNormal         0         /* normal during life stage */
#define csDestroying     1         /* destroy() started */
#define csFrozen         2         /* cleanup() started - no messages
				      available at this point */
#define csFinalizing     3         /* done() started */
#define csDead           4         /* destroy() finished - no methods
				      available at this point */

/* Notification types */
#define ntPrivateFirst   0x0
#define ntCustomFirst    0x1
#define ntSingle         0x0
#define ntMultiple       0x2
#define ntEvent          0x4

/* Modality types */
#define mtNone           0
#define mtShared         1
#define mtExclusive      2

/* Command event types */
#define ctQueueMask      0x00070000	/* masks bits that defines behavior
					   in !csNormal stages: */
#define ctCacheable      0x00000000	/* Command caches in the queue */
#define ctDiscardable    0x00010000	/* Command should be discarded */
#define ctPassThrough    0x00020000	/* Command passes as normal */
#define ctSingle         0x00040000	/* Command caches in the queue only
					   once, then changes ct bits to */
#define ctSingleResponse 0x00050000	/* ctSingleResponse */
#define ctNoInhibit      0x00080000	/* Valid for csDestroying and csFrozen */

/* Apricot events */
#define cmHelp           0x00000002	/* WM_HELP analog */
#define cmOK             0x00000003	/* std OK cmd */
#define cmCancel         0x00000004	/* std Cancel cmd */
#define cmClose         (0x00000005|ctDiscardable)
					/* on dialog close, WM_CLOSE analog */
#define cmCreate         0x0000000A	/* WM_CREATE analog */
#define cmDestroy       (0x0000000B|ctPassThrough|ctNoInhibit) /* WM_DESTROY analog */
#define cmHide          (0x0000000C|ctDiscardable) /* visible flag aware */
#define cmShow          (0x0000000D|ctDiscardable) /*           commands */
#define cmReceiveFocus  (0x0000000E|ctDiscardable) /* focused flag aware */
#define cmReleaseFocus  (0x0000000F|ctDiscardable) /*           commands */
#define cmPaint         (0x00000010|ctSingle)      /* WM_PAINT analog */
#define cmRepaint       (0x00000010|ctSingleResponse) /* and it's response
							 action */
#define cmSize          (0x00000011|ctPassThrough) /* WM_SIZE analog */
#define cmMove          (0x00000012|ctPassThrough) /* WM_MOVE analog */
#define cmColorChanged  (0x00000013|ctDiscardable) /* generates when color
						      changed */
#define cmZOrderChanged (0x00000014|ctDiscardable) /* z-order change command */
#define cmEnable        (0x00000015|ctDiscardable) /* enabled flag aware */
#define cmDisable       (0x00000016|ctDiscardable) /*           commands */
#define cmActivate      (0x00000017|ctDiscardable) /* commands for window */
#define cmDeactivate    (0x00000018|ctDiscardable) /* active stage change */
#define cmFontChanged   (0x00000019|ctDiscardable) /* generates when font
						      changed */
#define cmWindowState   (0x0000001A|ctDiscardable) /* generates when window
						      state changed */
#define cmTimer          0x0000001C                /* WM_TIMER analog */
#define cmClick          0x0000001D                /* common click */
#define cmCalcBounds     0x0000001E                /* query on change size */
#define cmPost           0x0000001F                /* posted message */
#define cmPopup          0x00000020                /* interactive popup
						      request */
#define cmExecute        0x00000021                /* dialog execution start */
#define cmSetup          0x00000022                /* first message for alive
						      and active widget */
#define cmHint           0x00000023                /* hint show/hide message */
#define cmDragDrop       0x00000024                /* Drag'n'drop aware */
#define cmDragOver       0x00000025                /*         constants */
#define cmEndDrag        0x00000026                /* * */
#define cmMenu          (0x00000027|ctDiscardable) /* send when menu going to be activated */
#define cmEndModal       0x00000028                /* dialog execution end */

#define cmMenuCmd        0x00000050                /* interactive menu command */
#define cmKeyDown        0x00000051                /* generic key down handler cmd */
#define cmKeyUp          0x00000052                /* generic key up handler cmd (rare used) */
#define cmMouseDown      0x00000053                /* WM_BUTTONxDOWN & WM_BUTTONxDBLCLK analog */
#define cmMouseUp        0x00000054                /* WM_BUTTONxUP analog */
#define cmMouseMove      0x00000055                /* WM_MOUSEMOVE analog */
#define cmMouseWheel     0x00000056                /* WM_MOUSEWHEEL analog */
#define cmMouseClick     0x00000057                /* click response command */
#define cmMouseEnter     0x00000058                /* mouse entered window area */
#define cmMouseLeave     0x00000059                /* mouse left window area */
#define cmTranslateAccel 0x0000005A                /* key event spred to non-focused windows */
#define cmDelegateKey    0x0000005B                /* reserved for key mapping */
#define cmUser           0x00000100                /* first user-defined message */

/* mouse buttons */
#define mb1		1
#define mb2		2
#define mb3		4
#define mb4		8
#define mb5		16
#define mb6		32
#define mb7		64
#define mb8		128
#define mbLeft          mb1
#define mbRight         mb3
#define mbMiddle        mb2

/* key constants */
#define kbShift         0x100000
#define kbCtrl          0x400000
#define kbAlt           0x800000

#define kbNoKey         0xFF00
#define kbBackspace     0x0b00
#define kbTab           0x0c00
#define kbShiftTab      0x0d00
#define kbPause         0x0e00
#define kbEsc           0x0f00
#define kbSpace         0x1000
#define kbPgUp          0x1100
#define kbPgDn          0x1200
#define kbEnd           0x1300
#define kbHome          0x1400
#define kbLeft          0x1500
#define kbUp            0x1600
#define kbRight         0x1700
#define kbDown          0x1800
#define kbPrintScr      0x1900
#define kbInsert        0x1a00
#define kbDelete        0x1b00
#define kbEnter         0x1e00
#define kbSysRq         0x1f00
#define kbF1            0x2000
#define kbF2            0x2100
#define kbF3            0x2200
#define kbF4            0x2300
#define kbF5            0x2400
#define kbF6            0x2500
#define kbF7            0x2600
#define kbF8            0x2700
#define kbF9            0x2800
#define kbF10           0x2900
#define kbF11           0x2a00
#define kbF12           0x2b00
#define kbF13           0x2c00
#define kbF14           0x2d00
#define kbF15           0x2e00
#define kbF16           0x2f00

/* Please, please, PLEASE!  Do not use directly! */

typedef struct _VmtPatch
{
   void *vmtAddr;
   void *procAddr;
   char *name;
} VmtPatch;

typedef struct _VMT {         /* Whatever VMT */
   char *className;
   struct _VMT *super;
   struct _VMT *base;
   int instanceSize;
   VmtPatch *patch;
   int patchLength;
   int vmtSize;
} VMT, *PVMT;

typedef struct _AnyObject {   /* Whatever Object */
   PVMT self;
   PVMT *super;
   SV   *mate;
   struct _AnyObject *killPtr;
} AnyObject, *PAnyObject;

extern FillPattern fillPatterns[];

/* gencls rtl support */

#define C_NUMERIC_UNDEF   -90909090
#define C_STRING_UNDEF    "__C_CHAR_UNDEF__"
#define C_POINTER_UNDEF   nilSV

/* run-time class information functions */

extern Bool
kind_of( Handle object, void *cls);

extern Bool
type_of( Handle object, void *cls);

/* debugging functions */
extern Bool
log_write( const char *format, ...);

extern int
debug_write( const char *format, ...);

/* perl links */
#if (PERL_PATCHLEVEL < 5)
/* ...(perl stinks)... */
#undef  SvREFCNT_inc
#define SvREFCNT_inc(sv) ((Sv = (SV*)(sv)),		\
			  (void)(Sv && ++SvREFCNT(Sv)),	\
			  (SV*)Sv)
#endif

#ifdef PERL_CALL_SV_DIE_BUG_AWARE
extern I32
clean_perl_call_method( char* methname, I32 flags);

extern I32
clean_perl_call_pv( char* subname, I32 flags);
#endif

extern Bool
build_dynamic_vmt( void *vmt, const char *ancestorName, int ancestorVmtSize);

extern PVMT
gimme_the_vmt( const char *className);

extern Handle
gimme_the_mate( SV *perlObject);

extern Handle
create_mate( SV *perlObject);

extern SV*
eval( char* string);

extern CV*
sv_query_method( SV * object, char *methodName, Bool cacheIt);

extern CV*
query_method( Handle object, char *methodName, Bool cacheIt);

extern SV*
call_perl_indirect( Handle self, char *subName, const char *format,
		    Bool cdecl, Bool coderef, va_list params);

extern SV*
call_perl( Handle self, char *subName, const char *format, ...);

extern SV*
sv_call_perl( SV * mate, char *subName, const char *format, ...);

extern SV*
notify_perl( Handle self, char *methodName, const char *format, ...);

extern SV*
cv_call_perl( SV * mate, SV * coderef, const char *format, ...);

extern SV*
delegate_sub( Handle self, char * methodName, const char *format, ...);

extern Handle
Object_create( char * className, HV * profile);

extern void
Object_destroy( Handle self);

extern void
protect_object( Handle obj);

extern void
unprotect_object( Handle obj);

extern HV*
parse_hv( I32 ax, SV **sp, I32 items, SV **mark,
	  int expected, const char *methodName);

extern void
push_hv( I32 ax, SV **sp, I32 items, SV **mark, int callerReturns, HV *hv);

extern SV**
push_hv_for_REDEFINED( SV **sp, HV *hv);

extern int
pop_hv_for_REDEFINED( SV **sp, int count, HV *hv, int shouldBe);

extern void*
create_object( const char *objClass, const char *types, ...);

extern SV **temporary_prf_Sv;

#define pexist( key) hv_exists( profile, # key, strlen( #key))
#define pdelete( key) hv_delete( profile, # key, strlen( #key), G_DISCARD)

#define pget_sv( key) ((( temporary_prf_Sv = hv_fetch( profile, # key, strlen( # key), 0)) == nil) ? croak( "Panic: bad profile key (``%s'') requested\n", # key), &sv_undef : *temporary_prf_Sv)
#undef pget_sv
#define pget_sv( key) ((( temporary_prf_Sv = hv_fetch( profile, # key, strlen( # key), 0)) == nil) ? croak( "Panic: bad profile key (``%s'') requested in ``%s'', line %d\n", # key, __FILE__, __LINE__ ), &sv_undef : *temporary_prf_Sv)
#define pget_i( key)  ( pget_sv( key), SvIV( *temporary_prf_Sv))
#define pget_f( key)  ( pget_sv( key), SvNV( *temporary_prf_Sv))
#define pget_c( key)  ( pget_sv( key), SvPV( *temporary_prf_Sv, na))
#define pget_H( key)  gimme_the_mate( pget_sv( key))
#define pget_B( key)  ( SvTRUE( pget_sv( key)))

#define pset_sv_noinc( key, value) hv_store( profile, # key, strlen( # key), value, 0);
#define pset_sv( key, value) pset_sv_noinc( key, newSVsv( value))
#define pset_i( key, value)  pset_sv_noinc( key, newSViv( value))
#define pset_f( key, value)  pset_sv_noinc( key, newSVnv( value))
#define pset_c( key, value)  pset_sv_noinc( key, newSVpv( value, 0))
#define pset_H( key, value)  pset_sv_noinc( key, (value) ? newSVsv((( PAnyObject) value)-> mate) : nilSV)

#define create_instance( obj)  (                                   \
   ( Handle) temporary_prf_Sv = Object_create( obj, profile),      \
   ( temporary_prf_Sv ?                                            \
       --SvREFCNT( SvRV((( PAnyObject) temporary_prf_Sv)-> mate))  \
       : 0),                                                       \
   ( Handle) temporary_prf_Sv                                      \
   )

#ifdef POLLUTE_NAME_SPACE
#define TransmogrifyHandle(c,h)		((P##c)(h))
#define PAbstractMenu(h)		TransmogrifyHandle(AbstractMenu,(h))
#define CAbstractMenu(h)		(PAbstractMenu(h)->self)
#define PApplication(h)			TransmogrifyHandle(Application,(h))
#define CApplication(h)			(PApplication(h)-> self)
#define PComponent(h)			TransmogrifyHandle(Component,(h))
#define CComponent(h)			(PComponent(h)-> self)
#define PDrawable(h)			TransmogrifyHandle(Drawable,(h))
#define CDrawable(h)			(PDrawable(h)-> self)
#define PIcon(h)			TransmogrifyHandle(Icon,(h))
#define CIcon(h)			(PIcon(h)-> self)
#define PImage(h)			TransmogrifyHandle(Image,(h))
#define CImage(h)			(PImage(h)-> self)
#define PObject(h)			TransmogrifyHandle(Object,(h))
#define CObject(h)			(PObject(h)-> self)
#define PTimer(h)			TransmogrifyHandle(Timer,(h))
#define CTimer(h)			(PTimer(h)-> self)
#define PWidget(h)			TransmogrifyHandle(Widget,(h))
#define CWidget(h)			(PWidget(h)-> self)
#define PWindow(h)			TransmogrifyHandle(Window,(h))
#define CWindow(h)			(PWindow(h)-> self)
#endif POLLUTE_NAME_SPACE


/* mapping functions */

#define endCtx          0x19740108

extern int
ctx_remap_def ( int value, int * table, Bool direct, int default_value);

#define ctx_remap_end(a,b,c)	ctx_remap_def((a),(b),(c), endCtx)
#define ctx_remap(a,b,c)	ctx_remap_def((a),(b),(c), 0)

/* utility functions */

extern char *
duplicate_string( const char *);

/* lists support */

typedef struct _List
{
   Handle * items;
   int    count;
   int    size;
   int    delta;
} List, *PList;

typedef Bool ListProc ( Handle item, void * params);
typedef ListProc *PListProc;

extern void
list_create( PList self, int size, int delta);

extern PList
plist_create( int size, int delta);

extern void
list_destroy( PList self);

extern void
plist_destroy( PList self);

extern int
list_add( PList self, Handle item);

extern void
list_insert_at( PList self, Handle item, int pos);

extern Handle
list_at( PList self, int index);

extern void
list_delete( PList self, Handle item);

extern void
list_delete_at( PList self, int index);

extern void
list_delete_all( PList self, Bool kill);

extern int
list_first_that( PList self, void * action, void * params);

extern int
list_index_of( PList self, Handle item);

/* hashes support */
/* It's a mere coincidence that hashes in Prima guts implemented */
/* by means of Perl hashes */

#ifdef POLLUTE_NAME_SPACE
#define hash_create	   prima_hash_create
#define hash_destroy	   prima_hash_destroy
#define hash_fetch	   prima_hash_fetch
#define hash_delete	   prima_hash_delete
#define hash_store	   prima_hash_store
#define hash_count	   prima_hash_count
#define hash_first_that	prima_hash_first_that
#endif

typedef HV *PHash;
typedef Bool HashProc( void * item, int keyLen, void * key, void * params);
typedef HashProc *PHashProc;

extern PHash primaObjects;

extern PHash
prima_hash_create( void);

extern void
prima_hash_destroy( PHash self, Bool killAll);

extern void*
prima_hash_fetch( PHash self, const void *key, int keyLen);

extern void*
prima_hash_delete( PHash self, const void *key, int keyLen, Bool kill);

extern Bool
prima_hash_store( PHash self, const void *key, int keyLen, void *val);

#define prima_hash_count(hash) (HvKEYS(( HV*)hash))


extern void*
prima_hash_first_that( PHash self, void *action, void *params,
		       int *pKeyLen, void **pKey);


/* OS types */
#define apcOS2                  1
#define apcWin32                2
#define apcUnix                 3

/* GUI types */
#define guiDefault              0
#define guiPM                   1
#define guiWindows              2
#define guiXLib                 3
#define guiOpenLook             4
#define guiMotif                5

/* drives types (for platforms which have 'em) */
#define dtUnknown               0
#define dtNone                  1
#define dtFloppy                2
#define dtHDD                   3
#define dtNetwork               4
#define dtCDROM                 5
#define dtMemory                6

/* apc error constants */
#define errOk                    0x0000
#define errApcError              0x0001
#define errInvObject             0x0002
#define errInvParams             0x0003
#define errInvWindowIcon         0x0100
#define errInvClipboardData      0x0101
#define errInvPrinter            0x0102
#define errNoPrinters            0x0103
#define errNoPrnSettableOptions  0x0103
#define errUserCancelled         0x0104


/* system-independent object option flags */
typedef struct _ObjectOptions_ {
   unsigned optInDraw              : 1;   /* Drawable */
   unsigned optInDrawInfo          : 1;
   unsigned optTextOutBaseLine     : 1;
   unsigned optBriefKeys           : 1;   /* Widget */
   unsigned optBuffered            : 1;
   unsigned optModalHorizon        : 1;
   unsigned optOwnerBackColor      : 1;
   unsigned optOwnerColor          : 1;
   unsigned optOwnerFont           : 1;
   unsigned optOwnerHint           : 1;
   unsigned optOwnerShowHint       : 1;
   unsigned optOwnerPalette        : 1;
   unsigned optSetupComplete       : 1;
   unsigned optSelectable          : 1;
   unsigned optShowHint            : 1;
   unsigned optSystemSelectable    : 1;
   unsigned optTabStop             : 1;
   unsigned optScaleChildren       : 1;
   unsigned optPreserveType        : 1;   /* Image */
   unsigned optVScaling            : 1;
   unsigned optHScaling            : 1;
   unsigned optAutoPopup           : 1;   /* Popup */
   unsigned optActive              : 1;   /* Timer */
} ObjectOptions;

#define opt_set( option)           (PObject(self)-> options. option = 1)
#define opt_clear( option)         (PObject(self)-> options. option = 0)
#define is_opt( option)            (PObject(self)-> options. option)
#define opt_assign( option, value) (PObject(self)->options. option = \
				    (value) ? 1 : 0)
#define opt_InPaint                ( is_opt( optInDraw) \
				     || is_opt( optInDrawInfo))

/* apc class constants */
#define wcUndef               0x0000000
#define wcButton              0x0010000
#define wcCheckBox            0x0020000
#define wcCombo               0x0030000
#define wcDialog              0x0040000
#define wcEdit                0x0050000
#define wcInputLine           0x0060000
#define wcLabel               0x0070000
#define wcListBox             0x0080000
#define wcMenu                0x0090000
#define wcPopup               0x00A0000
#define wcRadio               0x00B0000
#define wcScrollBar           0x00C0000
#define wcSlider              0x00D0000
#define wcWidget              0x00E0000
#define wcWindow              0x00F0000
#define wcApplication         0x0100000
#define wcMask                0xFFF0000

/* widget grow constats */
#define gmGrowLoX             0x001
#define gmGrowLoY             0x002
#define gmGrowHiX             0x004
#define gmGrowHiY             0x008
#define gmGrowAll             0x00F
#define gmXCenter             0x010
#define gmYCenter             0x020
#define gmCenter              (gmXCenter+gmYCenter)
#define gmDontCare            0x040

/* border icons */
#define    biSystemMenu    1
#define    biMinimize      2
#define    biMaximize      4
#define    biTitleBar      8

/* border styles */
#define   bsNone           0
#define   bsSizeable       1
#define   bsSingle         2
#define   bsDialog         3

/* window states */
#define   wsNormal         0
#define   wsMinimized      1
#define   wsMaximized      2

/* z-order query constants */
#define   zoFirst          0
#define   zoLast           1
#define   zoNext           2
#define   zoPrev           3

/* system values */
#define   svYMenu           0
#define   svYTitleBar       1
#define   svXIcon           2
#define   svYIcon           3
#define   svXSmallIcon      4
#define   svYSmallIcon      5
#define   svXPointer        6
#define   svYPointer        7
#define   svXScrollbar      8
#define   svYScrollbar      9
#define   svXCursor         10
#define   svAutoScrollFirst 11
#define   svAutoScrollNext  12
#define   svInsertMode      13
#define   svXbsNone         14
#define   svYbsNone         15
#define   svXbsSizeable     16
#define   svYbsSizeable     17
#define   svXbsSingle       18
#define   svYbsSingle       19
#define   svXbsDialog       20
#define   svYbsDialog       21
#define   svMousePresent    22
#define   svMouseButtons    23
#define   svWheelPresent    24
#define   svSubmenuDelay    25
#define   svFullDrag        26
#define   svDblClickDelay   27


extern Handle application;
extern long   apcError;

/* *****************
*  apc functions   *
***************** */

/* Application management */
extern Bool
apc_application_begin_paint( Handle self);

extern Bool
apc_application_begin_paint_info( Handle self);

extern Bool
apc_application_create( Handle self);

extern void
apc_application_close( Handle self);

extern void
apc_application_destroy( Handle self);

extern void
apc_application_end_paint( Handle self);

extern void
apc_application_end_paint_info( Handle self);

extern int
apc_application_get_gui_info( char * description, int len);

extern Handle
apc_application_get_view_from_point( Handle self, Point point);

extern Handle
apc_application_get_handle( Handle self, ApiHandle apiHandle);

extern int
apc_application_get_os_info( char *system, int slen,
			     char *release, int rlen,
			     char *vendor, int vlen,
			     char *arch, int alen);

extern Point
apc_application_get_size( Handle self);

extern void
apc_application_go( Handle self);

extern void
apc_application_lock( Handle self);

extern void
apc_application_unlock( Handle self);

extern void
apc_application_yield( void);

/* Component */
extern void
apc_component_create( Handle self);

extern void
apc_component_destroy( Handle self);

extern void
apc_component_fullname_changed_notify( Handle self);

/* Window */
extern Bool
apc_window_create( Handle self, Handle owner, Bool syncPaint,
		   Bool clipOwner, int borderIcons, int borderStyle,
		   Bool taskList, int windowState, Bool useOrigin, Bool useSize);

extern void
apc_window_activate( Handle self);

extern Bool
apc_window_is_active( Handle self);

extern Bool
apc_window_close( Handle self);

extern Handle
apc_window_get_active( void);

extern int
apc_window_get_border_icons( Handle self);

extern int
apc_window_get_border_style( Handle self);

extern Point
apc_window_get_client_pos( Handle self);

extern Point
apc_window_get_client_size( Handle self);

extern Bool
apc_window_get_icon( Handle self, Handle icon);

extern int
apc_window_get_window_state( Handle self);

extern Bool
apc_window_get_task_listed( Handle self);

extern void
apc_window_set_caption( Handle self, const char* caption);

extern void
apc_window_set_client_pos( Handle self, int x, int y);

extern void
apc_window_set_client_size( Handle self, int x, int y);

extern Bool
apc_window_set_menu( Handle self, Handle menu);

extern void
apc_window_set_icon( Handle self, Handle icon);

extern void
apc_window_set_window_state( Handle self, int state);

extern Bool
apc_window_execute( Handle self, Handle insertBefore);

extern Bool
apc_window_execute_shared( Handle self, Handle insertBefore);

extern void
apc_window_end_modal( Handle self);


/* Widget management */
extern Point
apc_widget_client_to_screen( Handle self, Point p);

extern Bool
apc_widget_create( Handle self, Handle owner, Bool syncPaint,
		   Bool clipOwner, Bool transparent);

extern Bool
apc_widget_begin_paint( Handle self, Bool insideOnPaint);

extern Bool
apc_widget_begin_paint_info( Handle self);

extern void
apc_widget_destroy( Handle self);

extern PFont
apc_widget_default_font( PFont copyTo);

extern void
apc_widget_end_paint( Handle self);

extern void
apc_widget_end_paint_info( Handle self);

extern Bool
apc_widget_get_clip_owner( Handle self);

extern Rect
apc_widget_get_clip_rect( Handle self);

extern Color
apc_widget_get_color( Handle self, int index);

extern Bool
apc_widget_get_first_click( Handle self);

extern Handle
apc_widget_get_focused( void);

extern ApiHandle
apc_widget_get_handle( Handle self);

extern Rect
apc_widget_get_invalid_rect( Handle self);

extern Handle
apc_widget_get_z_order( Handle self, int zOrderId);

extern Point
apc_widget_get_pos( Handle self);

extern Point
apc_widget_get_size( Handle self);

extern Bool
apc_widget_get_sync_paint( Handle self);

extern Bool
apc_widget_get_transparent( Handle self);

extern Bool
apc_widget_is_captured( Handle self);

extern Bool
apc_widget_is_enabled( Handle self);

extern Bool
apc_widget_is_exposed( Handle self);

extern Bool
apc_widget_is_focused( Handle self);

extern Bool
apc_widget_is_responsive( Handle self);

extern Bool
apc_widget_is_showing( Handle self);

extern Bool
apc_widget_is_visible( Handle self);

extern void
apc_widget_invalidate_rect( Handle self, Rect * rect);

extern Point
apc_widget_screen_to_client( Handle self, Point p);

extern void
apc_widget_scroll( Handle self, int horiz, int vert, Rect * rect,
		   Bool scrollChildren);

extern void
apc_widget_set_capture( Handle self, Bool capture, Handle confineTo);

extern void
apc_widget_set_clip_rect( Handle self, Rect clipRect);

extern void
apc_widget_set_color( Handle self, Color color, int index);

extern void
apc_widget_set_enabled( Handle self, Bool enable);

extern void
apc_widget_set_first_click( Handle self, Bool firstClick);

extern void
apc_widget_set_focused( Handle self);

extern void
apc_widget_set_font( Handle self, PFont font);

extern void
apc_widget_set_palette( Handle self);

extern void
apc_widget_set_pos( Handle self, int x, int y);

extern void
apc_widget_set_size( Handle self, int width, int height);

extern void
apc_widget_set_tab_order( Handle self, int tabOrder);

extern void
apc_widget_set_visible( Handle self, Bool show);

extern void
apc_widget_set_z_order( Handle self, Handle behind, Bool top);

extern void
apc_widget_update( Handle self);

extern void
apc_widget_validate_rect( Handle self, Rect rect);

/* standard system pointers */
#define crDefault      -1
#define crArrow        0
#define crText         1
#define crWait         2
#define crSize         3
#define crMove         4
#define crSizeWE       5
#define crSizeNS       6
#define crSizeNWSE     7
#define crSizeNESW     8
#define crInvalid      9
#define crUser         10

/* Widget attributes */
extern void
apc_cursor_set_pos( Handle self, int x, int y);

extern void
apc_cursor_set_size( Handle self, int x, int y);

extern void
apc_cursor_set_visible( Handle self, Bool visible);

extern Point
apc_cursor_get_pos( Handle self);

extern Point
apc_cursor_get_size( Handle self);

extern Bool
apc_cursor_get_visible( Handle self);

extern Point
apc_pointer_get_hot_spot( Handle self);

extern Point
apc_pointer_get_pos( Handle self);

extern int
apc_pointer_get_shape( Handle self);

extern Point
apc_pointer_get_size( Handle self);

extern Bool
apc_pointer_get_bitmap( Handle self, Handle icon);

extern Bool
apc_pointer_get_visible( Handle self);

extern void
apc_pointer_set_pos( Handle self, int x, int y);

extern void
apc_pointer_set_shape( Handle self, int sysPtrId);

extern Bool
apc_pointer_set_user( Handle self, Handle icon, Point hotSpot);

extern void
apc_pointer_set_visible( Handle self, Bool visible);

extern int
apc_pointer_get_state( Handle self);

extern int
apc_kbd_get_state( Handle self);

/* Clipboard */
#define cfText     0
#define cfBitmap   1
#define cfCustom   2

extern Bool
apc_clipboard_create( void);

extern void
apc_clipboard_destroy( void);

extern Bool
apc_clipboard_open( void);

extern void
apc_clipboard_close( void);

extern void
apc_clipboard_clear( void);

extern Bool
apc_clipboard_has_format( long id);

extern void*
apc_clipboard_get_data( long id, int *length);

extern ApiHandle
apc_clipboard_get_handle( Handle self);

extern Bool
apc_clipboard_set_data( long id, void *data, int length);

extern long
apc_clipboard_register_format( const char *format);

extern void
apc_clipboard_deregister_format( long id);

/* Menus & popups */

typedef struct _MenuItemReg {   /* Menu item registration record */
   char * variable;             /* perl variable name */
   char * text;                 /* menu text */
   char * accel;                /* accelerator text */
   int    key;                  /* accelerator key, kbXXX */
   int    id;                   /* unique id */
   char * perlSub;              /* sub name */
   Bool   checked;              /* true if item is checked */
   Bool   disabled;             /* true if item is disabled */
   Bool   rightAdjust;          /* true if right adjust ordered */
   Bool   divider;              /* true if it's line divider */
   Handle bitmap;               /* bitmap if not nil */
   SV *   code;                 /* code if not nil */
   struct _MenuItemReg* down;   /* pointer to submenu */
   struct _MenuItemReg* next;   /* pointer to next item */
} MenuItemReg, *PMenuItemReg;

extern Bool
apc_menu_create( Handle self, Handle owner);

extern void
apc_menu_update( Handle self, PMenuItemReg oldBranch, PMenuItemReg newBranch);

extern void
apc_menu_destroy( Handle self);

extern PFont
apc_menu_default_font( PFont font);

extern Color
apc_menu_get_color( Handle self, int index);

extern PFont
apc_menu_get_font( Handle self, PFont font);

extern void
apc_menu_set_color( Handle self, Color color, int index);

extern void
apc_menu_set_font( Handle self, PFont font);

extern void
apc_menu_item_delete( Handle self, PMenuItemReg m);

extern void
apc_menu_item_set_accel( Handle self, PMenuItemReg m, const char * accel);

extern void
apc_menu_item_set_check( Handle self, PMenuItemReg m, Bool check);

extern void
apc_menu_item_set_enabled( Handle self, PMenuItemReg m, Bool enabled);

extern void
apc_menu_item_set_image( Handle self, PMenuItemReg m, Handle image);

extern void
apc_menu_item_set_key( Handle self, PMenuItemReg m, int key);

extern void
apc_menu_item_set_text( Handle self, PMenuItemReg m, const char* text);

extern ApiHandle
apc_menu_get_handle( Handle self);

extern Bool
apc_popup_create( Handle self, Handle owner);

extern PFont
apc_popup_default_font( PFont font);

extern Bool
apc_popup( Handle self, int x, int y, Rect * anchor);

/* Timer */
extern Bool
apc_timer_create( Handle self, Handle owner, int timeout);

extern void
apc_timer_destroy( Handle self);

extern int
apc_timer_get_timeout( Handle self);

extern void
apc_timer_set_timeout( Handle self, int timeout);

extern Bool
apc_timer_start( Handle self);

extern void
apc_timer_stop( Handle self);

extern ApiHandle
apc_timer_get_handle( Handle self);

/* Help */
#define  hmpNone                     0
#define  hmpOwner                   -1
#define  hmpMain                    -2
#define  hmpContents                -3
#define  hmpExtra                   -4

extern Bool
apc_help_open_topic( Handle self, long command);

extern void
apc_help_close( Handle self);

extern void
apc_help_set_file( Handle self, const char* helpFile);

/* Messages */
#define mbError        0x0100
#define mbWarning      0x0200
#define mbInformation  0x0400
#define mbQuestion     0x0800

extern void
apc_message( Handle self, PEvent ev, Bool post);

extern void
apc_show_message( const char* message);


/* graphics constants */
#define ARGB(r,g,b) ((long)(((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(r))<<16)))

/* colors */
#define    clBlack            ARGB(0,0,0)
#define    clBlue             ARGB(0,0,128)
#define    clGreen            ARGB(0,128,0)
#define    clCyan             ARGB(0,128,128)
#define    clRed              ARGB(128,0,0)
#define    clMagenta          ARGB(128,0,128)
#define    clBrown            ARGB(128,128,0)
#define    clLightGray        ARGB(192,192,192)
#define    clDarkGray         ARGB(63,63,63)
#define    clLightBlue        ARGB(0,0,255)
#define    clLightGreen       ARGB(0,255,0)
#define    clLightCyan        ARGB(0,255,255)
#define    clLightRed         ARGB(255,0,0)
#define    clLightMagenta     ARGB(255,0,255)
#define    clYellow           ARGB(255,255,0)
#define    clWhite            ARGB(255,255,255)
#define    clGray             ARGB(128,128,128)
#define    clInvalid          (long)(0x80000000)
#define    clNormalText       (long)(0x80000001)
#define    clFore             (long)(0x80000001)
#define    clNormal           (long)(0x80000002)
#define    clBack             (long)(0x80000002)
#define    clHiliteText       (long)(0x80000003)
#define    clHilite           (long)(0x80000004)
#define    clDisabledText     (long)(0x80000005)
#define    clDisabled         (long)(0x80000006)
#define    clLight3DColor     (long)(0x80000007)
#define    clDark3DColor      (long)(0x80000008)
#define    clMaxSysColor      (long)(0x80000008)

/* color indices */
#define    ciNormalText    0
#define    ciFore          0
#define    ciNormal        1
#define    ciBack          1
#define    ciHiliteText    2
#define    ciHilite        3
#define    ciDisabledText  4
#define    ciDisabled      5
#define    ciLight3DColor  6
#define    ciDark3DColor   7
#define    ciMaxId         7

typedef Color ColorSet[ ciMaxId + 1];

/* raster operations */
typedef enum {
   ropCopyPut = 0,	/* dest  = src */
   ropXorPut,		/* dest ^= src */
   ropAndPut,		/* dest &= src */
   ropOrPut,		/* dest |= src */
   ropNotPut,		/* dest = !src */
   ropNotBlack,		/* dest = (src <> 0) ? src */
   ropNotDestXor,	/* dest = (!dest) ^ src */
   ropNotDestAnd,	/* dest = (!dest) & src */
   ropNotDestOr,	/* dest = (!dest) | src */
   ropNotSrcXor,	/* dest ^= !src */
   ropNotSrcAnd,	/* dest &= !src */
   ropNotSrcOr,		/* dest |= !src */
   ropNotXor,		/* dest = !(src ^ dest) */
   ropNotAnd,		/* dest = !(src & dest) */
   ropNotOr,		/* dest = !(src | dest) */
   ropNotBlackXor,	/* dest ^= (src <> 0) ? src */
   ropNotBlackAnd,	/* dest &= (src <> 0) ? src */
   ropNotBlackOr,	/* dest |= (src <> 0) ? src */
   ropNoOper,		/* dest = dest */
   ropBlackness,	/* dest = 0 */
   ropWhiteness,	/* dest = white */
   ropInvert,		/* dest = !dest */
   ropPattern,		/* dest = pattern */
   ropXorPattern,	/* dest ^= pattern */
   ropAndPattern,	/* dest &= pattern */
   ropOrPattern,	/* dest |= pattern */
   ropNotSrcOrPat,	/* dest |= pattern | (!src) */
   ropSrcLeave,		/* dest = (src != fore color) ? src : figa */
   ropDestLeave,	/* dest = (src != back color) ? src : figa */
} ROP;

/* line ends */
#define    leFlat           0
#define    leSquare         1
#define    leRound          2

/* line patterns */
#define    lpNull           0x0000     /* */
#define    lpSolid          0xFFFF     /* ___________ */
#define    lpDash           0xF0F0     /* __ __ __ __ */
#define    lpLongDash       0xFF00     /* _____ _____ */
#define    lpShortDash      0xCCCC     /* _ _ _ _ _ _ */
#define    lpDot            0x5555     /* . . . . . . */
#define    lpDotDot         0x4444     /* ............ */
#define    lpDashDot        0xFAFA     /* _._._._._._ */
#define    lpDashDotDot     0xEAEA     /* _.._.._.._.. */


/* font styles */
#define    fsNormal         0x0000
#define    fsBold           0x0001
#define    fsThin           0x0002
#define    fsItalic         0x0004
#define    fsUnderlined     0x0008
#define    fsStruckOut      0x0010
#define    fsOutline        0x0020

/* font pitches */
#define    fpDefault        0x0000
#define    fpVariable       0x0001
#define    fpFixed          0x0002

/* font weigths */
#define    fwUltraLight     1
#define    fwExtraLight     2
#define    fwLight          3
#define    fwSemiLight      4
#define    fwMedium         5
#define    fwSemiBold       6
#define    fwBold           7
#define    fwExtraBold      8
#define    fwUltraBold      9

/* fill constants */
#define    fpEmpty          0 /*   Uses background color */
#define    fpSolid          1 /*   Uses draw color fill */
#define    fpLine           2 /*   --- */
#define    fpLtSlash        3 /*   /// */
#define    fpSlash          4 /*   /// thick */
#define    fpBkSlash        5 /*   \\\ thick */
#define    fpLtBkSlash      6 /*   \\\ light */
#define    fpHatch          7 /*   Light hatch */
#define    fpXHatch         8 /*   Heavy cross hatch */
#define    fpInterleave     9 /*   Interleaving line */
#define    fpWideDot       10 /*   Widely spaced dot */
#define    fpCloseDot      11 /*   Closely spaced dot */
#define    fpSimpleDots    12 /*   . . . . . . . . . . */
#define    fpBorland       13 /*   #################### */
#define    fpParquet       14 /*   \/\/\/\/\/\/\/\/\/\/ */
#define    fpCritters      15 /*   critters */
#define    fpMaxId         15

#define    imNone                0
#define    imbpp1                0x001
#define    imbpp4                0x004
#define    imbpp8                0x008
#define    imbpp16               0x010
#define    imbpp24               0x018
#define    imbpp32               0x020
#define    imbpp64               0x040
#define    imbpp128              0x080
#define    imBPP                 0x0FF

#define    imGrayScale           0x1000
#define    imRealNumber          0x2000
#define    imComplexNumber       0x4000
#define    imTrigComplexNumber   0x8000

/* Image conversion types */
#define    ictNone               0
#define    ictHalftone           1
#define    ictErrorDiffusion     2

/* Shortcuts and composites */
#define    imMono           imbpp1
#define    imBW             (imMono|imGrayScale)
#define    im16             imbpp4
#define    imNibble         im16
#define    im256            imbpp8
#define    imRGB            imbpp24
#define    imTriple         imRGB
#define    imByte           (imbpp8|imGrayScale)
#define    imShort          (imbpp16|imGrayScale)
#define    imLong           (imbpp32|imGrayScale)
#define    imFloat          ((sizeof(float)*8)|imGrayScale|imRealNumber)
#define    imDouble         ((sizeof(double)*8)|imGrayScale|imRealNumber)
#define    imComplex        ((sizeof(float)*8*2)|imGrayScale|imComplexNumber)
#define    imDComplex       ((sizeof(double)*8*2)|imGrayScale|imComplexNumber)
#define    imTrigComplex    ((sizeof(float)*8*2)|imGrayScale|imTrigComplexNumber)
#define    imTrigDComplex   ((sizeof(double)*8*2)|imGrayScale|imTrigComplexNumber)

/* image & bitmaps */
extern Bool
apc_image_create( Handle self);

extern void
apc_image_destroy( Handle self);

extern Bool
apc_image_begin_paint( Handle self);

extern Bool
apc_image_begin_paint_info( Handle self);

extern void
apc_image_end_paint( Handle self);

extern void
apc_image_end_paint_info( Handle self);

extern Bool
apc_image_read( const char *filename, PList imgInfo, Bool readData);

extern Bool
apc_image_save( const char *filename, const char *format, PList imgInfo);

extern void
apc_image_update_change( Handle self);

extern const char *
apc_image_get_error_message( char *errorMsgBuf, int bufLen);

extern ApiHandle
apc_image_get_handle( Handle self);


extern Bool
apc_dbm_create( Handle self, Bool monochrome);

extern void
apc_dbm_destroy( Handle self);

extern ApiHandle
apc_dbm_get_handle( Handle self);

/* text wrap options */
#define twCalcMnemonic    0x001    /* calculate first ~ entry */
#define twCalcTabs        0x002    /* calculate tabs */
#define twBreakSingle     0x004    /* return single empty line if text cannot be fitted in */
#define twNewLineBreak    0x008    /* break line at \n */
#define twSpaceBreak      0x010    /* break line at spaces */
#define twReturnLines     0x000    /* return wrapped lines */
#define twReturnChunks    0x020    /* return array of offsets & lengths */
#define twWordBreak       0x040    /* break line at word boundary, if necessary */
#define twExpandTabs      0x080    /* expand tabs */
#define twCollapseTilde   0x100    /* remove ~ from line */
#define twDefault         (twNewLineBreak|twCalcTabs|twExpandTabs|twReturnLines|twWordBreak)

typedef struct _TextWrapRec {
   char * text;                        /* text to be wrapped */
   int    textLen;                     /* text length */
   int    width;                       /* width to wrap with */
   int    tabIndent;                   /* \t replace to tabIndent spaces */
   int    options;                     /* twXXX constants */
   int    count;                       /* count of lines returned */
   int    t_start;                     /* ~ starting point */
   int    t_end;                       /* ~ ending point */
   int    t_line;                      /* ~ line */
   char   t_char;                      /* letter next to ~ */
} TextWrapRec, *PTextWrapRec;

typedef struct _FontABC
{
   float a;
   float b;
   float c;
} FontABC, *PFontABC;

/* gpi functions underplace */
extern void
apc_gp_init( Handle self);

extern void
apc_gp_done( Handle self);

extern void
apc_gp_arc( Handle self, int x, int y, int radX, int radY,
	    double angleStart, double angleEnd);

extern void
apc_gp_bar( Handle self, int x1, int y1, int x2, int y2);

extern void
apc_gp_clear( Handle self);

extern void
apc_gp_chord( Handle self, int x, int y, int radX, int radY,
	      double angleStart, double angleEnd);

extern void
apc_gp_draw_poly( Handle self, int numPts, Point * points);

extern void
apc_gp_draw_poly2( Handle self, int numPts, Point * points);

extern void
apc_gp_ellipse( Handle self, int x, int y, int radX, int radY);

extern void
apc_gp_fill_chord( Handle self, int x, int y, int radX, int radY,
		   double angleStart, double angleEnd);

extern void
apc_gp_fill_ellipse( Handle self, int x, int y, int radX, int radY);

extern void
apc_gp_fill_poly( Handle self, int numPts, Point * points);

extern void
apc_gp_fill_sector( Handle self, int x, int y, int radX, int radY,
		    double angleStart, double angleEnd);

extern void
apc_gp_flood_fill( Handle self, int x, int y, Color borderColor,
		   Bool singleBorder);

extern Color
apc_gp_get_pixel( Handle self, int x, int y);

extern void
apc_gp_line( Handle self, int x1, int y1, int x2, int y2);

extern void
apc_gp_put_image( Handle self, Handle image, int x, int y,
		  int xFrom, int yFrom, int xLen, int yLen, int rop);
extern void
apc_gp_rectangle( Handle self, int x1, int y1, int x2, int y2);

extern void
apc_gp_sector( Handle self, int x, int y, int radX, int radY,
	       double angleStart, double angleEnd);

extern void
apc_gp_set_pixel( Handle self, int x, int y, Color color);

extern void
apc_gp_stretch_image( Handle self, Handle image,
		      int x, int y, int xFrom, int yFrom,
		      int xDestLen, int yDestLen, int xLen, int yLen,
		      int rop);

extern void
apc_gp_text_out( Handle self, const char* text, int x, int y, int len);

extern char**
apc_gp_text_wrap( Handle self, TextWrapRec * t);

/* gpi settings */
extern Color
apc_gp_get_back_color( Handle self);

extern int
apc_gp_get_bpp( Handle self);

extern Color
apc_gp_get_color( Handle self);

extern Rect
apc_gp_get_clip_rect( Handle self);

extern PFontABC
apc_gp_get_font_abc( Handle self);

extern FillPattern *
apc_gp_get_fill_pattern( Handle self);

extern ApiHandle
apc_gp_get_handle( Handle self);

extern int
apc_gp_get_line_end( Handle self);

extern int
apc_gp_get_line_width( Handle self);

extern int
apc_gp_get_line_pattern( Handle self);

extern Color
apc_gp_get_nearest_color( Handle self, Color color);

extern PRGBColor
apc_gp_get_physical_palette( Handle self, int * colors);

extern Point
apc_gp_get_resolution( Handle self);

extern int
apc_gp_get_rop( Handle self);

extern int
apc_gp_get_rop2( Handle self);

extern Point*
apc_gp_get_text_box( Handle self, const char* text, int len);

extern Bool
apc_gp_get_text_opaque( Handle self);

extern int
apc_gp_get_text_width( Handle self, const char* text, int len, Bool addOverhang);

extern Point
apc_gp_get_transform( Handle self);

extern void
apc_gp_set_back_color( Handle self, Color color);

extern void
apc_gp_set_clip_rect( Handle self, Rect clipRect);

extern void
apc_gp_set_color( Handle self, Color color);

extern void
apc_gp_set_fill_pattern( Handle self, FillPattern pattern);

extern void
apc_gp_set_font( Handle self, PFont font);

extern void
apc_gp_set_line_end( Handle self, int lineEnd);

extern void
apc_gp_set_line_width( Handle self, int lineWidth);

extern void
apc_gp_set_line_pattern( Handle self, int pattern);

extern void
apc_gp_set_palette( Handle self);

extern void
apc_gp_set_rop( Handle self, int rop);

extern void
apc_gp_set_rop2( Handle self, int rop);

extern void
apc_gp_set_transform( Handle self, int x, int y);

extern void
apc_gp_set_text_opaque( Handle self, Bool opaque);

/* printer */
extern Bool
apc_prn_create( Handle self);

extern void
apc_prn_destroy( Handle self);

extern PrinterInfo*
apc_prn_enumerate( Handle self, int * count);

extern Bool
apc_prn_select( Handle self, const char* printer);

extern ApiHandle
apc_prn_get_handle( Handle self);

extern char*
apc_prn_get_selected( Handle self);

extern Point
apc_prn_get_size( Handle self);

extern Point
apc_prn_get_resolution( Handle self);

extern char*
apc_prn_get_default( Handle self);

extern Bool
apc_prn_setup( Handle self);

extern Bool
apc_prn_begin_doc( Handle self, const char* docName);

extern Bool
apc_prn_begin_paint_info( Handle self);

extern void
apc_prn_end_doc( Handle self);

extern void
apc_prn_end_paint_info( Handle self);

extern void
apc_prn_new_page( Handle self);

extern void
apc_prn_abort_doc( Handle self);

/* fonts */
extern PFont
apc_font_default( PFont font);

extern int
apc_font_load( const char* filename);

extern void
apc_font_pick( Handle self, PFont source, PFont dest);

extern PFont
apc_fonts( const char *facename, int *retCount);

/* system metrics */
extern Bool
apc_sys_get_insert_mode( void);

extern PFont
apc_sys_get_msg_font( PFont copyTo);

extern PFont
apc_sys_get_caption_font( PFont copyTo);

extern int
apc_sys_get_value( int sysValue);

extern void
apc_sys_set_insert_mode( Bool insMode);

/* etc */
extern void
apc_beep( int style);

extern void
apc_beep_tone( int freq, int duration);

extern Color
apc_lookup_color( const char *colorName);

extern char *
apc_system_action( const char *params);

extern void
apc_query_drives_map( const char *firstDrive, char *result, int len);

extern int
apc_query_drive_type( const char *drive);

extern char*
apc_get_user_name( void);

extern void*
apc_dlopen(char *path, int mode);

/* Memory bugs debugging tools */
#ifdef PARANOID_MALLOC
extern void *
_test_malloc( size_t size, int ln, char *fil, Handle self);

extern void
_test_free( void *ptr, int ln, char *fil, Handle self);

#define plist_create( sz, delta) paranoid_plist_create( sz, delta, __FILE__, __LINE__)
#define list_create( slf, sz, delta) paranoid_list_create( slf, sz, delta, __FILE__, __LINE__)
extern PList
paranoid_plist_create( int, int, char *, int);

extern void
paranoid_list_create( PList, int, int, char *, int);

extern Handle self;

#undef malloc
#undef free
#define malloc(sz) _test_malloc((sz),__LINE__,__FILE__,self)
#define free(ptr) _test_free((ptr),__LINE__,__FILE__,self)
#endif /* PARANOID_MALLOC */

#endif
