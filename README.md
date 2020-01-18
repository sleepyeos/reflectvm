#ReflectVM

A tiny, 8-bit, Gameboy inspired virtual machine for Linux, Mac, and Windows

## Summary

 * Variable-length instructions (Inspired by Nintendo's Gameboy)
 * Big-Endian architecture
 * 16 8-bit registers (r0 - rF)
 * Registers addressable as 16-bit pairs (r0:r1)
 * 64 KiB addressable memory ($0000 - $FFFF)
 * Stack grows downwards from $FFFF
 * Console I/O performed with `sys` calls


##Example Programs:

**Hello World**

```
	;; Register r3 will hold the length of the string
	mov r3, $12

	;; Registers r0:r1 will hold a pointer to the string
	mov r0:r1, _s
_loop:
	;; Output the character located at the address
	;; pointed to by r0:r1
	sys r0:r1, $02

	;; Increment r1 to move to the next character
	inc r1

	;; Decrement r3 to keep track of how many
	;; characters we have left to print
	dec r3

	;; If there are still more characters, jump
	;; to the beginning of the loop
	jnz _loop

	;; Otherwise, exit the program
	hlt

	;; This block is simply "Hello, ReflectVM!\n"
	;; encoded in ASCII
_s:	
	db 48 65 6C 6C 6F 2C 20 52 65
	db 66 6C 65 63 74 56 4D 21 0A


```



**Echo**

```
	;; This program reads a line from stdin
	;; and echos it back to stdout
	call _prompt
	mov r0:r1, $1000
_input_loop:	
	sys r0:r1, $3
	mov r3, [r0:r1]
	inc r1
	cmp r3, $0A
	jnz _input_loop
	mov r2, r1
	mov r0:r1, $1000
_output_loop:
	sys r0:r1, $2
	inc r1
	cmp r1, r2
	jnz _output_loop
	hlt
_prompt:
	mov r0:r1, _prompt_text
	sys r0:r1, $2
	ret
_prompt_text:
	db 3E


```


**FizzBuzz**

```
_start:
	mov r0, $00
	mov r8:r9, $1000
_loop:
	mov r2, $00
	mod r0, $03
	jnz _skipfizz
	call _print_fizz
	mov r2, $01
_skipfizz:
	mod r0, $05
	jnz _skipbuzz
	call _print_buzz
	mov r2, $01
_skipbuzz:
	cmp r2, $00
	jnz _endloop
	mov [r8:r9], r0
	sys r8:r9, $06
_endloop:
	call _newline
	inc r0
	cmp r0, $65
	jnz _loop
	hlt
_print_fizz:
	mov r4:r5 _fizztext
	mov r6, $04
	call _print_loop
	ret
_print_buzz:
	mov r4:r5 _buzztext
	mov r6, $04
	call _print_loop
	ret
_print_loop:	
	sys r4:r5, $02
	inc r5
	dec r6
	jnz _print_loop
	ret
_buzztext:
	db 62 75 7A 7A
_fizztext:
	db 66 69 7A 7A
_newline:
	push $0A
	sys $00
	ret


```


## Instruction Set:

ReflectVM currently has 38 distinct instructions; this number may grow slightly as new features are implemented. Below is a table of each opcode along with an example asm instruction and the full hex value that it will assemble to. Note that the instructions are arranged with the first byte indicating the operation, the high 4 bits of the second byte indicating the first register, and the low 4 bits of the second byte indicating the second register. If 3 registers are involved, the second byte indicates the pair and a third byte will indicate the remaining register.


| **Opcode** | **Assembly Example** | **Assembled Output (hex)** |             **Notes**              |
|------------|----------------------|----------------------------|------------------------------------|
| 0x00       | nop                  | 0x00 0x00                  |                                    |
| 0x01       | mov r5, r9           | 0x01 0x59                  |                                    |
| 0x02       | mov r5, $7B          | 0x02 0x50 0x7B             |                                    |
| 0x03       | mov [$7FFF], r5      | 0x03 0x05 0x7F 0xFF        |                                    |
| 0x04       | mov r5, [$7FFF]      | 0x04 0x50 0x7F 0xFF        |                                    |
| 0x05       | mov r0:r1, $1000     | 0x05 0x01 0x10 0x00        |                                    |
| 0x06       | mov [r0:r1], $5F     | 0x06 0x01 0x5F             |                                    |
| 0x07       | mov [r0:r1], r2      | 0x07 0x01 0x02             |                                    |
| 0x08       | mov r2, [r0:r1]      | 0x08 0x01 0x02             |                                    |
| 0x09       | hlt                  | 0x09 0x00                  |                                    |
| 0x0A       | add r0, r1           | 0x0A 0x01                  |                                    |
| 0x0B       | sub r0, r1           | 0x0B 0x01                  |                                    |
| 0x0C       | inc r3               | 0x0C 0x30                  |                                    |
| 0x0D       | dec r3               | 0x0D 0x30                  |                                    |
| 0x0E       | cmp r5, r9           | 0x0E 0x59                  | cmp instructions set the zero flag |
| 0x0F       | cmp r5, $7B          | 0x0F 0x50, 0x7B            |                                    |
| 0x10       | jmp $7FFF            | 0x10 0x00 0x7F 0xFF        |                                    |
| 0x11       | jz $7FFF             | 0x11 0x00 0x7F 0xFF        |                                    |
| 0x12       | jnz $7FFF            | 0x12 0x00 0x7F 0xFF        |                                    |
| 0x13       | jmp r7:r8            | 0x13 0x78                  |                                    |
| 0x14       | jz r7:r8             | 0x14 0x78                  |                                    |
| 0x15       | jnz r7:r8            | 0x15 0x78                  |                                    |
| 0x16       | call $7FFF           | 0x16 0x00 0x7F 0xFF        |                                    |
| 0x17       | call r3:r4           | 0x17 0x34                  |                                    |
| 0x18       | ret                  | 0x18 0x00                  |                                    |
| 0x19       | push r3              | 0x19 0x03                  |                                    |
| 0x1A       | pop r2               | 0x1A 0x20                  |                                    |
| 0x1B       | push $4A             | 0x1B 0x00 0x4A             |                                    |
| 0x1C       | and r0, r1           | 0x1C 0x01                  |                                    |
| 0x1D       | or r3, r4            | 0x1D 0x34                  |                                    |
| 0x1E       | xor r3, r4           | 0x1E 0x34                  |                                    |
| 0x1F       | mul r3, r4           | 0x1F 0x34                  |                                    |
| 0x20       | sys $0               | 0x20 0x00 0x00             | A table of sys calls is below      |
| 0x21       | div r3, r4           | 0x21 0x34                  |                                    |
| 0x22       | mul r3, $0A          | 0x22 0x30 0x0A             |                                    |
| 0x23       | div r3, $0A          | 0x23 0x30 0x0A             |                                    |
| 0x24       | mod r0, r1           | 0x24 0x01                  | mod instructions set the zero flag |
| 0x25       | mod r5, $02          | 0x25 0x50, 0x02            |                                    |


## Sys Calls

`sys` calls provide a way for the VM to peform console I/O. File and network `sys` calls may be added to the VM in a future release.

| Assembly Example | Description                                                         | Assembled Output (hex) |
|------------------|---------------------------------------------------------------------|------------------------|
| sys $00          | Pop character off the stack and print it to stdout                  | 0x20 0x00 0x00         |
| sys $01          | Read a character from stdin and push it onto the stack              | 0x20 0x00 0x01         |
| sys r0:r1, $02   | Print the character value stored in the address pointed to by r0:r1 | 0x20 0x01 0x02         |
| sys r0:r1, $03   | Read a character from stdin into the address pointed to by r0:r1    | 0x20 0x01 0x03         |
| sys $04          | Pop an integer off the stack and print it to stdout                 | 0x20 0x00 0x04         |
| sys $05          | Read an integer from stdin and push it onto the stack               | 0x20 0x00 0x05         |
| sys r0:r1, $06   | Print the integer value stored in the address pointed to by r0:r1   | 0x20 0x01 0x06         |
| sys r0:r1, $07   | Read an integer from stdin into the address pointed to by r0:r1     | 0x20 0x01 0x07         |


## Roadmap

 * Test suites for VM and toolchain
 * Assembler in C
 * File I/O `sys` calls
 * Network I/O `sys` calls
 * Applications for VM
     * Text editor
     * Roguelike
     * Web browser
     * Stack VM (Or Reflect written for Reflect)
     * BASIC interpreter?
 * Android app