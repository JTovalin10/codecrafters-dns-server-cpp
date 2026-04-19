#include <cstdlib>
#include <iostream>

#include "network/network.hpp"

int main(int argc, char** argv) {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  if (setvbuf(stdout, nullptr, _IONBF, BUFFER_SIZE) < 0) {
    std::cerr << "Disabling output buffer failed" << '\n';
    return 1;
  }

  std::cout << "Logs from your program will appear here!" << '\n';
  // quick check to ensure usage
  // create server socket
  int sSocket;
  struct sockaddr_in clientAddress{};
  if (Slime::create_ssocket(sSocket, clientAddress) < 0) {
    std::cerr << "Error creating socket\n";
    return 1;
  }

  // if argc == 1 then we are not forwading
  if (argc == 1) {
    Slime::non_forward_server(sSocket, clientAddress);
  } else if (argc == 3) {
    // if argc == 3 then we are forwarding
    int fSocket;
    struct sockaddr_in resolverAddr{};
    if (Slime::create_fsocket(fSocket, resolverAddr, argv[2]) < 0) {
      return 1;
    }
    Slime::forward_server(sSocket, clientAddress, fSocket, resolverAddr);
    close(fSocket);
  }
  close(sSocket);
  return 0;
}
