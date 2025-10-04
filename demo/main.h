/*------------------------------------------------------------------------------
    ByteBuffer Implementation in x86_64 Assembly Language with C
    Interface

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

#define BUFFER_SIZE     256

void getNativeByteOrder();

double getDouble (bb_buffer_t *);
float getfloat (bb_buffer_t *);
int16_t getI16 (bb_buffer_t *);
int32_t getI32 (bb_buffer_t *);
int64_t getI64 (bb_buffer_t *);
char * getText (bb_buffer_t *);

void putDouble (bb_buffer_t *, double const);
void putFloat (bb_buffer_t *, float const);
void putI16 (bb_buffer_t *, int16_t const);
void putI32 (bb_buffer_t *, int32_t const);
void putI64 (bb_buffer_t *, int64_t const);
void putText (bb_buffer_t *, char const *);

void putValues (bb_buffer_t *);
void getValues (bb_buffer_t *);

void toDouble ();
void toFloat ();
