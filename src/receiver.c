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
    size_t read;
    socklen_t len;
    char addr[256];
    const int TRUE = 1;
	char buffer[BUFSIZ];
    struct ip_mreq mreq;
	struct sockaddr_in multicAddr, sendAddr;

    memset(&multicAddr, 0, sizeof(multicAddr));

	if (argc != 3)
    {
        printf("Expected 2 arguments but only received .\n", (argc - 1));
        printf("Usage: ./receiver [multicast group] [multicast port]");
        printf("Ex: ./receiver 224.1.1.1 5000");
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
    if (inet_pton(AF_INET, argv[1], &multicAddr.sin_addr.s_addr) < 0)
    {
        perror("Multicast socket address conversion w/ inet_pton failed");
	    return -1;   
    }

    // Set the multicast socket port information to the port provided
	multicAddr.sin_port = htons(atoi(argv[2]));

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
	if (inet_pton(AF_INET, argv[1], &mreq.imr_multiaddr.s_addr) < 0)
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
    
    // Enter infinite loop and continously receive packets from multicast socket
    while (TRUE)
	{
        // Block until some packet is received from the multicast socket and store packet into the buffer.
		read = recvfrom(recvfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &sendAddr, (socklen_t *) sizeof(sendAddr));
        
        // If some packet was read into buffer, output to stdout and flush
        if (read > 0)
        {
            fputs(buffer, stdout);
            fflush(stdout);
        }

        // Clear buffer of its content
		memset(&buffer, 0, sizeof(buffer));
	}

    close(recvfd);

	return 0;
}