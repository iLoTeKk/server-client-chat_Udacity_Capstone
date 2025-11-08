#include"server.h"
#include<string.h>
void recur();

int main() {
	
	server test;
#ifdef _WIN32
	test.wsastart();
#endif
	test.creating_server_socket();
	test.bind_socket();
	test.listening_for_connections(30);//max clients at the same time is 30
	std::thread t1 = std::thread(&server::connection_loop, &test);//make a thread for accepting clients
	std::thread t2 = std::thread(&server::comminication_loop, &test);//make a thread for communication between clients and server
	t1.detach();//detach 
	t2.detach();//detach
	recur();//this is a recursion interface so if the user want to close the server in a profficinal way

	return 0;
}


void recur() {
	std::string any;
	std::cout << "Press type 'exit' to exit" << std::endl;
	std::cin >> any;
	if (any == "exit")
	{
		return;
	}
	else {
		recur();
	}
}



















/*
#include"server.h"
#include<string.h>
void recur();

int main() {

	std::shared_ptr<server> test = std::make_shared<server>();
#ifdef _WIN32
	test->wsastart();
#endif
	test->creating_server_socket();
	test->bind_socket();
	test->listening_for_connections(30);
	std::thread t1 = std::thread(&server::connection_loop, test);
	std::thread t2 = std::thread(&server::comminication_loop, test);
	t1.detach();
	t2.detach();
	recur();

	return 0;
}


void recur() {
	std::string any;
	std::cout << "Press type 'exit' to exit" << std::endl;
	std::cin >> any;
	if (any == "exit")
	{
		return;
	}
	else {
		recur();
	}
}*/