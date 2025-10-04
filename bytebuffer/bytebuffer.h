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
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H  1

#include <unistd.h>
#include <endian.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>


#if !defined (__BYTE_ORDER) && !defined (BYTE_ORDER)
#error neither __BYTE_ORDER nor BYTE_ORDER are definded!
#endif

#define BUFFER_STRING_SIZE  255

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

enum byte_order { NONE, BIG_END, LITTLE_END };

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

typedef struct bb_buffer bb_buffer_t;

struct bb_buffer {
  size_t        bound;
  size_t        index;
  size_t        size;
  bb_commit_cb  cb_commit;
  uint8_t *     buffer;
};

#define bb_buffer_alloc() (calloc(1, sizeof(bb_buffer_t)))
#define bb_buffer_free(P) (free(P), P = NULL)

int bb_buffer_init (bb_buffer_t *, size_t, bb_commit_cb);
void bb_buffer_term (bb_buffer_t *);

size_t bb_get_bound (bb_buffer_t *);
size_t bb_get_index (bb_buffer_t *);
bool_t bb_has_more (bb_buffer_t *);
size_t bb_get_remaining (bb_buffer_t *);
size_t bb_get_size (bb_buffer_t *);

void bb_clear (bb_buffer_t *);
void bb_reset (bb_buffer_t *);
void bb_rewind_buffer (bb_buffer_t *);
void bb_set_index (bb_buffer_t *, size_t);
void bb_trip (bb_buffer_t *);

byte_t bb_get (bb_buffer_t *);
byte_t bb_get_at (bb_buffer_t *, size_t);
void bb_duplicate (bb_buffer_t *, bb_buffer_t *);
void bb_copy_at (bb_buffer_t *, bb_buffer_t *, size_t);
void bb_copy (bb_buffer_t *, bb_buffer_t *, size_t, size_t);
char bb_get_char (bb_buffer_t *);
char bb_get_char_at (bb_buffer_t *, size_t);
double bb_get_double (bb_buffer_t *);
double bb_get_double_at (bb_buffer_t *, size_t);
float bb_get_float (bb_buffer_t *);
float bb_get_float_at (bb_buffer_t *, size_t);
int16_t bb_get_int16 (bb_buffer_t *);
int16_t bb_get_int16_at (bb_buffer_t *, size_t);
int32_t bb_get_int32 (bb_buffer_t *);
int32_t bb_get_int32_at (bb_buffer_t *, size_t);
int64_t bb_get_int64 (bb_buffer_t *);
int64_t bb_get_int64_at (bb_buffer_t *, size_t);
char * bb_get_varchar (bb_buffer_t *, size_t);
char * bb_get_varchar_at (bb_buffer_t *, size_t, size_t);
byte_order_t bb_order (bb_buffer_t *);
void bb_put (bb_buffer_t *, byte_t);
void bb_put_at (bb_buffer_t *, size_t, byte_t);
void bb_put_char (bb_buffer_t *, char);
void bb_put_char_at (bb_buffer_t *, size_t, char);
void bb_put_double (bb_buffer_t *, double);
void bb_put_double_at (bb_buffer_t *, size_t, double);
void bb_put_float (bb_buffer_t *, float);
void bb_put_float_at (bb_buffer_t *, size_t, float);
void bb_put_int16 (bb_buffer_t *, int16_t);
void bb_put_int16_at (bb_buffer_t *, size_t, int16_t);
void bb_put_int32 (bb_buffer_t *, int32_t);
void bb_put_int32_at (bb_buffer_t *, size_t, int32_t);
void bb_put_int64 (bb_buffer_t *, int64_t);
void bb_put_int64_at (bb_buffer_t *, size_t, int64_t);
void bb_put_varchar (bb_buffer_t *, char const *);
void bb_put_varchar_at (bb_buffer_t *, size_t, char const *);

byte_t* bb_backing (bb_buffer_t *);
void bb_set_commit_cb (bb_buffer_t *, bb_commit_cb);

#endif
