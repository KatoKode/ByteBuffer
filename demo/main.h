/*------------------------------------------------------------------------------
    ByteBuffer Implementation in x86_64 Assembly Language with C Interface

    Copyright (C) 2025  J. McIntosh

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
------------------------------------------------------------------------------*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "../bytebuffer/bytebuffer.h"

// To get debug output uncomment the following line and remake the demo program.
// #define BB_DEBUG  1

#define BUFFER_SIZE     256

#define INDEX_160   160
#define INDEX_64    170

void getNativeByteOrder();

void getBufferMeta (bytebuffer_t *);

double getDouble (bytebuffer_t *);
double getDoubleAt (bytebuffer_t *, size_t);
float getfloat (bytebuffer_t *);
float getFloatAt (bytebuffer_t *, size_t);
int16_t getI16 (bytebuffer_t *);
int16_t getI16At (bytebuffer_t *, size_t);
int32_t getI32 (bytebuffer_t *);
int32_t getI32At (bytebuffer_t *, size_t);
int64_t getI64 (bytebuffer_t *);
int64_t getI64At (bytebuffer_t *, size_t);
char * getText (bytebuffer_t *);
char * getTextAt (bytebuffer_t *, size_t);

void putDouble (bytebuffer_t *, double const);
void putDoubleAt (bytebuffer_t *, size_t, double const);
void putFloat (bytebuffer_t *, float const);
void putFloatAt (bytebuffer_t *, size_t, float const);
void putI16 (bytebuffer_t *, int16_t const);
void putI16At (bytebuffer_t *, size_t, int16_t const);
void putI32 (bytebuffer_t *, int32_t const);
void putI32At (bytebuffer_t *, size_t, int32_t const);
void putI64 (bytebuffer_t *, int64_t const);
void putI64At (bytebuffer_t *, size_t, int64_t const);
void putText (bytebuffer_t *, char const *);
void putTextAt (bytebuffer_t *, size_t, char const *);

void putValues (bytebuffer_t *);
void getValues (bytebuffer_t *);

void toDouble ();
void toFloat ();
