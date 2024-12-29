#include "queue.h"
#include <stdio.h>

struct queue *create_queue() {
	struct queue *new_queue = (struct queue *) malloc(sizeof(struct queue));

	if (new_queue == NULL)
		return NULL;

	new_queue->head = NULL;
	new_queue->tail = NULL;

	return new_queue;
}


void enqueue(struct queue *q, int client) {
	struct client *new_client = (struct client *) malloc(sizeof(struct client));
	new_client->socket = client;
	new_client->next = NULL;

	if (q->tail == NULL) {
		q->head = new_client;
		q->tail = new_client;
		return;
	}
	q->tail->next = new_client;
	q->tail = new_client;

}


int dequeue(struct queue *q) {
		
	struct client *tmp_client = NULL;
	int client_socket = 0;
	
	if (queue_empty(q))
		return 0;
	

	tmp_client = q->head;
	q->head = q->head->next;

	if (q->head == NULL)
		q->tail = NULL;
	
	client_socket = tmp_client->socket;

	free(tmp_client);
	
	return client_socket;
}


bool queue_empty(struct queue *q) {
	if (q->head == NULL && q->tail == NULL) {
		return true;
	} else {
		return false;
	}
}
