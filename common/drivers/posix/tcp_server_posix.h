#ifdef POSIX_ENABLED

#ifndef TCP_SERVER_POSIX_H
#define TCP_SERVER_POSIX_H

#include "os/tcp_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "hash_stuff.h"
#include <arpa/inet.h>
#include <vector>
	using std::vector;

#include "hash_stuff.h"

class TcpServerPosix : public TcpServer {

	enum {

		BUFF_SIZE=500,
		MSG_SIZE_LIMIT=20*1024*1024 //20mb
	};

	int connection_id_count;
	int listen_fd;
	struct sockaddr_in my_addr;    // my address information
	int listen_port;
	int max_connections;
	Uint8 buffer[BUFF_SIZE];

	struct Remote_Socket {

		struct sockaddr_in their_addr; // connector's address information
		int sock_fd;
		int conn_id;
		vector<Uint8> msg_buffer;
		int msg_len;

		Remote_Socket() {

			sock_fd=-1;
			msg_len=-1;
		}

		~Remote_Socket() {
			if (sock_fd!=-1) close(sock_fd);
		}
	};

	typedef hash_map<int,Remote_Socket> Remote_Socket_List;
	Remote_Socket_List socket_list;

	bool start_listening();
	void poll_accept();
	bool poll_remote_socket(Remote_Socket *p_socket);

public:

	bool start_server(int p_port,int p_max_connections=10);
	void stop_server();
	void poll_connections();

	//sending to p_connection -1 will send to all connections
	void send_data(int p_connection_id,Uint8 *p_data, int p_data_len);

	TcpServerPosix();
	~TcpServerPosix();
};

#endif

#endif
