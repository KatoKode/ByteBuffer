
---

Just Another Armchair Programmer

ByteBuffer Implementation in C

by Jerry McIntosh

---

# INTRODUCTION
This is an C implementation of a ByteBuffer.  The ByteBuffer is implemented as a shared-library. There is also a C demo program.

## LIST OF REQUIREMENTS:

+ Linux OS
+ Programming languages: C
+ GCC compiler, and the make utility
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
#define BUFFER_SIZE    28
```
Modifying the define will change the amount of memory allocated to the ByteBuffer.

Have Fun!

---
