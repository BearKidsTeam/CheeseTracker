#ifdef POSIX_ENABLED

#include "tcp_client_posix.h"

#define INFO(a,b) WARN(a b)

void TcpClientPosix::connect_to_host(string p_host,int p_port) {

	INFO("TCP Client Posix","Connecting to host %s:%d\n" << p_host.c_str() <<  p_port);
	if (sockfd != -1) {
		ERROR("Trying to connect when already connected!\n");
		return;
	}
	INFO("TCP Client Posix","getting hostname...(%s)\n" << p_host.c_str());
	if ((he=gethostbyname(p_host.c_str())) == NULL) {  // get the host info
		ERROR("gethostbyname failed!");
		//perror("gethostbyname");
		return;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		ERROR("Socket creation failed!");
		//perror("socket");
		return;
	}
	their_addr.sin_family = AF_INET;    // host byte order
	their_addr.sin_port = htons(p_port);  // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);
	if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
		ERROR("Connection to remote host failed!");
		//perror("connect");
		sockfd=-1;
		return;
	}
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	INFO("TCP Client Posix","Connected!\n");
}

void TcpClientPosix::send_data(Uint8* p_data,int p_length) {
	int data_to_send = p_length;
	Uint8 *offset = p_data;
	if (sockfd == -1) return;
	errno = 0;
	while (data_to_send) {
		int sent_amount = send(sockfd, offset, data_to_send, MSG_NOSIGNAL);
		//printf("Sent TCP data of %d bytes, errno %d\n", sent_amount, errno);
		while ((sent_amount == -1) && (errno == EAGAIN)) {
			errno = 0;
			//printf("Retrying to send TCP data of %d bytes\n", data_to_send, offset);
			sent_amount = send(sockfd, offset, data_to_send, MSG_NOSIGNAL);
			usleep(1); //sucks
		}
		if (sent_amount == -1) {
			//perror("shit?");
			close(sockfd);
			sockfd = -1;
			ERROR("Server disconnected!\n");
			return;
		}
		data_to_send -= sent_amount;
		offset += sent_amount;
	}
}

int TcpClientPosix::get_local_port(){
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&addr_len);
	return ntohs(addr.sin_port);
}

void TcpClientPosix::disconnect() {
	close(sockfd);
	sockfd=-1;
}

bool TcpClientPosix::poll_data() {
	int gotamount;
	bool got_some=false;
	if (sockfd==-1) {
		ERROR("Trying to poll when not connected!\n");
		return false;
	}
	errno = 0;
	gotamount=recv(sockfd,bufaux,MAXDATASIZE-1,0);
	while (gotamount>0) {
		int zero=buffer.size();
		buffer.resize(zero+gotamount);
		for (int i=0;i<gotamount;i++) buffer[zero+i]=bufaux[i];
		got_some=true;
		gotamount=recv(sockfd,bufaux,MAXDATASIZE-1,0);
		//if (gotamount > 0) printf("Received TCP data. %d Bytes on socket %d\n", gotamount, socket);
	}
	if (gotamount==0) {
		disconnect();
	}
	if ((errno) && (errno != EAGAIN)){
		//perror("Error receiving TCP data");
		ERROR("Error receiving TCP data");
	}

	process_msgs(got_some);

	return got_some;
}

void TcpClientPosix::process_msgs(int p_got_some) {

	if (p_got_some) {
		do {
			//There is a message to extract!
			if ((msg_len>=0) && ((int)buffer.size()>=(msg_len+8))) {
				//finished mess
				deliver_event.emit(&buffer[8], msg_len);
				buffer.erase(buffer.begin(),buffer.begin()+(msg_len+8)); //erase from mem
				msg_len=-1; // no message!
			}

			//if we DO have a message begin, configure msg len
			if ((msg_len<0) && (buffer.size()>=8)) {
				msg_len=buffer[7];
				msg_len<<=8;
				msg_len|=buffer[6];
				msg_len<<=8;
				msg_len|=buffer[5];
				msg_len<<=8;
				msg_len|=buffer[4];
				if ((msg_len<0) || (msg_len>=MSG_SIZE_LIMIT)) {
					//invalid message, DIE!
					disconnect();
					ERROR("Server sent a ____HUGE____ Packet! > " << MSG_SIZE_LIMIT << " bytes");
					break;
				}
			}
			//repeat if we still have messages in the buffer
		} while ((msg_len>=0) && ((int)buffer.size()>=(msg_len+8)));
	}
	//PRINT("*************procesing %d messages\n" << p_msgs.size());
};

bool TcpClientPosix::is_connected() {
	return (sockfd!=-1);
}

vector<Uint8>& TcpClientPosix::get_recv_buffer() {
	return buffer;
}

TcpClientPosix::TcpClientPosix(){
	sockfd=-1;
	msg_len=-1;
}
TcpClientPosix::~TcpClientPosix(){
}

#endif
