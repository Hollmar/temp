#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "fileproto.pb.h"
#include "common.h"

bool requestSensorInformation(int fd)
{
	RequestSensorInformation request = {};
	pb_ostream_t output = pb_ostream_from_socket(fd);

	if (!pb_encode_delimited(&output, RequestSensorInformation_fields, &request))
    {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
            return false;
    }

	SendSensorInformation response = {};
	pb_istream_t input = pb_istream_from_socket(fd);

	if (!pb_decode_delimited(&input, SendSensorInformation_fields, &response))
    {
        fprintf(stderr, "Decode failed: %s\n", PB_GET_ERROR(&input));
        return false;
    }

	printf("Sensor received. Following Specs:\n Resolution: %d\n Number of Channels: %d\n Sample Rate: %d\n", response.resolution, response.numberOfChannels, response.sampleRate);
	return true;
}

bool sendConfiguration(int fd)
{
	SendConfiguration request = {};
	pb_ostream_t output = pb_ostream_from_socket(fd);

	int sensor;
	int sampleRate;
	int resolution;
	int numberOfChannels;

	printf("Which Sensor do you wish to configure\n");
	printf("1 = ECG \n");
	printf("2 = EEG \n");
	printf("3 = Pulse \n");

	scanf("%d", &sensor);

	if(sensor == 1)
		printf("ECG selected. Choose Configurations:\n");
	else if(sensor == 2)
		printf("EEG selected. Choose Configurations:\n");
	else if(sensor == 3)
		printf("Pulse Sensor selected. Choose Configurations:\n");
	
	printf("Select new Sample Rate:\n");
	scanf("%d", &sampleRate);
	printf("Select new resolution: \n");
	scanf("%d", &resolution);
	printf("Select new number of Channels: \n");
	scanf("%d", &numberOfChannels);

	request.sensor = sensor;
	request.sampleRate = sampleRate;
	request.resolution = resolution;
	request.numberOfChannels = numberOfChannels;

	if (!pb_encode_delimited(&output, SendConfiguration_fields, &request))
    {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
            return false;
    }

	return true;
}

void requestData(int fd)
{

}

void sendMessageType(int fd, int type)
{
	SendMessageType message = {};
	pb_ostream_t output = pb_ostream_from_socket(fd);

	message.messageType = type;

	if (!pb_encode_delimited(&output, SendMessageType_fields, &message))
    {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
            return;
    }
}

void printHelp()
{
	printf("Usage: ./client <> \n");
	printf("1 = RequestSensorInformation\n");
	printf("2 = SendConfiguration\n");	
	printf("3 = RequestData\n");		
}

int main (int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in serveraddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serveraddr.sin_port = htons(1234);

	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) != 0)
	{
		perror("connect");
		return 1;
	}
	
	int a = atoi(argv[1]);
	sendMessageType(sockfd, a);
	int b = 0; 

	switch(a)
	{
		case 0: 
			printHelp();
			break;
		case 1: 
			while (b != 3) 
			{ 
				if(!requestSensorInformation(sockfd))
					return 2;
				b++;
			} 
			break;
		case 2: 
			sendConfiguration(sockfd);
			break;
		case 3:
			requestData(sockfd);
			break;
	}
	close(sockfd);
	return 0;
}
