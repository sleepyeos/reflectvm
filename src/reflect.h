/* anewkirk */

#pragma once
#include <stdint.h>
#include "bool.h"

// Represents an instance of ReflectVM
typedef struct _rvm {
  // Registers
  uint8_t reg[0x10];

  // Memory
  uint8_t mem[0x10000];

  // Stack pointer
  uint16_t sp;

  // Program counter
  uint16_t pc;

  // First decoded register from current instruction
  uint8_t reg_d;

  // Second decoded register from current instruction
  uint8_t reg_s;

  // Opcode decoded from current instruction
  uint8_t opcode;

  // Fetched instruction
  uint16_t fetched;
  
  // Running flag
  bool r_flag;
  
  // Zero flag
  bool z_flag;
} RVM;

/*
 * Allocates, initializes, and returns a pointer to
 * a new ReflectVM instance.
 */
RVM *new_rvm();

/*
 * Loads the specified file into the memory of the VM
 */
void load_code(RVM *rvm, uint8_t *filename);

/*
 * Retrieves a 16-bit immediate value from memory at
 * pc, and advances pc by 2 bytes
 */
uint16_t get_imm16(RVM *rvm);

/*
 * Retrieves an 8-bit immediate value from memory at
 * pc, and advances pc by 1 byte
 */
uint8_t get_imm8(RVM *rvm);

/*
 * Reads a 16-bit value from a pair of registers
 * indicated by rvm->reg_d and rvm->reg_s
 * as if they were one unit (rd:rs).
 */
uint16_t read_16b_reg(RVM *rvm);

/*
 * Retrieves 16 bits from memory at pc
 */
void fetch(RVM *rvm);

/*
 * Loads the opcode and register values specified in 
 * an instruction returned by fetch() into the global
 * variables opcode, reg_d, and reg_s to be used in 
 * execute().
 */
void decode(RVM *rvm);

/*
 * Executes the instruction held in opcode, reg_d,
 * and reg_s.
 */
void execute(RVM *rvm);

/*
 * Sets r_flag to true, and begins execution of the 
 * program. Execution will be halted when the VM 
 * encounters a hlt instruction (0x09 0x00).
 */
void run(RVM *rvm);
