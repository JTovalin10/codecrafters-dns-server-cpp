#include <cstdlib>
#include <iostream>

#include "network/network.hpp"

auto main() -> int {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Disable output buffering
  if (setvbuf(stdout, nullptr, _IONBF, BUFFER_SIZE) < 0) {
    std::cerr << "Disabling output buffer failed" << '\n';
    return 1;
  }
  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!" << '\n';

  int udpSocket;
  struct sockaddr_in clientAddress;
  if (Slime::initalize_udp(udpSocket, clientAddress) == EXIT_FAILURE) {
    return 1;
  }

  Slime::read_from(udpSocket, clientAddress);
  // upon recvfrom returning -1 we close the exit
  close(udpSocket);
  return 0;
}
