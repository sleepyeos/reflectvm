/* 
 * anewkirk 
 *
 * A tiny, portable 8-bit bytecode VM
 *
 */

#include "bool.h"
#include "reflect.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

RVM *new_rvm() {
  RVM *rvm = malloc(sizeof(RVM));
  rvm->sp = 0;
  rvm->pc = 0;
  rvm->reg_d = 0;
  rvm->reg_s = 0;
  rvm->opcode = 0;
  rvm->fetched = 0;
  rvm->r_flag = 0;
  rvm->z_flag = 0;
  return rvm;
}

void load_code(RVM *rvm, uint8_t *filename) {
  FILE *fp;
  uint32_t len;
  fp = fopen(filename, "rb");
  // Seek to end of file to determine file length
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  if(len > 0xFFFF) {
    printf("Program size too large; Exiting");
    exit(1);
  }
  rewind(fp);

  // Read program into VM memory
  fread(rvm->mem, len, 1, fp);
  fclose(fp);
}

uint16_t get_imm16(RVM *rvm) {
  return rvm->mem[rvm->pc++] << 8 | rvm->mem[rvm->pc++];
}

uint8_t get_imm8(RVM *rvm) {
  return rvm->mem[rvm->pc++];
}

uint16_t read_16b_reg(RVM *rvm) {
  // Concatenate bytes into a 16-bit value
  uint8_t reg_a = rvm->reg_d;
  uint8_t reg_b = rvm->reg_s;
  return rvm->reg[reg_a] << 8 | rvm->reg[reg_b];
}

void fetch(RVM *rvm) {
  uint8_t b1 = rvm->mem[rvm->pc++];
  uint8_t b2 = rvm->mem[rvm->pc++];
  rvm->fetched = (b1 << 8) | b2;
}

void decode(RVM *rvm) {
  rvm->opcode = rvm->fetched >> 8;
  rvm->reg_s = rvm->fetched & 0xF;
  rvm->reg_d = (rvm->fetched & 0xFF) >> 4;
}

void execute(RVM *rvm) {
  switch(rvm->opcode) {
  case 0x00: {
    // nop
    break;
  }
  case 0x01: {
    // mov rd, rs
    rvm->reg[rvm->reg_d] = rvm->reg[rvm->reg_s];
    break;
  }
  case 0x02: {
    // mov rd, $imm8
    uint8_t imm = get_imm8(rvm);
    rvm->reg[rvm->reg_d] = imm;
    break;
  }
  case 0x03: {
    // mov [imm16], rs
    uint16_t imm_addr = get_imm16(rvm);
    rvm->mem[imm_addr] = rvm->reg[rvm->reg_s];
    break;
  }
  case 0x04: {
    // mov rd, [$imm16]
    uint16_t imm_addr = get_imm16(rvm);
    rvm->reg[rvm->reg_d] = rvm->mem[imm_addr];
    break;
  }
  case 0x05: {
    // mov rx:ry, $imm16
    uint16_t imm_val = get_imm16(rvm);
    rvm->reg[rvm->reg_s] = imm_val & 0xFF;
    rvm->reg[rvm->reg_d] = imm_val >> 8;
    break;
  }
  case 0x06: {
    // mov [rx:ry], $imm8
    uint8_t imm_val = get_imm8(rvm);
    uint16_t addr = read_16b_reg(rvm);
    rvm->mem[addr] = imm_val;
    break;
  }
  case 0x07: {
    // mov [rx:ry], rc
    uint8_t r_src = get_imm8(rvm);
    uint16_t addr = read_16b_reg(rvm);
    rvm->mem[addr] = rvm->reg[r_src];
    break;
  }
  case 0x08: {
    // mov rc, [rx:ry]
    uint8_t r_dest = get_imm8(rvm);
    uint16_t addr = read_16b_reg(rvm);
    rvm->reg[r_dest] = rvm->mem[addr];
    break;
  }
  case 0x09: {
    // hlt
    rvm->r_flag = false;
    break;
  }
  case 0x0A: {
    // add rd, rs
    rvm->reg[rvm->reg_d] += rvm->reg[rvm->reg_s];
    rvm->z_flag = rvm->reg[rvm->reg_d] == 0 ? 1 : 0;
    break;
  }
  case 0x0B: {
    // sub rd, rs
    rvm->reg[rvm->reg_d] -= rvm->reg[rvm->reg_s];
    rvm->z_flag = rvm->reg[rvm->reg_d] == 0 ? 1 : 0;
    break;
  }
  case 0x0C: {
    // inc rd
    rvm->reg[rvm->reg_d]++;
    rvm->z_flag = rvm->reg[rvm->reg_d] == 0 ? 1 : 0;
    break;
  }
  case 0x0D: {
    // dec rd
    rvm->reg[rvm->reg_d]--;
    rvm->z_flag = rvm->reg[rvm->reg_d] == 0 ? 1 : 0;
    break;
  }
  case 0x0E: {
    // cmp rd, rs
    rvm->z_flag = rvm->reg[rvm->reg_d] == rvm->reg[rvm->reg_s] ? 1 : 0;
    break;
  }
  case 0x0F: {
    // cmp rd, $imm8
    uint8_t imm_val = get_imm8(rvm);
    rvm->z_flag = rvm->reg[rvm->reg_d] == imm_val ? 1 : 0;
    break;
  }
  case 0x10: {
    // jmp $imm16
    uint16_t addr = get_imm16(rvm);
    rvm->pc = addr;
    break;
  }
  case 0x11: {
    // jz $imm16
    uint16_t addr = get_imm16(rvm);
    if(rvm->z_flag) {
      rvm->pc = addr;
    }
    break;
  }
  case 0x12: {
    // jnz $imm16
    uint16_t addr = get_imm16(rvm);
    if(!rvm->z_flag) {
      rvm->pc = addr;
    }
    break;
  }
  case 0x13: {
    // jmp [rx:ry]
    uint16_t addr = read_16b_reg(rvm);
    rvm->pc = addr;
    break;
  }
  case 0x14: {
    // jz [rx:ry]
    uint16_t addr = read_16b_reg(rvm);
    if(rvm->z_flag) {
      rvm->pc = addr;
    }
    break;
  }
  case 0x15: {
    // jnz [rx:ry]
    uint16_t addr = read_16b_reg(rvm);
    if(!rvm->z_flag) {
      rvm->pc = addr;
    }
    break;
  }
  case 0x16: {
    // call $imm16
    uint16_t addr = get_imm16(rvm);
    rvm->mem[rvm->sp--] = rvm->pc >> 8;
    rvm->mem[rvm->sp--] = rvm->pc & 0xFF;
    rvm->pc = addr;
    break;
  }
  case 0x17: {
    // call [rx:ry]
    rvm->mem[rvm->sp--] = rvm->pc >> 8;
    rvm->mem[rvm->sp--] = rvm->pc & 0xFF;
    rvm->pc = read_16b_reg(rvm);
    break;
  }
  case 0x18: {
    // ret
    uint16_t ret_addr = rvm->mem[++rvm->sp] | rvm->mem[++rvm->sp] << 8;
    rvm->pc = ret_addr;
    break;
  }
  case 0x19: {
    // push rs
    rvm->mem[rvm->sp--] = rvm->reg[rvm->reg_s];
    break;
  }
  case 0x1A: {
    // pop rd
    rvm->reg[rvm->reg_d] = rvm->mem[++rvm->sp];
    break;
  }
  case 0x1B: {
    // push $imm8
    uint8_t imm_val = get_imm8(rvm);
    rvm->mem[rvm->sp--] = imm_val;
    break;
  }
  case 0x1C: {
    // and rx, ry
    rvm->reg[rvm->reg_d] &= rvm->reg[rvm->reg_s];
    break;
  }
  case 0x1D: {
    // or rx, ry
    rvm->reg[rvm->reg_d] |= rvm->reg[rvm->reg_s];
    break;
  }
  case 0x1E: {
    // xor rx, ry
    rvm->reg[rvm->reg_d] ^= rvm->reg[rvm->reg_s];
    break;
  }
  case 0x1F: {
    // mul rx, ry
    rvm->reg[rvm->reg_d] *= rvm->reg[rvm->reg_s];
    break;
  }
  case 0x20: {
    // sys
    uint8_t syscall = get_imm8(rvm);
    switch(syscall) {
    case 0x00: {
      uint8_t c = rvm->mem[++rvm->sp];
      printf("%c", c);
      break;
    }
    case 0x01: {
      uint8_t c = fgetc(stdin);
      rvm->mem[rvm->sp--] = c;
      break;
    }
    case 0x02: {
      uint8_t c = rvm->mem[read_16b_reg(rvm)];
      printf("%c", c);
      break;
    }
    case 0x03: {
      uint8_t c = fgetc(stdin);
      rvm->mem[read_16b_reg(rvm)] = c;
      break;
    }
    case 0x04: {
      uint8_t i = rvm->mem[++rvm->sp];
      printf("%d", i);
      break;
    }
    case 0x05: {
      uint8_t i = 0;
      scanf("%d", &i);
      rvm->mem[rvm->sp--] = i;
      break;
    }
    case 0x06: {
      uint8_t i = rvm->mem[read_16b_reg(rvm)];
      printf("%d", i);
      break;
    }
    case 0x07: {
      uint8_t i = 0;
      scanf("%d", &i);
      rvm->mem[read_16b_reg(rvm)] = i;
      break;
    }
    }
    break;
  }
  case 0x21: {
    // div rx, ry
    rvm->reg[rvm->reg_d] /= rvm->reg[rvm->reg_s];
    break;
  }
  case 0x22: {
    // mul rx, $imm8
    uint8_t imm = get_imm8(rvm);
    rvm->reg[rvm->reg_d] *= imm;
    break;
  }
  case 0x23: {
    // div rx, $imm8
    uint8_t imm = get_imm8(rvm);
    rvm->reg[rvm->reg_d] /= imm;
    break;
  }
  case 0x24: {
    // mod rx, ry
    if(rvm->reg[rvm->reg_d] % rvm->reg[rvm->reg_s] == 0) {
      rvm->z_flag = true;
    } else {
      rvm->z_flag = false;
    }
    break;
  }
  case 0x25: {
    // mod rx, $imm8
    uint8_t imm = get_imm8(rvm);
    rvm->z_flag = rvm->reg[rvm->reg_d] % imm == 0 ? true : false;
    break;
  }
    
  default: {
    printf("Illegal orvm->pcode: 0x%x\n", rvm->opcode);
    break;
  }
  }
}

void run(RVM *rvm) {
  rvm->r_flag = true;
  while(rvm->r_flag) {
    fetch(rvm);
    decode(rvm);
    execute(rvm);
  }
}
