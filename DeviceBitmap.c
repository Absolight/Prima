#include "apricot.h"
#include "DeviceBitmap.h"
#include "DeviceBitmap.inc"

#undef  my
#define inherited CDrawable->
#define my  ((( PDeviceBitmap) self)-> self)->
#define var (( PDeviceBitmap) self)->

void
DeviceBitmap_init( Handle self, HV * profile)
{
   inherited init( self, profile);
   var w = pget_i( width);
   var h = pget_i( height);
   var monochrome = pget_B( monochrome);
   if ( !apc_dbm_create( self, var monochrome))
      croak("RTC0110: Cannot create device bitmap");
   inherited begin_paint( self);
   opt_set( optInDraw);
}

void
DeviceBitmap_done( Handle self)
{
   apc_dbm_destroy( self);
   inherited done( self);
}

Bool DeviceBitmap_begin_paint      ( Handle self) { return true;}
Bool DeviceBitmap_begin_paint_info ( Handle self) { return true;}
void DeviceBitmap_end_paint        ( Handle self) { return;}
Bool DeviceBitmap_get_monochrome   ( Handle self) { return var monochrome; }

static Handle xdup( Handle self, char * className)
{
   Handle h;
   PDrawable i;
   HV * profile = newHV();

   pset_H( owner,        var owner);
   pset_i( width,        var w);
   pset_i( height,       var h);
   pset_i( type,         var monochrome ? imMono : imRGB);

   h = Object_create( className, profile);
   sv_free(( SV *) profile);
   i = ( PDrawable) h;
   i-> self-> put_image( h, 0, 0, self);
   --SvREFCNT( SvRV( i-> mate));
   return h;
}

Handle DeviceBitmap_image( Handle self) { return xdup( self, "Image"); }
Handle DeviceBitmap_icon( Handle self) { return xdup( self, "Icon"); }

SV *
DeviceBitmap_get_handle( Handle self)
{
   char buf[ 256];
   snprintf( buf, 256, "0x%08lx", apc_dbm_get_handle( self));
   return newSVpv( buf, 0);
}

