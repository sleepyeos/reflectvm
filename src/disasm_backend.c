/*
 * anewkirk 
 *
 * A disassembler backend for ReflectVM application images; this file handles
 * the disassembly of individual instructions for the debugger or the 
 * static disassembler
 */

#include "disasm_backend.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAXLEN 100

uint8_t *disassemble(uint8_t *bytes, uint8_t *num_bytes_advanced) {
  uint8_t opcode = bytes[0];
  uint8_t reg_d = bytes[1] >> 4;
  uint8_t reg_s = bytes[1] & 0xF;
  uint8_t *r = malloc(sizeof(uint8_t) * MAXLEN);
  
  switch(opcode) {
  case 0x00: {
    // nop
    snprintf(r, MAXLEN, "nop");
    *num_bytes_advanced = 2;
    break;
  }
  case 0x01: {
    // mov rx, ry
    snprintf(r, MAXLEN, "mov r%d, r%d", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x02: {
    // mov rx, $imm8
    snprintf(r, MAXLEN, "mov r%d, $%02X", reg_d, bytes[2]);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x03: {
    // mov [$imm16], rs
    uint16_t imm_addr = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "mov [$%04X], r%X", imm_addr, reg_d);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x04: {
    // mov rd, [$imm16]
    uint16_t imm_addr = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "mov r%X, [$%04X]", reg_d, imm_addr);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x05: {
    // mov rx:ry, $imm16
    uint16_t imm_val = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "mov r%X:r%X, $%04X",reg_d, reg_s, imm_val);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x06: {
    // mov [rx:ry], $imm8
    uint8_t imm_val = bytes[2];
    snprintf(r, MAXLEN, "mov [r%X:r%X], $%02X", reg_d, reg_s, imm_val);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x07: {
    // mov [rx:ry], rz
    uint8_t r_src = bytes[2];
    snprintf(r, MAXLEN, "mov [r%X:r%X], r%X", reg_d, reg_s, r_src);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x08: {
    // mov rx, [ry:rz]
    uint8_t r_dest = bytes[2];
    snprintf(r, MAXLEN, "mov r%X, [r%X:r%X]", r_dest, reg_d, reg_s);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x09: {
    // hlt
    snprintf(r, MAXLEN, "hlt");
    *num_bytes_advanced = 2;
    break;
  }
  case 0x0A: {
    // add rx, ry
    snprintf(r, MAXLEN, "add r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x0B: {
    // sub rx, ry
    snprintf(r, MAXLEN, "sub r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x0C: {
    // inc rx
    snprintf(r, MAXLEN, "inc r%X", reg_d);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x0D: {
    // dec rx
    snprintf(r, MAXLEN, "dec r%X", reg_d);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x0E: {
    // cmp rx, ry
    snprintf(r, MAXLEN, "cmp r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x0F: {
    // cmp rx, $imm8
    uint8_t imm_val = bytes[2];
    snprintf(r, MAXLEN, "cmp r%X, $%02X", reg_d, imm_val);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x10: {
    // jmp $imm16
    uint16_t addr = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "jmp $%04X", addr);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x11: {
    // jz $imm16
    uint16_t addr = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "jz $%04X", addr);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x12: {
    // jnz $imm16
    uint16_t addr = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "jnz $%04X", addr);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x13: {
    // jmp rx:ry
    snprintf(r, MAXLEN, "jmp r%X:r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x14: {
    // jz rx:ry
    snprintf(r, MAXLEN, "jz r%X:r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x15: {
    // jnz rx:ry
    snprintf(r, MAXLEN, "jnz r%X:r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x16: {
    // call $imm16
    uint16_t addr = bytes[2] << 8 | bytes[3];
    snprintf(r, MAXLEN, "call $%04X", addr);
    *num_bytes_advanced = 4;
    break;
  }
  case 0x17: {
    // call rx:ry
    snprintf(r, MAXLEN, "call r%X:r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x18: {
    // ret
    snprintf(r, MAXLEN, "ret");
    *num_bytes_advanced = 2;
    break;
  }
  case 0x19: {
    // push rx
    snprintf(r, MAXLEN, "push r%X", reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x1A: {
    // pop rx
    snprintf(r, MAXLEN, "pop r%X", reg_d);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x1B: {
    // push $imm8
    uint8_t imm_val = bytes[2];
    snprintf(r, MAXLEN, "push $%02X", imm_val);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x1C: {
    // and rx, ry
    snprintf(r, MAXLEN, "and r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x1D: {
    //or rx, ry
    snprintf(r, MAXLEN, "or r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x1E: {
    // xor rx, ry
    snprintf(r, MAXLEN, "xor r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x1F: {
    // mul rx, ry
    snprintf(r, MAXLEN, "mul r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x20: {
    // sys
    switch(bytes[2]) {
    case 0x00:
    case 0x01:
    case 0x04:
    case 0x05:
      snprintf(r, MAXLEN, "sys $%02X", bytes[2]);
      *num_bytes_advanced = 3;
      break;
    case 0x02:
    case 0x03:
    case 0x06:
    case 0x07:
      snprintf(r, MAXLEN, "sys r%X:r%X, $%02X", reg_d, reg_s, bytes[2]);
      *num_bytes_advanced = 3;
      break;
    }
    break;
  }
  case 0x21: {
    // div rx, ry
    snprintf(r, MAXLEN, "div r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x22: {
    // mul rx, $imm8
    snprintf(r, MAXLEN, "mul r%X, $%02X", reg_d, bytes[2]);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x23: {
    // div rx, $imm8
    snprintf(r, MAXLEN, "div r%X, $%02X", reg_d, bytes[2]);
    *num_bytes_advanced = 3;
    break;
  }
  case 0x24: {
    // mod rx, ry
    snprintf(r, MAXLEN, "mod r%X, r%X", reg_d, reg_s);
    *num_bytes_advanced = 2;
    break;
  }
  case 0x25: {
    // mod rx, $imm8
    snprintf(r, MAXLEN, "mod r%X, $%02X", reg_d, bytes[2]);
    *num_bytes_advanced = 3;
    break;
  }
  default: {
    free(r);
    return NULL;
  }
  }

  return r;
}
