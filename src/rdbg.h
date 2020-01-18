/* anewkirk */

#pragma once

#include "reflect.h"
#include <stdint.h>

typedef enum _command {
  STEP,
  CONTINUE,
  //Insert breakpoint at pc
  IBREAK,
  //Insert breakpoint at specified address
  IBREAKADDR,
  //List breakpoints
  LBREAK,
  //Remove breakpoint
  RBREAK,
  //Print memory
  PMEM,
  //Print registers
  PREG,
  HELP,
  EXIT,
  UNKNOWN
} Command;

typedef struct _breakpoint {
  struct _breakpoint *next;
  uint16_t addr;
} Breakpoint;

void print_startup();

void print_prompt(RVM *rvm);

Command read_command();

void run_command(RVM *rvm, Command c);

void print_help();

void print_registers(RVM *rvm);

void print_memory();

uint16_t read_address();

void add_breakpoint(uint16_t bp);

bool is_breakpoint(uint16_t bp);
