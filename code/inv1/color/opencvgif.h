/* opencvgif.h
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

#include <opencv/cv.h>

// loadGIF
// Loads an image from a GIF file
//
IplImage *loadGIF(const char *name);
//    name
//        Name of the file to be loaded.
//
// The function loadGIF loads an image from the specified GIF file
// and returns the pointer to the loaded image. If the image cannot be
// loaded, it returns NULL. Otherwise, the image has always 3 channels
// (i.e. the palette is applied, even though the palette is grayscale).
// GIF87a and GIF89a are supported. If the file contains an animated
// GIF, the first frame of the file is loaded.


// saveGIF
// Saves an image to a GIF file
//
int saveGIF(const char *name, IplImage *img);
//    name
//        Name of the file to be saved.
//    img
//        Image to be saved.
//
// The function saveGIF saves the image to the specified GIF file.
// Only 8U-bit single-channel or 3-channel (with 'BGR' channel order)
// images can be saved using this function. In the first case, a
// grayscale palette is used. In the second case, a fixed palette is
// used, with 3 bits for red, 3 bits for green, and 2 bits for blue.
// A simple dithering algorithm is applied. On error, the function
// returns 0, otherwise 1.


#pragma once
