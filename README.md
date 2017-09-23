# Simple TCP & UDP Networks Assignment

This assignment covers basic understanding of the following topics
1. Client-Server and P2P models.
2. Unicast and Broadcast traffic.
3. TCP and UDP transport protocols.

## Make and Run

- There are three directories: 
	* `Broadcast`
	* `TCP_P2P`
	* `Concurrent_TCP_Server`

- `cd <directory>`

- `make`

- To run:
	* `./server` and `socat -u udp-recv:50000,reuseport -`: For UDP Broadcast Program
	* `./server1` and `./server2`: For TCP P2P Message Exchange Program
	* `./server`, and `nc 127.0.0.1 50000`: For Concurrent TCP Server

### UDP Broadcast Program
It **broadcasts** the message `hello` and sends it through an ephemeral port. To receive this message, run the command `socat -u udp-recv:50000,reuseport -` from an other terminal/multiple terminals. `netcat` has a different behaviour. <https://stackoverflow.com/questions/7696862/strange-behavoiur-of-netcat-with-udp>  
### TCP-P2P
This takes user input for sending text messages to the other peer and also prints text messages received from the other peer. Both programs are able to send and receive messages simultaneously. Run both the executables generated within 3-5 seconds of each other, else there will be a timeout. Refer <http://cs.baylor.edu/~donahoo/practical/CSockets/>  
### Concurrent TCP  
This TCP server can handle multiple requests coming from single/multiple clients simultaneously. While serving a request, the server doesn't block the requests coming from other clients. The server functionality is a simple echo - it echoes back the received message. Run the executable, and from an other terminal, use the command `nc 127.0.0.1 50000`, and send messages to the server. Refer <http://www.microhowto.info/howto/listen_for_and_accept_tcp_connections_in_c.html>  
