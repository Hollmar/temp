#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "fileproto.pb.h"
#include "common.h"

typedef struct sensor {
	int resolution;
	int numberOfChannels;
	int sampleRate;
	char name[10];
}SENSOR;

SENSOR sensors[] = 
{
	{10, 5, 1000, "ECG"}, 
	{15, 3, 500, "EEG"}, 
	{5, 1, 250, "Pulse"}
};

void handle_connection(int connfd)
{
	RequestSensorInformation request = {};
	pb_istream_t input = pb_istream_from_socket(connfd);

	if (!pb_decode_delimited(&input, RequestSensorInformation_fields, &request))
    {
        printf("Decode failed: %s\n", PB_GET_ERROR(&input));
        return;
    }

	printf("Sensor Information was requested\n");
	printf("Sending Sensor Information..\n");
	int x = 0;
	
	SendSensorInformation response = {};
	pb_ostream_t output = pb_ostream_from_socket(connfd);
	for(x = 0; x < 3; ++x)
	{
		response.resolution = sensors[x].resolution;
		response.numberOfChannels = sensors[x].numberOfChannels;
		response.sampleRate = sensors[x].sampleRate;
		
		
		if (!pb_encode_delimited(&output, SendSensorInformation_fields, &response))
		{
			printf("Encoding failed: %s\n", PB_GET_ERROR(&output));
		}
	}
}

void handle_configuration(int connfd)
{
	SendConfiguration message = {};
	pb_istream_t input = pb_istream_from_socket(connfd);

	if (!pb_decode_delimited(&input, SendConfiguration_fields, &message))
    {
        printf("Decode failed: %s\n", PB_GET_ERROR(&input));
        return;
    }
	int x = message.sensor;
	sensors[x - 1].sampleRate = message.sampleRate;
	sensors[x - 1].resolution = message.resolution;
	sensors[x - 1].numberOfChannels = message.numberOfChannels;
}

void handle_data(int connfd)
{

}

int getMessageType(int connfd)
{
	SendMessageType message = {};
	pb_istream_t input = pb_istream_from_socket(connfd);

	if (!pb_decode_delimited(&input, SendMessageType_fields, &message))
    {
        printf("Decode failed: %s\n", PB_GET_ERROR(&input));
        return -1;
    }
	return message.messageType;
}

int main(int argc, char** argv)
{
	int listenfd, connfd;
	struct sockaddr_in serveraddr;
	int reuse = 1;
	int messageType = 0;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serveraddr.sin_port = htons(1234);

	if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) != 0)
	{
		perror("bind");
		return 1;
	}

	if (listen(listenfd, 5) != 0)
	{
		perror("listen");
		return 1;
	}

	for(;;)
	{
		connfd = accept(listenfd, NULL, NULL);

		if(connfd < 0)
		{
			perror("accept");
			return 1;
		}

		printf("Got connection. \n");

		messageType = getMessageType(connfd);

		switch(messageType)
		{
			case 1: 
				handle_connection(connfd);
				break;
			case 2: 
				handle_configuration(connfd);
				break;
			case 3:
				handle_data(connfd);
				break;
		}
		printf("Close Connection. \n");
		close(connfd);
	}
	return 0;
}
