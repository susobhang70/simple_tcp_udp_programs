#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>

void handle_session(int session_fd){
    while(1){
        char readbuffer[1024] = {0};
        int valread = read(session_fd, readbuffer, 1024);  // read infromation received into the buffer
        if(valread <= 0){
            printf("Client disconnected. Exiting\n");
            exit(EXIT_FAILURE);
        }
        printf("Message received: %s", readbuffer);

        ssize_t count=write(session_fd,readbuffer,valread);
        if (count<0){
            if (errno==EINTR) continue;
            printf("failed to write to socket (errno=%d)", errno);
            exit(EXIT_FAILURE);
        }
    }
}

int main(){
    const char* hostname="0.0.0.0";
    const char* portname="50000";
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_protocol=IPPROTO_TCP;
    // hints.ai_flags=AI_PASSIVE;
    struct addrinfo* res=0;
    int err=getaddrinfo(hostname,portname,&hints,&res);
    if(err!=0){
        printf("failed to resolve local socket address (err=%d)",err);
        exit(EXIT_FAILURE);
    }

    int server_fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(server_fd==-1){
        printf("%s",strerror(errno));
        exit(EXIT_FAILURE);
    }

    int reuseaddr=1;
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr))==-1){
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(bind(server_fd,res->ai_addr,res->ai_addrlen)==-1){
        printf("%s",strerror(errno));
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
    if(listen(server_fd,SOMAXCONN)){
        printf("failed to listen for connections (errno=%d)",errno);
        exit(EXIT_FAILURE);
    }

    for(;;){
        int session_fd=accept(server_fd,0,0);
        if(session_fd==-1){
            if(errno=EINTR) continue;
            printf("failed to accept connection (errno=%d)",errno);
            exit(EXIT_FAILURE);
        }
        pid_t pid=fork();
        if(pid==-1){
            printf("failed to create child process (errno=%d)",errno);
            exit(EXIT_FAILURE);
        }
        else if(pid==0){
            close(server_fd);
            handle_session(session_fd);
            close(session_fd);
            _exit(0);
        }
        else{
            close(session_fd);
        }
    }

}