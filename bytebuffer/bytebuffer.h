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
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H  1

#include <unistd.h>
#include <endian.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>


#if !defined (__BYTE_ORDER) && !defined (BYTE_ORDER)
#error neither __BYTE_ORDER nor BYTE_ORDER are definded!
#endif

#define BUFFER_STRING_SIZE  255

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
typedef uint8_t bool_t;
typedef uint8_t byte_t;


uint16_t const  MASK_16_BYTE_0  = 0x00FF;
uint16_t const  MASK_16_BYTE_1  = 0xFF00;

uint32_t const  MASK_32_BYTE_0  = 0x000000FF;
uint32_t const  MASK_32_BYTE_1  = 0x0000FF00;
uint32_t const  MASK_32_BYTE_2  = 0x00FF0000;
uint32_t const  MASK_32_BYTE_3  = 0xFF000000;

uint64_t const  MASK_64_BYTE_0  = 0x00000000000000FF;
uint64_t const  MASK_64_BYTE_1  = 0x000000000000FF00;
uint64_t const  MASK_64_BYTE_2  = 0x0000000000FF0000;
uint64_t const  MASK_64_BYTE_3  = 0x00000000FF000000;
uint64_t const  MASK_64_BYTE_4  = 0x000000FF00000000;
uint64_t const  MASK_64_BYTE_5  = 0x0000FF0000000000;
uint64_t const  MASK_64_BYTE_6  = 0x00FF000000000000;
uint64_t const  MASK_64_BYTE_7  = 0xFF00000000000000;

uint32_t const  SHIFT_0   = 0;
uint32_t const  SHIFT_8   = 8;
uint32_t const  SHIFT_16  = 16;
uint32_t const  SHIFT_24  = 24;
uint32_t const  SHIFT_32  = 32;
uint32_t const  SHIFT_40  = 40;
uint32_t const  SHIFT_48  = 48;
uint32_t const  SHIFT_56  = 56;

typedef enum byte_order byte_order_t;

enum byte_order { NONE, BIG_END=1, LITTLE_END=4 };

byte_order_t bb_native_byte_order (void);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//

double bb_to_double (byte_t, byte_t, byte_t, byte_t, byte_t,
    byte_t, byte_t, byte_t);
float bb_to_float (byte_t, byte_t, byte_t, byte_t);
int16_t bb_to_int16 (byte_t, byte_t);
int32_t bb_to_int32 (byte_t, byte_t, byte_t, byte_t);
int64_t bb_to_int64 (byte_t, byte_t, byte_t, byte_t, byte_t,
    byte_t, byte_t, byte_t);
uint16_t bb_to_uint16 (byte_t, byte_t);
uint32_t bb_to_uint32 (byte_t, byte_t, byte_t, byte_t);
uint64_t bb_to_uint64 (byte_t, byte_t, byte_t, byte_t, byte_t,
    byte_t, byte_t, byte_t);
 
uint64_t bb_double_to_u64 (double);
uint32_t bb_float_to_u32 (float);
uint16_t bb_s16_to_u16 (int16_t);
uint32_t bb_s32_to_u32 (int32_t);
uint64_t bb_s64_to_u64 (int64_t);
float bb_u32_to_float (uint32_t);
double bb_u64_to_double (uint64_t);

uint16_t bb_u16_shift_left (byte_t, uint32_t);
byte_t bb_u16_shift_right (uint16_t, uint16_t, uint32_t);
uint32_t bb_u32_shift_left (byte_t, uint32_t);
byte_t bb_u32_shift_right (uint32_t, uint32_t, uint32_t);
uint64_t bb_u64_shift_left (byte_t, uint32_t);
byte_t bb_u64_shift_right (uint64_t, uint64_t, uint32_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// buffer

typedef void (*bb_commit_cb) (void);

typedef struct bytebuffer bytebuffer_t;

struct bytebuffer {
  size_t        bound;
  size_t        index;
  ssize_t       mark;
  size_t        size;
  byte_t *      buffer;
};

#define bb_alloc() (calloc(1, sizeof(bytebuffer_t)))
#define bb_free(P) (free(P), P = NULL)

int bb_init (bytebuffer_t *, size_t, bb_commit_cb);
void bb_term (bytebuffer_t *);

size_t bb_get_bound (bytebuffer_t *);
byte_t* bb_get_buffer (bytebuffer_t *);
size_t bb_get_index (bytebuffer_t *);
bool_t bb_has_more (bytebuffer_t *);
size_t bb_get_remaining (bytebuffer_t *);
size_t bb_get_size (bytebuffer_t *);

void bb_clear (bytebuffer_t *);
void bb_reset (bytebuffer_t *);
void bb_rewind_buffer (bytebuffer_t *);
void bb_set_index (bytebuffer_t *, size_t);
void bb_flip (bytebuffer_t *);

byte_t bb_get (bytebuffer_t *);
byte_t bb_get_at (bytebuffer_t *, size_t);
void bb_duplicate (bytebuffer_t *, bytebuffer_t *);
void bb_copy_at (bytebuffer_t *, bytebuffer_t *, size_t);
void bb_copy (bytebuffer_t *, bytebuffer_t *, size_t, size_t);
char bb_get_char (bytebuffer_t *);
char bb_get_char_at (bytebuffer_t *, size_t);
double bb_get_double (bytebuffer_t *);
double bb_get_double_at (bytebuffer_t *, size_t);
float bb_get_float (bytebuffer_t *);
float bb_get_float_at (bytebuffer_t *, size_t);
int16_t bb_get_int16 (bytebuffer_t *);
int16_t bb_get_int16_at (bytebuffer_t *, size_t);
int32_t bb_get_int32 (bytebuffer_t *);
int32_t bb_get_int32_at (bytebuffer_t *, size_t);
int64_t bb_get_int64 (bytebuffer_t *);
int64_t bb_get_int64_at (bytebuffer_t *, size_t);
int16_t bb_get_uint16 (bytebuffer_t *);
int16_t bb_get_uint16_at (bytebuffer_t *, size_t);
int32_t bb_get_uint32 (bytebuffer_t *);
int32_t bb_get_uint32_at (bytebuffer_t *, size_t);
int64_t bb_get_uint64 (bytebuffer_t *);
int64_t bb_get_uint64_at (bytebuffer_t *, size_t);
char * bb_get_varchar (bytebuffer_t *, size_t);
char * bb_get_varchar_at (bytebuffer_t *, size_t, size_t);
void bb_put (bytebuffer_t *, byte_t);
void bb_put_at (bytebuffer_t *, size_t, byte_t);
void bb_put_char (bytebuffer_t *, char);
void bb_put_char_at (bytebuffer_t *, size_t, char);
void bb_put_double (bytebuffer_t *, double);
void bb_put_double_at (bytebuffer_t *, size_t, double);
void bb_put_float (bytebuffer_t *, float);
void bb_put_float_at (bytebuffer_t *, size_t, float);
void bb_put_int16 (bytebuffer_t *, int16_t);
void bb_put_int16_at (bytebuffer_t *, size_t, int16_t);
void bb_put_int32 (bytebuffer_t *, int32_t);
void bb_put_int32_at (bytebuffer_t *, size_t, int32_t);
void bb_put_int64 (bytebuffer_t *, int64_t);
void bb_put_int64_at (bytebuffer_t *, size_t, int64_t);
void bb_put_uint16 (bytebuffer_t *, int16_t);
void bb_put_uint16_at (bytebuffer_t *, size_t, int16_t);
void bb_put_uint32 (bytebuffer_t *, int32_t);
void bb_put_uint32_at (bytebuffer_t *, size_t, int32_t);
void bb_put_uint64 (bytebuffer_t *, int64_t);
void bb_put_uint64_at (bytebuffer_t *, size_t, int64_t);
void bb_put_varchar (bytebuffer_t *, char const *);
void bb_put_varchar_at (bytebuffer_t *, size_t, char const *);

#endif
