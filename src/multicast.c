#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * A UDP 1-N Multicast application.
 * 
 * @author Vincent.Nigro
 * @version 0.0.1
 */

/**
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char** argv)
{
    if (argc != 5)
    {
        printf("Expected 4 arguments but only received .\n", (argc - 1));
        printf("./multicast [multicastAddress] [multicastPort] [recvHost] [recvPort]\n");
        printf("Ex: ./multicast 224.1.1.1 5000 192.168.0.12 5000");
        return 1;
    }

    const int on = 1;
    int pubfd, recvfd;
    char buffer[BUFSIZ];
    struct sockaddr_in pubAddr, recvAddr;

    const int TRUE = 1;
    const int recvPort = atoi(argv[4]);
    const int groupPort = atoi(argv[2]);
    const char* const recvHost = argv[3];
    const char* const groupHost = argv[1];

    // Clears memory of sockaddr_in structures
    memset(&pubAddr, 0, sizeof(pubAddr));
    memset(&recvAddr, 0, sizeof(recvAddr));

    /// MULTICAST PUBLISH SOCKET SET UP ///

    // On success returns a file descriptor for new allocated socket, failure -1 is returned and errno is set
    // Attempts to allocate an IPv4 socket for UDP datagram packets
	if ((pubfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Publish socket creation failed");
        return -1;
    }

    // Set the publish socket to be reusable by others
	if (setsockopt(pubfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("Publish socket setsockopt SO_REUSEADDR failed");
        return -1;
    }

    // Set the publish socket addr family information to be IPv4
	pubAddr.sin_family = AF_INET;

    // Set the publish socket address information to the group address provided
	if (inet_pton(AF_INET, groupHost, &pubAddr.sin_addr.s_addr) < 0)
    {
        perror("Publish socket address conversion w/ inet_pton failed");
        return -1;
    }
    
    // Set the publish socket port information to the port provided
	pubAddr.sin_port = htons(groupPort);

    /// RECEIVE SOCKET SET UP ///

    // On success returns a file descriptor for new allocated socket, failure -1 is returned and errno is set
    // Attempts to allocate an IPv4 socket for UDP datagram packets
    if ((recvfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Receive socket creation failed");
        return -1;
    }

    // Set the receive socket addr family information to be IPv4
    recvAddr.sin_family = AF_INET;
	
    // Set the publish socket address information to the group address provided
    if (inet_pton(AF_INET, recvHost, &recvAddr.sin_addr.s_addr) < 0)
    {
        perror("Receive socket address conversion w/ inet_pton failed");
	    return -1;   
    }

    // Set the receive socket port information to the port provided
	recvAddr.sin_port = htons(recvPort);

    // Set the receive socket to be reusable by others
	if (setsockopt(recvfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("Receive socket setsockopt SO_REUSEADDR failed");
	    return -1;
    }

    // Attempt to bind to the receive address with the receive socket
	if (bind(recvfd, (struct sockaddr*) &recvAddr, sizeof(recvAddr)) < 0)
    {
        perror("Receive socket bind failed");
	    return -1;
    }

    // Enter infinite loop to continuously receive packets from the recv socket and pass the packets to the multicast publish socket
	while (TRUE)
	{
        // Block until some packet is received from the receive socket and store packet into the buffer
        recvfrom(recvfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &recvAddr, (socklen_t *) sizeof(recvAddr));
		
        // Submit buffer containing captured packet to multicast publish socket
        sendto(pubfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &pubAddr, sizeof(pubAddr));
        
        // Clear buffer of its contents
        memset(&buffer, 0, sizeof(buffer));
	}

    close(pubfd);
    close(recvfd);

	return 0;
}