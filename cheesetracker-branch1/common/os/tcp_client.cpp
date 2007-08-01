
#include "tcp_client.h"

//void * Mutex_Lock::mutex_creation_function=NULL;
TcpClient* TcpClient::default_type=NULL;

TcpClient* TcpClient::create_client() {

	if (default_type==NULL) return NULL;

	return default_type->create_client_type();
}

