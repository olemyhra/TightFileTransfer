#include <stdio.h>
#include <stdlib.h>
#include <stoml.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>


#define MAX_TOML_ENTRIES 5
#define MAX_TOML_VALUE_LENGTH 50
#define NULL_TERM_CHAR_SIZE 1
#define SEND_BUFFER_SIZE 1024
#define MAX_FILENAME_LENGTH 50

int main(int argc, char **argv)
{
	if (argc > 1) {
		stoml_data *configuration[MAX_TOML_ENTRIES];
		FILE *toml_file = NULL;
		stoml_data *key = NULL;
		int return_value = 0;
		char server_ip_address[MAX_TOML_VALUE_LENGTH];
		int server_port_nr = 0;
		struct sockaddr_in server_address = {0};
		FILE *file_to_send = NULL;
		char *send_buffer = NULL;
		int number_of_bytes_read = 0;
		struct timeval socket_options;
		char send_filename[MAX_FILENAME_LENGTH];
		
	
		memset(configuration, 0, sizeof(stoml_data *) * MAX_TOML_ENTRIES);
		memset(server_ip_address, 0, sizeof(char) * MAX_TOML_VALUE_LENGTH);
		memset(send_filename, 0, sizeof(char) * MAX_FILENAME_LENGTH);
		send_buffer = malloc(sizeof(char) * SEND_BUFFER_SIZE);
		memset(send_buffer, 0, sizeof(char) * SEND_BUFFER_SIZE);
		
		if (send_buffer == NULL) {
			fprintf(stderr, "Unable to allocate memory to send buffer!\n");
			exit(EXIT_FAILURE);
		}

		toml_file = fopen("../tft_client.toml", "r");
		if (toml_file == NULL) {
			fprintf(stderr, "Unable to open configuration file!\n");
			exit(EXIT_FAILURE);
		}

		return_value = stoml_read(configuration, MAX_TOML_ENTRIES, toml_file);
		if (return_value != 0) {
			fprintf(stderr, "Error occuring reading from configuration file!\n");
			exit(EXIT_FAILURE);
		}

		key = stoml_search(configuration, MAX_TOML_ENTRIES, "port_number");
		if (key == NULL) {
			fprintf(stderr,
				"Unable to retrieve value for port number from toml file!\n");
			exit(EXIT_FAILURE);
		}
		server_port_nr = atoi(get_value(key));

		key = NULL;
		key = stoml_search(configuration, MAX_TOML_ENTRIES, "server_address");
		if (key == NULL) {
			fprintf(stderr, 
				"Unable to retrieve value for server ip-address from toml file!\n");
			exit(EXIT_FAILURE);
		}	
		strncpy(server_ip_address, get_value(key), 
			MAX_TOML_VALUE_LENGTH - NULL_TERM_CHAR_SIZE);

		fclose(toml_file);

		int client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (client_socket == -1) {
			fprintf(stderr, "Unable to create socket!\n");
			exit(EXIT_FAILURE);
		}

		server_address.sin_addr.s_addr = inet_addr(server_ip_address);
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(server_port_nr);

		printf("Connecting to server.\n");

		if (connect(client_socket, (struct sockaddr *) &server_address, 
			sizeof(server_address)) < 0) {
			fprintf(stderr, "Unable to connect to server!\n");
			exit(EXIT_FAILURE);
		}

		socket_options.tv_sec =  20;
		socket_options.tv_usec = 0;
		
		if (setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (char *) &socket_options,
			sizeof(socket_options)) < 0) {
			fprintf(stderr, "Unable to set socket options!\n");
			exit(EXIT_FAILURE);
		}		
				
		
		file_to_send = fopen(argv[1], "r");
		if (file_to_send == NULL) {
			fprintf(stderr, "Unable to open input file!\n");
			exit(EXIT_FAILURE);	
		}		
	

		strncpy(send_filename, argv[1], 
			strlen(argv[1]) < MAX_FILENAME_LENGTH
				 ? strlen(argv[1]) : MAX_FILENAME_LENGTH);
	
		if (write(client_socket, send_filename, MAX_FILENAME_LENGTH) <  0) {
			fprintf(stderr, "Unable to send filename to client!\n");
			exit(EXIT_FAILURE);
		}
		

		while ((number_of_bytes_read = fread(send_buffer,sizeof(char),
			  SEND_BUFFER_SIZE, file_to_send)) > 0) {
			if (write(client_socket, send_buffer, number_of_bytes_read) < 0) {
				fprintf(stderr, "Unable to send data to server!\n");
				exit(EXIT_FAILURE);
			}
			number_of_bytes_read = 0;
		}
		
		close(client_socket);
		fclose(file_to_send);

		for (int i=0;i<MAX_TOML_ENTRIES;i++) {
			if (configuration[i] != NULL)
				free(configuration[i]);
		}
			
		free(send_buffer);

	} else {
		printf("usage: ft_client filename\n");
	}

	return EXIT_SUCCESS;
}
