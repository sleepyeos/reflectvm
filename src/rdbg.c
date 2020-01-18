/*
 * anewkirk 
 *
 * A CLI debugger for ReflectVM application images
 *
 */

#include "rdbg.h"
#include "bool.h"
#include "reflect.h"
#include "disasm_backend.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define VERSION 0.2

Breakpoint *head = NULL;

int main(int argc, char *argv[]) {
  RVM *rvm = new_rvm();
  load_code(rvm, argv[1]);
  print_startup();

  for(;;) {

    print_prompt(rvm);
    Command c = read_command();
    run_command(rvm, c);

  }
}

void print_startup() {
  printf("RDBG v%.1f\n", VERSION);
}

void print_prompt(RVM *rvm) {
  printf("[rdbg@0x%04x] > ", rvm->pc);
}

Command read_command() {
  char *line = NULL;
  size_t n = 0;
  getline(&line, &n, stdin);
  int o = 1;

  if(strlen(line) == 5) {
    o = strcmp(line, "help\n");
    if(!o) {
      free(line);
      return HELP;
    }

    o = strcmp(line, "exit\n");
    if(!o) {
      free(line);
      return EXIT;
    }
  }
  if(strlen(line) == 3) {
    o = strcmp(line, "pr\n");
    if(!o) {
      free(line);
      return PREG;
    }
    
    o = strcmp(line, "pm\n");
    if (!o) {
      free(line);
      return PMEM;
    }

    o = strcmp(line, "br\n");
    if(!o) {
      free(line);
      return IBREAK;
    }

    o = strcmp(line, "ba\n");
    if(!o) {
      free(line);
      return IBREAKADDR;
    }

    o = strcmp(line, "lb\n");
    if(!o) {
      free(line);
      return LBREAK;
    }

    o = strcmp(line, "rb\n");
    if(!o) {
      free(line);
      return RBREAK;
    }
  }
  if(strlen(line) == 2) {
    o = strcmp(line, "s\n");
    if(!o) {
      free(line);
      return STEP;
    }

    o = strcmp(line, "c\n");
    if(!o) {
      free(line);
      return CONTINUE;
    }
  }
  return UNKNOWN;
}

void run_command(RVM *rvm, Command c) {
  switch(c) {
  case STEP:{

    uint8_t b[4];
    b[0] = rvm->mem[rvm->pc];
    b[1] = rvm->mem[rvm->pc + 1];
    b[2] = rvm->pc <= 0xFFFD ? rvm->mem[rvm->pc + 2] : 0x00;
    b[3] = rvm->pc <= 0xFFFC ? rvm->mem[rvm->pc + 3] : 0x00;
    uint8_t bytes_advanced = 0;
    uint8_t *disassembly = disassemble(b, &bytes_advanced);
    printf("%s\n", disassembly);
    free(disassembly);
    
    fetch(rvm);
    decode(rvm);
    execute(rvm);
    printf("\n");
    break;
  }
  case CONTINUE:
    if(is_breakpoint(rvm->pc)) {
      run_command(rvm, STEP);
    }
    while(!is_breakpoint(rvm->pc) && rvm->opcode != 0x09) {
      fetch(rvm);
      decode(rvm);
      execute(rvm);
    }
    printf("\n");
    break;
  case IBREAK:
    add_breakpoint(rvm->pc);
    break;
  case IBREAKADDR: {
    uint16_t addr = read_address();
    add_breakpoint(addr);
    break;
  }
  case LBREAK: {
    Breakpoint *current = head;
    while(current) {
      printf("[+] 0x%04x\n", current->addr);
      current = current->next;
    }
    break;
  }
  case RBREAK: {
    uint16_t addr = read_address();
    Breakpoint *current = head;
    Breakpoint *previous = NULL;
    while(current) {
      if(current->addr == addr) {
	if(previous) {
	  previous->next = current->next;
	}
	if(head == current) {
	  head = current->next;
	}
      }
      previous = current;
      current = current->next;
    }
    break;
  }
  case PMEM: {
    uint16_t addr = read_address();
    printf("0x%02X\n", addr, rvm->mem[addr]);
    break;
  }
  case PREG:
    print_registers(rvm);
    break;
  case HELP:
    print_help();
    break;
  case EXIT:
    exit(0);
    break;
  case UNKNOWN:
    printf("[-] Unrecognized command. Type 'help' for help.\n");
  }

  if(rvm->opcode == 0x09) {
    printf("[!] VM halted\n");
    exit(0);
  }
}

void print_help() {
  printf("Commands:\n");
  printf("[+] s: step\n");
  printf("[+] c: continue\n");
  printf("[+] br: insert breakpoint at pc\n");
  printf("[+] ba: insert breakpoint at address\n");
  printf("[+] lb: list breakpoints\n");
  printf("[+] rb: remove breakpoint at address\n");
  printf("[+] pm: print value at memory address\n");
  printf("[+] pr: print register values\n");
  printf("[+] help: display this help menu\n");
  printf("[+] exit: halt VM and exit debugger\n");
}

void print_registers(RVM *rvm) {
  for(uint8_t i = 0; i < 16; i++) {
    printf("r%x: 0x%02x\n", i, rvm->reg[i]);
  }
}

uint16_t read_address() {
  printf("Enter address:\n");
  
  char *line = NULL;
  size_t n = 0;
  getline(&line, &n, stdin);

  uint16_t addr;

  sscanf(line, "0x%x\n", &addr);
  return addr;
}

void add_breakpoint(uint16_t bp) {
  Breakpoint *n = malloc(sizeof(Breakpoint));
  n->addr = bp;
  n->next = NULL;
  Breakpoint *current = head;
  if(current) {
    while(current->next) {
      current = current->next;
    }
    current->next = n;
  } else {
    head = n;
  }

}

bool is_breakpoint(uint16_t bp) {
  Breakpoint *current = head;
  while(current) {
    if(current->addr == bp) {
      return true;
    }
    current = current->next;
  }
  return false;
}
