all: server client

server: server/server.cpp server/main_server.cpp
	@g++ server/server.cpp server/main_server.cpp -o server/server

client: client/client.cpp client/cleintmain.cpp
	@g++ client/client.cpp client/cleintmain.cpp -o client/client

clean:
	@rm -f server/server client/client

rebuild: clean all
.PHONY: all clean rebuild server client
