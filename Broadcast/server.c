#include <stdio.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for sleep() */

#define PORT 50000

int main(int argc, char *argv[])
{
    int server_fd;                          /* Socket File Descriptor */
    int opt = 1;                            /* Socket opt to set permission to broadcast */
    struct sockaddr_in broadcastAddress;    /* Broadcast address sockaddr_in - references elements 
                                            of the socket address. "in" for internet */
    char *broadcastIP = "127.255.255.255";  /* Broadcast IP address */
    char *message = "Hello\n";              /* Hello message to broadcast */
    int messageLen = strlen(message);       /* Length of message to broadcast */

    /* Create socket for sending datagrams */
    if ((server_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) /* creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP */
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /* Set socket to allow broadcast */
    if (setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, /* SOL_SOCKET is the socket layer itself */
                        (void *) &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    memset(&broadcastAddress, 0, sizeof(broadcastAddress));
    broadcastAddress.sin_family = AF_INET;                      /* Internet address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.  */
    broadcastAddress.sin_addr.s_addr = inet_addr(broadcastIP);  /* Broadcast IP address */
    broadcastAddress.sin_port = htons(PORT);                    /* Broadcast port. Htons converts to Big Endian - Left to Right. RTL is Little Endian */

    /* Send periodically, endlessly */
    while(1)
    {
        /* Broadcast hello message every 5 seconds*/
        if (sendto(server_fd, message, messageLen, 0, (struct sockaddr *) 
           &broadcastAddress, sizeof(broadcastAddress)) != messageLen)
        {
            perror("Sendto Failure");
            exit(EXIT_FAILURE);
        }

        sleep(5);
    }
}
