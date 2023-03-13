# intel_8086_instruction_decoder
Software implementation of Intel 8086/88 instruction decoding.

* Implements MOV instruction decoding. 

Done as an exercise for the "Performance-Aware Programming" series by Casey Muratori: https://www.computerenhance.com/

### Usage

`./decoder [filename]` e.g. `./decoder program_input/listing_0038_many_register_mov` 

```
$ ./decoder program_input/listing_0038_many_register_mov
bits 16

mov cx, bx
mov ch, ah
mov dx, bx
mov si, bx
mov bx, di
mov al, cl
mov ch, ch
mov bx, ax
mov bx, si
mov sp, di
mov bp, ax
```
