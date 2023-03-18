# intel_8086_instruction_decoder
Software implementation of Intel 8086/88 instruction decoding.

* Implements a subset of MOV instructions. 

Done as an exercise for the "Performance-Aware Programming" series by Casey Muratori: https://www.computerenhance.com/

### Building

Simply compiling `main.c` with any C compiler is enough, no build tools are needed. For example:    

`gcc main.c -o decoder`

### Usage

`./decoder [filename]`

```
$ ./decoder program_input/listing_0039_more_movs
bits 16

mov si, bx
mov dh, al
mov cl, 12
mov ch, 244
mov cx, 12
mov cx, 65524
mov dx, 3948
mov dx, 61588
mov al, [bx + si]
mov bx, [bp + di]
mov dx, [bp]
mov ah, [bx + si + 4]
mov al, [bx + si + 4999]
mov [bx + di], cx
mov [bp + si], cl
mov [bp], ch
```
