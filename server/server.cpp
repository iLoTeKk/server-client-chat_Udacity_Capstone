#include"server.h"

using namespace std;

//helper functions 
std::string get_time() {//thjis function is used for logging information
	auto now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	std::tm* localtime = std::localtime(&current_time);
	std::ostringstream oss;
	oss << std::put_time(localtime, "[%d/%m/%Y] %H:%M:%S");
	std::string formatted = oss.str();
	return formatted;
}//got it from AI


#ifdef _WIN32
int server::wsastart() {//if it's a windows machine we need to initiliaze you can find it in windows api doc
	_wVersionRequested = MAKEWORD(2, 2);//version of windows socket it should be DWORD 'windows data type'
	_errwsa = WSAStartup(_wVersionRequested, &_wsa);//wsa is a pointer of wsadata that will be retrieved
	if (_errwsa != 0) {//if succeeded it returns 0
		std::cout << "dll initialization didn't succeeded \n";
		return _errwsa;
	}
	else {
		std::cout << "dll initialization succeeded with status of "<< _wsa.szSystemStatus << std::endl;
		return _errwsa;
	}
}
#endif


SOCKET server::creating_server_socket() {
	_serversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// AF_INET is an address family for 1pv4 that allows
															//communication and it's for TCP and UDP 
															//scok_stream type of the socket that allow TCP transmission
															//IPPROTO_TCP is the protocol to be used
	if (_serversocket == INVALID_SOCKET) {
#ifdef _WIN32
		WSACleanup();
		cout << "error creating socket: " << WSAGetLastError() << endl;
#else
		cout << "error creating socket " << endl;
#endif
		return _serversocket;
	}
	else {//if success it returns 0 and will create a new socket
		cout <<"Socket created successfully \n";
		return _serversocket;
	}
}

int server::bind_socket() {//you should bind the socket to an ip address before setting it in lestining status
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(PORT);//the port number should be in network byte order that we used htons funciotn
	_sockaddr.sin_addr.s_addr = INADDR_ANY;//any interfaces avaialable -can be set to a specific one-

	if(bind(_serversocket, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr))== SOCKET_ERROR)
	{//the function take an unbound socket + a stuck of sockaddr + size of that sockaddr
		cout << "bind failed "  << endl;
		closesocket(_serversocket);
	#ifdef _WIN32
		WSACleanup();
	#endif
		return 1;
	}
	else {
		cout << "bind successful " << endl;
		return 0;
	}


}


int server::listening_for_connections(int n) {
	if (listen(_serversocket, n))//listen takes two inputs one is server socket and the other is the maximumx connecttion
	{
#ifdef _WIN32
		cout << "can't listen error" << WSAGetLastError()<<endl;
#else
		cout << "can't listen error" << endl;
#endif
		return 1;
	}
	else {
		char addrBuffer[INET_ADDRSTRLEN];//a buffer for the address with the length of the addr 
		inet_ntop(AF_INET, &_sockaddr.sin_addr, addrBuffer, INET_ADDRSTRLEN);//it just convert the addr from sin_addr which is
																			//is inside the sockaddr struct and make it into human readable format
		cout << "listening on "<< addrBuffer<< "port "<< ntohs(_sockaddr.sin_port) << endl;//ntohs is the reversed of ntons it convert from network to host format
		return 0;
	}
}


socketc server::connecting_client() {
	socketc  _server_client_socket;//here i'm creating a scoketc class whcich is has a member of SOCKET so ican return it when connecting the client
	_server_client_socket.set(accept(_serversocket, NULL, NULL));//since it's a private member we previously declared .set() to set the scoket
	if (_server_client_socket.get() == INVALID_SOCKET) {
#ifdef _WIN32
		cout << "accept failed " << WSAGetLastError() << endl;
		WSACleanup();
#else
		cout << "accept failed " << endl;
#endif
		return _server_client_socket;
	}
	else {
		//from here 
#ifdef _WIN32
		u_long mode = 1;
		ioctlsocket(_server_client_socket.get(), FIONBIO, &mode);
#else
		int flags = fcntl(_server_client_socket.get(), F_GETFL, 0);//this means we're getting the socket flags
		fcntl(_server_client_socket.get(), F_SETFL, flags | O_NONBLOCK);//this means we want to keep all the flags in the socket and O_NONBLOCK
																		//so we can hadnle multiple users chat at same time
#endif
		//to here it's from AI to solve blocking issue 
		cout << "[+] client accepted " << endl;
		return _server_client_socket;
	}

}



std::string server::receive_from_client(socketc& _server_client_socket) {//this function will recieve from clients
	char _receive_buff[4150];//buffer to store the message recieved
	int byte_count = recv(_server_client_socket.get(), _receive_buff, sizeof(_receive_buff) - 1, 0);

	if (byte_count > 0) {
		_receive_buff[byte_count] = '\0';//since it's a buffer we should null terminate the last char of the buff
		std::cout << "message received: " << _receive_buff << std::endl;
		return std::string(_receive_buff);//return this buff as a string so we can broadcast it later
	}
	else if (byte_count == 0) { // [-] client disconnected
		std::cout << "[-] client disconnected socket_id: " << _server_client_socket.get() << std::endl;
		closesocket(_server_client_socket.get());
		_server_client_socket.set(INVALID_SOCKET);
		return "[-] client disconnected";
	}
	else {
		//from here
#ifdef _WIN32
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return ""; // no data available just return empty string
#else
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return ""; // since we made it non-blocking we should add those so we don't end the socket if recv failed
#endif
		//to here from gpt it's to solve blocking issue 
		else {
			std::cerr << "[-] client disconnected " << std::endl;
			closesocket(_server_client_socket.get());
			_server_client_socket.set(INVALID_SOCKET);
			return "[-] client disconnected"; // or handle differently
		}
	}
}


void server::send_to_client(SOCKET s, std::string mess_recv) {// this is the sending function
	int byte_count = send(s, mess_recv.c_str(), mess_recv.size(), 0);//since it's a string we should use c_str() function which will convert it
																	// to a cahr * since send also takes a buffer
	if (byte_count > 0) {
		cout << "message sent" << endl;
	}
	else {
		cout << "failed to send message " << endl;
	}
}


int server::close_server_socket() {
#ifdef _WIN32
	closesocket(_serversocket);
#else
	close(_serversocket);
#endif
	return 0;
}


void server::comminication_loop() {// this loop so we can handle messaging on seperated thread
	while (true)
	{//infinite loop
		
			std::this_thread::sleep_for(std::chrono::milliseconds(10));//better to add timing when dealing with thread to know any issues

			for (auto cli = _client_vector.begin(); cli != _client_vector.end();)//this is both eterator and element it's used when dealing with vectors
			{
				std::string recv_mess = receive_from_client(**cli);//because we're using this iterator and since it's a smartpointer we should to double pointers the first will
																//will access the element and the 2nd will access the socket inside the pointer
				
				if (recv_mess == "[-] client disconnected")
				{//remove client form vector if it disconnected
					std::lock_guard<std::mutex> lock(_mtx_con);//so we don't get a daata race
					for (auto& cli2 : _client_vector)
					{//check the same user so we don't send the message for him
						if (cli2 == *cli)
						{
							continue;//if it's him continue to the next
						}
						send_to_client(cli2->get(), recv_mess);//send to all users that the client disconnected
						std::string log_mes = get_time() + " " + recv_mess + "\n";//place this in the log file
					}

					cli = _client_vector.erase(cli);//erase user from the vector and make it invalid iterator and return an iterater to the next one so we assign it to clie
					continue;
				}
				std::lock_guard<std::mutex> lock(_mtx_con);//so we don't get data race
				if (!recv_mess.empty())
				{//if message is not empty
					for (auto& cli2 : _client_vector)
					{
						if (cli2 == *cli)//skip the same user
						{
							continue;
						}
						send_to_client(cli2->get(), recv_mess);//send to other users
						std::cout << "message sent to " << cli2->get() << std::endl;//server notice
					}
					std::string log_mes = get_time() + " " + recv_mess + "\n";
					add_to_log_file(log_mes);//add this mess to the log
				}
				++cli;
		}
	}
}//will be tranfered for messaging thread


void server::clients_vector(socketc&& s) {// this will add every socket to the vector so we can then send them the messages
	{
		std::string join_msg = "q new client has joined the chat";//a new user has joined
		std::lock_guard<std::mutex> lock(_mtx_con);//so don't get a data race
		for (auto& cli : _client_vector)
		{
			send_to_client(cli->get(), join_msg);// notify every users
		}
	}
	{
		std::lock_guard<std::mutex> lock(_mtx_add);//so we don't get data race from other threads
		_client_vector.emplace_back(make_unique<socketc>(std::move(s)));//since we don't allow copying and since it's a unique pointer thus we're creating it as a new unique pointer
																		//+ moving it to the vector, and we used make_unqiuee because our vector in the calss is defined to take unique pointers
		std::cout << "client added to the vector " << std::endl;
		return;
	}
}


void server::connection_loop() {//so we keep accepting clients
	while (true)
	{
		socketc clienttmp = connecting_client();//get the new client socket
		clients_vector(std::move(clienttmp));//move it to the vector and from there it'll be moved again and will be converted into a smart pointer
	}
}


void server::add_to_log_file(std::string log_mess){
	std::ofstream logfile("server_client_logfile.txt",ios::app);//this is the flag that allow us to append https://mathbits.com/MathBits/CompSci/Files/Append.htm
	if (logfile.is_open())//check that it's open
	{
		logfile.write(log_mess.c_str(), log_mess.size());//added to the file 
	}
	logfile.close();//close the file
}