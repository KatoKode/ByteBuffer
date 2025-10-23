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
%ifndef BYTEBUFFER_ASM
%define BYTEBUFFER_ASM  1
;
extern calloc
extern free
extern memset
extern strlen
extern memmove64
;
ALIGN_SIZE    EQU     16
ALIGN_WITH    EQU     (ALIGN_SIZE - 1)
ALIGN_MASK    EQU     ~(ALIGN_WITH)
;
;-------------------------------------------------------------------------------
;
%macro ALIGN_STACK_AND_CALL 2-4
      mov     %1, rsp               ; backup stack pointer (rsp)
      and     rsp, QWORD ALIGN_MASK ; align stack pointer (rsp) to
                                    ; 16-byte boundary
      call    %2 %3 %4              ; call C function
      mov     rsp, %1               ; restore stack pointer (rsp)
%endmacro
;
; Example: Call LIBC function
;         ALIGN_STACK_AND_CALL r15, calloc, wrt, ..plt
;
; Example: Call C callback function with address in register (rcx)
;         ALIGH_STACK_AND_CALL r12, rcx
;-------------------------------------------------------------------------------
;
%include "bytebuffer.inc"
;
section .text
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   byte_order_t bb_native_byte_order (void);
;
; return:
;
;   NONE | BIG_END | LITTLE_END
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_native_byte_order:function
bb_native_byte_order:
      mov       eax, 0x01000004
      and       eax, MASK_LSB
      cmp       eax, BIG_END
      je        .have_endian
      cmp       eax, LITTLE_END
      je        .have_endian
      xor       rax, rax
.have_endian:
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Initialize bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int bb_init (bytebuffer_t *bb, size_t size);
;
; param:
;
;   rdi = bb
;   rsi = size
;
; stack:
;
;   QWORD [rbp - 8] = rdi (bb);
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_init:function
bb_init:
      push      rbp
      mov       rbp, rsp
      sub       rsp, 8
      push      rbx
; QWORD [rbp - 8] = rdi (bb);
      mov       QWORD [rbp - 8], rdi
; bb->bound = size;
      mov       QWORD [rdi + bytebuffer.bound], rsi
; bb->index = 0;
      xor       rax, rax
      mov       QWORD [rdi + bytebuffer.index], rax
; bb->mark = -1;
      mov       rax, -1
      mov       QWORD [rdi + bytebuffer.mark], rax
; bb->size = size;
      mov       QWORD [rdi + bytebuffer.size], rsi
; bb->buffer = calloc(1, size);
      ALIGN_STACK_AND_CALL rbx, calloc, wrt, ..plt
      mov       rdi, QWORD [rbp - 8]
      mov       QWORD [rdi + bytebuffer.buffer], rax
; return (bb->buffer ? 1 : -1);
      mov       ebx, 1
      test      rax, rax
      jnz       .success
      neg       ebx
.success:
      mov       eax, ebx
      pop       rbx
      mov       rsp, rbp
      pop       rbp
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Terminate bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_term (bytebuffer_t *bb);
;
; param:
;
;   rdi = bytebuffer
;
; stack:
;
;   QWORD [rbp - 8] = rdi (bb)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_term:function
bb_term:
      push      rbp
      mov       rbp, rsp
      sub       rsp, 8
      push      rbx
; QWORD [rbp - 8] = rdi (bb)
      mov       QWORD [rbp - 8], rdi
; free(bb->buffer);
      mov       rdi, QWORD [rdi + bytebuffer.buffer]
      ALIGN_STACK_AND_CALL rbx, free, wrt, ..plt
; (void) memset(bb, 0, sizeof(bytebuffer_t));
      mov       rdi, QWORD [rbp - 8]
      xor       rsi, rsi
      mov       rdx, bytebuffer_size
      ALIGN_STACK_AND_CALL rbx, memset, wrt, ..plt
; return
      pop       rbx
      mov       rsp, rbp
      pop       rbp
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Return pointer to buffer of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   byte_t* bb_get_buffer (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   rax = address of buffer of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_buffer:function
bb_get_buffer:
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Return the bound of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   size_t bb_get_bound (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   rax = bound of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_bound:function
bb_get_bound:
      mov       rax, QWORD [rdi + bytebuffer.bound]
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Return the index of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   size_t bb_get_index (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   rax = index of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_index:function
bb_get_index:
      mov       rax, QWORD [rdi + bytebuffer.index]
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Is there one or more bytes available in the bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   bool_t bb_has_more (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   0 (false) | 1 (true)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_has_more:function
bb_has_more:
      mov       rax, 1
      mov       rcx, QWORD [rdi + bytebuffer.bound]
      cmp       rcx, QWORD [rdi + bytebuffer.index]
      ja        .has_more
      xor       rax, rax
.has_more:
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Return the number of bytes (if any) before the bound
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   size_t bb_get_remaining (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   rax = number of bytes remaining (if any) before bound
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_remaining:function
bb_get_remaining:
      mov       rax, QWORD [rdi + bytebuffer.bound]
      sub       rax, QWORD [rdi + bytebuffer.index]
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Return the size (in bytes) of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   size_t bb_get_size (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   rax = size (in bytes) of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_size:function
bb_get_size:
      mov       rax, QWORD [rdi + bytebuffer.size]
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Clear a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_clear (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_clear:function
bb_clear:
; bb->bound = bb->size;
      mov       rax, QWORD [rdi + bytebuffer.size]
      mov       QWORD [rdi + bytebuffer.bound], rax
; bb->index = 0;
      xor       rax, rax
      mov       QWORD [rdi + bytebuffer.index], rax
; bb->mark = -1;
      mov       rax, -1
      mov       QWORD [rdi + bytebuffer.mark], rax
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Mark a position in a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_mark (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_mark:function
bb_mark:
; bb->mark = bb->index;
      mov       rax, QWORD [rdi + bytebuffer.index]
      mov       QWORD [rdi + bytebuffer.mark], rax
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Set index to previously marked position in a bytebuffer
; NOTE: Nothing is done if (mark < 0 || index <= mark)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_reset (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_reset:function
bb_reset:
      mov       rax, QWORD [rdi + bytebuffer.mark]
      cmp       rax, 0
      jl        .return
      cmp       rax, QWORD [rdi + bytebuffer.index]
      jae       .return
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Rewind a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_rewind (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_rewind:function
bb_rewind:
; bb->index = 0;
      xor       rax, rax
      mov       QWORD [rdi + bytebuffer.index], rax
; bb->mark = -1;
      mov       rax, -1
      mov       QWORD [rdi + bytebuffer.mark], rax
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Set the index of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_set_index (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_set_index:function
bb_set_index:
; bb->index = index;
      mov       rax, rsi
      mov       QWORD [rdi + bytebuffer.index], rax
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Flip the bound and index of a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_flip (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_flip:function
bb_flip:
; bb->bound = bb->index;
      mov       rax, QWORD [rdi + bytebuffer.index]
      mov       QWORD [rdi + bytebuffer.bound], rax
; bb->index = 0;
      xor       rax, rax
      mov       QWORD [rdi + bytebuffer.index], rax
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get next byte from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   byte_t bb_get (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   al = bb->buffer[bb->index] | '\0'
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get:function
bb_get:
; if (bb->index >= bb->bound) return '\0';
      xor       rcx, rcx
      mov       rax, QWORD [rdi + bytebuffer.index]
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t b = bb->buffer[bb->index];
      mov       rsi, QWORD [rdi + bytebuffer.buffer]
      add       rsi, rax
      mov       cl, BYTE [rsi]
; bb->index += 1;
      inc       rax
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      mov       rax, rcx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a byte at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   byte_t bb_get_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; return:
;
;   al = bb->buffer[index] | '\0'
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_at:function
bb_get_at:
; if (index >= bb->bound) return '\0';;
      xor       rcx, rcx
      cmp       rsi, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t b = bb->buffer[index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rsi, rax
      mov       cl, BYTE [rsi]
.return:
      mov       rax, rcx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get next char from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   char bb_get_char (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   al = (char) bb->buffer[bb->index] | '\0'
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_char:function
bb_get_char:
      call      bb_get
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a char at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   char bb_get_char_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
;
; return:
;
;   al = (char) bb->buffer[index] | '\0'
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_char_at:function
bb_get_char_at:
      call      bb_get_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a double from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   double bb_get_double (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   xmm0 = double value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_double:function
bb_get_double:
      push      rbx
; clear xmm0 register
      xorpd     xmm0, xmm0
; if (bb->index + 8 >= bb->bound) return DBLNAN;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 4
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_32
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 5
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_40
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 6
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_48
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 7
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_56
      shlx      rbx, rax, rcx
      or        rdx, rbx
; bb->index += 8;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      mov       QWORD [rdi + bytebuffer.index], rax
; return double value
      movq      xmm0, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a double at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   double bb_get_double_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; return:
;
;   xmm0 = double value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_double_at:function
bb_get_double_at:
      push      rbx
; if (index + 8 >= bb->bound) return DBLNAN;
      xorpd     xmm0, xmm0
      mov       rax, rsi
      add       rax, 8
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, rsi
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 4
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_32
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 5
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_40
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 6
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_48
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 7
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_56
      shlx      rbx, rax, rcx
      or        rdx, rbx
; return double value
      movq      xmm0, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a float from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   float bb_get_float (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   xmm0 = float value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_float:function
bb_get_float:
      push      rbx
; if (bb->index + 4 >= bb->bound) return 0;
      xorpd     xmm0, xmm0
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
; bb->index += 4;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      mov       QWORD [rdi + bytebuffer.index], rax
; return uint32_t value
      movq      xmm0, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a float at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   float bb_get_float_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; return:
;
;   xmm0 = float value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_float_at:function
bb_get_float_at:
      push      rbx
; if (index + 4 >= bb->bound) return 0;
      xorpd     xmm0, xmm0
      mov       rax, rsi
      add       rax, 4
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[index];
      add       rsi, QWORD [rdi + bytebuffer.buffer]
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
; return uint32_t value
      movq      xmm0, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a int16_t from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int16_t bb_get_int16 (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = bb->index
;
; return:
;
;   ax = int16_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_int16:function
bb_get_int16:
      call      bb_get_uint16
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get an int16_t at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int16_t bb_get_int16_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = bb->index
;
; return:
;
;   ax = int16_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_int16_at:function
bb_get_int16_at:
      call      bb_get_uint16_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a int32_t from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int32_t bb_get_int32 (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   eax = int32_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_int32:function
bb_get_int32:
      call      bb_get_uint32
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a int32_t at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int32_t bb_get_int32_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = bb->index
;
; return:
;
;   eax = int32_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_int32_at:function
bb_get_int32_at:
      call      bb_get_uint32_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a int64_t from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int64_t bb_get_int64 (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; stack:
;
;   QWORD [rbp - 8]   = b7
;   QWORD [rbp - 16]  = b6
;
; return:
;
;   rax = int64_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_int64:function
bb_get_int64:
      call      bb_get_uint64
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a int64_t at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   int64_t bb_get_int64_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = org_index (bb->index)
;
; return:
;
;   rax = int64_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_int64_at:function
bb_get_int64_at:
      call      bb_get_uint64_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a uint16_t from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   uint16_t bb_get_uint16 (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   ax = uint16_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_uint16:function
bb_get_uint16:
      push      rbx
; if (bb->index + 2 >= bb->bound) return 0;
      xor       rax, rax
      mov       rcx, QWORD [rdi + bytebuffer.index]
      add       rcx, 2
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
; bb->index += 2;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 2
      mov       QWORD [rdi + bytebuffer.index], rax
; return uint16_t value
      mov       rax, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get an uint16_t at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   uint16_t bb_get_uint16_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; return:
;
;   ax = uint16_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_uint16_at:function
bb_get_uint16_at:
      push      rbx
; if (index + 2 >= bb->bound) return 0;
      xor       rax, rax
      mov       rcx, rsi
      add       rcx, 2
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, rsi
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
; return uint16_t value
      mov       rax, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a uint32_t from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   uint32_t bb_get_uint32 (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   eax = uint32_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_uint32:function
bb_get_uint32:
      push      rbx
; if (bb->index + 4 >= bb->bound) return 0;
      xor       rax, rax
      mov       rcx, QWORD [rdi + bytebuffer.index]
      add       rcx, 4
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
; bb->index += 4;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      mov       QWORD [rdi + bytebuffer.index], rax
; return uint32_t value
      mov       rax, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a uint32_t at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   uint32_t bb_get_uint32_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; return:
;
;   eax = uint32_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_uint32_at:function
bb_get_uint32_at:
      push      rbx
; if (index + 4 >= bb->bound) return 0;
      xor       rax, rax
      mov       rcx, rsi
      add       rcx, 4
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, rsi
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
; return uint32_t value
      mov       rax, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a uint64_t from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   uint64_t bb_get_uint64 (bytebuffer_t *bb);
;
; param:
;
;   rdi = bb
;
; return:
;
;   rax = uint64_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_uint64:function
bb_get_uint64:
      push      rbx
; if (bb->index + 8 >= bb->bound) return 0;
      xor       rax, rax
      mov       rcx, QWORD [rdi + bytebuffer.index]
      add       rcx, 8
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 4
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_32
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 5
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_40
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 6
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_48
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 7
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_56
      shlx      rbx, rax, rcx
      or        rdx, rbx
; bb->index += 8;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      mov       QWORD [rdi + bytebuffer.index], rax
; return uint64_t value
      mov       rax, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a uint64_t at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   uint64_t bb_get_uint64_at (bytebuffer_t *bb, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; return:
;
;   rax = uint64_t value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_uint64_at:function
bb_get_uint64_at:
      push      rbx
; if (index + 8 >= bb->bound) return 0;
      xor       rax, rax
      mov       rcx, rsi
      add       rcx, 8
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, rsi
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       rsi, rax
; byte 0
      xor       rdx, rdx
      xor       rax, rax
      mov       al, BYTE [rsi]
      or        rdx, rax
      inc       rsi
; byte 1
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_8
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 2
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_16
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 3
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_24
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 4
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_32
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 5
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_40
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 6
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_48
      shlx      rbx, rax, rcx
      or        rdx, rbx
      inc       rsi
; byte 7
      mov       al, BYTE [rsi]
      mov       rcx, SHIFT_56
      shlx      rbx, rax, rcx
      or        rdx, rbx
; return uint64_t value
      mov       rax, rdx
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a varchar from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   char* bb_get_varchar (bytebuffer_t *bb, size_t size);
;
; param:
;
;   rdi = bb
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = rsi (size)
;   QWORD [rbp - 24]  = buffer
;
; return:
;
;   rax = varchar | NULL
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_varchar:function
bb_get_varchar:
; prologue
      push      rbp
      mov       rbp, rsp
      sub       rsp, 24
      push      rbx
; QWORD [rbp - 8] = rdi (bb)
      mov       QWORD [rbp - 8], rdi
; QWORD [rbp - 16] = rsi (size)
      mov       QWORD [rbp - 16], rsi
; if (bb->index + size >= bb->bound) return NULL;
      xor       rax, rax
      mov       rcx, QWORD [rdi + bytebuffer.index]
      add       rcx, rsi
      cmp       rcx, QWORD [rdi + bytebuffer.bound]
      ja        .epilogue
; if ((buffer = calloc(1, size)) == NULL) return NULL;
      mov       rdi, 1
      inc       rsi
      ALIGN_STACK_AND_CALL rbx, calloc, wrt, ..plt
      mov       QWORD [rbp - 24], rax
      test      rax, rax
      jz        .epilogue
; (void)memmove64(buffer, &bb->buffer[bb->index], size);
      mov       rdi, QWORD [rbp - 8]
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       rsi, rax
      mov       rdi, QWORD [rbp - 24]
      mov       rdx, QWORD [rbp - 16]
      call      memmove64 wrt ..plt
; bb->index = bb->index + size
      mov       rdi, QWORD [rbp - 8]
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rbp - 16]
      mov       QWORD [rdi + bytebuffer.index], rax
; return buffer;
      mov       rax, QWORD [rbp - 24]
.epilogue:
      pop       rbx
      mov       rsp, rbp
      pop       rbp
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Get a varchar at index from a bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   char bb_get_varchar_at (bytebuffer_t *bb, size_t size, size_t index);
;
; param:
;
;   rdi = bb
;   rsi = index
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = rsi (size)
;   QWORD [rbp - 24]  = rdx (index)
;   QWORD [rbp - 32]  = (buffer)
;
; return:
;
;   rax = varchar | NULL
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_get_varchar_at:function
bb_get_varchar_at:
; prologue
      push      rbp
      mov       rbp, rsp
      sub       rsp, 24
; QWORD [rbp - 8] = rdi (bb)
      mov       QWORD [rbp - 8], rdi
; QWORD [rbp - 16] = rsi (size)
      mov       QWORD [rbp - 16], rsi
; QWORD [rbp - 24] = rdx (index)
      mov       QWORD [rbp - 24], rdx
; if (index + size >= bb->bound) return NULL;
      mov       rax, rsi
      add       rax, rdx
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .epilogue
; if ((buffer = calloc(1, size)) == NULL) return NULL;
      mov       rdi, 1
      inc       rsi
      ALIGN_STACK_AND_CALL rbx, calloc, wrt, ..plt
      mov       QWORD [rbp - 32], rax
      test      rax, rax
      jz        .epilogue
; (void)memmove64(buffer, &bb->buffer[bb->index], size);
      mov       rdi, QWORD [rbp - 8]
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rbp - 24]
      mov       rsi, rax
      mov       rdi, QWORD [rbp - 32]
      mov       rdx, QWORD [rbp - 16]
      call      memmove64 wrt ..plt
.epilogue:
      mov       rsp, rbp
      pop       rbp
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put byte_t value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put (bytebuffer_t *bb, byte_t value);
;
; param:
;
;   rdi = bb
;   rsi = value
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put:function
bb_put:
; if (bb->index >= bb->bound) return;
      mov       rax, QWORD [rdi + bytebuffer.index]
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; bb->buffer[index] = value;
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       BYTE [rax], sil
      mov       rax, QWORD [rdi + bytebuffer.index]
      inc       rax
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put byte_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_at (bytebuffer_t *bb, size_t index, byte_t value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_at:function
bb_put_at:
; if (index >= bb->bound) return;
      mov       rax, rsi
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; bb->buffer[index] = value;
      add       rax, QWORD [rdi + bytebuffer.buffer]
      mov       BYTE [rax], dl
.return:
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put char value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_char (bytebuffer_t *bb, char value);
;
; param:
;
;   rdi = bb
;   rsi = value
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_char:function
bb_put_char:
      call      bb_put
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put char value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_char_at (bytebuffer_t *bb, size_t index, byte_t value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_char_at:function
bb_put_char_at:
      call      bb_put_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put double value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_double (bytebuffer_t *bb, double value);
;
; param:
;
;   rdi   = bb
;   xmm0  = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_double:function
bb_put_double:
      push      rbx
; if (bb->index + 8 >= bb->bound) return;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       rsi, rax
; put byte 0 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 2 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 3 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 4 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_4
      and       rax, rbx
      mov       rcx, SHIFT_32
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 5 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_5
      and       rax, rbx
      mov       rcx, SHIFT_40
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 6 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_6
      and       rax, rbx
      mov       rcx, SHIFT_48
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 7 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_7
      and       rax, rbx
      mov       rcx, SHIFT_56
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; bb->index += 8;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put double value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_double_at (bytebuffer_t *bb, size_t index, double value);
;
; param:
;
;   rdi   = bb
;   rsi   = index
;   xmm0  = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_double_at:function
bb_put_double_at:
      push      rbx
; if (index + 8 >= bb->bound) return;
      mov       rax, rsi
      add       rax, 8
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, rsi
      mov       rsi, rax
; put byte 0 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 2 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 3 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 4 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_4
      and       rax, rbx
      mov       rcx, SHIFT_32
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 5 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_5
      and       rax, rbx
      mov       rcx, SHIFT_40
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 6 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_6
      and       rax, rbx
      mov       rcx, SHIFT_48
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 7 of double value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_7
      and       rax, rbx
      mov       rcx, SHIFT_56
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put float value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_float (bytebuffer_t *bb, float value);
;
; param:
;
;   rdi   = bb
;   xmm0  = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_float:function
bb_put_float:
      push      rbx
; if (bb->index + 4 >= bb->bound) return;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       rsi, rax
; put byte 0 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 2 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 3 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
; update index of bytebuffer
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put float value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_float_at (bytebuffer_t *bb, size_t index, float value);
;
; param:
;
;   rdi   = bb
;   rsi   = index
;   xmm0  = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_float_at:function
bb_put_float_at:
      push      rbx
; if (index + 4 >= bb->bound) return;
      mov       rax, rsi
      add       rax, 4
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rsi, rax
; put byte 0 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 2 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 3 of float value into bytebuffer
      movq      rax, xmm0
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put int16_t value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_int16 (bytebuffer_t *bb, int16_t value);
;
; param:
;
;   rdi = bb
;   rsi = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_int16:function
bb_put_int16:
      call      bb_put_uint16
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put int16_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_int16_at (bytebuffer_t *bb, size_t index, int16 value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_int16_at:function
bb_put_int16_at:
      call      bb_put_uint16_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put int32_t value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_int32 (bytebuffer_t *bb, int32_t value);
;
; param:
;
;   rdi = bb
;   rsi = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_int32:function
bb_put_int32:
      call      bb_put_uint32
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put int32_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_int32_at (bytebuffer_t *bb, size_t index, int32_t value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_int32_at:function
bb_put_int32_at:
      call      bb_put_uint32_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put int64 value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_int64 (bytebuffer_t *bb, int64 value);
;
; param:
;
;   rdi = bb
;   rsi = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_int64:function
bb_put_int64:
      call      bb_put_uint64
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put int64_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_int64_at (bytebuffer_t *bb, size_t index, int64_t value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_int64_at:function
bb_put_int64_at:
      call      bb_put_uint64_at
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put uint16_t value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_uint16 (bytebuffer_t *bb, uint16_t value);
;
; param:
;
;   rdi = bb
;   rsi = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_uint16:function
bb_put_uint16:
      push      rbx
; if (bb->index + 2 >= bb->bound) return;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 2
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       r8, rax
; put byte 0 of uint16_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [r8], al
      inc       r8
; put byte 1 of uint16_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
; bb->index += 2;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 2
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put uint16_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_uint16_at (bytebuffer_t *bb, size_t index, uint16 value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_uint16_at:function
bb_put_uint16_at:
      push      rbx
; if (index + 2 >= bb->bound) return;
      mov       rax, rsi
      add       rax, 2
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rsi, rax
; put byte 0 of uint16_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of uint16_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put uint32_t value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_uint32 (bytebuffer_t *bb, uint32_t value);
;
; param:
;
;   rdi = bb
;   rsi = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_uint32:function
bb_put_uint32:
      push      rbx
; if (bb->index + 4 >= bb->bound) return;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, qword [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       r8, rax
; put byte 0 of uint32_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [r8], al
      inc       r8
; put byte 1 of uint32_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 2 of uint32_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 3 of uint32_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; update index of bytebuffer
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 4
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put uint32_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_uint32_at (bytebuffer_t *bb, size_t index, uint32_t value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_uint32_at:function
bb_put_uint32_at:
      push      rbx
; if (index + 4 >= bb->bound) return;
      mov       rax, rsi
      add       rax, 4
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, qword [rdi + bytebuffer.buffer]
      add       rsi, rax
; put byte 0 of uint32_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of uint32_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 2 of uint32_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 3 of uint32_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put uint64 value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_uint64 (bytebuffer_t *bb, uint64 value);
;
; param:
;
;   rdi = bb
;   rsi = value
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_uint64:function
bb_put_uint64:
      push      rbx
; if (bb->index + 8 >= bb->bound) return;
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[bb->index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       r8, rax
; put byte 0 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [r8], al
      inc       r8
; put byte 1 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 2 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 3 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 4 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_4
      and       rax, rbx
      mov       rcx, SHIFT_32
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 5 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_5
      and       rax, rbx
      mov       rcx, SHIFT_40
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 6 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_6
      and       rax, rbx
      mov       rcx, SHIFT_48
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; put byte 7 of uint64_t value into bytebuffer
      mov       rax, rsi
      mov       rbx, MASK_64_BYTE_7
      and       rax, rbx
      mov       rcx, SHIFT_56
      shrx      rbx, rax, rcx
      mov       BYTE [r8], bl
      inc       r8
; update index of bytebuffer
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, 8
      mov       QWORD [rdi + bytebuffer.index], rax
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put uint64_t value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_uint64_at (bytebuffer_t *bb, size_t index, uint64_t value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = org_index (bb->index)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_uint64_at:function
bb_put_uint64_at:
      push      rbx
; if (index + 8 >= bb->bound) return;
      mov       rax, rsi
      add       rax, 8
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .return
; byte_t *bp = &bb->buffer[index];
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rsi, rax
; put byte 0 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_0
      and       rax, rbx
      mov       BYTE [rsi], al
      inc       rsi
; put byte 1 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_1
      and       rax, rbx
      mov       rcx, SHIFT_8
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 2 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_2
      and       rax, rbx
      mov       rcx, SHIFT_16
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 3 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_3
      and       rax, rbx
      mov       rcx, SHIFT_24
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 4 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_4
      and       rax, rbx
      mov       rcx, SHIFT_32
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 5 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_5
      and       rax, rbx
      mov       rcx, SHIFT_40
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 6 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_6
      and       rax, rbx
      mov       rcx, SHIFT_48
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
      inc       rsi
; put byte 7 of uint64_t value into bytebuffer
      mov       rax, rdx
      mov       rbx, MASK_64_BYTE_7
      and       rax, rbx
      mov       rcx, SHIFT_56
      shrx      rbx, rax, rcx
      mov       BYTE [rsi], bl
.return:
      pop       rbx
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put varchar value in bytebuffer
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_varchar (bytebuffer_t *bb, char *value);
;
; param:
;
;   rdi = bb
;   rsi = value
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = rsi (value)
;   QWORD [rbp - 24]  = value_len (length of value)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_varchar:function
bb_put_varchar:
; prologue
      push      rbp
      mov       rbp, rsp
      sub       rsp, 24
; QWORD [rbp - 8] = rdi (bb)
      mov       QWORD [rbp - 8], rdi
; QWORD [rbp - 16] = rsi (value)
      mov       QWORD [rbp - 16], rsi
; size_t value_len = strlen(value);
      mov       rdi, rsi
      call      strlen wrt ..plt
      mov       QWORD [rbp - 24], rax
; if (bb->index + strlen(value) >= bb->bound) return;
      mov       rdi, QWORD [rbp - 8]
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rbp - 24]
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .epilogue
; (void)memmove64(&bb->buffer[bb->index], value, value_len);
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rdi + bytebuffer.index]
      mov       rdi, rax
      mov       rsi, QWORD [rbp - 16]
      mov       rdx, QWORD [rbp - 24]
      call      memmove64 wrt ..plt
; bb_set_index(bb->index + strlen(value))
      mov       rdi, QWORD [rbp - 8]
      mov       rax, QWORD [rdi + bytebuffer.index]
      add       rax, QWORD [rbp - 24]
      mov       QWORD [rdi + bytebuffer.index], rax
.epilogue:
      mov       rsp, rbp
      pop       rbp
      ret
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Put varchar value in bytebuffer at index
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; C definition
;
;   void bb_put_varchar_at (bytebuffer_t *bb, size_t index, char *value);
;
; param:
;
;   rdi = bb
;   rsi = index
;   rdx = value
;
; stack:
;
;   QWORD [rbp - 8]   = rdi (bb)
;   QWORD [rbp - 16]  = rsi (index)
;   QWORD [rbp - 24]  = rdx (value)
;   QWORD [rbp - 32]  = value_len (length of value)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;
      global bb_put_varchar_at:function
bb_put_varchar_at:
; prologue
      push      rbp
      mov       rbp, rsp
      sub       rsp, 32
; QWORD [rbp - 8] = rdi (bb)
      mov       QWORD [rbp - 8], rdi
; QWORD [rbp - 16] = rsi (value)
      mov       QWORD [rbp - 16], rsi
; QWORD [rbp - 24] = rdx (value)
      mov       QWORD [rbp - 24], rdx
; size_t value_len = strlen(value);
      mov       rdi, rdx
      call      strlen wrt ..plt
      mov       QWORD [rbp - 32], rax
; if (index + value_len >= bb->bound) return;
      mov       rdi, QWORD [rbp - 8]
      mov       rax, QWORD [rbp - 16]
      add       rax, QWORD [rbp - 32]
      cmp       rax, QWORD [rdi + bytebuffer.bound]
      jae       .epilogue
; (void)memmove64(&bb->buffer[bb->index], value, value_len);
      mov       rax, QWORD [rdi + bytebuffer.buffer]
      add       rax, QWORD [rbp - 16]
      mov       rdi, rax
      mov       rsi, QWORD [rbp - 24]
      mov       rdx, QWORD [rbp - 32]
      call      memmove64 wrt ..plt
.epilogue:
      mov       rsp, rbp
      pop       rbp
      ret
%endif
