#include "network.hpp"

#include <netinet/in.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "../packet/packet.hpp"

namespace Slime {
int initalize_udp(int& udpSocket, struct sockaddr_in& clientAddress) {
  udpSocket = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL);
  if (udpSocket == -1) {
    std::cerr << "Socket creation failed: " << strerror(errno) << "..." << '\n';
    return EXIT_FAILURE;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << '\n';
    return EXIT_FAILURE;
  }

  sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(SIN_PORT),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(udpSocket, reinterpret_cast<struct sockaddr*>(&serv_addr),
           sizeof(serv_addr)) != 0) {
    std::cerr << "Bind failed: " << strerror(errno) << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void read_from(int& udpSocket, struct sockaddr_in& clientAddress) {
  ssize_t bytesRead;
  std::array<char, BUFFER_SIZE> buffer{};
  socklen_t clientAddrLen = sizeof(clientAddress);
  // empty response
  while (true) {
    // Receive data
    bytesRead = recvfrom(udpSocket, buffer.data(), buffer.size(), 0,
                         reinterpret_cast<struct sockaddr*>(&clientAddress),
                         &clientAddrLen);
    if (bytesRead == -1) {
      perror("Error receiving data");
      break;
    }
    if (bytesRead < 12) {
      std::cerr << "Packet too small\n";
    }
    std::array<char, BUFFER_SIZE> response{};
    std::memcpy(&response, buffer.data(), BUFFER_SIZE);

    Header header{};
    std::memcpy(&header, response.data(), sizeof(Header));
    header.flags = ntohs(header.flags);

    flags_bg fb(header);
    fb.set_qr(1);

    header.flags = htons(fb.release());
    std::memcpy(response.data(), &header, sizeof(Header));
    /**
    buffer[bytesRead] = '\0';
    std::cout << "Received " << bytesRead << " bytes: " << buffer.data()
              << '\n';
    */

    // Send response
    if (sendto(udpSocket, response.data(), response.size(), 0,
               reinterpret_cast<struct sockaddr*>(&clientAddress),
               sizeof(clientAddress)) == -1) {
      perror("Failed to send response");
    }
  }
}
}  // namespace Slime
