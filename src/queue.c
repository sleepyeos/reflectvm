/* anewkirk 
 *
 * A simple implementation of a FIFO queue
 */

#include <stdint.h>
#include <stdlib.h>
#include "queue.h"

void enqueue(Queue *q, uint16_t value) {
  Node *new = malloc(sizeof(Node));
  new->location = value;
  new->next_in_queue = NULL;
  if(q->back) {
    q->back->next_in_queue = new;
  }
  
  if(!q->front) {
    q->front = new;
  }
  
  q->back = new;
  q->size++;
}

uint16_t dequeue(Queue *q) {
  Node *first = q->front;
  q->front = first->next_in_queue;
  uint16_t val = first->location;
  if(q->back == first) {
    q->back = NULL;
  }
  free(first);
  q->size--;
  return val;
}

Queue *new_queue() {
  Queue *q = malloc(sizeof(Queue));
  q->front = NULL;
  q->back = NULL;
  q->size = 0;
}

void destroy_queue(Queue *q) {
  while(q->size) {
    dequeue(q);
  }
  free(q);
}
