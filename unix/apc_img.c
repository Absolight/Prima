#include "unix/guts.h"
#include "Image.h"
#include "unix/gif_support.h"

/***********************************************************/
/*                                                         */
/*  Prima project                                          */
/*                                                         */
/*  System dependent image routines (unix, x11)            */
/*                                                         */
/*  Copyright (C) 1997,1998 The Protein Laboratory,        */
/*  University of Copenhagen                               */
/*                                                         */
/***********************************************************/

#define APCIMG_VERSION		1

PList imgFormats = nil;
U32 maxPrereadSize = 0;

#define APCERRBUF_LENGTH 1024

#define APCERRT_NONE 0
#define APCERRT_DRIVER 1
#define APCERRT_INTERNAL 2
#define APCERRT_LIBC 3
static int apcErrorType = APCERRT_NONE;

/* apc_image internal error codes */
#define APCIMG_NO_ERROR 0
#define APCIMG_FORMAT_REGISTERED 1
#define APCIMG_EMPTY_FORMAT_LIST 2
#define APCIMG_NOT_ENOUGH_MEMORY 3
#define APCIMG_NO_FORMAT 4
#define APCIMG_INV_PROPERTY_TYPE 5
static int apcErrorCode = APCIMG_NO_ERROR;
static int errDriverIdx = -1;

const char *
apc_image_get_error_message( char *errorMsgBuf, int bufLen)
{
    static char errBuf[ APCERRBUF_LENGTH];
    char *p = errBuf, *msg = "*** error missed ***";
    int len = APCERRBUF_LENGTH;

    if ( errorMsgBuf != NULL) {
	p = errorMsgBuf;
	len = bufLen;
    }

    strncpy( p, "no error", len);
    p[ len - 1] = 0;

    switch ( apcErrorType) {
	case APCERRT_NONE:
	    msg = "no error";
	    break;
	case APCERRT_LIBC:
	    msg = strerror( apcErrorCode);
	    break;
	case APCERRT_INTERNAL:
	    switch ( apcErrorCode) {
		case APCIMG_NO_ERROR:
		    msg = "no error";
		    break;
		case APCIMG_FORMAT_REGISTERED:
		    msg = "image format already registered";
		    break;
		case APCIMG_EMPTY_FORMAT_LIST:
		    msg = "no formats registered";
		    break;
		case APCIMG_NOT_ENOUGH_MEMORY:
		    msg = "not enough memory";
		    break;
		case APCIMG_NO_FORMAT:
		    msg = "no appropriate format found";
		    break;
		case APCIMG_INV_PROPERTY_TYPE:
		    msg = "invalid property type";
		    break;
		default:
		    msg = "*** APC internal: unknown error code";
	    }
	    break;
	case APCERRT_DRIVER:
	    {
		PImgFormat imgf;
		if ( ( errDriverIdx >= 0) && ( errDriverIdx < imgFormats->count)) {
		    fprintf( stderr, "Asking driver %d for error\n", errDriverIdx);
		    imgf = ( PImgFormat) list_at( imgFormats, errDriverIdx);
		    msg = ( char *) imgf->getError( NULL, 0);
		}
		else {
		    msg = "internal error: wrong driver index!";
		}
	    }
	    break;
	default:
	    msg = "*** APC internal: unknown error type";
    }
    strncpy( p, msg, len);
    p[ len - 1] = 0;

    return p;
}

static void
__apc_image_set_error( int type, int code, ...)
{
    apcErrorType = type;
    apcErrorCode = code;
    if ( type == APCERRT_DRIVER) {
	va_list args;
	va_start( args, code);
	errDriverIdx = va_arg( args, int);
	va_end( args);
    }
}

static void
__apc_image_clear_error()
{
    __apc_image_set_error( APCERRT_NONE, APCIMG_NO_ERROR);
}

/* image & bitmaps */
Bool
apc_image_create( Handle self)
{
    fprintf( stderr, "apc_image_create()\n");
    return false;
}

void
apc_image_destroy( Handle self)
{
    fprintf( stderr, "apc_image_destroy()\n");
}

Bool
apc_image_begin_paint( Handle self)
{
    fprintf( stderr, "apc_image_begin_paint()\n");
    return false;
}

Bool
apc_image_begin_paint_info( Handle self)
{
    fprintf( stderr, "apc_image_begin_paint_info()\n");
    return false;
}

void
apc_image_end_paint( Handle self)
{
    fprintf( stderr, "apc_image_end_paint()\n");
}

void
apc_image_end_paint_info( Handle self)
{
    fprintf( stderr, "apc_image_end_paint_info()\n");
}

void
apc_image_update_change( Handle self)
{
    fprintf( stderr, "apc_image_update_change()\n");
}

Bool
apc_dbm_create( Handle self, Bool monochrome)
{
    fprintf( stderr, "apc_dbm_create()\n");
    return false;
}

void
apc_dbm_destroy( Handle self)
{
    fprintf( stderr, "apc_dbm_destroy()\n");
}

static Bool
image_format_exists( Handle item, void *params)
{
    const char *format_name = ( const char *) params;
    PImgFormat imgFormat = ( PImgFormat) item;
    return ( strcasecmp( format_name, imgFormat->id) == 0);
}

/*
 * apc_register_image_format returns 0 on success or error code. For
 * the first time the only error code available is 1.
 */
Bool
apc_register_image_format( int version, PImgFormat imgf)
{
    if ( ( version > 1)
	 || ( version < 1)
	 || ( imgf == nil)
	 || ( imgf->id == nil)
	 || ( imgf->capabilities == nil)
	 || ( imgf->propertyList == nil)
	 || ( imgf->load == nil)
	 || ( imgf->save == nil)
	 || ( imgf->is_loadable == nil)
	 || ( imgf->is_storable == nil)
	 || ( imgf->getInfo == nil)) {
	/* Invalid parameters */
	return 1;
    }

    if ( imgFormats == nil) {
	imgFormats = plist_create( 10, 5);
    }

    if ( list_first_that( imgFormats, image_format_exists, ( void*)imgf->id) >= 0) {
	/* The format already registered */
	__apc_image_set_error( APCERRT_INTERNAL, APCIMG_FORMAT_REGISTERED);
	return false;
    }

    list_add( imgFormats, ( Handle) imgf);

    if ( imgf->preread_size > maxPrereadSize) {
	maxPrereadSize = imgf->preread_size;
    }

    return true;
}

typedef struct {
    const char *filename; /* Image file name */
    int fd;               /* Image file descriptor */
    char *preread_buf;    /* Preread data */
    int preread_size;     /* This field's value could differ from
			     maxPrereadSize depending on result 
			     of read() or further calls to
			     apc_image_fetch_more. */
    const char *desiredFormat;
    int error;            /* Error code if error occured during checks. */
} __ImgLoadData, *__PImgLoadData;

static __PImgLoadData
load_prepare_data()
{
    __PImgLoadData load_data = ( __PImgLoadData) malloc( sizeof( __ImgLoadData));
    if ( load_data != nil) {
	load_data->preread_buf = (load_data->preread_size = maxPrereadSize) > 0 ?
	    ( char*) malloc( load_data->preread_size) :
	    nil;
	load_data->error = 0;
	load_data->desiredFormat =
	load_data->filename = nil;
	load_data->fd = -1; /* The only valid value to indicate uninitialized file descriptor */
    }
    else {
	__apc_image_set_error( APCERRT_INTERNAL, APCIMG_NOT_ENOUGH_MEMORY);
    }
    return load_data;
}

static void
load_cleanup_data( __PImgLoadData *load_data)
{
    if ( *load_data != nil) {
	if ( ( *load_data)->preread_buf != nil) {
	    free( ( *load_data)->preread_buf);
	}
	free( *load_data);
	*load_data = nil;
    }
}

static Bool
load_img_loadable( Handle item, void *params)
{
    __PImgLoadData load_data = ( __PImgLoadData) params;
    PImgFormat imgFormat = ( PImgFormat) item;
    fprintf( stderr, "checking %s whether it is loadable as %s\n", load_data->filename, imgFormat->id);
    if ( ( load_data->preread_size < imgFormat->preread_size)
	 || ( ( load_data->desiredFormat != nil)
	      && ( strcasecmp( load_data->desiredFormat, imgFormat->id) != 0))) {
	return false;
    }
    return imgFormat->is_loadable( load_data->fd, 
				   load_data->filename, 
				   load_data->preread_buf, 
				   load_data->preread_size);
}

Bool
__apc_image_correct_properties( PList propList, PImgFormat imgFormat)
{
   PImgProps fmtProps = imgFormat->propertyList;
   int i, j, n;
   Bool rc = true;

   for ( i = ( propList->count - 1); ( i >= 0) && rc; i--) {
       PImgProperty imgProp = ( PImgProperty) list_at( propList, i);
       for ( j = 0; fmtProps[ j].id; j++) {
	   if ( strcmp( imgProp->id, fmtProps[ j].id) == 0) {
	       break;
	   }
       }
       if ( fmtProps[ j].id) {
	   if ( ( ( fmtProps[ j].type[ 1] == '*') && imgProp->size == -1)
		|| ( ( fmtProps[ j].type[ 1] == '\0') && ( imgProp->size >= 0))) {
	       __apc_image_set_error( APCERRT_INTERNAL, APCIMG_INV_PROPERTY_TYPE);
	       fprintf( stderr, "Property %s: wrong type\n", imgProp->id);
	       rc = false;
	   }
	   else {
	       switch ( fmtProps[ j].type[ 0]) {
		   case 'i':
		       if ( fmtProps[ j].type[ 0] == '*') {
			   int *props = imgProp->size ? malloc( sizeof( int) * imgProp->size) : nil;
			   for ( n = 0; n < imgProp->size; n++) {
			       props[ n] = atoi( imgProp->val.pString[ n]);
			       free( imgProp->val.pString[ n]);
			   }
			   free( imgProp->val.pString);
			   imgProp->val.pInt = props;
		       }
		       else {
			   int prop = atoi( imgProp->val.String);
			   free( imgProp->val.String);
			   imgProp->val.Int = prop;
		       }
		       break;
		   case 'n':
		       if ( fmtProps[ j].type[ 0] == '*') {
			   double *props = imgProp->size ? malloc( sizeof( double) * imgProp->size) : nil;
			   for ( n = 0; n < imgProp->size; n++) {
			       props[ n] = atof( imgProp->val.pString[ n]);
			       free( imgProp->val.pString[ n]);
			   }
			   free( imgProp->val.pString);
			   imgProp->val.pDouble = props;
		       }
		       else {
			   double prop = atof( imgProp->val.String);
			   free( imgProp->val.String);
			   imgProp->val.Double = prop;
		       }
		       break;
		   case 'b':
		       if ( fmtProps[ j].type[ 0] == '*') {
			   Byte *props = imgProp->size ? malloc( sizeof( Byte) * imgProp->size) : nil;
			   for ( n = 0; n < imgProp->size; n++) {
			       props[ n] = atoi( imgProp->val.pString[ n]);
			       free( imgProp->val.pString[ n]);
			   }
			   free( imgProp->val.pString);
			   imgProp->val.pByte = props;
		       }
		       else {
			   Byte prop = atoi( imgProp->val.String);
			   free( imgProp->val.String);
			   imgProp->val.Byte = prop;
		       }
		       break;
	       }
	   }
       }
       else {
	   list_delete_at( propList, i);
       }
   }

   fprintf( stderr, "__apc_image_set_error: returning %d\n", ( int) rc);
   return rc;
}

/*
 * noLoad - false value means that only information about the image should be obtained.
 *          I.e. getInfo call must be issued.
 */
Bool
apc_image_read( const char *filename, PList imgInfo, Bool readData)
{
    Bool rc = false;
    char *format = nil;
    __apc_image_clear_error();
    if ( imgFormats != nil) {
	__PImgLoadData load_data = load_prepare_data();
	if ( load_data != nil) {
	    load_data->filename = filename;
	    load_data->desiredFormat = format;
/* Local hack. Must be removed in future! */
#ifndef O_BINARY
#define O_BINARY 0
#endif
	    load_data->fd = open( load_data->filename, O_RDONLY | O_BINARY);
	    if ( load_data->fd != -1) {
		load_data->preread_size = read( load_data->fd, load_data->preread_buf, load_data->preread_size);
		if ( load_data->preread_size != -1) {
		    int format_idx = list_first_that( imgFormats, load_img_loadable, load_data);
		    fprintf( stderr, "format_idx: %d\n", format_idx);
		    if ( format_idx != -1) {
			int i;
			Bool correction_succeed = true;
			PImgFormat imgFormat = ( PImgFormat) list_at( imgFormats, format_idx);
			fprintf( stderr, "%s for %s as %s\n", 
				 ( readData ? "Loading image" : "Getting info"), 
				 load_data->filename, 
				 imgFormat->id
			    );
			for ( i = 0; ( i < imgInfo->count) && correction_succeed; i++) {
			    PImgInfo imageInfo = ( PImgInfo) list_at( imgInfo, i);
			    correction_succeed = __apc_image_correct_properties( imageInfo->propList, imgFormat);
			}
			if ( correction_succeed) {
			    rc = ( readData ?
				   imgFormat->load( load_data->fd, load_data->filename, imgInfo) :
				   imgFormat->getInfo( load_data->fd, load_data->filename, imgInfo));
			    if ( ! rc) {
				fprintf( stderr, "Error in driver %d\n", format_idx);
				__apc_image_set_error( APCERRT_DRIVER, 0, format_idx);
			    }
			}
		    }
		    else {
			__apc_image_set_error( APCERRT_INTERNAL, APCIMG_NO_FORMAT);
		    }
		}
		else {
		    __apc_image_set_error( APCERRT_LIBC, errno);
		}
	    }
	    else {
		__apc_image_set_error( APCERRT_LIBC, errno);
	    }
	    load_cleanup_data( &load_data);
	}
    }
    else {
	__apc_image_set_error( APCERRT_INTERNAL, APCIMG_EMPTY_FORMAT_LIST);
    }
    fprintf( stderr, "apc_image_read: returning %d\n", ( int) rc);
    return rc;
}

typedef struct {
    const char *filename;
    PList imgInfo;
    const char *desiredFormat;
} __ImgSaveData, *__PImgSaveData;

static __PImgSaveData
save_prepare_data()
{
    __PImgSaveData save_data = ( __PImgSaveData) malloc( sizeof( __ImgSaveData));
    if ( save_data != nil) {
	save_data->filename = nil;
    }
    return save_data;
}

static void
save_cleanup_data( __PImgSaveData *save_data)
{
    if ( *save_data != nil) {
	free( *save_data);
	*save_data = nil;
    }
}

static Bool
save_img_storable( Handle item, void *params)
{
    PImgFormat imgFormat = ( PImgFormat) item;
    __PImgSaveData save_data = ( __PImgSaveData) params;
    if ( ( save_data->desiredFormat != nil)
	 && ( strcasecmp( save_data->desiredFormat, imgFormat->id) != 0)) {
	return false;
    }
    return imgFormat->is_storable( save_data->filename, save_data->imgInfo);
}

Bool
apc_image_save( const char *filename, const char *format, PList imgInfo)
{
    Bool rc = false;
    __apc_image_clear_error();
    if ( ( imgInfo != nil) && ( filename != nil)) {
	__PImgSaveData save_data = save_prepare_data();
	if ( save_data != nil) {
	    int format_idx;
	    save_data->filename = filename;
	    save_data->desiredFormat = format;
	    save_data->imgInfo = imgInfo;
	    format_idx = list_first_that( imgFormats, save_img_storable, ( void*) save_data);
	    if ( format_idx != -1) {
		PImgFormat imgFormat = ( PImgFormat) list_at( imgFormats, format_idx);
		rc = ( imgFormat->save( save_data->filename, save_data->imgInfo) == 0);
	    }
	    save_cleanup_data( &save_data);
	}
    }
    return rc;
}

/* This is driver suppport only function. */
Bool
apc_image_fetch_more( __PImgLoadData load_data, int preread_size)
{
    char *newbuf;
    int rd;
    Bool rc = false;

    if ( preread_size <= load_data->preread_size) {
	return true;
    }
    if ( lseek( load_data->fd, 0, SEEK_SET) != -1) {
	if ( ( newbuf = ( char *) malloc( preread_size)) != nil) {
	    rd = read( load_data->fd, newbuf, preread_size);
	    if ( rd < preread_size) {
		free( newbuf);
	    }
	    else {
		free( load_data->preread_buf);
		load_data->preread_buf = newbuf;
		load_data->preread_size = preread_size;
		rc = true;
	    }
	}
    }
    return rc;
}

PImgProperty
apc_image_add_property( PImgInfo imageInfo, const char *propName, int propArraySize)
{
    PImgProperty imgProp;

    if ( ( imageInfo == nil) || ( imageInfo->propList == nil) || ( propName == nil)) {
	return nil;
    }

    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    if ( imgProp != nil) {
	imgProp->id = duplicate_string( propName);
	imgProp->size = propArraySize;
	list_add( imageInfo->propList, ( Handle) imgProp);
    }

    return imgProp;
}

void
prima_init_image_subsystem( void)
{
   apc_register_image_format( APCIMG_VERSION, &gifFormat);
}

void
___apc_image_test()
{
    PList imgInfo = nil;
    PImgInfo imageInfo;
    PImgProperty imgProp;

    fprintf( stderr, "___apc_image_test\n");
    fprintf( stderr, "registering GIF format: %s\n", apc_register_image_format( APCIMG_VERSION, &gifFormat) ? "ok" : "failed");

    imgInfo = plist_create( 5, 5);

    imageInfo = ( PImgInfo) malloc( sizeof( ImgInfo));
    imageInfo->propList = plist_create( 5, 5);
    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    imgProp->id = duplicate_string( "index");
    imgProp->size = -1;
    imgProp->val.Int = 5;
    list_add( imageInfo->propList, ( Handle) imgProp);
    list_add( imgInfo, ( Handle) imageInfo);
    fprintf( stderr, "First one\n");

    imageInfo = ( PImgInfo) malloc( sizeof( ImgInfo));
    imageInfo->propList = plist_create( 5, 5);
    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    imgProp->id = duplicate_string( "index");
    imgProp->size = -1;
    imgProp->val.Int = 2;
    list_add( imageInfo->propList, ( Handle) imgProp);
    list_add( imgInfo, ( Handle) imageInfo);

    imageInfo = ( PImgInfo) malloc( sizeof( ImgInfo));
    imageInfo->propList = plist_create( 5, 5);
    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    imgProp->id = duplicate_string( "index");
    imgProp->size = -1;
    imgProp->val.Int = 10;
    list_add( imageInfo->propList, ( Handle) imgProp);
    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    imgProp->id = duplicate_string( "index1");
    imgProp->size = -1;
    imgProp->val.Int = 16;
    list_add( imageInfo->propList, ( Handle) imgProp);
    list_add( imgInfo, ( Handle) imageInfo);

    imageInfo = ( PImgInfo) malloc( sizeof( ImgInfo));
    imageInfo->propList = plist_create( 5, 5);
    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    imgProp->id = duplicate_string( "index");
    imgProp->size = -1;
    imgProp->val.Int = 1;
    list_add( imageInfo->propList, ( Handle) imgProp);
    list_add( imgInfo, ( Handle) imageInfo);

    imageInfo = ( PImgInfo) malloc( sizeof( ImgInfo));
    imageInfo->propList = plist_create( 5, 5);
    imgProp = ( PImgProperty) malloc( sizeof( ImgProperty));
    list_add( imgInfo, ( Handle) imageInfo);

    fprintf( stderr, "Getting info about image...\n");

//    apc_image_read( "/home/voland/src/home_page/ico/email_anim.gif", nil, imgInfo, true);
    if ( ! apc_image_read( "/home/voland/tmp/eagle8.gif", imgInfo, true)) {
//    if ( ! apc_image_read( "/home/voland/tmp/powerlogo.bmp", nil, imgInfo, true)) {
	fprintf( stderr, "getinfo failed: ");
	fprintf( stderr, "%s\n", apc_image_get_error_message( NULL, 0));
    }
}
