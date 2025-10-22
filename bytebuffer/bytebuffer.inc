;-------------------------------------------------------------------------------
;   ByteBuffer Implementation in x86_64 Assembly Language with C Interface
;
;   Copyright (C) 2025  J. McIntosh
;
;   This program is free software; you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation; either version 2 of the License, or
;   (at your option) any later version.
;
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License along
;   with this program; if not, write to the Free Software Foundation, Inc.,
;   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;-------------------------------------------------------------------------------
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
%ifndef BYTEBUFFER_INC
%define BYTEBUFFER_INC  1
;
MASK_LSB      EQU     0x000000FF
NONE          EQU     0
BIG_END       EQU     1
LITTLE_END    EQU     4
;
MASK_16_BYTE_0			EQU			0x00FF
MASK_16_BYTE_1			EQU			0xFF00
;
MASK_32_BYTE_0			EQU			0x000000FF
MASK_32_BYTE_1			EQU			0x0000FF00
MASK_32_BYTE_2			EQU			0x00FF0000
MASK_32_BYTE_3			EQU			0xFF000000
;
MASK_64_BYTE_0			EQU			0x00000000000000FF
MASK_64_BYTE_1			EQU			0x000000000000FF00
MASK_64_BYTE_2			EQU			0x0000000000FF0000
MASK_64_BYTE_3			EQU			0x00000000FF000000
MASK_64_BYTE_4			EQU			0x000000FF00000000
MASK_64_BYTE_5			EQU			0x0000FF0000000000
MASK_64_BYTE_6			EQU			0x00FF000000000000
MASK_64_BYTE_7			EQU			0xFF00000000000000
;
SHIFT_0 			EQU			0
SHIFT_8 			EQU			8
SHIFT_16			EQU			16
SHIFT_24			EQU			24
SHIFT_32			EQU			32
SHIFT_40			EQU			40
SHIFT_48			EQU			48
SHIFT_56			EQU			56
;
struc bytebuffer
  .bound:       resq      1     ; upper bound of bytebuffer
  .index:       resq      1     ; index of next byte to be read/written
  .mark:        resq      1     ; marked position in bytebuffer
  .size:        resq      1     ; size of bytebuffer
  .buffer:      resq      1     ; pointer to buffer
endstruc
;
%endif
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
