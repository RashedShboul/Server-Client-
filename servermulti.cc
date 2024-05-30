#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
const int PORT = 8090; 
const int BUFFER_SIZE = 1024; 
const int MAX_CONNECTIONS = 10; 

void handle_client(int client_sockfd, sockaddr_in addr) {
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
  std::cout << "connection accepted from " << client_ip << std::endl;
  char buffer[BUFFER_SIZE];
  int bytes_recevied;
  while (true) {
    int bytes_received = recv(client_sockfd, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
      std::string message(buffer, bytes_received);
      if (message == "exit")
        break;
      std::cout << client_ip << ": " << message << std::endl;
    } else if (bytes_received == 0) {
      std::cout << "Client disconnected." << std::endl;
      break;
    } else {
      perror("Error receiving data");
      break;
    }
  }
  close(client_sockfd);
}

int main() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cout << "failed to create socket\n";
    return 1;
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cout << "Failed to bind the socket\n";
    return 2;
  }

  if (listen(sockfd, MAX_CONNECTIONS) < 0) {
    std::cout << "failed to listen\n";
    close(sockfd);
    return 3;
  }
  int new_client_connection;
  while (true) {
    struct sockaddr_in new_client_addr;
    socklen_t len = sizeof(new_client_addr);
    new_client_connection =
        accept(sockfd, (struct sockaddr *)&new_client_addr, &len);
    if (new_client_connection < 0) {
      std::cout << "failed to connect\n";
      continue;
    }

    std::thread client_handler(handle_client, new_client_connection,
                               new_client_addr);
    client_handler.detach();
  }
  close(sockfd); 
  return 0;
}