#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdbool.h>

struct client {
	int socket;
	struct client *next;
};

struct queue {
	struct client *head;
	struct client *tail;
};

struct queue *create_queue();
void enqueue(struct queue * q, int client);
int dequeue(struct queue *q);
bool queue_empty(struct queue *q);



#endif
