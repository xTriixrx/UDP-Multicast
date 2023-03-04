#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv)
{
    int recvfd;
    socklen_t len;
    char addr[256];
    const int TRUE = 1;
    struct ip_mreq mreq;
    FILE* outFile = NULL;
    unsigned char buffer[BUFSIZ];
	struct sockaddr_in multicAddr, sendAddr;
    const char* const WRITE_BINARY_ONLY = "wb+";

    const int recvPort = atoi(argv[2]);
    const char* const recvHost = argv[1];
    const char* const outputFile = argv[3];

    memset(buffer, 0, sizeof(buffer));
    memset(&multicAddr, 0, sizeof(multicAddr));

	if (argc != 4)
    {
        printf("Expected 3 arguments but only received .\n", (argc - 1));
        printf("Usage: ./receiver [multicast group] [multicast port] [output_path]");
        printf("Ex: ./receiver 224.1.1.1 5000 output_file");
	    return -1;
    }

    // On success returns a file descriptor for new allocated socket, failure -1 is returned and errno is set
    // Attempts to allocate an IPv4 socket for UDP datagram packets 
	if ((recvfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Multicast socket creation failed");
	    return -1;
    }
	
    // Set the multicast socket addr family information to be IPv4
	multicAddr.sin_family = AF_INET;
	
    // Set the multicast socket address information to the group address provided
    if (inet_pton(AF_INET, recvHost, &multicAddr.sin_addr.s_addr) < 0)
    {
        perror("Multicast socket address conversion w/ inet_pton failed");
	    return -1;   
    }

    // Set the multicast socket port information to the port provided
	multicAddr.sin_port = htons(recvPort);

    // Set the multicast socket to be reusable by others
	if (setsockopt(recvfd, SOL_SOCKET, SO_REUSEADDR, &TRUE, sizeof(TRUE)) < 0)
    {
        perror("Multicast setsockopt SO_REUSEADDR failed");
	    return -1;
    }
	
    // Attempt to bind to the multicast address with the receive socket
	if (bind(recvfd, (struct sockaddr*) &multicAddr, sizeof(multicAddr)) < 0)
    {
        perror("Multicast socket bind failed");
	    return -1;
    }
	
    // Set the multicast mreq address information to the group address provided
	if (inet_pton(AF_INET, recvHost, &mreq.imr_multiaddr.s_addr) < 0)
    {
        perror("Multicast mreq address inet_pton failed.");
	    return -1;
    }

    // Set the local interface for the mreq structure
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    // Add the multicast mreq structure to the added membership of the group address provided
	if (setsockopt(recvfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("Multicast setsockopt ADD_MEMBERSHIP failed");
	    return -1;
    }

    // Open output file for writing binary data
    outFile = fopen(outputFile, WRITE_BINARY_ONLY);
    
    size_t read = 0;
    socklen_t slen = sizeof(struct sockaddr_in);

    // Enter infinite loop and continously receive packets from multicast socket
    while (TRUE)
	{
        // Block until some packet is received from the multicast socket and store packet into the buffer.
		read = recvfrom(recvfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &sendAddr, &slen);
        
        // If some packet was read into buffer, output to stdout and flush
        fwrite(buffer, sizeof(unsigned char), read, outFile);
        fflush(outFile);

        // Clear buffer of its content
		memset(buffer, 0, sizeof(buffer));
	}

    close(recvfd);
    fclose(outFile);

	return 0;
}