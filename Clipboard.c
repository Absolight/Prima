#include "apricot.h"
#include "Application.h"
#include "Image.h"
#include "Clipboard.h"
#include "Clipboard.inc"

#undef  my
#define inherited CComponent->
#define my  ((( PClipboard) self)-> self)->
#define var (( PClipboard) self)->

#define cefInit     0
#define cefDone     1
#define cefStore    2
#define cefFetch    3

struct _ClipboardFormatReg;
typedef SV* ClipboardExchangeFunc ( struct _ClipboardFormatReg * instance, int function, SV * data);
typedef ClipboardExchangeFunc *PClipboardExchangeFunc;

typedef struct _ClipboardFormatReg
{
   char                          *id;
   long                          sysId;
   ClipboardExchangeFunc         *server;
   void                          *data;
} ClipboardFormatReg, *PClipboardFormatReg;

SV * text_server  ( PClipboardFormatReg, int, SV *);
SV * image_server ( PClipboardFormatReg, int, SV *);
SV * binary_server( PClipboardFormatReg, int, SV *);

void *
Clipboard_register_format_proc( Handle self, char * format, void * serverProc);

void
Clipboard_init( Handle self, HV * profile)
{
   PApplication app = ( PApplication) application;
   if ( !application) croak("RTC0020: Cannot create clipboard without application instance");
   if ( app-> clipboard) croak( "RTC0021: Attempt to create more than one clipboard instance");
   inherited init( self, profile);
   CComponent( application)-> attach( application, self);
   if ( !apc_clipboard_create())
      croak( "RTC0022: Cannot create clipboard");
   Clipboard_register_format_proc( self, "Text",  text_server);
   Clipboard_register_format_proc( self, "Image", image_server);
}

void
Clipboard_done( Handle self)
{
   PApplication app = ( PApplication) application;
   if ( var openCount > 0) apc_clipboard_close();
   while( var formatCount)
       my deregister_format( self, (( PClipboardFormatReg) var formats)-> id);
   CComponent( application)-> detach( application, self, false);
   apc_clipboard_destroy();
   inherited done( self);
}

typedef Bool ActionProc ( Handle self, PClipboardFormatReg item, void * params);
typedef ActionProc *PActionProc;

static PClipboardFormatReg
first_that( Handle self, void * actionProc, void * params)
{
   int i;
   PClipboardFormatReg list = ( PClipboardFormatReg) var formats;
   if ( actionProc == nil) return nilHandle;
   for ( i = 0; i < var formatCount; i++)
   {
      if ((( PActionProc) actionProc)( self, list+i, params))
         return list+i;
   }
   return nil;
}

static Bool
find_format( Handle self, PClipboardFormatReg item, char * format)
{
   return strcmp( item-> id, format) == 0;
}

void *
Clipboard_register_format_proc( Handle self, char * format, void * serverProc)
{
   PClipboardFormatReg list = first_that( self, find_format, format);
   if ( list)
   {
      list = ( PClipboardFormatReg)( list-> server);
      my deregister_format( self, format);
   }
   list = malloc( sizeof( ClipboardFormatReg) * ( var formatCount + 1));
   if ( var formats != nil)
   {
      memcpy( list, var formats, sizeof( ClipboardFormatReg) * var formatCount);
      free( var formats);
   }
   var formats = list;
   list += var formatCount++;
   strcpy( list-> id  = malloc( strlen( format) + 1), format);
   list-> server      = ( ClipboardExchangeFunc *) serverProc;
   list-> sysId       = ( long) list-> server( list, cefInit, nilSV);
   return ( void *) list;
}

void
Clipboard_deregister_format( Handle self, char * format)
{
   PClipboardFormatReg fr = first_that( self, find_format, format);
   PClipboardFormatReg list = ( PClipboardFormatReg) var formats;
   if ( fr == nil) return;
   fr-> server( fr, cefDone, nilSV);
   free( fr-> id);
   var formatCount--;
   memcpy( fr, fr + 1, sizeof( ClipboardFormatReg) * ( var formatCount - ( fr - list)));
   if ( var formatCount > 0)
   {
      fr = malloc( sizeof( ClipboardFormatReg) * var formatCount);
      memcpy( fr, list, sizeof( ClipboardFormatReg) * var formatCount);
   } else
      fr = nil;
   free( var formats);
   var formats = fr;
}

Bool
Clipboard_open( Handle self)
{
   var openCount++;
   if ( var openCount > 1) return true;
   return apc_clipboard_open();
}

void
Clipboard_close( Handle self)
{
   if ( var openCount > 0)
   {
     var openCount--;
     if ( var openCount > 0) return;
     apc_clipboard_close();
   } else
      var openCount = 0;
}

Bool
Clipboard_format_exists( Handle self, char * format)
{
   Bool ret;
   PClipboardFormatReg fr = first_that( self, find_format, format);
   if ( !fr) return false;
   my open( self);
   ret = apc_clipboard_has_format( fr-> sysId);
   my close( self);
   return ret;
}

SV *
Clipboard_fetch( Handle self, char * format)
{
   PClipboardFormatReg fr = first_that( self, find_format, format);
   SV * ret;
   my open( self);
   if ( !fr || !my format_exists( self, format))
      ret = newSVsv( nilSV);
   else
      ret = fr-> server( fr, cefFetch, nilSV);
   my close( self);
   return ret;
}

void
Clipboard_store( Handle self, char * format, SV * data)
{
   PClipboardFormatReg fr = first_that( self, find_format, format);
   if ( !fr) return;
   my open( self);
   if ( var openCount == 1) apc_clipboard_clear();
   fr-> server( fr, cefStore, data);
   my close( self);
}

void
Clipboard_clear( Handle self)
{
   my open( self);
   apc_clipboard_clear();
   my close( self);
}

int
Clipboard_get_format_count( Handle self)
{
   PClipboardFormatReg list = ( PClipboardFormatReg) var formats;
   int i, ret = 0;
   my open( self);
   for ( i = 0; i < var formatCount; i++)
   {
      if ( !apc_clipboard_has_format( list[ i]. sysId)) continue;
      ret++;
   }
   my close( self);
   return ret;
}

SV *
Clipboard_get_handle( Handle self)
{
   char buf[ 256];
   snprintf( buf, 256, "0x%08lx", apc_clipboard_get_handle( self));
   return newSVpv( buf, 0);
}


int
Clipboard_get_registered_format_count( Handle self)
{
   return var formatCount;
}

Bool
Clipboard_register_format( Handle self, char * format)
{
   void * proc;
   if (( strlen( format) == 0)          ||
       ( strcmp( format, "Text") == 0)  ||
       ( strcmp( format, "Image") == 0))
      return false;
   proc = Clipboard_register_format_proc( self, format, binary_server);
   return proc != nil;
}


XS( Clipboard_get_formats_FROMPERL)
{
   dXSARGS;
   Handle self;
   int i;
   PClipboardFormatReg list;

   if ( items < 1)
      croak ("Invalid usage of Clipboard.get_formats");
   SP -= items;
   self = gimme_the_mate( ST( 0));
   if ( self == nilHandle)
      croak( "Illegal object reference passed to Clipboard.get_formats");
   my open( self);
   list = ( PClipboardFormatReg) var formats;
   for ( i = 0; i < var formatCount; i++)
   {
      if ( !apc_clipboard_has_format( list[ i]. sysId)) continue;
      XPUSHs( sv_2mortal( newSVpv( list[ i]. id, 0)));
   }
   my close( self);
   PUTBACK;
}

XS( Clipboard_get_registered_formats_FROMPERL)
{
   dXSARGS;
   Handle self;
   int i;
   PClipboardFormatReg list;

   if ( items < 1)
      croak ("Invalid usage of Clipboard.get_registered_formats");
   SP -= items;
   self = gimme_the_mate( ST( 0));
   if ( self == nilHandle)
      croak( "Illegal object reference passed to Clipboard.get_registered_formats");
   list = ( PClipboardFormatReg) var formats;
   EXTEND( sp, var formatCount);
   for ( i = 0; i < var formatCount; i++)
      PUSHs( sv_2mortal( newSVpv( list[ i]. id, 0)));
   PUTBACK;
}


void Clipboard_get_formats                     ( Handle self) { warn("Invalid call of Clipboard::get_formats"); }
void Clipboard_get_formats_REDEFINED           ( Handle self) { warn("Invalid call of Clipboard::get_formats"); }
void Clipboard_get_registered_formats          ( Handle self) { warn("Invalid call of Clipboard::get_registered_formats"); }
void Clipboard_get_registered_formats_REDEFINED( Handle self) { warn("Invalid call of Clipboard::get_registered_formats"); }


SV *
text_server ( PClipboardFormatReg instance, int function, SV * data)
{
   switch( function)
   {
      case cefInit:
         return ( SV *) cfText;
      case cefFetch:
         {
            int len;
            char *str = apc_clipboard_get_data( cfText, &len);
            SV * ret;
            if ( str) {
               ret = newSVpv( str, 0);
               free( str);
               return ret;
            }
         }
         break;
      case cefStore:
         {
            char * str = SvPV( data, na);
            apc_clipboard_set_data( cfText, ( void*) str, strlen( str)+1);
         }
         break;
   }
   return nilSV;
}

SV *
image_server( PClipboardFormatReg instance, int function, SV * data)
{
    switch( function)
    {
      case cefInit:
         return ( SV *) cfBitmap;
      case cefFetch:
         {
            HV * profile = newHV();
            Handle self = Object_create( "Image", profile);
            sv_free(( SV *) profile);
            if ( apc_clipboard_get_data( cfBitmap, (void*)(&self)) != nil) {
               --SvREFCNT( SvRV( var mate));
               return newSVsv( var mate);
            }
            Object_destroy( self);
         }
         break;
      case cefStore:
         {
            Handle image = gimme_the_mate( data);
            if ( !kind_of( image, CImage)) {
               warn("RTC0023: Not an image passed to clipboard");
               return nilSV;
            }
            apc_clipboard_set_data( cfBitmap, ( void *) image, 0);
         }
         break;
    }
    return nilSV;
}

SV *
binary_server( PClipboardFormatReg instance, int function, SV * data)
{
   switch( function)
   {
      case cefInit:
         return ( SV*) apc_clipboard_register_format( instance-> id);
      case cefDone:
         apc_clipboard_deregister_format( instance-> sysId);
         break;
   case cefFetch:
      {
         int len;
         void *xdata = apc_clipboard_get_data( instance-> sysId, &len);
         if ( xdata) {
            SV * ret = newSVpv( xdata, len);
            free( xdata);
            return ret;
         }
      }
      break;
   case cefStore:
      {
         int len;
         void * xdata = SvPV( data, len);
         apc_clipboard_set_data( instance-> sysId, xdata, len);
      }
      break;
   }
   return nilSV;
}
