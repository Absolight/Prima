#include "apricot.h"
#include "unix/gif_support.h"
#include <unistd.h>
#include <signal.h>

#define GIFPROP_WIDTH 0
#define GIFPROP_HEIGHT 1
#define GIFPROP_TYPE 2
#define GIFPROP_LINESIZE 3
#define GIFPROP_DATA 4
#define GIFPROP_PALETTE 5
#define GIFPROP_INDEX 6
#define GIFPROP_FORMAT 7
#define GIFPROP_INTERLACED 8
#define GIFPROP_X 9
#define GIFPROP_Y 10
#define GIFPROP_COLORRESOLUTION 11
#define GIFPROP_BGCOLOR 12
#define GIFPROP_PALETTESIZE 13
#define GIFPROP_PALETTEBPP 14
#define GIFPROP_NAME 15

ImgFormat gifFormat = {
    "GIF", "Graphics Interchange Format",
    ( ImgCapInfo[]) { 
	{ "type", "i*", "Valid image formats list", 2, { pInt:( int[]){ 
	    im256, 
	    imByte}}},
	{ "multi", "i", "Supports multiple images", 0, { Int:1}},
	{ nil, nil, nil, 0, { Int:0}}
    },
    ( ImgProps[]) {
	{ "width", GIFPROP_WIDTH, "i", "Image width"}, /* For index == -1 means screen width */
	{ "height", GIFPROP_HEIGHT, "i", "Image height"}, /* For index == -1 means screen height */
	{ "type", GIFPROP_TYPE, "i", "Image type (bpp)"}, 
	{ "name", GIFPROP_NAME, "s", "Image-specific description"}, /* Only returned by read */
	{ "lineSize", GIFPROP_LINESIZE, "i", "Length of a scan line in bytes"},
	{ "data", GIFPROP_DATA, "b*", "Image data"},
	{ "palette", GIFPROP_PALETTE, "b*", "Image palette"},
	{ "index", GIFPROP_INDEX, "i", "Position in image list"}, /* -1 refers to the whole image file */
	{ "format", GIFPROP_FORMAT, "s", "File format"}, /* May appear only for an image with index == -1 */
	{ "interlaced", GIFPROP_INTERLACED, "i", "Image is interlaced"},
	{ "X", GIFPROP_X, "i", "Left offset"},
	{ "Y", GIFPROP_Y, "i", "Top offset"},
	{ "colorResolution", GIFPROP_COLORRESOLUTION, "i", "How many colors can be generated"},
	{ "bgColor", GIFPROP_BGCOLOR, "i", "Background color"},
	{ "paletteSize", GIFPROP_PALETTESIZE, "i", "Number of entries in image palette"},
	{ "paletteBPP", GIFPROP_PALETTEBPP, "i", "Image palette BPP"},
	{ nil, 0, nil, nil}
    },
    3,
    __gif_load,
    __gif_save,
    __gif_loadable,
    __gif_storable,
    __gif_getinfo,
    __gif_geterror
};

#define GIFERR_MSGLEN           1024

#define GIFERRT_NONE            0
#define GIFERRT_LIBC            1
#define GIFERRT_GIFLIB          2
#define GIFERRT_DRIVER          3
static int errorType = GIFERRT_NONE;

/* Driver error codes */
#define DERR_NO_ERROR                           0
#define DERR_WRONG_PROPERTY_TYPE                1
#define DERR_NOT_ENOUGH_MEMORY                  2
#define DERR_INVALID_INDEX                      3
#define DERR_DATA_NOT_PRESENT                   4 /* No image data for queried index. */
#define DERR_NO_INDEX_PROP                      5 /* Internal: `index' property not found while filling in imgInfo. */


static int errorCode = DERR_NO_ERROR;

static int interlaceOffs[] = { 0, 4, 2, 1};
static int interlaceStep[] = { 8, 8, 4, 2};

static void
__gif_seterror( int type, int code)
{
    errorType = type;
    errorCode = code;
}

const char *
__gif_geterror( char *errorMsgBuf, int bufLen)
{
    static char errBuf[ GIFERR_MSGLEN];
    char *p = errBuf, *msg = "*** GIF error missed ***";
    int len = GIFERR_MSGLEN;

    if ( errorMsgBuf != NULL) {
	p = errorMsgBuf;
	len = bufLen;
    }

    switch ( errorType) {
	case GIFERRT_NONE:
	    msg = "no error";
	    break;
	case GIFERRT_LIBC:
	    msg = strerror( errorCode);
	    break;
	case GIFERRT_DRIVER:
	    switch ( errorCode) {
		case DERR_NO_ERROR:
		    msg = "no error";
		    break;
		case DERR_WRONG_PROPERTY_TYPE:
		    msg = "wrong property type";
		    break;
		case DERR_NOT_ENOUGH_MEMORY:
		    msg = "not enough memory";
		    break;
		case DERR_DATA_NOT_PRESENT:
		    msg = "*** GIF internal: no image data for the index";
		    break;
		case DERR_NO_INDEX_PROP:
		    msg = "*** GIF internal: no `index' property found while filling in images list";
		    break;
		default:
		    msg = "*** GIF internal: unknown error code";
	    }
	    break;
	case GIFERRT_GIFLIB:
	    switch ( errorCode) {
		case 0:
		    msg = "no error";
		    break;
		case D_GIF_ERR_OPEN_FAILED:
		case E_GIF_ERR_OPEN_FAILED:
		    msg = "open failed";
		    break;
		case D_GIF_ERR_READ_FAILED:
		    msg = "read failed";
		    break;
		case D_GIF_ERR_NOT_GIF_FILE:
		    msg = "not a valid GIF file";
		    break;
		case D_GIF_ERR_NO_SCRN_DSCR:
		    msg = "no screen description block in the file";
		    break;
		case D_GIF_ERR_NO_IMAG_DSCR:
		    msg = "no image description block in the file";
		    break;
		case D_GIF_ERR_NO_COLOR_MAP:
		    msg = "no color map in the file";
		    break;
		case D_GIF_ERR_WRONG_RECORD:
		    msg = "wrong record type";
		    break;
		case D_GIF_ERR_DATA_TOO_BIG:
		    msg = "queried data size is too big";
		    break;
		case D_GIF_ERR_NOT_ENOUGH_MEM:
		case E_GIF_ERR_NOT_ENOUGH_MEM:
		    msg = "not enough memory";
		    break;
		case D_GIF_ERR_CLOSE_FAILED:
		case E_GIF_ERR_CLOSE_FAILED:
		    msg = "close failed";
		    break;
		case D_GIF_ERR_NOT_READABLE:
		    msg = "file is not readable";
		    break;
		case D_GIF_ERR_IMAGE_DEFECT:
		    msg = "image defect detected";
		    break;
		case D_GIF_ERR_EOF_TOO_SOON:
		    msg = "unexpected end of file reached";
		    break;
		case E_GIF_ERR_WRITE_FAILED:
		    msg = "write failed";
		    break;
		case E_GIF_ERR_HAS_SCRN_DSCR:
		    msg = "screen descriptor already been set";
		    break;
		case E_GIF_ERR_HAS_IMAG_DSCR:
		    msg = "image descriptor is still active";
		    break;
		case E_GIF_ERR_NO_COLOR_MAP:
		    msg = "no color map specified";
		    break;
		case E_GIF_ERR_DATA_TOO_BIG:
		    msg = "data is too big (exceeds size of the image)";
		    break;
		case E_GIF_ERR_DISK_IS_FULL:
		    msg = "storage capcity exceeded";
		    break;
		case E_GIF_ERR_NOT_WRITEABLE:
		    msg = "file opened not for writing";
		    break;
		default:
		    msg = "unknown error";
	    }
	    break;
	default:
	    msg = "*** GIF internal: unknown error type";
    }
    strncpy( p, msg, len);
    p[ len - 1] = 0;

    return p;
}

static Bool
property_name( Handle item, void *params)
{
    PImgProperty imgProp = ( PImgProperty) item;
    const char *propName = ( const char *)params;
    return ( strcmp( imgProp->name, propName) == 0);
}

static Bool
__gif_read( int fd, const char *filename, PList imgInfo, Bool readData, Bool readAll)
{
    GifFileType *gif;
    GifRecordType gifType;
    GifImageDesc *gifChunk;
    GifPixelType **data;
    int lastIndex = -1;
    Bool succeed = true;
    int imgDescCount = 0;
    int i, gifrc;
    Bool done;
    List gifChunks, queriedIdx, imageData;

/*      fprintf( stderr, "Got %d profile(s)\n", imgInfo->count); */
    if ( readAll) {
	/* If been queried for all images then there must no more than one image profile 
	   in the imgInfo list. If it contains properties we must remove them. */
	PImgInfo imageInfo = ( PImgInfo) list_at( imgInfo, 0);
	for ( i = ( imageInfo->propList->count - 1); i >= 0; i++) {
	    PImgProperty imgProp = ( PImgProperty) list_at( imageInfo->propList, i);
	    apc_image_clear_property( imgProp);
	    free( imgProp);
	    list_delete_at( imageInfo->propList, i);
	}
    }
    else {
	list_create( &queriedIdx, imgInfo->count, 1);
	for ( i = 0; ( i < imgInfo->count) && succeed; i++) {
	    int indexPropIdx;
	    PImgProperty imgProp;
	    PImgInfo imageInfo = ( PImgInfo) list_at( imgInfo, i);
/*  	    fprintf( stderr, "Image #%d: %d properties\n", i, imageInfo->propList->count); */
	    indexPropIdx = list_first_that( imageInfo->propList, property_name, ( void*) "index");
	    if ( indexPropIdx >= 0) {
		imgProp = ( PImgProperty) list_at( imageInfo->propList, indexPropIdx);
	    }
	    else {
		imgProp = apc_image_add_property( imageInfo, "index", PROPTYPE_INT, -1);
		imgProp->val.Int = ++lastIndex;
	    }
	    if ( imgProp->size == -1) {
		if ( imgProp->val.Int > lastIndex) {
		    lastIndex = imgProp->val.Int;
		}
		list_add( &queriedIdx, imgProp->val.Int);
	    }
	    else {
		/* ERROR: The property contains an array. */
		__gif_seterror( GIFERRT_DRIVER, DERR_WRONG_PROPERTY_TYPE);
		succeed = false;
		continue;
	    }
	}
    }

    if ( ! succeed) {
	return false;
    }

/*      fprintf( stderr, "The last index is %d, queried for %d indexes\n", lastIndex, queriedIdx.count); */

/*      fprintf( stderr, "opening %s (%s)\n", filename, readAll ? "all images" : "selected"); */

    gif = DGifOpenFileName( filename);
    if ( gif == NULL) {
	__gif_seterror( GIFERRT_GIFLIB, GifLastError());
	return false;
    }

    list_create( &gifChunks, readAll ? 1 : queriedIdx.count, readAll ? 5 : 1);
    if ( readData) {
	list_create( &imageData, readAll ? 1 : queriedIdx.count, readAll ? 5 : 1);
    }

    done = false;
    do {
	Byte *extData;
	int extCode;

	gifrc = DGifGetRecordType( gif, &gifType);
	if ( gifrc != GIF_OK) {
/*  	    fprintf( stderr, "*** Can't get next record type\n"); */
	    __gif_seterror( GIFERRT_GIFLIB, GifLastError());
	    done = true;
	    succeed = false;
	    break;
	}

	/* We must add an entry for every image in the file... */
	while ( gifChunks.count <= imgDescCount) {
	    gifChunk = malloc( sizeof( GifImageDesc));
	    gifChunk->ColorMap = NULL;
	    list_add( &gifChunks, ( Handle) gifChunk);
	}
	if ( readData) {
	    /* And the same for the image content. */
	    while ( imageData.count <= imgDescCount) {
		data = malloc( sizeof( GifPixelType *));
		*data = NULL;
		list_add( &imageData, ( Handle) data);
	    }
	}

	switch ( gifType) {
	    case SCREEN_DESC_RECORD_TYPE:
		fprintf( stderr, "Screen record\n");
		break;
	    case TERMINATE_RECORD_TYPE:
		fprintf( stderr, "Termination record\n");
		break;
	    case IMAGE_DESC_RECORD_TYPE:
		fprintf( stderr, "Description record\n");
		gifrc = DGifGetImageDesc( gif);
		if ( gifrc != GIF_OK) {
		    __gif_seterror( GIFERRT_GIFLIB, GifLastError());
		    succeed = false;
		}
		else {
		    int codeSize;
		    Byte *codeBlock;
		    Bool idxQueried = readAll; /* Presume that we need all indexes... */

		    for ( i = 0; ( i < queriedIdx.count) && ( ! idxQueried); i++) {
			idxQueried = ( imgDescCount == list_at( &queriedIdx, i));
		    }

		    gifChunk = ( GifImageDesc *) list_at( &gifChunks, imgDescCount);
		    memcpy( ( void *) gifChunk, ( void *) &gif->Image, sizeof( GifImageDesc));
		    if ( ( gif->Image.ColorMap != NULL) && idxQueried) {
			gifChunk->ColorMap = MakeMapObject( gif->Image.ColorMap->ColorCount, gif->Image.ColorMap->Colors);
		    }
		    else {
			gifChunk->ColorMap = NULL;
		    }
		    fprintf( stderr, "Image count: %d; Scr: %dx%d, Img: %dx%d+%d+%d, ColorRes: %d\n", 
			     gif->ImageCount,
			     gif->SWidth,
			     gif->SHeight,
			     gifChunk->Width,
			     gifChunk->Height,
			     gifChunk->Left,
			     gifChunk->Top,
			     gif->SColorResolution
			);
		    if ( idxQueried && readData) {
			int pixelCount = gifChunk->Width * gifChunk->Height;
			data = ( GifPixelType **) list_at( &imageData, imgDescCount);
			*data = malloc( sizeof( GifPixelType) * pixelCount);
			if ( *data == NULL) {
			    __gif_seterror( GIFERRT_DRIVER, DERR_NOT_ENOUGH_MEMORY);
			    succeed = false;
			}
			else {
			    gifrc = DGifGetLine( gif, *data, pixelCount);
			}
		    }
		    else {
			gifrc = DGifGetCode( gif, &codeSize, &codeBlock);
			if ( gifrc == GIF_OK) {
			    while( ( codeBlock != NULL) && ( gifrc == GIF_OK)) {
				fprintf( stderr, ".");
				gifrc = DGifGetCodeNext( gif, &codeBlock);
			    }
			    fprintf( stderr, " ");
			}
		    }
		    if ( gifrc != GIF_OK) {
			__gif_seterror( GIFERRT_GIFLIB, GifLastError());
			succeed = false;
		    }
		    imgDescCount++;
		}
		break;
	    case EXTENSION_RECORD_TYPE:
		fprintf( stderr, "Extension record: ");
		extCode = -1;
		gifrc = DGifGetExtension( gif, &extCode, &extData);
		fprintf( stderr, "code is %02x, ", extCode);
		if ( gifrc == GIF_OK) {
		    do {
			int k;
			fprintf( stderr, "supposed to contain %d bytes {", ( int ) extData[ 0]);
			for ( k = 0; k < ( int ) extData[ 0]; k++) {
			    fprintf( stderr, "0x%02x(%c) ", extData[ k + 1], isprint( extData[ k + 1]) ? extData[ k + 1] : '?');
			}
			fprintf( stderr, "};");
			gifrc = DGifGetExtensionNext( gif, &extData);
		    } while ( ( extData != NULL) && ( gifrc == GIF_OK));
		}
		fprintf( stderr, "\n");
		if ( gifrc != GIF_OK) {
		    __gif_seterror( GIFERRT_GIFLIB, GifLastError());
		    succeed = false;
		}
		break;
	    default:
		fprintf( stderr, "Unknown record type\n");
	}
	done = done || ( ! succeed) || ( ( imgDescCount > lastIndex) && ( ! readAll));
    } while( ( gifType != TERMINATE_RECORD_TYPE) && ( ! done));

    if ( succeed) {
	fprintf( stderr, "Got %d images.\n", imgDescCount);

	if ( readAll) {
	    /* Now we must create slots for all the images have been readed. */
	    /* The first slot is already here. We can just fill it in. */
	    /* For all subsequent images the first one will be used as a template. */
	    PImgProperty imgProp;
	    PImgInfo firstImg = ( PImgInfo) list_at( imgInfo, 0);
	    imgProp = apc_image_add_property( firstImg, "index", PROPTYPE_INT, -1);
	    imgProp->val.Int = 0;

	    for ( i = 1; i < imgDescCount; i++) {
		PImgInfo imageInfo = malloc( sizeof( ImgInfo));
		imageInfo->extraInfo = firstImg->extraInfo;
		imageInfo->propList = plist_create( 15, 5);
		imgProp = apc_image_add_property( imageInfo, "index", PROPTYPE_INT, -1);
		imgProp->val.Int = i;
		list_add( imgInfo, ( Handle) imageInfo);
	    }
	}

	fprintf( stderr, "Have %d profile(s)\n", imgInfo->count);

	/* At this stage we must have fully prepared imgInfo list which all the items 
	   already contains valid `index' properties. */
	for ( i = 0; ( i < imgInfo->count) && succeed; i++) {
	    int indexPropIdx = -1, j, index = -1;
	    PImgInfo imageInfo;
	    imageInfo = ( PImgInfo) list_at( imgInfo, i);
	    indexPropIdx = list_first_that( imageInfo->propList, property_name, ( void *) "index");
	    if ( indexPropIdx >= 0) {
		index = ( ( PImgProperty) list_at( imageInfo->propList, indexPropIdx))->val.Int;
	    }
	    else {
		__gif_seterror( GIFERRT_DRIVER, DERR_NO_INDEX_PROP);
		succeed = false;
		continue;
	    }
	    fprintf( stderr, "idx: %d\n", index);
	    for ( j = 0; j < imageInfo->propList->count; j++) {
		apc_image_clear_property( ( PImgProperty) list_at( imageInfo->propList, j));
	    }
	    plist_destroy( imageInfo->propList);
	    imageInfo->propList = plist_create( 5, 5);

	    gifChunk = ( GifImageDesc *) list_at( &gifChunks, index);
	    if ( readData) {
		data = ( GifPixelType **) list_at( &imageData, index);
	    }

	    if ( index == -1) {
		PImgProperty imgProp;

		imgProp = apc_image_add_property( imageInfo, "index", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = -1;

		imgProp = apc_image_add_property( imageInfo, "format", PROPTYPE_STRING, -1);
		if ( imgProp != NULL) imgProp->val.String = duplicate_string( gifFormat.id);

		imgProp = apc_image_add_property( imageInfo, "width", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = gif->SWidth;

		imgProp = apc_image_add_property( imageInfo, "height", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = gif->SHeight;

		imgProp = apc_image_add_property( imageInfo, "type", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = im256;

		imgProp = apc_image_add_property( imageInfo, "colorResolution", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = gif->SColorResolution;

		imgProp = apc_image_add_property( imageInfo, "bgColor", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = gif->SBackGroundColor;

		if ( gif->SColorMap != NULL) {
		    int n;
		    imgProp = apc_image_add_property( imageInfo, "paletteSize", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gif->SColorMap->ColorCount;

		    imgProp = apc_image_add_property( imageInfo, "paletteBPP", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gif->SColorMap->ColorCount;

		    imgProp = apc_image_add_property( imageInfo, "palette", PROPTYPE_BYTE, 3 * gif->SColorMap->ColorCount);
		    imgProp->val.pByte = ( Byte *) malloc( imgProp->size);
		    for ( n = 0; n < gif->SColorMap->ColorCount; n++) {
			Byte *palEntry = ( imgProp->val.pByte + n * 3);
			palEntry[ 0] = gif->SColorMap->Colors[ n].Blue;
			palEntry[ 1] = gif->SColorMap->Colors[ n].Green;
			palEntry[ 2] = gif->SColorMap->Colors[ n].Red;
		    }
		}
	    }
	    else if ( index >= 0) {
		PImgProperty imgProp;
		ColorMapObject *palette;

		imgProp = apc_image_add_property( imageInfo, "index", PROPTYPE_INT, -1);
		if ( imgProp != NULL) imgProp->val.Int = index;

		imgProp = apc_image_add_property( imageInfo, "format", PROPTYPE_STRING, -1);
		if ( imgProp != NULL) imgProp->val.String = duplicate_string( gifFormat.id);

		if ( index < imgDescCount) {
		    int n;

		    imgProp = apc_image_add_property( imageInfo, "width", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gifChunk->Width;

		    imgProp = apc_image_add_property( imageInfo, "height", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gifChunk->Height;

		    imgProp = apc_image_add_property( imageInfo, "type", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = im256;

		    imgProp = apc_image_add_property( imageInfo, "X", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gifChunk->Left;

		    imgProp = apc_image_add_property( imageInfo, "Y", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gifChunk->Top;

		    imgProp = apc_image_add_property( imageInfo, "interlaced", PROPTYPE_INT, -1);
		    if ( imgProp != NULL) imgProp->val.Int = gifChunk->Interlace;

		    palette = ( gifChunk->ColorMap != NULL ?
				gifChunk->ColorMap :
				gif->SColorMap);

		    imgProp = apc_image_add_property( imageInfo, "palette", PROPTYPE_BYTE, 3 * palette->ColorCount);
		    imgProp->val.pByte = malloc( imgProp->size);
		    for ( n = 0; n < palette->ColorCount; n++) {
			Byte *palEntry = ( imgProp->val.pByte + n * 3);
			palEntry[ 0] = palette->Colors[ n].Blue;
			palEntry[ 1] = palette->Colors[ n].Green;
			palEntry[ 2] = palette->Colors[ n].Red;
		    }

		    if ( readData) {
			char *buf = malloc( PATH_MAX + 16);
			imgProp = apc_image_add_property( imageInfo, "name", PROPTYPE_STRING, -1);
			snprintf( buf, PATH_MAX + 16, "%s:%d", filename, index);
			imgProp->val.String = buf;

			if ( *data != NULL) {
			    int pixelCount = gifChunk->Width * gifChunk->Height;
			    int pass = 0, Y; /* Pass number for interlaced images. */
			    imgProp = apc_image_add_property( imageInfo, "data", PROPTYPE_BYTE, pixelCount);
			    if ( imgProp != NULL) {
				int ySrc, yDst;
				imgProp->val.pByte = malloc( sizeof( Byte) * pixelCount);
				for ( ySrc = 0, yDst = 0, Y = 0; 
				      ySrc < pixelCount;
				      ySrc += gifChunk->Width) {
				    memcpy( imgProp->val.pByte + pixelCount - yDst - gifChunk->Width,
					    *data + ySrc,
					    gifChunk->Width
					);
				    if ( gifChunk->Interlace) {
					yDst += gifChunk->Width * interlaceStep[ pass];
					Y += interlaceStep[ pass];
					if ( yDst >= pixelCount) {
					    pass++;
					    Y = interlaceOffs[ pass];
					    yDst = gifChunk->Width * interlaceOffs[ pass];
					}
				    }
				    else {
					yDst += gifChunk->Width;
				    }
				}
			    }
			    imgProp = apc_image_add_property( imageInfo, "lineSize", PROPTYPE_INT, -1);
			    if ( imgProp != NULL) imgProp->val.Int = gifChunk->Width;
			}
			else {
			    __gif_seterror( GIFERRT_DRIVER, DERR_DATA_NOT_PRESENT);
			    succeed = false;
			}
		    }
		}
	    }
	    else {
		__gif_seterror( GIFERRT_DRIVER, DERR_INVALID_INDEX);
		succeed = false;
	    }
	}
    }
    else { 
	fprintf( stderr, "*** FAILED!!! ***\n");
    }

    gifrc = DGifCloseFile( gif);
    if ( ( gifrc != GIF_OK) && succeed) {
	__gif_seterror( GIFERRT_GIFLIB, GifLastError());
	succeed = false;
    }

    for ( i = 0; i < gifChunks.count; i++) {
	gifChunk = ( GifImageDesc *) list_at( &gifChunks, i);
	if ( gifChunk->ColorMap != NULL) {
	    FreeMapObject( gifChunk->ColorMap);
	}
	free( gifChunk);

	data = ( GifPixelType **) list_at( &imageData, i);
	if ( *data) {
	    free( *data);
	}
	free( data);
    }
    list_destroy( &gifChunks);
    list_destroy( &imageData);
    if ( ! readAll) {
	list_destroy( &queriedIdx);
    }

    return succeed;
}

Bool
__gif_load( int fd, const char *filename, PList imgInfo, Bool readAll)
{
    return __gif_read( fd, filename, imgInfo, true, readAll);
}

Bool
__gif_save( const char *filename, PList imgInfo)
{
    return false;
}

Bool
__gif_loadable( int fd, const char *filename, Byte *preread_buf, U32 buf_size)
{
    fprintf( stderr, "__gif_loadable( %s)\n", filename);
    return ( strncmp( "GIF", preread_buf, 3) == 0);
}

Bool
__gif_storable( const char *filename, PList imgInfo)
{
    return false;
}

Bool
__gif_getinfo( int fd, const char *filename, PList imgInfo, Bool readAll)
{
    return __gif_read( fd, filename, imgInfo, false, readAll);
}
