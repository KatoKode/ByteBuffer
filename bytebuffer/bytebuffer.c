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
#include "bytebuffer.h"
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Get native byte order of computer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
byte_order_t bb_native_byte_order (void)
{
  union U {
    int32_t i32;
    byte_t  b[4];
  } u;
  u.i32 = 0x01000004;
  if (u.b[0] == 0x01) return BIG_END;
  else if (u.b[0] == 0x04) return LITTLE_END;
  return NONE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into double
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double bb_to_double (byte_t b0, byte_t b1, byte_t b2, byte_t b3,
    byte_t b4, byte_t b5, byte_t b6, byte_t b7)
{
  uint64_t x = bb_to_uint64(b0, b1, b2, b3, b4, b5, b6, b7);
  return bb_u64_to_double(x);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into float
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
float bb_to_float (byte_t b0, byte_t b1, byte_t b2, byte_t b3)
{
  uint32_t x = bb_to_uint32(b0, b1, b2, b3);
  return bb_u32_to_float(x);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into int16_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int16_t bb_to_int16 (byte_t b0, byte_t b1)
{
  return bb_to_uint16(b0, b1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into int32_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int32_t bb_to_int32 (byte_t b0, byte_t b1, byte_t b2, byte_t b3)
{
  return bb_to_uint32(b0, b1, b2, b3);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into int64_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int64_t bb_to_int64 (byte_t b0, byte_t b1, byte_t b2, byte_t b3,
    byte_t b4, byte_t b5, byte_t b6, byte_t b7)
{
  return bb_to_uint64(b0, b1, b2, b3, b4, b5, b6, b7);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into uint64_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint16_t bb_to_uint16 (byte_t b0, byte_t b1)
{
  uint16_t value = bb_u16_shift_left(b1, SHIFT_8)
    | bb_u16_shift_left(b0, SHIFT_0); 
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into uint32_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint32_t bb_to_uint32 (byte_t b0, byte_t b1, byte_t b2, byte_t b3)
{
  uint32_t value = bb_u32_shift_left(b3, SHIFT_24)
    | bb_u32_shift_left(b2, SHIFT_16)
    | bb_u32_shift_left(b1, SHIFT_8)
    | bb_u32_shift_left(b0, SHIFT_0);
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Assemble bytes into uint64_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint64_t bb_to_uint64 (byte_t b0, byte_t b1, byte_t b2, byte_t b3,
    byte_t b4, byte_t b5, byte_t b6, byte_t b7)
{
  uint64_t value = bb_u64_shift_left(b7, SHIFT_56)
    | bb_u64_shift_left(b6, SHIFT_48)
    | bb_u64_shift_left(b5, SHIFT_40)
    | bb_u64_shift_left(b4, SHIFT_32)
    | bb_u64_shift_left(b3, SHIFT_24)
    | bb_u64_shift_left(b2, SHIFT_16)
    | bb_u64_shift_left(b1, SHIFT_8)
    | bb_u64_shift_left(b0, SHIFT_0); 
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Mutate a double to a uint64_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint64_t bb_double_to_u64 (double value)
{
  uint64_t *pu64 = (uint64_t *)&value;
  return *pu64;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Mutate a float to a uint64_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint32_t bb_float_to_u32 (float value)
{
  uint32_t *pu32 = (uint32_t *)&value;
  return *pu32;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Convert a int16_t to a uint16_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint16_t bb_s16_to_u16 (int16_t value)
{
  return (uint16_t)value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Convert a int32_t to a uint32_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint32_t bb_s32_to_u32 (int32_t value)
{
  return (uint32_t)value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Convert a int64_t to a uint64_t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint64_t bb_s64_to_u64 (int64_t value)
{
  return (uint64_t)value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Mutate a uint32_t to a float
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
float bb_u32_to_float (uint32_t value)
{
  float *pflt = (float *)&value;
  return *pflt;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Mutate a uint64_t to a double
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double bb_u64_to_double (uint64_t value)
{
  double *pdbl = (double *)&value;
  return *pdbl;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Shift uint16_t left SHIFT number of bits
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint16_t bb_u16_shift_left (byte_t b, uint32_t SHIFT)
{
  uint16_t x = b;
  x <<= SHIFT;
  return x;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Shift uint16_t right SHIFT number of bits
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
byte_t bb_u16_shift_right (uint16_t value, uint16_t MASK, uint32_t SHIFT)
{
  return (byte_t)((value & MASK) >> SHIFT);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Shift uint32_t left SHIFT number of bits
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint32_t bb_u32_shift_left (byte_t b, uint32_t SHIFT)
{
  uint32_t x = b;
  x <<= SHIFT;
  return x;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Shift uint32_t right SHIFT number of bits
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
byte_t bb_u32_shift_right (uint32_t value, uint32_t MASK, uint32_t SHIFT)
{
  return (byte_t)((value & MASK) >> SHIFT);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Shift uint64_t left SHIFT number of bits
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint64_t bb_u64_shift_left (byte_t b, uint32_t SHIFT)
{
  uint64_t x = b;
  x <<= SHIFT;
  return x;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Shift uint64_t right SHIFT number of bits
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
byte_t bb_u64_shift_right (uint64_t value, uint64_t MASK, uint32_t SHIFT)
{
  return (byte_t)((value & MASK) >> SHIFT);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialize buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int bb_buffer_init (bb_buffer_t *buffer, size_t size, bb_commit_cb cb_commit)
{
  buffer->bound = size;
  buffer->index = 0;
  buffer->size = size;
  buffer->cb_commit = cb_commit;
  buffer->buffer = calloc(1, size);
  return (buffer->buffer ? 1 : -1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Terminate buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_buffer_term (bb_buffer_t *buffer)
{
  free(buffer->buffer);
  (void) memset(buffer, 0, sizeof(bb_buffer_t));
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Get the bound of a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t bb_get_bound (bb_buffer_t *buffer)
{
  return buffer->bound;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Get the index of a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t bb_get_index (bb_buffer_t *buffer)
{
  return buffer->index;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Get the size of a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t bb_get_size (bb_buffer_t *buffer)
{
  return buffer->size;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Is there one or more bytes available in the buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_t bb_has_more (bb_buffer_t *buffer)
{
  return (buffer->bound > buffer->index);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// How many bytes remain in the buffer before the bound
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t bb_remaining (bb_buffer_t *buffer)
{
  return (buffer->bound - buffer->index);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Set the bound of a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_set_bound (bb_buffer_t *buffer, size_t bound)
{
  if (bound < 0 || bound > buffer->size) return;

  buffer->bound = bound;

  if (buffer->index > bound) buffer->index = bound;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Clear a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_clear (bb_buffer_t *buffer)
{
  buffer->bound = buffer->size;
  buffer->index = 0;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Set index of a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_set_index (bb_buffer_t *buffer, size_t value)
{
  if (value < 0 || value > buffer->bound) return;

  buffer->index = value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Reset (Clear) a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_reset(bb_buffer_t *buffer)
{
  bb_clear(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Rewind a buffer
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_rewind (bb_buffer_t *buffer)
{
  buffer->index = 0;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Trip the bound and index of a buffer back.
void bb_trip (bb_buffer_t *buffer)
{
  buffer->bound = buffer->index;
  buffer->index = 0;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return byte value at get_index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
byte_t bb_get (bb_buffer_t *buffer)
{
  if (! bb_has_more(buffer)) return 0;
  size_t const i = bb_get_index(buffer);
  byte_t b = buffer->buffer[i];
  bb_set_index(buffer, i + 1);
  return b;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return byte value at get_index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
byte_t bb_get_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return bb_get(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Copy the contents of buffer (b_src) to buffer (b_dst). The number of
// bytes copied depends on the number of remaining bytes in each buffer.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_duplicate (bb_buffer_t *b_dst, bb_buffer_t *b_src)
{
  if (bb_remaining(b_src) > bb_remaining(b_dst)) return;
  while (bb_remaining(b_dst) > 0) bb_put(b_dst, bb_get(b_src));
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Copy the contents of buffer (b_src) to buffer (b_dst) starting at index (n).
// The number of bytes copied depends on the number of bytes remaining in each
// buffer.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void copy_at (bb_buffer_t *b_dst, bb_buffer_t *b_src, size_t n)
{
  bb_set_index(b_src, n);
  bb_duplicate(b_dst, b_src);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Copy (size) bytes of the contents of buffer (b_src) to buffer (b_dst)
// starting at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_copy (bb_buffer_t *b_dst, bb_buffer_t *b_src, size_t n, size_t size)
{
  bb_set_index(b_src, n);
  if (size > bb_remaining(b_src) || size > bb_remaining(b_dst)) return;
  while (size-- > 0) bb_put(b_dst, bb_get(b_src));
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return char value at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char bb_get_char (bb_buffer_t *buffer)
{
  return (char) bb_get(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return a char value at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char bb_get_char_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return (char) bb_get(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return double value at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double bb_get_double (bb_buffer_t *buffer)
{
  double value;
#if (BYTE_ORDER == BIG_ENDIAN)
  value = bb_to_double(bb_get(buffer), bb_get(buffer), bb_get(buffer),
      bb_get(buffer), bb_get(buffer), bb_get(buffer), bb_get(buffer),
      bb_get(buffer));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  size_t const i = bb_get_index(buffer);
  value = bb_to_double(bb_get_at(buffer, i + 7), bb_get_at(buffer, i + 6),
      bb_get_at(buffer, i + 5), bb_get_at(buffer, i + 4), bb_get_at(buffer, i + 3),
      bb_get_at(buffer, i + 2), bb_get_at(buffer, i + 1), bb_get_at(buffer, i));
  bb_set_index(buffer, i + 8);
#endif
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return double value at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double get_double_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return bb_get_double(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return float value at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
float bb_get_float (bb_buffer_t *buffer)
{
  float value;
#if (BYTE_ORDER == BIG_ENDIAN)
  value = bb_to_float(bb_get(buffer), bb_get(buffer),
      bb_get(buffer), bb_get(buffer));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  int32_t const i = bb_get_index(buffer);
  value = bb_to_float(bb_get_at(buffer, i + 3), bb_get_at(buffer, i + 2),
      bb_get_at(buffer, i + 1), bb_get_at(buffer, i));
  bb_set_index(buffer, i + 4);
#endif
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return float value at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
float bb_get_float_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return bb_get_float(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return int16_t value at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int16_t bb_get_int16 (bb_buffer_t *buffer)
{
  int16_t value;
#if (BYTE_ORDER == BIG_ENDIAN)
  value = bb_to_int16(bb_get(buffer), bb_get(buffer));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  size_t const i = bb_get_index(buffer);
  value = bb_to_int16(bb_get_at(buffer, i + 1), bb_get_at(buffer, i));
  bb_set_index(buffer, i + 2);
#endif
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return int16_t value at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int16_t bb_get_int16_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return bb_get_int16(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return int32_t value at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int bb_get_int32 (bb_buffer_t *buffer)
{
  int32_t value;
#if (BYTE_ORDER == BIG_ENDIAN)
  value = bb_to_int32(bb_get(buffer), bb_get(buffer),
      bb_get(buffer), bb_get(buffer));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  size_t const i = bb_get_index(buffer);
  value = bb_to_int32(bb_get_at(buffer, i + 3), bb_get_at(buffer, i + 2),
      bb_get_at(buffer, i + 1), bb_get_at(buffer, i));
  bb_set_index(buffer, i + 4);
#endif
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return int32_t value at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int32_t bb_get_int32_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return bb_get_int32(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return int64_t value at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int64_t bb_get_int64 (bb_buffer_t *buffer)
{
  int64_t value;
#if (BYTE_ORDER == BIG_ENDIAN)
  value = bb_to_int64(bb_get(buffer), bb_get(buffer), bb_get(buffer),
      bb_get(buffer), bb_get(buffer), bb_get(buffer), bb_get(buffer),
      bb_get(buffer));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  size_t const i = bb_get_index(buffer);
  value = bb_to_int64(bb_get_at(buffer, i + 7), bb_get_at(buffer, i + 6),
      bb_get_at(buffer, i + 5), bb_get_at(buffer, i + 4),
      bb_get_at(buffer, i + 3), bb_get_at(buffer, i + 2),
      bb_get_at(buffer, i + 1), bb_get_at(buffer, i));
  bb_set_index(buffer, i + 8);
#endif
  return value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return int64_t value at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int64_t bb_get_int64_at (bb_buffer_t *buffer, size_t n)
{
  bb_set_index(buffer, n);
  return bb_get_int64(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return varchar value (size) at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char * bb_get_varchar (bb_buffer_t *buffer, size_t size)
{
  char *s, *p;
  if ((s = p = calloc (1, size + 1)) == NULL) return NULL;
  while (size-- > 0) *p++ = bb_get_char(buffer);
  return s;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Return varchar value (size) at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char * bb_get_varchar_at (bb_buffer_t *buffer, size_t n, size_t size)
{
  bb_set_index(buffer, n);
  return bb_get_varchar(buffer, size);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Populate buffer with byte_t value.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put (bb_buffer_t *buffer, byte_t value)
{
  if (bb_remaining(buffer) == 0) return;
  size_t const i = bb_get_index(buffer);
  buffer->buffer[i] = value;
  bb_set_index(buffer, i + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put byte_t value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_at (bb_buffer_t *buffer, size_t n, byte_t value)
{
  bb_set_index(buffer, n);
  bb_put(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put char value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_char (bb_buffer_t *buffer, char value)
{
  bb_put(buffer, (byte_t)value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put char value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_char_at (bb_buffer_t *buffer, size_t n, char value)
{
  bb_put_at(buffer, n, (byte_t)value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put double value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_double (bb_buffer_t *buffer, double value)
{
  uint64_t ui64 = bb_double_to_u64(value);
#if (BYTE_ORDER == BIG_ENDIAN)
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_0, SHIFT_0));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_3, SHIFT_24));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_4, SHIFT_32));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_5, SHIFT_40));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_6, SHIFT_48));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_7, SHIFT_56));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_7, SHIFT_56));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_6, SHIFT_48));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_5, SHIFT_40));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_4, SHIFT_32));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_3, SHIFT_24));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_0, SHIFT_0));
#endif
  }
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put double value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_double_at (bb_buffer_t *buffer, size_t n, double value)
{
  bb_set_index(buffer, n);
  bb_put_double(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put float value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_float (bb_buffer_t *buffer, float value)
{
  uint32_t ui32 = bb_float_to_u32(value);
#if (BYTE_ORDER == BIG_ENDIAN)
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_0, SHIFT_0));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_3, SHIFT_24));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_3, SHIFT_24));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_0, SHIFT_0));
#endif
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put float value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_float_at (bb_buffer_t *buffer, size_t n, float value)
{
  bb_set_index(buffer, n);
  bb_put_float(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put int16_t value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_int16 (bb_buffer_t *buffer, int16_t value)
{
  uint16_t ui16 = bb_s16_to_u16(value);
#if (BYTE_ORDER == BIG_ENDIAN)
  bb_put(buffer, bb_u16_shift_right(ui16, MASK_16_BYTE_0, SHIFT_0));
  bb_put(buffer, bb_u16_shift_right(ui16, MASK_16_BYTE_1, SHIFT_8));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  bb_put(buffer, bb_u16_shift_right(ui16, MASK_16_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u16_shift_right(ui16, MASK_16_BYTE_0, SHIFT_0));
#endif
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put int16_t value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_int16_at (bb_buffer_t *buffer, size_t n, int16_t value)
{
  bb_set_index(buffer, n);
  bb_put_int16(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put int32_t value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_int32 (bb_buffer_t *buffer, int32_t value)
{
  uint32_t ui32 = bb_s32_to_u32(value);
#if (BYTE_ORDER == BIG_ENDIAN)
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_0, SHIFT_0));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_3, SHIFT_24));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_3, SHIFT_24));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u32_shift_right(ui32, MASK_32_BYTE_0, SHIFT_0));
#endif
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put int32_t value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_int32_at (bb_buffer_t *buffer, size_t n, int32_t value)
{
  bb_set_index(buffer, n);
  bb_put_int32(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put int64_t value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_int64 (bb_buffer_t *buffer, int64_t value)
{
  uint64_t ui64 = bb_s64_to_u64(value);
#if (BYTE_ORDER == BIG_ENDIAN)
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_0, SHIFT_0));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_3, SHIFT_24));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_4, SHIFT_32));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_5, SHIFT_40));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_6, SHIFT_48));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_7, SHIFT_56));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_7, SHIFT_56));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_6, SHIFT_48));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_5, SHIFT_40));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_4, SHIFT_32));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_3, SHIFT_24));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_2, SHIFT_16));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_1, SHIFT_8));
  bb_put(buffer, bb_u64_shift_right(ui64, MASK_64_BYTE_0, SHIFT_0));
#endif
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put int64_t value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_int64_at (bb_buffer_t *buffer, size_t n, int64_t value)
{
  bb_set_index(buffer, n);
  bb_put_int64(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put varchar value in buffer at index ().
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_varchar (bb_buffer_t *buffer, char const *value)
{
  for (char *ch = (char *)value; *ch != '\0'; ++ch)
    bb_put_char(buffer, *ch);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Put varchar value in buffer at index (n).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void bb_put_varchar_at (bb_buffer_t *buffer, size_t n, char const *value)
{
  bb_set_index(buffer, n);
  bb_put_varchar(buffer, value);
}

