/* anewkirk */

#pragma once

#include <stdint.h>

typedef struct _node {
  struct _node *next_in_queue;
  uint16_t location;
} Node;

typedef struct {
  Node *front;
  Node *back;
  uint32_t size;
} Queue;

void enqueue(Queue *q, uint16_t value);

uint16_t dequeue(Queue *q);

Queue *new_queue();

void destroy_queue(Queue *q);
