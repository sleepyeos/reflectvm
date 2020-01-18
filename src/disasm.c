/*
 * anewkirk 
 *
 * A disassembler for ReflectVM application images
 */

#include "bool.h"
#include "disasm.h"
#include "disasm_backend.h"
#include "queue.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint16_t pgm_len;
uint8_t *pgm;

/* Bytes marked as 0xFF in shadow have been
   disassembled */
uint8_t *shadow;

/* disassembly contains the disassembled program*/
uint8_t **disassembly;

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Usage: rdsm program.rvm output.rsm\n");
    exit(1);
  }

  init(argv[1]);
  Queue *analysis_queue = new_queue();
  enqueue(analysis_queue, 0x0000);
  disassemble_pgm(analysis_queue);


  // output disassembly and free memory
  remove(argv[2]);
  FILE *f = fopen(argv[2], "w");
  if(!f) {
    printf("Failed to open file: %s\n", argv[2]);
    exit(1);
  }
  
  for(uint16_t i = 0; i < pgm_len; i++) {
    if(disassembly[i] != NULL) {
      fprintf(f, ";; 0x%04X:\n", i);
      fprintf(f, "%s\n\n", disassembly[i]);
    } else if(shadow[i] != 0xFF) {
      fprintf(f, ";; 0x%04X:\n", i);
      fprintf(f, "db %02X\n\n", pgm[i]);
    }
    free(disassembly[i]);
  }

  // clean up
  fclose(f);
  free(shadow);
  free(pgm);
  free(disassembly);
  destroy_queue(analysis_queue);
}

void disassemble_pgm(Queue *q) {
  uint16_t pc;
  while(q->size) {
    pc = dequeue(q);
    bool running = true;
    while(running) {
      uint8_t adv = 0;
      uint8_t bytes[4];
      bytes[0] = pgm[pc];
      bytes[1] = pgm[pc + 1];
      bytes[2] = pc < pgm_len - 2 ? pgm[pc + 2] : 0x00;
      bytes[3] = pc < pgm_len - 3 ? pgm[pc + 3] : 0x00;

      if(!disassembly[pc]) {
	disassembly[pc] = disassemble(bytes, &adv);
      } else {
	break;
      }
      
      uint16_t dest = bytes[2] << 8 | bytes[3];
      switch(bytes[0]) {
      case 0x10:
	if(!disassembly[dest]) {
	  enqueue(q, dest);
	}
	running = false;
	break;
      case 0x09:
      case 0x18:
	running = false;
	break;
      case 0x11:
      case 0x12:
      case 0x16:
	if(!disassembly[dest]) {
	  enqueue(q, dest);
	}
	break;
      }

      for(uint8_t i = 0; i < adv; i++) {
	if(pc + i < pgm_len) {
	  shadow[pc + i] = 0xFF;
	}
      }
      
      pc += adv;
    }
  }
}



void init(uint8_t *filename) {
  FILE *fp;
  uint32_t len;
  fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  if(len > 0xFFFF) {
    printf("File size too large.\n");
    exit(1);
  }

  pgm_len = len;
  pgm = malloc(len);
  shadow = calloc(1, len);
  disassembly = calloc(len, sizeof(uint8_t *));  
  rewind(fp);
  fread(pgm, len, 1, fp);
  fclose(fp);
}
