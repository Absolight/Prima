

#include "img_conv.h"

#ifdef __cplusplus
extern "C" {
#endif


#define var (( PImage) self)
// Mono

BC( mono, nibble, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_mono_nibble( BCCONV);
}

BC( mono, byte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_mono_byte( BCCONV);
}

BC( mono, graybyte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_mono_graybyte( BCCONV, var->palette);
   memcpy( dstPal, map_RGB_gray, 256 * sizeof( RGBColor));
}

BC( mono, rgb, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_mono_rgb( BCCONV, var->palette);
}

// Nibble

BC( nibble, mono, None)
{
   dBCARGS;
   BCWARN;
   cm_fill_colorref( var->palette, srcColors, stdmono_palette, 2, colorref);
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_nibble_mono_cr( BCCONV, colorref);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( nibble, mono, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_nibble_mono_ht( BCCONV, var->palette, i);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( nibble, byte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_nibble_byte( BCCONV);
}

BC( nibble, graybyte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_nibble_graybyte( BCCONV, var->palette);
   memcpy( dstPal, map_RGB_gray, 256 * sizeof( RGBColor));
}

BC( nibble, rgb, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_nibble_rgb( BCCONV, var->palette);
}

// Byte
BC( byte, mono, None)
{
   dBCARGS;
   BCWARN;
   cm_fill_colorref( var->palette, srcColors, stdmono_palette, 2, colorref);
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_mono_cr( BCCONV, colorref);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( byte, mono, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_mono_ht( BCCONV, var->palette, i);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( byte, nibble, None)
{
   dBCARGS;
   BCWARN;
   {
      RGBColor dstPalBuf[ 16];
      cm_squeeze_palette( var->palette, srcColors, dstPalBuf, 16);
      cm_fill_colorref( var->palette, srcColors, dstPalBuf, 16, colorref);
      memcpy( dstPal, dstPalBuf, sizeof( dstPalBuf));
   }
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_nibble_cr( BCCONV, colorref);
}

BC( byte, nibble, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_nibble_ht( BCCONV, var->palette, i);
   memcpy( dstPal, cubic_palette8, 8 * sizeof( RGBColor));
}

BC( byte, graybyte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_graybyte( BCCONV, var->palette);
   memcpy( dstPal, map_RGB_gray, 256 * sizeof( RGBColor));
}

BC( byte, rgb, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_rgb( BCCONV, var->palette);
}

// Graybyte
BC( graybyte, mono, None)
{
   dBCARGS;
   BCWARN;
   cm_fill_colorref( var->palette, srcColors, stdmono_palette, 2, colorref);
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_mono_cr( BCCONV, colorref);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( graybyte, mono, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_graybyte_mono_ht( BCCONV, i);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( graybyte, nibble, None)
{
   dBCARGS;
   BCWARN;
   {
      RGBColor dstPalBuf[ 16];
      cm_squeeze_palette( var->palette, srcColors, dstPalBuf, 16);
      cm_fill_colorref( var->palette, srcColors, dstPalBuf, 16, colorref);
      memcpy( dstPal, dstPalBuf, sizeof( dstPalBuf));
   }
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_byte_nibble_cr( BCCONV, colorref);
}

BC( graybyte, nibble, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_graybyte_nibble_ht( BCCONV, i);
   memcpy( dstPal, std16gray_palette, sizeof( std16gray_palette));
}

BC( graybyte, byte, None)
{
   dBCARGS;
   BCWARN;
   if ( srcData != dstData) memcpy( dstData, srcData, dstLine * height);
}

BC( graybyte, rgb, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_graybyte_rgb( BCCONV);
}

// RGB
BC( rgb, mono, None)
{
   dBCARGS;
   Byte * convBuf = allocb( width);
   BCWARN;
   cm_fill_colorref(( PRGBColor) map_RGB_gray, 256, stdmono_palette, 2, colorref);
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
   {
      bc_rgb_graybyte( srcData, convBuf, width);
      bc_byte_mono_cr( convBuf, dstData, width, colorref);
   }
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
   free( convBuf);
}

BC( rgb, mono, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_rgb_mono_ht( BCCONV, i);
   memcpy( dstPal, stdmono_palette, sizeof( stdmono_palette));
}

BC( rgb, nibble, None)
{
   dBCARGS;
   BCWARN;
   memcpy( dstPal, cubic_palette16, sizeof( cubic_palette16));
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_rgb_nibble(BCCONV);
}

BC( rgb, nibble, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_rgb_nibble_ht( BCCONV, i);
   memcpy( dstPal, cubic_palette8, 8 * sizeof( RGBColor));
}

BC( rgb, byte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_rgb_byte( BCCONV);
   memcpy( dstPal, cubic_palette, sizeof( cubic_palette));
}

BC( rgb, byte, Halftone)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_rgb_byte_ht( BCCONV, i);
   memcpy( dstPal, cubic_palette, sizeof( cubic_palette));
}

BC( rgb, graybyte, None)
{
   dBCARGS;
   BCWARN;
   for ( i = 0; i < height; i++, srcData += srcLine, dstData += dstLine)
      bc_rgb_graybyte( BCCONV);
   memcpy( dstPal, map_RGB_gray, 256 * sizeof( RGBColor));
}

#ifdef __cplusplus
}
#endif
