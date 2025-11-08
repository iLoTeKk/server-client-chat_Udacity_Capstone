#include"client.h"

int main(int argc, char *argv[]) {

	char _default[100] = "192.168.122.16";
	client test;
#ifdef _WIN32
	std::cout << "Please enter the Server IP: ";
	char server_ip[30];
	std::cin >> server_ip;
	test.wsastart();
	test.creating_client_socket();
	test.connect_to_server(server_ip);
#else
	if (argc < 2)
	{
		std::cout << "Usage ./client [Server_IP]" << std::endl;
		return 1;
	}
	char* server_ip = argv[1];
	test.creating_client_socket();
	test.connect_to_server(server_ip);
#endif
	std::thread t1 = std::thread(&client::sending_loop, &test);
	std::thread t2 = std::thread(&client::receiving_loop, &test);
	t1.join();
	t2.join();
	return 0;
}