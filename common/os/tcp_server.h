#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "typedefs.h"
#include <sigc++/sigc++.h>
/**
  *@author
  */

class TcpServer {
public:

	SigC::Signal3<void,Uint8*,int,int> deliver_event;

	virtual bool start_server(int p_port,int p_max_connections=10)=0;
	virtual void stop_server()=0;
	virtual void poll_connections()=0;
	virtual void send_data(int p_connection_id,Uint8 *p_data, int p_data_len)=0;

	virtual ~TcpServer(){};
};

#endif
