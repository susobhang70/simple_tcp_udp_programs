#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define PORT1 50001
#define PORT2 50000

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, sock, valread;
    struct sockaddr_in address, serv_addr;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    int pid = fork();

    if(pid == 0)
    {
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
        {
            perror("socket failed");
            kill (getppid(), 9);
            exit(EXIT_FAILURE);
        }

        // This is to lose the pesky "Address already in use" error message
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                      &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
        {
            perror("setsockopt");
            kill (getppid(), 9);
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
        address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
        address.sin_port = htons( PORT1 );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

        // Forcefully attaching socket to the port 50000
        if (bind(server_fd, (struct sockaddr *)&address,
                                     sizeof(address))<0)
        {
            perror("bind failed");
            kill (getppid(), 9);
            exit(EXIT_FAILURE);
        }

        // Port bind is done. You want to wait for incoming connections and handle them in some way.
        // The process is two step: first you listen(), then you accept()
        if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
        {
            perror("listen");
            kill (getppid(), 9);
            exit(EXIT_FAILURE);
        }

        // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            kill (getppid(), 9);
            exit(EXIT_FAILURE);
        }
        while(1)
        {
            char buffer[1024] = {0};
            valread = read( new_socket , buffer, 1024);  // read infromation received into the buffer
            if(valread <= 0)
            {
                printf("Client disconnected. Exiting\n");
                kill (getppid(), 9);
                exit(EXIT_FAILURE);
            }
            printf("Message received: %s", buffer);
        }
    }
    else
    {
        sleep(3);
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket failed");
            kill(pid, SIGKILL);
            exit(EXIT_FAILURE);
        }

        memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                    // which is meant to be, and rest is defined below

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT2);

        // Converts an IP address in numbers-and-dots notation into either a 
        // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        {
            perror("Invalid address");
            kill(pid, SIGKILL);
            exit(EXIT_FAILURE);
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
        {
            perror("Connect Failed");
            kill(pid, SIGKILL);
            exit(EXIT_FAILURE);
        }

        while(1)
        {
            printf(">> ");
            char message[1024];
            fgets (message, 1024, stdin);
            send(sock , message , strlen(message) , 0 );  // send the message.
        }
    }
    return 0;
}
