all:client server
client:client.cpp
	g++ -std=c++0x $^ -o $@ -lboost_filesystem -lboost_system -lpthread
server:server.cpp
	g++ -std=c++0x $^ -o $@ -lboost_filesystem -lboost_system -lpthread
