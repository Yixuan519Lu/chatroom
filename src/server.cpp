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