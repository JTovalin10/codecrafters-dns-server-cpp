#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>

constexpr int BUFFER_SIZE = 512;
constexpr uint16_t SIN_PORT = 2053;
constexpr int SOCKET_PROTOCOL = 0;

int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Disable output buffering
  setbuf(stdout, NULL);
  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!" << '\n';

  int udpSocket;
  struct sockaddr_in clientAddress;

  udpSocket = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL);
  if (udpSocket == -1) {
    std::cerr << "Socket creation failed: " << strerror(errno) << "..." << '\n';
    return 1;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << '\n';
    return 1;
  }

  sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(SIN_PORT),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(udpSocket, reinterpret_cast<struct sockaddr*>(&serv_addr),
           sizeof(serv_addr)) != 0) {
    std::cerr << "Bind failed: " << strerror(errno) << '\n';
    return 1;
  }

  ssize_t bytesRead;
  std::array<char, BUFFER_SIZE> buffer{};
  socklen_t clientAddrLen = sizeof(clientAddress);
  // empty response
  std::array<char, 1> response = {'\0'};
  while (true) {
    // Receive data
    bytesRead = recvfrom(udpSocket, buffer.data(), buffer.size(), 0,
                         reinterpret_cast<struct sockaddr*>(&clientAddress),
                         &clientAddrLen);
    if (bytesRead == -1) {
      perror("Error receiving data");
      break;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Received " << bytesRead << " bytes: " << buffer.data()
              << '\n';

    // Send response
    if (sendto(udpSocket, response.data(), response.size(), 0,
               reinterpret_cast<struct sockaddr*>(&clientAddress),
               sizeof(clientAddress)) == -1) {
      perror("Failed to send response");
    }
  }

  close(udpSocket);

  return 0;
}
