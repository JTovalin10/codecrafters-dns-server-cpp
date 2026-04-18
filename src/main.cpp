#include <cstdlib>
#include <iostream>

#include "network/network.hpp"
#include "network/shared_vars.hpp"

int main(int argc, char **argv) {
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

  if (argc != 0 || argc != 2) {
    std::cerr << "./your_server [--resolver] [ip:port]";
    return EXIT_FAILURE;
  }
  int udpSocket;
  struct sockaddr_in clientAddress;
  if (argc == 0) {
    if (Slime::iudp(udpSocket, clientAddress) < 0) {
      return 1;
    }
  } else {
    if (Slime::irudp(udpSocket, clientAddress, argv[1]) < 0) {
      return 1;
    }
  }

  Slime::read_from(udpSocket, clientAddress);
  // upon recvfrom returning -1 we close the exit
  close(udpSocket);
  return 0;
}
