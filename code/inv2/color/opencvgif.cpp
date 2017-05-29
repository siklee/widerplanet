/* opencvgif.cpp
*
* (c) 2005, by Gines Garcia Mateos
*           Murcia, SPAIN
*           http://dis.um.es/profesores/ginesgm
*
* This software may be freely copied, modified and redistributed
* without fee provided that this copyright notice is preserved
* intact on all copies and modified copies.
*
* This library is based on:
* Whirlgif Revision 3.04 (http://www.danbbs.dk/~dino/whirlgif/)
* Copyright (c) 1997,1998,1999 by Hans Dinsen-Hansen
* Copyright (c) 1995,1996 by Kevin Kadow
* Copyright (c) 1990,1991,1992 by Mark Podlipec.
* All rights reserved.
*
* This software may be freely copied, modified and redistributed
* without fee provided that this copyright notice is preserved
* intact on all copies and modified copies.
*
* There is no warranty or other guarantee of fitness of this software.
* It is provided solely "as is". The author(s) disclaim(s) all
* responsibility and liability with respect to this software's usage
* or its effect upon hardware or computer systems.
*
* The Graphics Interchange format (c) is the Copyright property of
* Compuserve Incorporated.  Gif(sm) is a Service Mark property of
* Compuserve Incorporated.
*
*/

/* common includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv/cv.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* define constants and defaults */
/* Default amount of inter-frame time */
#define DEFAULT_TIME 10
/* If set to 1, Netscape 'loop' code will be added by default */
#define DEFAULT_LOOP 0
/* If set to 1, use the colormaps from all images, not just the first */
#define DEFAULT_USE_COLORMAP 0

/* Used in calculating the transparent color */
#define TRANS_NONE 1
#define TRANS_RGB 2
#define TRANS_MAP 3

#define DISP_NONE 0
#define DISP_NOT  1
#define DISP_BACK 2
#define DISP_PREV 3
#define DEFAULT_DISPOSAL DISP_NONE
/* set default disposal method here to any of the DISP_XXXX values */

#define BIGSTRING 256
#define MAXVAL  4096        /* maxval of lzw coding size */
#define MAXVALP 4096
#define TERMIN 'T'
#define LOOKUP 'L'
#define SEARCH 'S'
#define noOfArrays 20
/* defines the amount of memory set aside in the encoding for the
* LOOKUP type nodes; for a 256 color GIF, the number of LOOKUP
* nodes will be <= noOfArrays, for a 128 color GIF the number of
* LOOKUP nodes will be <= 2 * noOfArrays, etc.  */

/* define shorthand for various types */
#define LONG int
#define ULONG unsigned int
#define BYTE char
#define UBYTE unsigned char
#define SHORT short
#define USHORT unsigned short
#define WORD short int
#define UWORD unsigned short int


/* definition of various structures */
typedef struct Transparency {
	int type;
	UBYTE valid;
	UBYTE map;
	UBYTE red;
	UBYTE green;
	UBYTE blue;
} Transparency;

typedef struct Global {
	Transparency trans;
	int left;
	int top;
	unsigned int time;
	unsigned short disposal;
} Global;

typedef struct GifScreenHdr {
	int width;
	int height;
	UBYTE m;
	UBYTE cres;
	UBYTE pixbits;
	UBYTE bc;
	UBYTE aspect;
} GifScreenHdr;

typedef union GifColor {
	struct cmap {
		UBYTE red;
		UBYTE green;
		UBYTE blue;
		UBYTE pad;
	} cmap;
	ULONG pixel;
} GifColor;

typedef struct GifImageHdr {
	int left;
	int top;
	int width;
	int height;
	UBYTE m;
	UBYTE i;
	UBYTE pixbits;
	UBYTE reserved;
} GifImageHdr;

typedef struct GifTree {
	char typ;             /* terminating, lookup, or search */
	int code;             /* the code to be output */
	UBYTE ix;             /* the color map index */
	struct GifTree **node, *nxt, *alt;
} GifTree;

/* define inline functions */
#define GifPutShort(i, fout)    {fputc(i&0xff, fout); fputc(i>>8, fout);}
#define GifGetShort(fin)        (Xgetc(fin) | Xgetc(fin)<<8)

/* forward declaration of the functions  */
char *AddCodeToBuffer(int, short, char *);
void ClearTree(int, GifTree *);
void GifClearTable();
UBYTE *GifSendData(UBYTE *, int, UBYTE *);
void SetOffset(char *);
UBYTE Xgetc(FILE *fin);


/*
* Set some defaults, these can be changed on the command line
*/

UBYTE *buffer = NULL;

unsigned int loop = DEFAULT_LOOP, loopcount = 0,
useColormap = DEFAULT_USE_COLORMAP,
globmap = 0, minimize = 0;

int imagex = 0, imagey = 0, imagec = 0, GifBgcolor = 0, count = 0;

/* global settings for offset, transparency */

Global global;

GifColor gifGmap[256], gifCmap[256];
GifScreenHdr globscrn, gifscrn;

GifImageHdr gifimage, gifimageold;

extern ULONG gifMask[];
UBYTE *pixold = NULL;
ULONG gifMask[16] = { 0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,0,0 }, obits;
ULONG gifPtwo[16] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,0,0 };

char gifFileName[BIGSTRING];
FILE *ff;

long sq(UBYTE i, UBYTE j)
{
	return((i - j)*(i - j));
}

void ReadImageHeader(FILE *fp)
{
	int tnum, i, flag;

	gifimage.left = GifGetShort(fp);
	gifimage.top = GifGetShort(fp);
	gifimage.width = GifGetShort(fp);
	gifimage.height = GifGetShort(fp);
	flag = Xgetc(fp);
	gifimage.i = flag & 0x40;
	gifimage.pixbits = flag & 0x07;
	gifimage.m = flag & 0x80;

	imagex = gifimage.width;
	imagey = gifimage.height;
	tnum = gifPtwo[(1 + gifimage.pixbits)];

	/* if there is an image cmap then read it */
	if (gifimage.m) {
		/*
		* note below assignment, it may make the subsequent code confusing
		*/
		gifscrn.pixbits = gifimage.pixbits;

		for (i = 0; i < tnum; i++) {
			gifCmap[i].cmap.red = Xgetc(fp);
			gifCmap[i].cmap.green = Xgetc(fp);
			gifCmap[i].cmap.blue = Xgetc(fp);
		}
	}
	return;
}

void TheEnd()
{
	throw 0;
}

UBYTE Xgetc(FILE *fin)
{
	int i;
	if ((i = fgetc(fin)) == EOF) {
		TheEnd();
	}
	return(i & 0xff);
}

ULONG codeSize, expected, imgsize, mask, old, picI, rootCodeSize, first[MAXVAL];
UBYTE *topGifBuff, *picture, gifBuff[MAXVAL], last[MAXVAL];
int imgheight, imgwidth, interlaced, pass,
step[5] = { 7,7,3,1,0 }, start[5] = { 0,4,2,1,0 };

void GifDecode(FILE *fp, UBYTE *pix, GifImageHdr gifimage)
{
	UBYTE *chPos, firstCodeOut = 0, charBuff[256], cuenta;
	ULONG CLEAR, EOI, bits = 0, code = 0, codeFeched, buffCount = 0;
	int need = 0;

	interlaced = gifimage.i;
	imgwidth = gifimage.width;
	imgheight = gifimage.height;
	imgsize = imgwidth * imgheight;
	picture = pix;
	pass = picI = 0;
	rootCodeSize = Xgetc(fp);
	CLEAR = 1 << rootCodeSize;
	EOI = CLEAR + 1;

	GifClearTable();

	if ((buffCount = Xgetc(fp)) == 0) {
		TheEnd();
	}

	while (buffCount > 0) {
		cuenta = fread(charBuff, 1, buffCount, fp);
		if (cuenta != buffCount) {
			TheEnd();
		}
		for (chPos = charBuff; buffCount-- > 0; chPos++) {
			need += (int)*chPos << bits;
			bits += 8;
			while (bits >= codeSize) {
				code = need & mask;
				need >>= codeSize;
				bits -= codeSize;
				if (code > expected)
					TheEnd();
				if (code == EOI) {
					goto skipRest;
				}

				if (code == CLEAR) {
					GifClearTable();
					continue;
				}
				if (old == MAXVAL) {  /* i.e. first code after clear table */
					pix = GifSendData(pix, 1, &last[code]);
					firstCodeOut = last[code];
					old = code;
					continue;
				}
				codeFeched = code;
				if (code == expected) {
					*topGifBuff++ = firstCodeOut;
					code = old;
				}
				while (code > CLEAR) {
					*topGifBuff++ = last[code];
					code = first[code];
				}

				*topGifBuff++ = firstCodeOut = last[code];
				first[expected] = old;
				last[expected] = firstCodeOut;
				if (expected < MAXVAL) expected++;
				if (((expected & mask) == 0) && (expected < MAXVAL)) {
					codeSize++;
					mask += expected;
				}
				old = codeFeched;
				pix = GifSendData(pix, topGifBuff - gifBuff, gifBuff);
				topGifBuff = gifBuff;

			}   /* end of extracting codes */
		}   /* end of reading a block of data */
		if ((buffCount = Xgetc(fp)) == 0) {
			TheEnd();
		}
	}

skipRest:
	return;
}

UBYTE *GifSendData(UBYTE *pix, int bytes, UBYTE source[])

{
	int j = 0;
	for (j = bytes - 1; j >= 0; j--) {
		picI++;
		*pix = source[j]; pix++;
		if (interlaced && (picI % imgwidth == 0)) {
			picI += (imgwidth * step[pass]);
			if (picI >= imgsize) {
				picI = start[++pass] * imgwidth;
			}
			pix = &picture[picI];
		}
	}
	return(pix);
}

void GifClearTable()
{
	int i, maxi;
	maxi = 1 << rootCodeSize;
	expected = maxi + 2;
	old = MAXVAL;
	codeSize = rootCodeSize + 1;
	mask = (1 << codeSize) - 1;

	for (i = 0; i < maxi; i++) {
		first[i] = MAXVAL;
		last[i] = i & 0xff;
	}
	topGifBuff = gifBuff;
}



#define BLOKLEN 255
#define BUFLEN 1000


int chainlen = 0, maxchainlen = 0, nodecount = 0, lookuptypes = 0, nbits;
short need = 8;
GifTree *empty[256], GifRoot = { LOOKUP, 0, 0, empty, NULL, NULL },
*topNode, *baseNode, **nodeArray, **lastArray;

void GifEncode(FILE *fout, UBYTE *pixels, int depth, int siz)
{
	GifTree *first = &GifRoot, *newNode, *curNode;
	UBYTE   *end;
	int     cc, eoi, next, tel = 0;
	short   cLength;

	char    *pos, *buffer;

	empty[0] = NULL;
	need = 8;

	nodeArray = empty;
	memmove(++nodeArray, empty, 255 * sizeof(GifTree **));
	if ((buffer = (char *)malloc((BUFLEN + 1) * sizeof(char))) == NULL)
		TheEnd();
	buffer++;


	pos = buffer;
	buffer[0] = 0x0;

	cc = (depth == 1) ? 0x4 : 1 << depth;
	fputc((depth == 1) ? 2 : depth, fout);
	eoi = cc + 1;
	next = cc + 2;

	cLength = (depth == 1) ? 3 : depth + 1;

	if ((topNode = baseNode = (GifTree *)malloc(sizeof(GifTree) * 4094)) == NULL)
		TheEnd();
	if ((nodeArray = first->node = (GifTree **)malloc(256 * sizeof(GifTree *)*noOfArrays)) == NULL)
		TheEnd();
	lastArray = nodeArray + (256 * noOfArrays - cc);
	ClearTree(cc, first);

	pos = AddCodeToBuffer(cc, cLength, pos);

	end = pixels + siz;
	curNode = first;
	while (pixels < end) {

		if (curNode->node[*pixels] != NULL) {
			curNode = curNode->node[*pixels];
			tel++;
			pixels++;
			chainlen++;
			continue;
		}
		else if (curNode->typ == SEARCH) {
			newNode = curNode->nxt;
			while (newNode->alt != NULL) {
				if (newNode->ix == *pixels) break;
				newNode = newNode->alt;
			}
			if (newNode->ix == *pixels) {
				tel++;
				pixels++;
				chainlen++;
				curNode = newNode;
				continue;
			}
		}

		/* ******************************************************
		* If there is no more thread to follow, we create a new node.  If the
		* current node is terminating, it will become a SEARCH node.  If it is
		* a SEARCH node, and if we still have room, it will be converted to a
		* LOOKUP node.
		*/
		newNode = ++topNode;
		switch (curNode->typ) {
		case LOOKUP:
			newNode->nxt = NULL;
			newNode->alt = NULL,
				curNode->node[*pixels] = newNode;
			break;
		case SEARCH:
			if (nodeArray != lastArray) {
				nodeArray += cc;
				curNode->node = nodeArray;
				curNode->typ = LOOKUP;
				curNode->node[*pixels] = newNode;
				curNode->node[(curNode->nxt)->ix] = curNode->nxt;
				lookuptypes++;
				newNode->nxt = NULL;
				newNode->alt = NULL,
					curNode->nxt = NULL;
				break;
			}
			/*   otherwise do as we do with a TERMIN node  */
		case TERMIN:
			newNode->alt = curNode->nxt;
			newNode->nxt = NULL,
				curNode->nxt = newNode;
			curNode->typ = SEARCH;
			break;
		}
		newNode->code = next;
		newNode->ix = *pixels;
		newNode->typ = TERMIN;
		newNode->node = empty;
		nodecount++;
		/*
		* End of node creation
		* ******************************************************
		*/
		pos = AddCodeToBuffer(curNode->code, cLength, pos);
		if (chainlen > maxchainlen) maxchainlen = chainlen;
		chainlen = 0;
		if (pos - buffer>BLOKLEN) {
			buffer[-1] = BLOKLEN;
			fwrite(buffer - 1, 1, BLOKLEN + 1, fout);
			buffer[0] = buffer[BLOKLEN];
			buffer[1] = buffer[BLOKLEN + 1];
			buffer[2] = buffer[BLOKLEN + 2];
			buffer[3] = buffer[BLOKLEN + 3];
			pos -= BLOKLEN;
		}
		curNode = first;

		if (next == (1 << cLength)) cLength++;
		next++;

		if (next == 0xfff) {
			ClearTree(cc, first);
			pos = AddCodeToBuffer(cc, cLength, pos);
			if (pos - buffer>BLOKLEN) {
				buffer[-1] = BLOKLEN;
				fwrite(buffer - 1, 1, BLOKLEN + 1, fout);
				buffer[0] = buffer[BLOKLEN];
				buffer[1] = buffer[BLOKLEN + 1];
				buffer[2] = buffer[BLOKLEN + 2];
				buffer[3] = buffer[BLOKLEN + 3];
				pos -= BLOKLEN;
			}
			next = cc + 2;
			cLength = (depth == 1) ? 3 : depth + 1;
		}
	}

	pos = AddCodeToBuffer(curNode->code, cLength, pos);
	if (pos - buffer>BLOKLEN - 3) {
		buffer[-1] = BLOKLEN - 3;
		fwrite(buffer - 1, 1, BLOKLEN - 2, fout);
		buffer[0] = buffer[BLOKLEN - 3];
		buffer[1] = buffer[BLOKLEN - 2];
		buffer[2] = buffer[BLOKLEN - 1];
		buffer[3] = buffer[BLOKLEN];
		buffer[4] = buffer[BLOKLEN + 1];
		pos -= BLOKLEN - 3;
	}
	pos = AddCodeToBuffer(eoi, cLength, pos);
	pos = AddCodeToBuffer(0x0, -1, pos);
	buffer[-1] = pos - buffer;

	fwrite(buffer - 1, pos - buffer + 1, 1, fout);
	free(buffer - 1); free(first->node); free(baseNode);
	return;

}

void ClearTree(int cc, GifTree *root)
{
	int i;
	GifTree *newNode, **xx;

	maxchainlen = 0; lookuptypes = 1;
	nodecount = 0;
	nodeArray = root->node;
	xx = nodeArray;
	for (i = 0; i < noOfArrays; i++) {
		memmove(xx, empty, 256 * sizeof(GifTree **));
		xx += 256;
	}
	topNode = baseNode;
	for (i = 0; i<cc; i++) {
		root->node[i] = newNode = ++topNode;
		newNode->nxt = NULL;
		newNode->alt = NULL;
		newNode->code = i;
		newNode->ix = i;
		newNode->typ = TERMIN;
		newNode->node = empty;
		nodecount++;
	}
}

char *AddCodeToBuffer(int code, short n, char *buf)
{
	int    mask;

	if (n<0) {
		if (need<8) {
			buf++;
			*buf = 0x0;
		}
		need = 8;
		return buf;
	}

	while (n >= need) {
		mask = (1 << need) - 1;
		*buf += (mask&code) << (8 - need);
		buf++;
		*buf = 0x0;
		code = code >> need;
		n -= need;
		need = 8;
	}
	if (n) {
		mask = (1 << n) - 1;
		*buf += (mask&code) << (8 - need);
		need -= n;
	}
	return buf;
}


IplImage *loadGIF(const char *name)
{
	int temp, i, length, cuenta;
	FILE *fp;
	UBYTE *pix = NULL;
	IplImage *res;
	int x, y;
	unsigned char *ptr, valor;

	if ((fp = fopen(name, "rb")) == 0)
		return NULL;
	try {
		for (i = 0; i < 6; i++) {
			temp = Xgetc(fp);
		}
		gifscrn.width = GifGetShort(fp);
		gifscrn.height = GifGetShort(fp);
		temp = Xgetc(fp);
		gifscrn.m = temp & 0x80;
		gifscrn.cres = (temp & 0x70) >> 4;
		gifscrn.pixbits = temp & 0x07;
		gifscrn.bc = Xgetc(fp);
		temp = Xgetc(fp);
		temp = gifscrn.width * gifscrn.height * sizeof(UBYTE);
	}
	catch (...)
	{
		fclose(fp);
		return NULL;
	}
	if (temp == 0 || (pix = (UBYTE *)malloc(temp * sizeof(UBYTE))) == NULL)
		return NULL;  // Thanks to Menno Israel for the bug report
	try {
		imagec = gifPtwo[(1 + gifscrn.pixbits)];
		if (gifscrn.m) {
			for (i = 0; i < imagec; i++) {
				gifCmap[i].cmap.red = temp = Xgetc(fp);
				gifCmap[i].cmap.green = temp = Xgetc(fp);
				gifCmap[i].cmap.blue = temp = Xgetc(fp);
			}
		}
		do {
			switch (i = Xgetc(fp)) {
			case ',':
			case '\0':
				break;
			case '!':
				Xgetc(fp);
				for (i = Xgetc(fp); i > 0; i--) Xgetc(fp);
				while ((i = Xgetc(fp)) > 0) {
					for (i = i; i > 0; i--) Xgetc(fp);
				}
				break;
			default:
				fclose(fp);
				if (feof(fp) || i == ';')
					return NULL;
			}
		} while (i != ',');
		globscrn.m = gifscrn.m;
		globscrn.pixbits = gifscrn.pixbits;
		globscrn.bc = gifscrn.bc;
		if (globscrn.m) {
			for (i = gifMask[1 + globscrn.pixbits]; i >= 0; i--) {
				gifGmap[i].cmap.red = gifCmap[i].cmap.red;
				gifGmap[i].cmap.green = gifCmap[i].cmap.green;
				gifGmap[i].cmap.blue = gifCmap[i].cmap.blue;
			}
		}
		ReadImageHeader(fp);
		GifDecode(fp, pix, gifimage);
	}
	catch (...)
	{
		free(pix);
		fclose(fp);
		return NULL;
	}
	res = cvCreateImage(cvSize(gifimage.width, gifimage.height), IPL_DEPTH_8U, 3);
	if (!res) {
		free(pix);
		fclose(fp);
		return NULL;
	}
	ptr = pix;
	for (y = 0; y<gifimage.height; y++)
		for (x = 0; x<gifimage.width; x++) {
			valor = ptr[0];
			ptr++;
			cvSet2D(res, y, x, cvScalar(gifCmap[valor].cmap.blue, gifCmap[valor].cmap.green, gifCmap[valor].cmap.red, 0));
		}
	free(pix);
	fclose(fp);
	return res;
}

int saveGIF(const char *name, IplImage *img)
{
	int temp, i, length, cuenta, x, y;
	FILE *fout;
	UBYTE *pix = NULL;
	unsigned char *ptr, valor, rr, gg, bb;
	CvScalar sc;

	if (img->depth != IPL_DEPTH_8U)
		return 0;

	if ((fout = fopen(name, "wb+")) == 0)
		return 0;
	if ((pix = (unsigned char*)malloc(img->width*img->height)) == NULL) {
		fclose(fout);
		return 0;
	}
	fputc('G', fout);
	fputc('I', fout);
	fputc('F', fout);
	fputc('8', fout);
	fputc('9', fout);
	fputc('a', fout);
	GifPutShort(img->width, fout);
	GifPutShort(img->height, fout);
	fputc(247, fout);
	fputc(0, fout);
	fputc(0, fout);
	if (img->nChannels == 1) {
		for (i = 0; i < 256; i++) {
			fputc(i, fout);
			fputc(i, fout);
			fputc(i, fout);
		}
		for (y = 0; y<img->height; y++)
			memcpy(pix + y*img->width, img->imageData + y*img->widthStep, img->width);
	}
	else {
		for (i = 0; i < 256; i++) {
			valor = (i & 0xe0) + 0x10;
			fputc(valor, fout);
			valor = ((i & 0x1c) << 3) + 0x10;
			fputc(valor, fout);
			valor = ((i & 0x03) << 6) + 0x20;
			fputc(valor, fout);
		}
		ptr = pix;
		for (y = 0; y<img->height; y++)
			for (x = 0; x<img->width; x++) {
				sc = cvGet2D(img, y, x);
				temp = rand() % 31 - 15;
				temp += sc.val[2];
				rr = temp;
				if (temp<0) rr = 0;
				if (temp>255) rr = 255;
				temp = rand() % 31 - 15;
				temp += sc.val[1];
				gg = temp;
				if (temp<0) gg = 0;
				if (temp>255) gg = 255;
				temp = rand() % 63 - 31;
				temp += sc.val[0];
				bb = temp;
				if (temp<0) bb = 0;
				if (temp>255) bb = 255;
				valor = (rr & 0xe0) | ((gg & 0xe0) >> 3) | (bb >> 6);
				ptr[0] = valor;
				ptr++;
			}
	}
	fputc(0x2c, fout);
	GifPutShort(0, fout);
	GifPutShort(0, fout);
	GifPutShort(img->width, fout);
	GifPutShort(img->height, fout);
	fputc(0, fout);
	try {
		GifEncode(fout, pix, 8, img->width*img->height);
	}
	catch (...)
	{
		free(pix);
		fclose(fout);
		return 0;
	}
	fputc(0x0, fout);
	fputc(0x3b, fout);
	fclose(fout);
	free(pix);
	return 1;
}

