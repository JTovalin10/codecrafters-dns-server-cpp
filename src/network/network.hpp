#pragma once

#include "shared_vars.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

inline constexpr uint16_t SIN_PORT = 2053;
inline constexpr int SOCKET_PROTOCOL = 0;

namespace Slime {
/**
 * Initalizes a udp socket with the default SIN_PORT 2053
 *
 * ARGS:
 * udpSocket: reference to an int that will be the udpSocket fd
 * clientAddress: sockaddr_in sturct that will contain clientAddress information
 *
 * RETURNS:
 * returns -1 if an error occured, else 0
 */
int iudp(int &udpSocket, struct sockaddr_in &clientAddress,
         uint16_t port = SIN_PORT, uint32_t ip = INADDR_ANY);

/**
 * initalized a resolver udp based which assigned the udp to the given ip and
 * port
 *
 * ARGS:
 * udpSocket: reference to an int that will be the udpSocket fd
 * clientAddress: sockaddr_in sturct that will contain clientAddress information
 * resolver: char* const* that contains resolver ip:port
 *
 * RETURNS:
 * returns -1 if an error occured, else 0
 */
int irudp(int &udpSocket, struct sockaddr_in &clientAddress, char *resolver);

void read_from(int &udpSocket, struct sockaddr_in &clientAddress);
} // namespace Slime
