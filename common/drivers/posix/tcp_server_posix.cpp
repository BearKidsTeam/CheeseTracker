#ifdef POSIX_ENABLED

#include "tcp_server_posix.h"

bool TcpServerPosix::start_listening() {

	int yes=1;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {

		perror("socket");
		return false;
        }

	if (setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		return false;
        }

    my_addr.sin_family = AF_INET;         		// host byte order
	my_addr.sin_port = htons(listen_port);     	// short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; 		// automatically fill with my IP

	memset(&(my_addr.sin_zero), '\0', 8); 		// zero the rest of the struct

	if (bind(listen_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1) {

    		perror("bind");
		return false;
	}

	if (listen(listen_fd, max_connections) == -1) {
		perror("listen");
		return false;
	}
        fcntl(listen_fd, F_SETFL, O_NONBLOCK);

	return true;
}

void TcpServerPosix::poll_accept() {

 	unsigned sin_size = sizeof(struct sockaddr_in);
 	Remote_Socket new_connection;
 	bool success=true;

	if ((new_connection.sock_fd = accept(listen_fd, (struct sockaddr *)&new_connection.their_addr,&sin_size)) == -1) {

		success=false;
	}
	while (success) {

		connection_id_count++;
		new_connection.conn_id=connection_id_count;
		socket_list[connection_id_count]=new_connection;

		fcntl(new_connection.sock_fd, F_SETFL, O_NONBLOCK);

		printf("Server: Got connection from (%s)\n",inet_ntoa(new_connection.their_addr.sin_addr));

		if ((new_connection.sock_fd = accept(listen_fd, (struct sockaddr *)&new_connection.their_addr,&sin_size)) == -1) {

			success=false;
		}
	}
}

bool TcpServerPosix::poll_remote_socket(Remote_Socket *p_socket) { //return false if disconnect

	int gotamount;

	bool got_some=false;

	if (p_socket->sock_fd==-1) {

		ERROR("Trying to poll when not connected!\n");
	}

	gotamount=recv(p_socket->sock_fd,buffer,BUFF_SIZE-1,0);

	while (gotamount>0) {

		bool had_less_than_8=(p_socket->msg_buffer.size()<8);
		bool data_to_extract=true;
		int zero=p_socket->msg_buffer.size();

		p_socket->msg_buffer.resize(zero+gotamount);

		for (int i=0;i<gotamount;i++)
			p_socket->msg_buffer[zero+i]=buffer[i];

		//printf("got %i bytes\n",gotamount);

		do {

			//There is a message to extract!
			if ((p_socket->msg_len>=(int)0) && (p_socket->msg_buffer.size()>=(p_socket->msg_len+8))) {
				//finished mess

				//SEND MSG!
				//printf("DELIVER MSG\n");
				deliver_event.emit(&p_socket->msg_buffer[0],p_socket->msg_len,p_socket->conn_id);

				p_socket->msg_buffer.erase(p_socket->msg_buffer.begin(),p_socket->msg_buffer.begin()+(p_socket->msg_len+8)); //erase from mem
				p_socket->msg_len=-1; // no message!
			}



			//if we DO have a message begin, configure msg len
			if ((p_socket->msg_len<0) && (p_socket->msg_buffer.size()>=8)) {

				p_socket->msg_len=p_socket->msg_buffer[7];
				p_socket->msg_len<<=8;
				p_socket->msg_len|=p_socket->msg_buffer[6];
				p_socket->msg_len<<=8;
				p_socket->msg_len|=p_socket->msg_buffer[5];
				p_socket->msg_len<<=8;
				p_socket->msg_len|=p_socket->msg_buffer[4];

				//printf("-- %c%c%c%c\n",p_socket->msg_buffer[0],p_socket->msg_buffer[1],p_socket->msg_buffer[2],p_socket->msg_buffer[3]);
				//printf("-- %i %i %i %i\n",p_socket->msg_buffer[0],p_socket->msg_buffer[1],p_socket->msg_buffer[2],p_socket->msg_buffer[3]);
				//printf("buflen %i\n",p_socket->msg_buffer.size());

				if ((p_socket->msg_len<0) || (p_socket->msg_len>=MSG_SIZE_LIMIT)) {
					//invalid message, DIE!
					close(p_socket->sock_fd);
					p_socket->sock_fd=-1;
					break;
				}

				//printf("msg len is %i %i,%i,%i,%i\n",p_socket->msg_len,p_socket->msg_buffer[7],p_socket->msg_buffer[6],p_socket->msg_buffer[5],p_socket->msg_buffer[4]);
			}

			//repeat if we still have messages in the buffer
		} while ((p_socket->msg_len>=(int)0) && (p_socket->msg_buffer.size()>=(p_socket->msg_len+8)));

	 	//printf("get data from %s\n",inet_ntoa(p_socket->their_addr.sin_addr));

		got_some=true;

		if (p_socket->sock_fd!=-1) {

			gotamount=recv(p_socket->sock_fd,buffer,BUFF_SIZE-1,0);

			if (gotamount ==-1 && errno != EAGAIN) {
				break;
				// error?
			};
		} else {

			break;
		}

	}

	if (gotamount==0) {

		close(p_socket->sock_fd);
		p_socket->sock_fd=-1;
	}

	return got_some;
}

void TcpServerPosix::poll_connections() {

	Remote_Socket_List::iterator I,J;

	poll_accept();

	for (I=socket_list.begin();I!=socket_list.end();) {

		J=I;
		J++;
		if (I->second.sock_fd==-1) {

			socket_list.erase(I);
			I=J;
		} else {

			poll_remote_socket(&I->second);
			++I;
		}

	}
}


bool TcpServerPosix::start_server(int p_port,int p_max_connections) {

	if (listen_fd!=-1) {

		ERROR("trying to start TCP server when already active\n");
		return false;
	}

	listen_port=p_port;
	max_connections=p_max_connections;
	return start_listening();
}

void TcpServerPosix::send_data(int p_connection_id,Uint8 *p_data, int p_data_len) {

	Remote_Socket_List::iterator I;

	if (p_connection_id==-1) { //send to all sockets

		for (I=socket_list.begin();I!=socket_list.end();++I) {
			send_data(I->first,p_data,p_data_len);
		}
	} else { //send normally

		if ( (I=socket_list.find(p_connection_id)) == socket_list.end() ) {
			ERROR("Invalid Connection ID\n");
			return;
		}

		int data_to_send=p_data_len;
		Uint8 * offset=p_data;

		if (I->second.sock_fd==-1) return;
		int sockfd=I->second.sock_fd;

		while (data_to_send) {


			int sent_amount=send(sockfd,offset,data_to_send, MSG_NOSIGNAL);

			while ((sent_amount==-1) && (errno==EAGAIN)) {

				sent_amount=send(sockfd,offset,data_to_send, MSG_NOSIGNAL);

			}

			if (sent_amount==-1) {

				perror("send:");
				close(sockfd);
				I->second.sock_fd=-1;

				ERROR("<TcpServerPosix::send_data> Client disconnected!\n");
				return;
			}
			data_to_send-=sent_amount;
			offset+=sent_amount;
		}
	}
}


void TcpServerPosix::stop_server() {

	socket_list.clear();
	listen_fd=-1;
}


TcpServerPosix::TcpServerPosix(){

	connection_id_count=0;
	listen_fd=-1;
}
TcpServerPosix::~TcpServerPosix(){
}

#endif
