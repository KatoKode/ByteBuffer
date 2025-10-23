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
#include "main.h"

int main (int argc, char **argv)
{
  getNativeByteOrder();

  bytebuffer_t *buffer = bb_alloc();

  bb_init(buffer, BUFFER_SIZE, NULL);

  getBufferMeta(buffer);

  putValues(buffer);

  bb_flip(buffer);

  getValues(buffer);

  bb_term(buffer);

  bb_free(buffer);

  return 0;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void getNativeByteOrder()
{
  byte_order_t bo = bb_native_byte_order();
  char bo_text [32] = { "BIG ENDIAN" };
  if (bo == LITTLE_END) strcpy(bo_text, "LITTLE ENDIAN");
  else if (bo == NONE) strcpy(bo_text, "NONE?");
  printf("native byte order: %s\n", bo_text);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void getBufferMeta (bytebuffer_t *buffer)
{
  byte_t *bbuffer = bb_get_buffer(buffer);
  printf ("bb_get_buffer: %p\n", bbuffer);
  size_t bound = bb_get_bound(buffer);
  printf ("bb_get_bound: %lu\n", bound);
  size_t index = bb_get_index(buffer);
  printf ("bb_get_index: %lu\n", index);
  bool_t flag = bb_has_more(buffer);
  printf ("bb_has_more: %hu\n", flag);
  size_t remaining = bb_get_remaining(buffer);
  printf ("bb_remaining: %lu\n", remaining);
  size_t size = bb_get_size(buffer);
  printf ("bb_get_size: %lu\n", size);

}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
double getDouble (bytebuffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_double(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
double getDoubleAt (bytebuffer_t *buffer, size_t index)
{
  (void) bb_get_at(buffer, index);

  return bb_get_double_at(buffer, index + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
float getFloat (bytebuffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_float(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
float getFloatAt (bytebuffer_t *buffer, size_t index)
{
  (void) bb_get_at(buffer, index);

  return bb_get_float_at(buffer, index + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int16_t getI16 (bytebuffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_int16(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int16_t getI16At (bytebuffer_t *buffer, size_t index)
{
  (void) bb_get_at(buffer, index);

  return bb_get_int16_at(buffer, index + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int32_t getI32 (bytebuffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_int32(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int32_t getI32At (bytebuffer_t *buffer, size_t index)
{
  (void) bb_get_at(buffer, index);

  return bb_get_int32_at(buffer, index + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int64_t getI64 (bytebuffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_int64(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int64_t getI64At (bytebuffer_t *buffer, size_t index)
{
  (void) bb_get_at(buffer, index);

  return bb_get_int64_at(buffer, index + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
char * getText (bytebuffer_t *buffer)
{
  size_t n = bb_get(buffer);

  return bb_get_varchar(buffer, n);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
char * getTextAt (bytebuffer_t *buffer, size_t index)
{
  size_t n = bb_get_at(buffer, index);

  return bb_get_varchar_at(buffer, n, index + 1);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GETVALUES
void getValues (bytebuffer_t *buffer)
{
  // getText

  size_t index = bb_get_index(buffer);

  char *text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  // getTextAt

  text = getTextAt(buffer, index);

  printf("text: %s at: %lu\n", text, index);

  free(text);

  union U {
    double dbl;
    uint64_t u64;
    byte_t b[8];
  } u;

  // getDouble

  index = bb_get_index(buffer);

  u.dbl = getDouble(buffer);

  printf("double: %le\n", u.dbl);
#ifdef BB_DEBUG
  printf("DEBUG: %s: ", __func__);
  for (int i = 0; i < 8; ++i) printf("%02X, ", u.b[i]);
  printf("== %le\n", u.dbl);
#endif
  // getDoubleAt

  u.dbl = getDoubleAt(buffer, index);

  printf("double: %le at: %lu\n", u.dbl, index);

  // getText

  index = bb_get_index(buffer);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  // getTextAt

  text = getTextAt(buffer, index);

  printf("text: %s at: %lu\n", text, index);

  free(text);

  // getFloat

  index = bb_get_index(buffer);

  float flt = getFloat(buffer);

  printf("float: %e\n", flt);

  // getFloatAt

  flt = getFloatAt(buffer, index);

  printf("float: %e at: %lu\n", flt, index);

  // getText

  index = bb_get_index(buffer);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  // getTextAt

  text = getTextAt(buffer, index);

  printf("text: %s at: %lu\n", text, index);

  free(text);

  // getI64

  index = bb_get_index(buffer);

  int64_t i64 = getI64(buffer);

  printf("int64_t: %ld\n", i64);

  // getI64At

  i64 = getI64At(buffer, index);

  printf("int64_t: %ld at: %lu\n", i64, index);

  // getText

  index = bb_get_index(buffer);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  // getTextAt

  text = getTextAt(buffer, index);

  printf("text: %s at: %lu\n", text, index);

  free(text);

  // getI16

  index = bb_get_index(buffer);

  int16_t i16 = getI16(buffer);

  printf("int16_t: %hd\n", i16);

  // getI16At

  i16 = getI16At(buffer, index);

  printf("int16_t: %hd at: %lu\n", i16, index);

  // getText

  index = bb_get_index(buffer);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  // getTextAt

  text = getTextAt(buffer, index);

  printf("text: %s at: %lu\n", text, index);

  free(text);

  // getI32

  index = bb_get_index(buffer);

  int32_t i32 = getI32(buffer);

  printf("int32_t: %d\n", i32);

  // getI32At

  index = bb_get_index(buffer);

  i32 = getI32At(buffer, index);

  printf("int32_t: %d at: %lu\n", i32, index);

  for (int i = 0; i < 10; ++i)
  {
    text = getText(buffer);
    printf("text: %s\n", text);
    free(text);
  }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putDouble (bytebuffer_t *buffer, double const value)
{
  bb_put(buffer, (byte_t)8);
#ifdef BB_DEBUG
  size_t index = bb_get_index(buffer);
#endif
  bb_put_double(buffer, value);
#ifdef BB_DEBUG
  printf("DEBUG: %s: ", __func__);
  for (int i = index; i < index + 8; ++i)
    printf("%02X, ", buffer->buffer[i]);
  putchar('\n');
  printf("DEBUG: %le == %le\n",*(double*)(&buffer->buffer[index]), value);
#endif
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putDoubleAt (bytebuffer_t *buffer, size_t index, double const value)
{
  bb_put_at(buffer, index, (byte_t)8);

  bb_put_double_at(buffer, index + 1, value);
#ifdef BB_DEBUG
  printf("DEBUG: %s: ", __func__);
  for (int i = index + 1; i < index + 9; ++i)
    printf("%02X, ", buffer->buffer[i]);
  putchar('\n');
  printf("DEBUG: %le == %le\n",*(double*)(&buffer->buffer[index + 1]), value);
#endif
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putFloat (bytebuffer_t *buffer, float const value)
{
  bb_put(buffer, (byte_t)4);

  bb_put_float(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putFloatAt (bytebuffer_t *buffer, size_t index, float const value)
{
  bb_put_at(buffer, index, (byte_t)4);

  bb_put_float_at(buffer, index + 1, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI16 (bytebuffer_t *buffer, int16_t const value)
{
  bb_put(buffer, (byte_t)2);

  bb_put_int16(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI16At (bytebuffer_t *buffer, size_t index, int16_t const value)
{
  bb_put_at(buffer, index, (byte_t)2);

  bb_put_int16_at(buffer, index + 1, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI32 (bytebuffer_t *buffer, int32_t const value)
{
  bb_put(buffer, (byte_t)4);

  bb_put_int32(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI32At (bytebuffer_t *buffer, size_t index, int32_t const value)
{
  bb_put_at(buffer, index, (byte_t)4);

  bb_put_int32_at(buffer, index + 1, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI64 (bytebuffer_t *buffer, int64_t const value)
{
  bb_put(buffer, (byte_t)8);

  bb_put_int64(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI64At (bytebuffer_t *buffer, size_t index, int64_t const value)
{
  bb_put_at(buffer, index, (byte_t)8);

  bb_put_int64_at(buffer, index + 1, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putText (bytebuffer_t *buffer, char const *text)
{
  size_t n = strlen(text);

  bb_put(buffer, (byte_t)n);

  bb_put_varchar (buffer, text);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putTextAt (bytebuffer_t *buffer, size_t index, char const *text)
{
  size_t n = strlen(text);

  bb_put_at(buffer, index, (byte_t)n);

  bb_put_varchar_at(buffer, index + 1, text);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PUTVALUES
void putValues (bytebuffer_t *buffer)
{
  putText(buffer, "TEXT");
#ifdef BB_DEBUG
// This code was used to display the order of bytes in a double.
  union DU {
    double dbl;
    uint64_t u64;
    byte_t b[8];
  } du;

  du.dbl = DBL_MIN;
  printf("DEBUG: %s: ", __func__);
  for (int i = 0; i < 8; ++i) printf("%02X, ", du.b[i]);
  putchar('\n');
  printf("DEBUG: %le\n", du.dbl);
#endif
  size_t index = bb_get_index(buffer);

  putDouble(buffer, DBL_MAX);

  putDoubleAt(buffer, index, DBL_MIN);

  putText(buffer, "TEXT 1");

  union FU {
    float flt;
    uint32_t u32;
    byte_t b[4];
  } fu;

  fu.flt = 1.05432;
  printf("%s: hex bytes of float: ", __func__);
  for (int i = 0; i < 4; ++i) printf("%02X, ", fu.b[i]);
  printf(" == %f\n", fu.flt);

  index = bb_get_index(buffer);

  putFloat(buffer, FLT_MAX);

  putFloatAt(buffer, index, FLT_MIN);

  putText(buffer, "TEST 22");

  index = bb_get_index(buffer);

  putI64(buffer, LONG_MIN);

  putI64At(buffer, index, LONG_MAX);

  index = bb_get_index(buffer);

  putText(buffer, "TEST 777");

  putTextAt(buffer, index, "TEST 333");

  index = bb_get_index(buffer);

  putI16(buffer, SHRT_MAX);

  putI16At(buffer, index, SHRT_MIN);

  putText(buffer, "TEST 4444");

  index = bb_get_index(buffer);

  putI32(buffer, INT_MIN);

  putI32At(buffer, index, INT_MAX);

  for (int i = 0; i < 10; ++i)
  {
    char txt [ 16 ];
    (void) sprintf(txt, "TEST %02d", i);
    putText(buffer, txt);
  }
}

