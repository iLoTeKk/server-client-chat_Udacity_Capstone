## thanks 
Hello Udacity Team espcially C++ team more specifically David Silver, Stephen Welch, Bjarne Stroustrup and Andreas Haja Thank you for teaching me all of those skills

this is my basic server client chat project which i created within 4 days with the knowaledge that i have learned from this course + some sockets tutorials on youtube


## project overview
this project implements a simple multi-client chat system using tcp sockets in cpp.  
the server accepts multiple client connections using threads and smart pointers, manages message broadcasting, and writes logs to a file.  
the client connects to the server and supports concurrent sending and receiving using threads.


### prerequisites
- cpp11 or later
- cross-platform: works on windows, linux, and mac

### how to build
make server -> to build the server
make client -> to build the client
make all -> to build both
make clean -> to clean everything

## how to start
./server -> to start the server and it'll be listining on all available interfaces on port 5000
./client [serverip] || ./client then you'll be asked for server ip if you're on windows -> it'll connect to the server on port 5000

## feutures 
multi-client support using threads
message broadcasting to all connected clients
proper socket cleanup with destructors (raii)
message logs with timestamps written to a file
smart pointers (unique_ptr) for managing clients
mutex locks to prevent data races
cross- latform support for both windows and unix
move semantics implemented for socket objects
project demonstrates understanding of functions and control structures


## file function and I/O
many custom functions for socket setup, communication, file writing etc..
e.g. server::receive_from_client() at server.cpp:167

project reads/writes to file
logs messages to server_client_logfile.txt
 server::add_to_log_file() at server.cpp:360

project accepts and processes user input
user inputs messages and names in client::send_to_server() at client.cpp:83

## OOP
one or more classes with proper access specifiers
classes: server and socketc in server.h, and client in client.h

all members explicitly public or private
member initialization lists and constructors
socketc(SOCKET s) : soc(s) {} at server.h:116

move semantics implemented
move constructor and move assignment operator in socketc
server.h:126–140

raii and destructors
socketc::~socketc() automatically closes sockets when out of scope
server.h:120–125

overloaded constructors
socketc() and socketc(SOCKET s)

## memory management 
uses references and pass-by-reference
std::string server::receive_from_client(socketc& _server_client_socket)
server.cpp:155

uses smart pointers
_client_vector stores std::unique_ptr<socketc>
server.h:99

uses raii
socket cleanup is automatic in socketc destructor

rule of five
implemented destructor, deleted copy ctor and copy assignment, move ctor and move assignment
→ server.h:120–140

## concurrency 
uses multithreading
std::thread t1 for connection_loop() and std::thread t2 for comminication_loop()
main_server.cpp:13–15
client also uses two threads for sending/receiving concurrently
cleintmain.cpp:27–29

uses mutex or lock
multiple mutexes protect shared resources
_mtx_con, _mtx_add in server.h:96–97
used with std::lock_guard<std::mutex> in server.cpp:296

## how it works
the server starts and listens for clients on port 5000
when a client connects, it announces to others that it joined
clients can send messages to each other in real time
if a client disconnects, other users are notified
the server logs all chat activity with timestamps

## future improvement 
implement message queue templates
add private chats
add userlist and history
add login and session

## structure 
| project/
├── server/
│   ├── server.h
│   ├── server.cpp
│   └── main_server.cpp
├── client/
│   ├── client.h
│   ├── client.cpp
│   └── cleintmain.cpp
├── Makefile
└── README.md