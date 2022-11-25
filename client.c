#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "pb_encode.h"
#include "hr.pb.h"
#include "common.h"

#include <netinet/in.h>

bool send_data(int fd, int value);

int main(int argc, char **argv)
{

	uint8_t buffer[128];
	size_t message_length;
	bool status;

	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);;

	int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
	if (connection_status == -1)
	{
		printf("There was an error making a connection to the server socket\n\n");
	}
	
	if(argc > 1)
	{
		send_data(network_socket, argv[1]);
	}

	close(network_socket);

	return 0;
		
	
}

bool send_data(int fd, int value)
{
	HeartRateMessage message = {};
	
	pb_ostream_t output = pb_ostream_from_socket(fd);

	message.value = value;

	if(!pb_encode_delimited(&output, HeartRateMessage_fields, &message))
	{
		return false;
	}
}
