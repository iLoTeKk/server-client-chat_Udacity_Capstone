#include"client.h"
//the uncommented parts means that they're commented in the server side .cpp
using namespace std;

#ifdef _WIN32
int client::wsastart() {
	_wVersionRequested = MAKEWORD(2, 2);
	_errwsa = WSAStartup(_wVersionRequested, &_wsa);
	if (_errwsa != 0) {
		std::cout << "dll initialization didn't succeeded \n";
		return _errwsa;
	}
	else {
		std::cout << "dll initialization succeeded with status of " << _wsa.szSystemStatus << std::endl;
		return _errwsa;
	}
}
#endif

SOCKET client::creating_client_socket() {
	_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_client_socket == INVALID_SOCKET) {
#ifdef _WIN32
		WSACleanup();
		cout << "error creating socket: " << WSAGetLastError() << endl;
#else
		cout << "error creating socket " << endl;
#endif
		return _client_socket;
	}
	else {
		cout << "Socket created successfully \n";
		return _client_socket;
	}
}


int client::connect_to_server(char hi[]) {
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(PORT);
#ifdef _WIN32
	_sockaddr.sin_addr.s_addr = inet_addr(hi);
#else
	inet_pton(AF_INET, hi, &_sockaddr.sin_addr); //convert the ipv4 address into binary form and stores it into the sockadd struct address
#endif
	if (connect(_client_socket, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr)) == SOCKET_ERROR)
	{
		cout << "connection failed " << endl;
		closesocket(_client_socket);
#ifdef _WIN32
		WSACleanup();
#endif
		return 1;
	}
	else {
		cout << "[+] connection successful " << endl;
		cout << "Please enter your name(without spaces): ";//since cin only takes input untill spaces 
		std::cin >> _name;
		std::string connected = "[+] " + _name + "  Connected ";
		send(_client_socket, connected.c_str(), connected.size(), 0);//notif others that i'm connected with my name
		return 0;
	}
}


void client::send_to_server() {
	
	//cout << "please enter the message " << endl;
	cin.getline(_send_buff, 4095);//we use getline because it also graps white spaces
	std::string msg = _name + ": " + _send_buff;
	int byte_count = send(_client_socket, msg.c_str(), msg.size(), 0);
	//if (byte_count > 0) {
	//	cout << "message sent" << endl;
	//}
	//else {
	//	cout << "failed to send message " << endl;
	//}
}


void client::receive_from_server() {
	int byte_count = recv(_client_socket, _receive_buff, sizeof(_receive_buff)-1, 0);
	if (byte_count > 0) {
		_receive_buff[byte_count] = '\0';
		cout << _receive_buff << endl;
	}
	//else {
	//	cout << "didn't receive the message " << endl;
	//}
}

 
void client::close_client_socket() {
	closesocket(_client_socket);

}



void client::sending_loop() {
	while (true)
	{
		std::lock_guard<std::mutex> lock(_send_mtx);//so we don't get data race but i think it's not neaccesseary here but i'll keept it
		send_to_server();//just a loop
	}
}


void client::receiving_loop() {
	while (true)
	{
		std::lock_guard<std::mutex> lock(_receive_mtx);//so we don't get a data race since multiple users will sned at the same time
		receive_from_server();
	}
}
