// server.cpp
#include "config.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

std::vector<int> clientSockets;
std::mutex clientsMutex;

void recv_func(int client_socket, int idx) {
	const int BUFFER_SIZE = 1024;
	char recv_buff[BUFFER_SIZE];
	while (true) {
		memset(recv_buff, 0, BUFFER_SIZE);
		int r = recv(client_socket, recv_buff, sizeof(recv_buff), 0);
		if (r > 0) {
			printf("Client %d -----> Server : %s\n", idx, recv_buff);
		}

		if (strncmp(recv_buff, "quit", 4) == 0 || r <= 0) {
			printf("Client %d quit\n", idx);
			break;
		}

		std::lock_guard<std::mutex> lock(clientsMutex);
		for (size_t i = 0; i < clientSockets.size(); ++i) {
			if (i != idx) {
				int send_result = send(clientSockets[i], recv_buff, r, 0);
				if (send_result == -1) {
					std::cerr << "Failed to send message to client " << i << std::endl;
				}
			}
		}
	}

	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		close(client_socket);
		clientSockets.erase(clientSockets.begin() + idx);
	}
}
int main() {
	int server_socket;
	struct sockaddr_in server_addr;

	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		close(server_socket);
		exit(EXIT_FAILURE);
	}

	if (listen(server_socket, 5) < 0) {
		perror("listen");
		close(server_socket);
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on " << SERVER_IP << ":" << PORT << std::endl;

	while (true) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
		if (client_socket < 0) {
			perror("accept");
			continue;
		}

		int idx;
		{
			std::lock_guard<std::mutex> lock(clientsMutex);
			clientSockets.push_back(client_socket);
			idx = clientSockets.size() - 1;
		}

		std::cout << "Client " << idx << " connected from " << inet_ntoa(client_addr.sin_addr) << ":"
		          << ntohs(client_addr.sin_port) << std::endl;

		std::thread(recv_func, client_socket, idx).detach();
	}

	close(server_socket);
	return 0;
}