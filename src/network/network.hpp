#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

inline constexpr int BUFFER_SIZE = 512;
inline constexpr uint16_t SIN_PORT = 2053;
inline constexpr int SOCKET_PROTOCOL = 0;

namespace Slime {
int initalize_udp(int& udpSocket, struct sockaddr_in& clientAddress);
void read_from(int& udpSocket, struct sockaddr_in& clientAddress);
}  // namespace Slime
