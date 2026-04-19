#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared_vars.hpp"

inline constexpr uint16_t SIN_PORT = 2053;
inline constexpr int SOCKET_PROTOCOL = 0;

namespace Slime {
/**
 * Initalizes a udp socket with the default SIN_PORT 2053
 *
 * ARGS:
 * sSocket: reference to an int that will be the udpSocket fd
 * clientAddress: sockaddr_in sturct that will contain clientAddress information
 *
 * RETURNS:
 * returns -1 if an error occured, else 0
 */
int create_ssocket(int& sSocket, struct sockaddr_in& clientAddress,
                   uint16_t port = SIN_PORT, uint32_t ip = INADDR_ANY);

/**
 * initalized a resolver udp based which assigned the udp to the given ip and
 * port
 *
 * ARGS:
 * fSocket: reference to an int that will be the udpSocket fd
 * clientAddress: sockaddr_in sturct that will contain clientAddress information
 * resolver: char* const* that contains resolver ip:port
 *
 * RETURNS:
 * returns -1 if an error occured, else 0
 */
int create_fsocket(int& rSocket, struct sockaddr_in& resolverAddress,
                   char* resolver);

/**
 * creates a forwarding server
 *
 * ARGS:
 * sSocket: socketfd that contains the udpSocket
 * clientAddress: contasins client information
 * fSocket: socketfd that contains the forward socekt Udp
 */
void forward_server(int& sSocket, struct sockaddr_in& clientAddress,
                    int& rSocket, struct sockaddr_in& resolverAddress);

/**
 * Creates a non forwarding server which was used to pass past sections
 *
 * ARGS:
 * sSocket: socketfd that contains the udpSocket
 * clientAddress: contasins client information
 */
void non_forward_server(int& sSocket, struct sockaddr_in& clientAddress);
}  // namespace Slime
