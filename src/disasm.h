/* anewkirk */

#pragma once

#include "queue.h"
#include <stdint.h>

void init(uint8_t *filename);

void disassemble_pgm(Queue *q);
