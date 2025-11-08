#pragma once
#ifdef _WIN32 //if it's windows include those headers and configuration
			  //because socket differes between unix OS's and windows OS
#pragma comment (lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<winsock2.h>
#include<WS2tcpip.h>
#else // if unix include those
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int //change the first to 2nd if using unix os's
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)
#define closesocket close
#endif//default includes for both
#include<iostream>
#include<vector>
#include<thread>
#include<stdio.h>
#include<mutex>
#include <ctime>
#include<chrono>
#include<iomanip>
#include<sstream>
#include<memory>
#include <fstream>  
#include<fcntl.h>


class socketc;//define it so we can use it in class server
const short int PORT = 5000;//defualt port can be changed

class server {
public:
#ifdef _WIN32
	int wsastart();//windows socket nead a startup to initilaize it's own API
#endif
	SOCKET creating_server_socket();
	int bind_socket();
	int listening_for_connections(int n);
	socketc connecting_client();
	std::string receive_from_client(socketc& _server_client_socket);
	void send_to_client(SOCKET s, std::string mess_recv);
	int close_server_socket();
	void comminication_loop();
	void clients_vector(socketc &&s);
	void connection_loop();
	void add_to_log_file(std::string log_mess);


private:
#ifdef _WIN32
	WSADATA _wsa;
	WORD _wVersionRequested;
	int _errwsa;
#endif
	SOCKET _serversocket = INVALID_SOCKET;
	sockaddr_in _sockaddr;
	std::vector<std::string> _message_logs;
	std::mutex _mtx_con;
	std::mutex _mtx_add;
	std::vector<std::unique_ptr<socketc>> _client_vector;
	std::thread _thread;
	
};

class socketc {//this class is for initiliazing clients socket so it calls the destructer whenever 
			  // the socket goes out of scope so we don't mangae it manually and so we can use 
			  //smart_pointers for an execlusive ownership of each client socket in the server
public:
	socketc() {};//defualt construct
	socketc(SOCKET s) : soc(s) {};//constructer overload
	~socketc() {//destructor so we don't close the clients socket manually thus it'll be so hard
				// and may cause a lot of issues
		if (soc != INVALID_SOCKET)
		{
			closesocket(soc);
		}
	}

	socketc(const socketc& source) = delete;//don't allow copying since we need execlusive ownership
		socketc& operator=(const socketc& source) = delete;//don't allow assigning operator copying
												
		socketc(socketc&& source) {//creating moving semtics so we can move our unique pointer if needed
			soc = source.soc;
			source.soc = INVALID_SOCKET;
		}
		socketc& operator=(socketc&& source) {//assingning operator moving
			if (this == &source)
			{
				return *this;
			}
			soc = source.soc;
			source.soc = INVALID_SOCKET;
			return *this;
		}
		SOCKET get() { return soc; }//so we can use the socket inside the class
		void set(SOCKET s) {//this is for updating the socket
			soc = s;
		}


private:
	SOCKET soc = INVALID_SOCKET;//default socket is empty 

};


/*
template <class define_type>//didn't used it i was planning to make a message queue but didn't find
							// a way to fit it in the code so it can be commented
class messagequue {



};
*/