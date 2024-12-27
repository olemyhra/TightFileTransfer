#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stoml.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_TOML_ENTRIES 5
#define MAX_TOML_VALUE_LENGTH 50
#define NULL_TERM_CHAR_SIZE 1
#define RECIEVE_BUFFER_SIZE 1024
#define MAX_CLIENT_QUEUE 10
#define MAX_RECIEVE_FILENAME_LENGTH 50

void handler(int sig, siginfo_t *si, void *unused);
void *recieve_data(void *connected_socket);
int server_socket = 0;
char *recieve_buffer = NULL;

int main(void)
{
	stoml_data *configuration[MAX_TOML_ENTRIES];
	FILE *toml_file = NULL;
	stoml_data *key = NULL;
	int server_port = 0;
	int client_socket = 0;
	struct sockaddr client_address = {0};
	int client_address_length = 0;
	struct sockaddr_in server_address = {0};
	struct sigaction sa;
	pthread_t recieve_thread;

	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = handler;	
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		fprintf(stderr, "Unable to set signal action!\n");
		exit(EXIT_FAILURE);
	}

	
	memset(configuration, 0, sizeof(stoml_data *) * MAX_TOML_ENTRIES);
	recieve_buffer = malloc(sizeof(char) * RECIEVE_BUFFER_SIZE);
	memset(recieve_buffer, 0, sizeof(char) * RECIEVE_BUFFER_SIZE);
	
	if (recieve_buffer == NULL) {
		fprintf(stderr, "Unable to allocate memory to recieve buffer!\n");
		exit(EXIT_FAILURE);
	}

	toml_file = fopen("../tft_server.toml", "r");
	if(toml_file == NULL) {
		fprintf(stderr, "Unable to open configuration file!\n");
		exit(EXIT_FAILURE);
	}

	if (stoml_read(configuration, MAX_TOML_ENTRIES, toml_file) != 0) {
		fprintf(stderr, "Error occured reading from configuration file!\n");
		exit(EXIT_FAILURE);
	}

	key = stoml_search(configuration, MAX_TOML_ENTRIES, "port_number");
	if (key == NULL) {
		fprintf(stderr, "Unable to retrieve value for port number from the toml file!\n");
		exit(EXIT_FAILURE);
	}

	server_port = atoi(get_value(key));
	fclose(toml_file);

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		fprintf(stderr, "Unable to create server socket!\n");
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(server_port);

	if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "Unable to bind address to socket!\n");
		exit(EXIT_FAILURE);
	}

	listen(server_socket, MAX_CLIENT_QUEUE);

	printf("Server listening at port %d for new client connections...\n", server_port);

	while(1) {
		client_address_length = sizeof(struct sockaddr_in);
		client_socket = accept(server_socket, (struct sockaddr *) &client_address, 
					(socklen_t *) &client_address_length);

		if (client_socket == 0) {
			fprintf(stderr, "Failed to accept client connection!\n");
			exit(EXIT_FAILURE);
		}
	
		pthread_create(&recieve_thread, NULL, recieve_data, (void *) &client_socket);
	//	recieve_data((void *) &client_socket);
	}

	return EXIT_SUCCESS;
}



void handler(int sig, siginfo_t *si, void *unused) {
	printf("\nStopping server...\n");
	
	close(server_socket);
	free(recieve_buffer);

	exit(EXIT_SUCCESS);
}

void *recieve_data(void *connected_socket) {
	bool filename_recieved = false;
	char recieved_filename[MAX_RECIEVE_FILENAME_LENGTH];
	FILE *recieved_file = NULL;
	int client_recieved_data_length = -1;
	int client_socket = *((int *) connected_socket);

	memset(recieved_filename, 0, sizeof(char) * MAX_RECIEVE_FILENAME_LENGTH);
		
	while ((client_recieved_data_length= recv(client_socket, 
		recieve_buffer, RECIEVE_BUFFER_SIZE, 0)) != 0) {

		if (client_recieved_data_length < 0) {
			fprintf(stderr, "Recieving data from client failed!\n");
			exit(EXIT_FAILURE);
		}
	
			
		if (!filename_recieved) {
			strncpy(recieved_filename, 
				recieve_buffer, 
				client_recieved_data_length > MAX_RECIEVE_FILENAME_LENGTH
				? MAX_RECIEVE_FILENAME_LENGTH
				: client_recieved_data_length);

			filename_recieved = true;
			
			recieved_file = fopen("test", "w");
			if (recieved_filename == NULL) {
				fprintf(stderr, "Unable to create file %s!\n", 
					recieved_filename);
				exit(EXIT_FAILURE);
			}
				
			if (client_recieved_data_length > MAX_RECIEVE_FILENAME_LENGTH) {
				if ((fwrite(&(recieve_buffer[MAX_RECIEVE_FILENAME_LENGTH]),
					sizeof(char),client_recieved_data_length
					 - MAX_RECIEVE_FILENAME_LENGTH,
					recieved_file)) == 0) {
					fprintf(stderr, "Unable to create and  write to file %s!\n",
					recieved_filename);
					exit(EXIT_FAILURE);
				}
			}
			continue;
		}
			
		if (fwrite(recieve_buffer, sizeof(char), client_recieved_data_length, 
			recieved_file) == 0) {
			fprintf(stderr, "Unable to write to file %s!\n", 
				recieved_filename);
			exit(EXIT_FAILURE);
		}	
	}

	close(client_socket);
	memset(recieved_filename, 0, sizeof(char) * MAX_RECIEVE_FILENAME_LENGTH);
	filename_recieved = false;
	fclose(recieved_file);

	return NULL;
	

}				

