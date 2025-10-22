#-------------------------------------------------------------------------------
#   ByteBuffer Implementation in x86_64 Assembly Language with C Interface
#
#   Copyright (C) 2025  J. McIntosh
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#-------------------------------------------------------------------------------
libbytebuffer.so: bytebuffer_asm.o bytebuffer.o
	gcc -g -march=x86-64 -m64 -Wunused-function -z noexecstack -shared \
		bytebuffer_asm.o bytebuffer.o -lm -o libbytebuffer.so
bytebuffer.o: bytebuffer.c
	gcc -g -march=x86-64 -m64 -lm -Wall -fPIC -c bytebuffer.c -o bytebuffer.o
bytebuffer_asm.o: bytebuffer.asm
	nasm -g -f elf64 bytebuffer.asm -o bytebuffer_asm.o
clean:
	rm -f libbytebuffer.so bytebuffer.o bytebuffer_asm.o
