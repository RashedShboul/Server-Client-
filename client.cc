#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Failed to create a socket\n";
    return 1;
  }
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(2222);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof serverAddr) < 0) {
    std::cerr << "Faied to connect to the server\n";
    close(sockfd);
    return 2;
  }

  std::cout << "Connected to server!" << std::endl;
  std::string message;
  while (true) {
    std::cout << "Please enter your message: "; 
    std::getline(std::cin, message);
    if (message == "exit") break;
    ssize_t bytesSent = send(sockfd, message.c_str(), message.length(), 0);
    if (bytesSent < 0) {
      std::cerr << "Failed to send message\n";
      close(sockfd);
      return 3;
    }
    std::cout << "Message sent successfully!\n";
  }
    close(sockfd); 
  return 0;
}