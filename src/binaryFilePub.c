#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        printf("Expected 3 arguments but only received %d\n.", (argc - 1));
        printf("./binaryPub 127.0.0.1 6000 sampleFile");
        return -1;
    }

    FILE* fp;
    int pubfd;
    size_t len = 0;
    const int on = 1;
    ssize_t read = 0;
    char buffer[BUFSIZ];
    struct sockaddr_in pubAddr;
    const char* const READ_ONLY = "r";

    // On success returns a file descriptor for new allocated socket, failure -1 is returned and errno is set
    // Attempts to allocate an IPv4 socket for UDP datagram packets
	if ((pubfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Publish socket creation failed");
        return -1;
    }

    //
	if (setsockopt(pubfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("setsockopt SO_REUSEADDR failed");
        return -1;
    }

	pubAddr.sin_family = AF_INET;

	if (inet_pton(AF_INET, argv[1], &pubAddr.sin_addr.s_addr) < 0)
    {
        perror("inet_pton");
        return -1;
    }
	pubAddr.sin_port = htons(atoi(argv[2]));

    fp = fopen(argv[3], READ_ONLY);

    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp, 0, SEEK_SET);

    while ((len = fread(&buffer, sizeof(char), BUFSIZ, fp)) > 0)
    {
        sendto(pubfd, &buffer, len, 0, (struct sockaddr*) &pubAddr, sizeof(pubAddr));
        memset(&buffer, 0, sizeof(buffer));
    }

    fclose(fp);
}