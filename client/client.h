#pragma once
#pragma once
#ifdef _WIN32
#pragma comment (lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<winsock2.h>
#include<WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)
#define closesocket close
#endif
#include<iostream>
#include<vector>
#include<thread>
#include<stdio.h>
#include<mutex>
#include<chrono>

const short int PORT = 5000;

class client {
public:
#ifdef _WIN32
	int wsastart();
#endif
	SOCKET creating_client_socket();
	int connect_to_server(char hi[]);
	void send_to_server();
	void receive_from_server();
	void close_client_socket();
	void sending_loop();
	void receiving_loop();
	SOCKET get() { return _client_socket; }
private:
#ifdef _WIN32
	WSADATA _wsa;
	WORD _wVersionRequested;
	int _errwsa;
#endif
	SOCKET _client_socket = INVALID_SOCKET;
	sockaddr_in _sockaddr;
	char _receive_buff[4500];
	char _send_buff[4096];
	std::string _name;
	std::mutex _receive_mtx;
	std::mutex _send_mtx;

};


