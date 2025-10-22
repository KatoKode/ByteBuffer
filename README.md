
---

ByteBuffer Implementation in x86_64 Assembly Language with C Interface.

by Jerry McIntosh

---

# INTRODUCTION
This is an Assembly Language implementation of a ByteBuffer.  The ByteBuffer is implemented as a shared-library. There is also a C demo program.

## LIST OF REQUIREMENTS:

+ Linux OS
+ Programming languages: Assembly, C
+ Netwide Assembler (NASM), GCC compiler, and the make utility
+ your favorite text editor
+ and working at the command line

---

# CREATE THE DEMO
Run the following command in the `ByteBuffer-main` folder:
```bash
sh ./bytebuffer_make.sh
```

---

# RUN QUEUE DEMO
In the `demo` folder enter the following command:
```bash
./go_demo.sh
```

---

# THINGS TO KNOW
You can modify a define in the C header file `main.h`:
```c
#define BUFFER_SIZE    256
```
Modifying the define will change the amount of memory allocated to the ByteBuffer.

You can uncomment the define `BB_DEBUG` to display debug output from the demo program.

Remember to recompile the demo program should you modify it:

In the `./demo` folder enter the following:

```bash
make clean; make
```
Have Fun!

---
