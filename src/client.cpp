#include "config.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>

void recv_messages(int client_socket) {
	char buffer[1024];
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytes_received <= 0) {
			std::cerr << "Server disconnected." << std::endl;
			close(client_socket);
			break;
		}
		std::cout << "Received: " << buffer << std::endl;
	}
}

int main() {
	int client_socket;
	struct sockaddr_in server_addr;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Connection to server failed." << std::endl;
		close(client_socket);
		return 1;
	}

	std::cout << "Connected to server." << std::endl;

	std::thread recv_thread(recv_messages, client_socket);
	recv_thread.detach();

	std::string message;
	while (true) {
		std::getline(std::cin, message);
		if (message == "quit") {
			send(client_socket, message.c_str(), message.size(), 0);
			break;
		}
		send(client_socket, message.c_str(), message.size(), 0);
	}

	close(client_socket);
	return 0;
}
