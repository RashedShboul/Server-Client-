#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // create a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Socket creation failed\n";
    close(sockfd);
    return 1;
  }

  struct sockaddr_in address; // sockaddr_in used to store IP addresses
  int port = 2222;

  address.sin_family = AF_INET;         // addressing type IPv4 of IPv6
  address.sin_addr.s_addr = INADDR_ANY; // recive connections from any interface
  address.sin_port = htons(port);       // set the port number for the socket
  // htons function convert port# form host byte order to network byte order
  memset(address.sin_zero, '\0',
         sizeof(address.sin_zero)); // make sure the padding is zeros it is not
                                    // necessary

  // binding the socket to an address
  if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "Bind failed\n";
    return 2;
  }

  // lesten for a connection
  if (listen(sockfd, 10) < 0) {
    std::cerr << "Listen failed\n";
    return 3;
  }
  // accepting
  int new_connection;
  while (true) {
    struct sockaddr_in clientAdrr;
    socklen_t clientLen = sizeof(clientAdrr);
    new_connection = accept(sockfd, (struct sockaddr *)&clientAdrr, &clientLen);
    if (new_connection < 0) {
      std::cerr << "Accept failed\n";
      continue;
      // close(sockfd);
      // break;
    }
    // close(sockfd);  //added this to handle just one client
    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(
        AF_INET, &(clientAdrr.sin_addr), clientIp,
        INET_ADDRSTRLEN); // function convert network address to char string
    std::cout << "Connection accepted from " << clientIp << std::endl;
    // receving data
    char buffer[1024] = {0};
    ssize_t bytesReceived;
    while ((bytesReceived = recv(new_connection, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << clientIp << ": " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));
    }

    if (bytesReceived < 0) {
        std::cerr << "Receive error: " << strerror(errno) << '\n';
    }

    close(new_connection);
  }
  close(sockfd);
  return 0;
}