
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "typedefs.h"
#include <vector>
	using std::vector;
#include <string>
	using std::string;
#include <sigc++/sigc++.h>

class TcpClient {
public:

	SigC::Signal2<void,Uint8*,int> deliver_event;

	static TcpClient *default_type; ///< a tcp client of a specific class has to be initialized and linked here. why not a function pointer? no clue. blame C++. It didnt allow me. I suppose i can try again at some other time
	static TcpClient *create_client();

	virtual TcpClient* create_client_type()=0; ///< gives me a client, i guess i'll be able to figure out a better way.

	virtual void connect_to_host(string p_host,int p_port)=0;

	virtual void send_data(Uint8* p_data,int p_length)=0;
	virtual bool poll_data()=0;
	virtual vector<Uint8>& get_recv_buffer()=0;
	virtual void disconnect()=0;
	virtual bool is_connected()=0;
	virtual int get_local_port() = 0;

	TcpClient() {};
	virtual ~TcpClient() {};
};

#endif
