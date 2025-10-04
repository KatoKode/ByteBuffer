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
#include "main.h"

int main (int argc, char **argv)
{
  getNativeByteOrder();

  bb_buffer_t *buffer = bb_buffer_alloc();

  bb_buffer_init(buffer, BUFFER_SIZE, NULL);

  putValues(buffer);

  bb_trip(buffer);

  getValues(buffer);

  toDouble();

  toFloat();

  bb_buffer_term(buffer);

  bb_buffer_free(buffer);

  return 0;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void getNativeByteOrder()
{
  byte_order_t bo = bb_native_byte_order();
  char bo_text [32] = { "BIG_ENDIAN" };
  if (bo == LITTLE_END) strcpy(bo_text, "LITTLE_END");
  else if (bo == NONE) strcpy(bo_text, "NONE");
  printf("native byte order: %s\n", bo_text);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
double getDouble (bb_buffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_double(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
float getfloat (bb_buffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_float(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int16_t getI16 (bb_buffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_int16(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int32_t getI32 (bb_buffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_int32(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
int64_t getI64 (bb_buffer_t *buffer)
{
  (void) bb_get(buffer);

  return bb_get_int64(buffer);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
char * getText (bb_buffer_t *buffer)
{
  size_t n = bb_get(buffer);

  return bb_get_varchar(buffer, n);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GETVALUES
void getValues (bb_buffer_t *buffer)
{
  char *text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  double dbl = getDouble(buffer);

  printf("double: %1.11lf\n", dbl);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  float flt = getfloat(buffer);

  printf("float: %f\n", flt);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  int64_t i64 = getI64(buffer);

  printf("int64_t: %ld\n", i64);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  int16_t i16 = getI16(buffer);

  printf("int16_t: %hd\n", i16);

  text = getText(buffer);

  printf("text: %s\n", text);

  free(text);

  int32_t i32 = getI32(buffer);

  printf("int32_t: %d\n", i32);

  for (int i = 0; i < 10; ++i)
  {
    text = getText(buffer);
    printf("text: %s\n", text);
    free(text);
  }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putDouble (bb_buffer_t *buffer, double const value)
{
  bb_put(buffer, (byte_t)8);

  bb_put_double(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putFloat (bb_buffer_t *buffer, float const value)
{
  bb_put(buffer, (byte_t)4);

  bb_put_float(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI16 (bb_buffer_t *buffer, int16_t const value)
{
  bb_put(buffer, (byte_t)2);

  bb_put_int16(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI32 (bb_buffer_t *buffer, int32_t const value)
{
  bb_put(buffer, (byte_t)4);

  bb_put_int32(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putI64 (bb_buffer_t *buffer, int64_t const value)
{
  bb_put(buffer, (byte_t)8);

  bb_put_int64(buffer, value);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
void putText (bb_buffer_t *buffer, char const *text)
{
  size_t n = strlen(text);

  bb_put(buffer, (byte_t)n);

  bb_put_varchar (buffer, text);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PUTVALUES
void putValues (bb_buffer_t *buffer)
{
  putText(buffer, "TEXT");

  putDouble(buffer, 0.00009876543);

  putText(buffer, "TEXT 1");

  putFloat(buffer, 1.05432);

  putText(buffer, "TEST 22");

  putI64(buffer, 9876543210);

  putText(buffer, "TEST 333");

  putI16(buffer, -32768);

  putText(buffer, "TEST 4444");

  putI32(buffer, 131072);

  for (int i = 0; i < 10; ++i)
  {
    char txt [ 16 ];
    (void) sprintf(txt, "TEST %02d", i);
    putText(buffer, txt);
  }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODOUBLE
void toDouble ()
{
  union U {
    double    d;
    byte_t    b [ sizeof(double) ];
  } u;
  u.d = 1234567890.0987654;
  double e = bb_to_double(u.b[0], u.b[1], u.b[2], u.b[3], u.b[4], u.b[5], u.b[6],
      u.b[7]);
  printf("double u.d: %18.7lf\tdouble e: %18.7lf\n", u.d, e);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TOFLOAT
void toFloat ()
{
  union U {
    float     f;
    byte_t    b [ sizeof(float) ];
  } u;
  u.f = 1.098765;
  float e = bb_to_float(u.b[0], u.b[1], u.b[2], u.b[3]);
  printf("float u.f: %8.6f\tfloat e: %8.6f\n", u.f, e);
}

