#ifndef _UNIX_GUTS_H_
#define _UNIX_GUTS_H_

#define Drawable        XDrawable
#define Font            XFont
#define Window          XWindow
#undef  Bool
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
#include <X11/extensions/shape.h>
#endif
#undef Font
#undef Drawable
#undef Bool
#undef Window
#define ComplexShape 0
#define XBool int
#undef Complex

#ifndef Button6
#define Button6		6
#endif
#ifndef Button7
#define Button7		7
#endif
#ifndef Button8
#define Button8		8
#endif

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

#include "../guts.h"
#include "Widget.h"

typedef struct _RequestInformation
{
   unsigned long request;
   char *file;
   int line;
} RequestInformation, *PRequestInformation;

#define REQUEST_RING_SIZE 512

#define kbModKeyMask	0x00010000
#define kbCodeCharMask	0x00020000
#define kbVirtualMask	0x00040000
#define kbFunctionMask	0x00080000

typedef struct _FontFlags {
   int height           : 1;
   int width            : 1;
   int style            : 1;
   int pitch            : 1;
   int direction        : 1;
   int resolution       : 1;
   int name             : 1;
   int size             : 1;
   int codepage         : 1;
   int family           : 1;
   int vector           : 1;
   int ascent           : 1;
   int descent          : 1;
   int weight           : 1;
   int maximalWidth     : 1;
   int internalLeading  : 1;
   int externalLeading  : 1;
   int xDeviceRes       : 1;
   int yDeviceRes       : 1;
   int firstChar        : 1;
   int lastChar         : 1;
   int breakChar        : 1;
   int defaultChar      : 1;
   /* extras */
   int bad_vector	: 1;
} FontFlags;

typedef struct _FontInfo {
   FontFlags    flags;
   Font         font;
   char         lc_family[256];
   char         lc_name[256];
   Bool         sloppy;
   char        *vecname;
   char        *xname;
} FontInfo, *PFontInfo;

typedef struct _CachedFont {
   FontFlags    flags;
   Font         font;
   XFontStruct *fs;
   char        *load_name;
   XFont        id;
   int          ref_cnt;
} CachedFont, *PCachedFont;

union       _unix_sys_data;
struct     _timer_sys_data;
struct  _drawable_sys_data;

#define VIRGIN_GC_MASK  (       GCBackground    \
                        |       GCClipMask      \
                        |       GCForeground    \
                        |       GCFunction      \
                        |       GCLineStyle     \
                        |       GCLineWidth     )

typedef struct _gc_list
{
   struct _gc_list *next;
   struct _gc_list *prev;
   GC gc;
   struct _drawable_sys_data *holder;
} GCList;

typedef struct _paint_list
{
   struct _paint_list *next;
   Handle obj;
} PaintList, *PPaintList;

struct _UnixGuts
{
   /* Event management */
   Atom                         create_event;
   fd_set                       excpt_set;
   PList                        files;
   long                         handled_events;
   Time                         last_time;
   int                          max_fd;
   fd_set                       read_set;
   long                         total_events;
   long                         skipped_events;
   long                         unhandled_events;
   fd_set                       write_set;
   /* Graphics */
   GCList                      *free_gcl;
   GC                           menugc;
   PPaintList                   paint_list;
   GCList                      *used_gcl;
   /* Font management */
   PHash                        font_hash;
   PFontInfo                    font_info;
   char                       **font_names;
   Atom                         fxa_average_width;
   Atom                         fxa_foundry;
   Atom                         fxa_pixel_size;
   Atom                         fxa_relative_weight;
   Atom                         fxa_resolution_x;
   Atom                         fxa_resolution_y;
   Atom                         fxa_spacing;
   int                          n_fonts;
   /* Resource management */
   XrmDatabase                  db;
   XrmQuark                     qBackground;
   XrmQuark                     qbackground;
   XrmQuark                     qBlinkinvisibletime;
   XrmQuark                     qblinkinvisibletime;
   XrmQuark                     qBlinkvisibletime;
   XrmQuark                     qblinkvisibletime;
   XrmQuark                     qFont;
   XrmQuark                     qfont;
   XrmQuark                     qForeground;
   XrmQuark                     qforeground;
   XrmQuark                     qString;
   XrmQuark                     qWheeldown;
   XrmQuark                     qwheeldown;
   XrmQuark                     qWheelup;
   XrmQuark                     qwheelup;
   /* Timers & cursors */
   int                          cursor_height;
   Point                        cursor_pixmap_size;
   Pixmap                       cursor_save;
   Bool                         cursor_shown;
   struct _timer_sys_data      *cursor_timer;
   int                          cursor_width;
   Pixmap                       cursor_xor;
   Bool                         insert;
   int                          invisible_timeout;
   struct _timer_sys_data      *oldest;
   int                          visible_timeout;
   /* Window management */
   Handle                       focused;
   PHash                        menu_windows;
   PHash                        windows;
   /* WM dependancies */
   void                       (*wm_cleanup)( void);
   void                       (*wm_create_window)( Handle, ApiHandle);
   void                        *wm_data;
   Bool                       (*wm_translate_event)( Handle, XEvent *, PEvent);
   /* XServer info */
   int                          bit_order;
   unsigned char                buttons_map[256];
   int                          byte_order;
   int                          connection;
   int                          depth;
   Display                     *display;
   struct {
      long XDrawArcs;
      long XDrawLines;
      long XDrawRectangles;
      long XDrawSegments;
      long XFillArcs;
      long XFillPolygon;
      long XFillRectangles;
      long request_length;
   }                            limits;
   int                          mouse_buttons;
   int                          mouse_wheel_down;
   int                          mouse_wheel_up;
   NPoint                       resolution;
   RequestInformation           ri[REQUEST_RING_SIZE];
   int                          ri_head;
   int                          ri_tail;
   int                          screen_number;
   Bool                         shape_extension;
   int                          shape_event;
   int                          shape_error;
} guts;

#define FXA_RESOLUTION_X guts. fxa_resolution_x
#define FXA_RESOLUTION_Y guts. fxa_resolution_y
#define FXA_POINT_SIZE XA_POINT_SIZE
#define FXA_PIXEL_SIZE guts. fxa_pixel_size
#define FXA_SPACING guts. fxa_spacing
#define FXA_WEIGHT XA_WEIGHT
#define FXA_RELATIVE_WEIGHT guts. fxa_relative_weight
#define FXA_FOUNDRY guts. fxa_foundry
#define FXA_FAMILY_NAME XA_FAMILY_NAME
#define FXA_AVERAGE_WIDTH guts. fxa_average_width

#define XCHECKPOINT						\
   STMT_START {							\
      guts. ri[ guts. ri_head]. line = __LINE__;			\
      guts. ri[ guts. ri_head]. file = __FILE__;			\
      guts. ri[ guts. ri_head]. request = NextRequest(DISP);	\
      guts. ri_head++;						\
      if ( guts. ri_head >= REQUEST_RING_SIZE)			\
	 guts. ri_head = 0;					\
      if ( guts. ri_tail == guts. ri_head) {			\
	 guts. ri_tail++;					\
	 if ( guts. ri_tail >= REQUEST_RING_SIZE)		\
	    guts. ri_tail = 0;					\
      }								\
   } STMT_END

#define APC_BAD_SIZE INT_MAX
#define APC_BAD_ORIGIN INT_MAX

#define COMPONENT_SYS_DATA \
   XrmQuarkList q_class_name; \
   XrmQuarkList q_instance_name; \
   int n_class_name; \
   int n_instance_name

typedef struct _drawable_sys_data
{
   COMPONENT_SYS_DATA;
   XDrawable udrawable;
   XDrawable gdrawable;
   XWindow parent;
   NPoint resolution;
   Point origin, known_origin;
   Point size, known_size;
   Point transform, gtransform;
   Handle owner;  /* The real one */
   XWindow real_parent; /* top levels */
   XGCValues gcv;
   GC gc;
   GCList *gcl;
   XColor fore, back, saved_fore, saved_back;
   ColorSet colors;
   Region region;
   Region stale_region;
   XRectangle exposed_rect;
   XRectangle clip_rect;
   Rect scroll_rect; /* clipping */
   FillPattern fill_pattern;
   int rop, paint_rop;
   char *dashes, *paint_dashes;
   int ndashes, paint_ndashes;
   PCachedFont font;
   Font saved_font;
   Point cursor_pos;
   Point cursor_size;
   struct {
      int clip_owner			: 1;
      int sync_paint			: 1;
      int mapped			: 1;
      int exposed			: 1;
      int paint                 	: 1;
      int saved_zero_line       	: 1;
      int zero_line             	: 1;
      int grab                  	: 1;
      int enabled               	: 1;
      int focused       	        : 1;
      int reload_font			: 1;
      int do_size_hints			: 1;
      int no_size			: 1;
      int cursor_visible		: 1;
      int process_configure_notify	: 1;
   } flags;
   XImage *image_cache;
   XImage *icon_cache;
   XColor bitmap_fore, bitmap_back;
} DrawableSysData, *PDrawableSysData;

#define CURSOR_TIMER	((Handle)11)

typedef struct _timer_sys_data
{
   COMPONENT_SYS_DATA;
   int timeout;
   Handle who;
   struct _timer_sys_data *older;
   struct _timer_sys_data *younger;
   struct timeval when;
} TimerSysData, *PTimerSysData;

typedef struct _menu_item
{
   int          x;
   int          y;
   int          ux;
   int          uy;
   int          ul;
   char        *text;
} UnixMenuItem, *PUnixMenuItem;

typedef struct _menu_window
{
   Handle               self;
   XWindow              w;
   Point                sz;
   PMenuItemReg         m;
   int                  num;
   PUnixMenuItem        um;
   struct _menu_window *next;
   struct _menu_window *prev;
} MenuWindow, *PMenuWindow;

typedef struct _menu_sys_data
{
   COMPONENT_SYS_DATA;
   PMenuWindow          w;
   PCachedFont          font;
   XColor               c[ciMaxId+1];
} MenuSysData, *PMenuSysData;

typedef struct _clipboard_sys_data
{
   COMPONENT_SYS_DATA;
   Time                 have_primary_since;
} ClipboardSysData, *PClipboardSysData;

typedef union _unix_sys_data
{
   ClipboardSysData             clipboard;
   struct {
      COMPONENT_SYS_DATA;
   }                            component;
   DrawableSysData              drawable;
   MenuSysData                  menu;
   TimerSysData                 timer;
} UnixSysData, *PUnixSysData;

#define DISP		(guts. display)
#define SCREEN		(guts. screen_number)
#define DRIN		guts. display, guts. screen_number
#define X_WINDOW	(PComponent(self)-> handle)
#define X(obj)		((PDrawableSysData)(PComponent((obj))-> sysData))
#define DEFXX		PDrawableSysData selfxx = X(self)
#define XX		selfxx
#define WHEEL_DELTA	120

extern Handle
prima_xw2h( XWindow win);

extern void
prima_handle_event( XEvent *ev, XEvent *next_event);

extern void
prima_handle_menu_event( XEvent *ev, XWindow win, Handle self);

extern void
prima_get_gc( PDrawableSysData);

extern void
prima_rebuild_watchers( void);

extern void
prima_release_gc( PDrawableSysData);

extern void
prima_init_font_subsystem( void);

extern XColor*
prima_allocate_color( Handle self, Color color);

extern void
prima_cleanup_drawable_after_painting( Handle self);

extern void
prima_cleanup_font_subsystem( void);

extern void
prima_cursor_tick( void);

extern void
prima_no_cursor( Handle self);

extern void
prima_prepare_drawable_for_painting( Handle self);

extern void
prima_update_cursor( Handle self);

extern int
unix_rm_get_int( Handle self, XrmQuark class_detail, XrmQuark name_detail, int default_value);

extern void
prima_rect_union( XRectangle *t, const XRectangle *s);

extern void
prima_rect_intersect( XRectangle *t, const XRectangle *s);

extern void
prima_send_create_event( XWindow win);

extern void
prima_wm_init( void);

typedef Bool (*prima_wm_hook)( void);

#endif

/* this does not belong here */
#ifdef PRIMA_WM_SUPPORT
static prima_wm_hook registered_window_managers[] = {
   prima_wm_generic     /* This must be the last item */
};
#endif

