#ifdef POSIX_ENABLED

#ifndef TCP_CLIENT_POSIX_H
#define TCP_CLIENT_POSIX_H


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

#include "os/tcp_client.h"

/**
  *@author
  */


#define MAXDATASIZE 300

class TcpClientPosix : public TcpClient  {

	enum {
		MSG_SIZE_LIMIT = 2 * 1024 * 1024,
	};

	int sockfd, numbytes;
	Uint8 bufaux[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information

	vector<Uint8> buffer;

	int msg_len;

	void process_msgs(int p_got_some);

public:

	void connect_to_host(string p_host,int p_port);
	void send_data(Uint8* p_data,int p_length);
	bool poll_data();
	bool is_connected();
	vector<Uint8>& get_recv_buffer();
	int get_local_port();

	void disconnect();

	TcpClient* create_client_type() {

		TcpClient* client_new = new TcpClientPosix;
		return client_new;
	}

	TcpClientPosix();
	~TcpClientPosix();
};

#endif
#endif
