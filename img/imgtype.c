#include <img_conv.h>
#define var (( PImage) self)

/* Color mappers */
#define BCPARMS      self, dstData, dstPal, dstType
#define BCSELFGRAY   self, var->data, dstPal, imByte
#define ic_MIDCONVERT(from,to)                                    \
{                                                                 \
   Byte * sData = var->data;                                       \
   int  sDataSize = var->dataSize, sLineSize = var->lineSize;       \
   Byte * n = malloc((( var->w * 8 + 31) / 32) * 4 * var->h);       \
   ic_##from##_graybyte_ictNone(self, n, dstPal, imByte);         \
   var->data = n;                                                  \
   var->type = imByte;                                             \
   var->lineSize = (( var->w * 8 + 31) / 32) * 4;                   \
   var->dataSize = var->lineSize * var->h;                           \
   ic_Byte_##to( self, dstData, dstPal, dstType);                 \
   var->data = sData;                                              \
   var->lineSize = sLineSize;                                      \
   var->dataSize = sDataSize;                                      \
   free( n);                                                      \
}

void
ic_type_convert( Handle self,
                 Byte * dstData, PRGBColor dstPal, int dstType)
{
   int srcType = var->type;
   int orgDstType = dstType;

   switch( srcType)
   {
      case imBW:
      case im16  + imGrayScale:
      case imRGB + imGrayScale:
         srcType &=~ imGrayScale;
         break;
   }

   switch( dstType)
   {
      case imBW:
      case im16  + imGrayScale:
      case imRGB + imGrayScale:
         dstType &=~ imGrayScale;
         break;
   }

   if ( srcType == dstType)
   {
      memcpy( dstData, var->data, var->dataSize);
      if ( dstPal != var->palette)
         memcpy( dstPal, var->palette, var->palSize);
      else if ( orgDstType & imGrayScale) switch( dstType)
      {
         case imbpp1: memcpy( dstPal, stdmono_palette,    sizeof( stdmono_palette)); break;
         case imbpp4: memcpy( dstPal, std16gray_palette,  sizeof( std16gray_palette)); break;
         case imbpp8: memcpy( dstPal, std256gray_palette, sizeof( std256gray_palette)); break;
      }
      return;
   }

   switch( srcType)
   {
      case imMono: switch( dstType)
      {
         case im16:     ic_mono_nibble_ictNone(BCPARMS);   break;
         case im256:    ic_mono_byte_ictNone(BCPARMS);     break;
         case imByte:   ic_mono_graybyte_ictNone(BCPARMS); break;
         case imRGB:    ic_mono_rgb_ictNone(BCPARMS);      break;
         case imShort:  ic_MIDCONVERT(mono, short);        break;
         case imLong:   ic_MIDCONVERT(mono, long);         break;
         case imFloat:  ic_MIDCONVERT(mono, float);        break;
         case imDouble: ic_MIDCONVERT(mono, double);       break;
      }
      break; /* imMono */

      case im16: switch( dstType)
      {
         case imMono:
            switch ( var->conversion)
            {
               case ictNone:
                  ic_nibble_mono_ictNone(BCPARMS);     break;
               case ictHalftone:
                  ic_nibble_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            ic_nibble_byte_ictNone(BCPARMS);           break;
         case imByte:
            ic_nibble_graybyte_ictNone(BCPARMS);       break;
         case imRGB:
            ic_nibble_rgb_ictNone(BCPARMS);            break;
         case imShort:  ic_MIDCONVERT(nibble, short);        break;
         case imLong:   ic_MIDCONVERT(nibble, long);         break;
         case imFloat:  ic_MIDCONVERT(nibble, float);        break;
         case imDouble: ic_MIDCONVERT(nibble, double);       break;
      }
      break; /* im16 */

      case im256: switch( dstType)
      {
         case imMono:
            switch ( var->conversion)
            {
               case ictNone:
                  ic_byte_mono_ictNone(BCPARMS);       break;
                  break;
               case ictHalftone:
                  ic_byte_mono_ictHalftone(BCPARMS);   break;
            }
            break;
         case im16:
            switch ( var->conversion)
            {
               case ictNone:
                   ic_byte_nibble_ictNone(BCPARMS);     break;
               case ictHalftone:
                   ic_byte_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case imByte:
            ic_byte_graybyte_ictNone(BCPARMS);          break;
         case imRGB:
            ic_byte_rgb_ictNone(BCPARMS); break;
         case imShort:  ic_MIDCONVERT(byte, short);        break;
         case imLong:   ic_MIDCONVERT(byte, long);         break;
         case imFloat:  ic_MIDCONVERT(byte, float);        break;
         case imDouble: ic_MIDCONVERT(byte, double);       break;
      }
      break; /* im256 */

      case imByte: switch( dstType)
      {
         case imMono:
            switch ( var->conversion)
            {
               case ictNone:
                  ic_graybyte_mono_ictNone(BCPARMS); break;
               case ictHalftone:
                  ic_graybyte_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im16:
            switch ( var->conversion)
            {
               case ictNone:
                   ic_graybyte_nibble_ictNone(BCPARMS); break;
               case ictHalftone:
                   ic_graybyte_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            break;
         case imRGB:
            ic_graybyte_rgb_ictNone(BCPARMS); break;
         case imShort  : ic_Byte_short( BCPARMS);  break;
         case imLong   : ic_Byte_long( BCPARMS);   break;
         case imFloat  : ic_Byte_float( BCPARMS);  break;
         case imDouble : ic_Byte_double( BCPARMS); break;
      }
      break; /* imByte */

      case imShort:  switch ( dstType)
      {
         case imMono  :
            ic_short_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_mono_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im16  :
            ic_short_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_nibble_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            ic_short_Byte(BCPARMS);
            break;
         case imRGB   :
            ic_short_Byte( BCSELFGRAY);
            var->type = imByte;
            ic_graybyte_rgb_ictNone( BCPARMS);
            break;
         case imByte   : ic_short_Byte( BCPARMS);   break;
         case imLong   : ic_short_long( BCPARMS);   break;
         case imFloat  : ic_short_float( BCPARMS);  break;
         case imDouble : ic_short_double( BCPARMS); break;
      }
      break;
      /* imShort */

      case imLong:  switch ( dstType)
      {
         case imMono  :
            ic_long_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_mono_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im16  :
            ic_long_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_nibble_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            ic_long_Byte(BCPARMS);
            break;
         case imRGB   :
            ic_long_Byte( BCSELFGRAY);
            var->type = imByte;
            ic_graybyte_rgb_ictNone( BCPARMS);
            break;
         case imByte   : ic_long_Byte( BCPARMS);   break;
         case imShort  : ic_long_short( BCPARMS);  break;
         case imFloat  : ic_long_float( BCPARMS);  break;
         case imDouble : ic_long_double( BCPARMS); break;
      }
      break;
      /* imLong */

      case imFloat:  switch ( dstType)
      {
         case imMono  :
            ic_float_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_mono_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im16  :
            ic_float_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_nibble_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            ic_float_Byte(BCPARMS);
            break;
         case imRGB   :
            ic_float_Byte( BCSELFGRAY);

            ic_graybyte_rgb_ictNone( BCPARMS);
            break;
         case imByte   : ic_float_Byte( BCPARMS);   break;
         case imShort  : ic_float_short( BCPARMS);  break;
         case imLong   : ic_float_long( BCPARMS);   break;
         case imDouble : ic_float_double( BCPARMS); break;
      }
      break;
      /* imFloat */


      case imDouble:  switch ( dstType)
      {
         case imMono  :
            ic_double_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_mono_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im16  :
            ic_double_Byte( BCSELFGRAY);
            var->type = imByte;
            switch ( var->conversion)
            {
               case ictNone:     ic_graybyte_nibble_ictNone(BCPARMS);     break;
               case ictHalftone: ic_graybyte_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            ic_double_Byte(BCPARMS);
            break;
         case imRGB   :
            ic_double_Byte( BCSELFGRAY);
            var->type = imByte;
            ic_graybyte_rgb_ictNone( BCPARMS);
            break;
         case imByte   : ic_double_Byte( BCPARMS);   break;
         case imShort  : ic_double_short( BCPARMS);  break;
         case imLong   : ic_double_long( BCPARMS);   break;
         case imFloat  : ic_double_float( BCPARMS);  break;
      }
      break;
      /* imDouble */

      case imRGB: switch( dstType)
      {
         case imMono:
            switch ( var->conversion)
            {
               case ictNone:
                  ic_rgb_mono_ictNone(BCPARMS); break;
               case ictHalftone:
                  ic_rgb_mono_ictHalftone(BCPARMS); break;
            }
            break;
         case im16:
            switch ( var->conversion)
            {
               case ictNone:
                  ic_rgb_nibble_ictNone(BCPARMS); break;
               case ictHalftone:
                  ic_rgb_nibble_ictHalftone(BCPARMS); break;
            }
            break;
         case im256:
            switch ( var->conversion)
            {
            case ictNone:
               ic_rgb_byte_ictNone(BCPARMS); break;
            case ictHalftone:
               ic_rgb_byte_ictHalftone(BCPARMS); break;
            }
            break;
         case imByte:
            ic_rgb_graybyte_ictNone(BCPARMS); break;
            break;
         case imShort:  ic_MIDCONVERT(rgb, short);        break;
         case imLong:   ic_MIDCONVERT(rgb, long);         break;
         case imFloat:  ic_MIDCONVERT(rgb, float);        break;
         case imDouble: ic_MIDCONVERT(rgb, double);       break;
      }
      break; /* imRGB */
   }
}

typedef struct _ImageSignatures
{
   int type;
   int size;
   char *sig;
} ImageSignatures;

#define itUnknown (-1)
#define itBMP  0
#define itGIF  1
#define itPCX  2
#define itTIF  3
#define itTGA  4
#define itLBM  5
#define itVID  6
#define itPGM  7
#define itPPM  8
#define itKPS  9
#define itIAX  10
#define itXBM  11
#define itSPR  12
#define itPSG  13
#define itGEM  14
#define itCVP  15
#define itJPG  16
#define itPNG  17

static ImageSignatures signatures[] =
{
   { itBMP, 2, "BM" },
   { itGIF, 6, "GIF87a" },
   { itGIF, 6, "GIF89a" },
   { itPCX, 3, "\x0a\x04\x01" },
   { itPCX, 3, "\x0a\x05\x01" },
   { itTIF, 2, "II" },
   { itTIF, 2, "MM" },
   /* { itTGA, ?, ? }, */
   { itLBM, 4, "FORM" },
   { itVID, 6, "YUV12C" },
   { itPGM, 2, "P5" },
   { itPPM, 2, "P6" },
   { itKPS, 8, "DFIMAG00" },
   /* { itIAX, */
   /* { itXBM, */
   /* { itSPR, */
   /* { itPSG, */
   /* { itGEM, */
   /* { itCVP, */
   { itJPG, 4, "\xff\xd8\xff\xe0" },
   { itJPG, 4, "\xe0\xff\xd8\xff" },
   { itPNG, 8, "\x89PNG\r\n\x1a\n"}
};

#define N_SIGS ( sizeof( signatures) / sizeof( signatures[ 0]))

int
image_guess_type( int fd)
{
   char buf[ 8];
   int i;
   off_t savePos = lseek( fd, 0, SEEK_SET);
   memset( buf, 0, 8);
   read( fd, buf, 8);
   lseek( fd, savePos, SEEK_SET);
   for ( i = 0; i < N_SIGS; i++)
      if ( memcmp( buf, signatures[ i]. sig, signatures[ i]. size) == 0)
         return signatures[ i]. type;
   return itUnknown;
}

void
init_image_support()
{
   cm_init_colormap();
}
