#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "simple.pb.h"
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char** str_split(char a_str, const char a_delim);
void *handler();
void send_data();
void get_sensor_data();
void handle_connection(int connfd);



typedef struct heart_rate_sensor {
	int value;
	int sample_max;
};

typedef struct accelerometer_sensor {
	int x;
	int y;
	int z;
};

typedef struct pulse_sensor {
	int value;
	int sample_max;
};

int main(int argc, char **argv) 
{
	uint8_t buffer[128];
	size_t message_length;
	bool status;
	
	struct heart_rate_sensor * hr_s; 
	struct accelerometer_sensor  * a_s;
	struct pulse_sensor * p_s; 

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3; 
		
	pthread_create(&thread1, NULL, handler, NULL);
       	pthread_create(&thread2, NULL, handler, NULL);
	pthread_create(&thread3, NULL, handler, NULL);
	
	//pthread_join(thread1, NULL);
	
	// create a socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	//specify address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	listen(server_socket, 5);
	
	int client_socket;
	
	for(;;)
	{
		client_socket = accept(server_socket, NULL, NULL);

		if(client_socket < 0) 
		{
			perror("accept");
			return 1;
		}

		handle_connection(client_socket);
		close(client_socket);
	}
	
	
	return 0;
}

void handle_connection(int connfd)
{
	HeartRateMessage message = {};
	pb_istream_t input = pb_istream_from_socket(connfd);
	
	if(!pb_decode_delimited(&input, HeartRateMessage_fields, &message))
	{
		return;
	}
	
	printf("Value: %d\n", message.value);
}

void *handler()
{
			
}

char** str_split(char a_str, const char a_delim)
{
	char ** result = 0;
	size_t count   = 0;
	char* tmp      = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;

	while( *tmp)
	{
		if (a_delim == *tmp)
		{
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	count += last_comma < (a_str + strlen(a_str) - 1);

	count++;

	result = malloc(sizeof(char*) * count);

	if(result)
	{
		size_t idx = 0;
		char* token = strtok(a_str, delim);

		while(token)
		{
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}
	return result;
}


int send_Data()
{
	
}

void get_sensor_data()
{
	return;
}
