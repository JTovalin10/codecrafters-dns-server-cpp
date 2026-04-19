#include "network.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>

#include "helper.hpp"
#include "shared_vars.hpp"

namespace Slime {

// ─── Socket init ─────────────────────────────────────────────────────────────

int create_ssocket(int& sSocket, struct sockaddr_in& cAddr, uint16_t port,
                   uint32_t ip) {
  sSocket = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL);
  if (sSocket < 0) {
    std::cerr << "Socket creation failed: " << strerror(errno) << '\n';
    return -1;
  }

  int reuse = 1;
  if (setsockopt(sSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << '\n';
    return -1;
  }

  sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr = {htonl(ip)},
  };

  if (bind(sSocket, reinterpret_cast<struct sockaddr*>(&serv_addr),
           sizeof(serv_addr)) != 0) {
    std::cerr << "Bind failed: " << strerror(errno) << '\n';
    return -1;
  }
  return 0;
}

int create_fsocket(int& fSocket, struct sockaddr_in& resolverAddr,
                   char* resolver) {
  std::string tmp(resolver);
  size_t split = tmp.find(':');
  if (split == std::string::npos) {
    std::cerr << "Could not parse resolver address\n";
    return -1;
  }

  std::string ip_str = tmp.substr(0, split);
  uint16_t port = static_cast<uint16_t>(std::stoul(tmp.substr(split + 1)));

  fSocket = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL);
  if (fSocket < 0) {
    std::cerr << "Forwarder socket creation failed: " << strerror(errno)
              << '\n';
    return -1;
  }

  resolverAddr = {};
  resolverAddr.sin_family = AF_INET;
  resolverAddr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip_str.c_str(), &resolverAddr.sin_addr) <= 0) {
    std::cerr << "Invalid resolver IP: " << ip_str << '\n';
    return -1;
  }

  return 0;
}

// ─── Non-forwarding server ───────────────────────────────────────────────────

constexpr int FLAGS = 0;
void non_forward_server(int& sSocket, struct sockaddr_in& cAddr) {
  std::array<uint8_t, BUFFER_SIZE> buffer{};
  socklen_t cAddrLen = sizeof(cAddr);

  while (true) {
    ssize_t bytesRead =
        recvfrom(sSocket, buffer.data(), buffer.size(), FLAGS,
                 reinterpret_cast<struct sockaddr*>(&cAddr), &cAddrLen);
    if (bytesRead < 0) {
      perror("Error receiving data");
      break;
    }
    if (bytesRead < static_cast<ssize_t>(HEADER_SIZE)) {
      perror("Header too small");
      break;
    }

    std::array<uint8_t, BUFFER_SIZE> request = buffer;
    std::array<uint8_t, BUFFER_SIZE> response = buffer;
    size_t write_offset = HEADER_SIZE;
    exec_non_forward(request, response, write_offset);

    if (sendto(sSocket, response.data(), write_offset, 0,
               reinterpret_cast<struct sockaddr*>(&cAddr), cAddrLen) < 0) {
      perror("Failed to send response");
    }
  }
}

// ─── Forwarding server ───────────────────────────────────────────────────────

void forward_server(int& sSocket, struct sockaddr_in& cAddr, int& fSocket,
                    struct sockaddr_in& fAddr) {
  std::array<uint8_t, BUFFER_SIZE> buffer{};
  socklen_t cAddrLen = sizeof(cAddr);
  socklen_t fAddrLen = sizeof(fAddr);
  size_t bread;
  while (true) {
    bread = recvfrom(sSocket, buffer.data(), buffer.size(), FLAGS,
                     reinterpret_cast<struct sockaddr*>(&cAddr), &cAddrLen);
    if (bread < 0) {
      std::cerr << "Error receiving data\n";
      break;
    }
    if (bread < static_cast<ssize_t>(HEADER_SIZE)) {
      std::cerr << "Header is too small\n";
      break;
    }

    std::array<uint8_t, BUFFER_SIZE> request = buffer;
    std::array<uint8_t, BUFFER_SIZE> response = buffer;
    size_t write_offset = HEADER_SIZE;
    Slime::execf(request, response, write_offset, fSocket, fAddr);
    // send the response back to the client
    if (sendto(sSocket, response.data(), write_offset, FLAGS,
               reinterpret_cast<struct sockaddr*>(&cAddr), cAddrLen) < 0) {
      std::cerr << "Failed to send data\n";
      break;
    }
  }
}

}  // namespace Slime
