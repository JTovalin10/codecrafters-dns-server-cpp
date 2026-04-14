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

namespace {

const int PID = 1234;

void complete_header(std::array<char, BUFFER_SIZE>& response, Header& header) {
  std::memcpy(&header, response.data(), sizeof(Header));
  header.flags = ntohs(header.flags);
  set_pid(header, PID);
  set_qr(header, QR::RESPONSE);
  increment_ancount(header);
  header.flags = htons(header.flags);

  std::memcpy(response.data(), &header, sizeof(Header));
}

void complete_question(std::array<char, BUFFER_SIZE>& response, Question& que,
                       int offset) {
  set_que_name(que, "codecrafters.io");
  set_que_class(que, Slime::classes::IN);
  set_que_type(que, Slime::records::A);

  char* cursor = response.data() + offset;
  std::memcpy(cursor, que.name.data(), que.name.size());
  cursor += que.name.size();
  std::memcpy(cursor, &que.type, sizeof(que.type));
  cursor += sizeof(que.type);
  std::memcpy(cursor, &que._class, sizeof(que._class));
}

void complete_answer(std::array<char, BUFFER_SIZE>& response, Answer& ans,
                     int offset) {
  Slime::set_ans_name(ans, "codecrafters.io");
  Slime::set_ans_type(ans, Slime::records::A);
  Slime::set_class(ans, Slime::classes::IN);
  Slime::set_ttl(ans);
  Slime::set_data(ans);

  const uint32_t ttl_n = htonl(ans.TTL);
  const uint16_t rdlen_n = htons(ans.RDLENGTH);

  char* cursor = response.data() + offset;
  std::memcpy(cursor, ans.name.data(), ans.name.size());
  cursor += ans.name.size();
  std::memcpy(cursor, &ans.type, sizeof(ans.type));
  cursor += sizeof(ans.type);
  std::memcpy(cursor, &ans._class, sizeof(ans._class));
  cursor += sizeof(ans._class);
  std::memcpy(cursor, &ttl_n, sizeof(ttl_n));
  cursor += sizeof(ttl_n);
  std::memcpy(cursor, &rdlen_n, sizeof(rdlen_n));
  cursor += sizeof(rdlen_n);
  std::memcpy(cursor, ans.data.data(), ans.data.size());
}
};  // namespace

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

    std::array<char, BUFFER_SIZE> response{};
    std::memcpy(&response, buffer.data(), BUFFER_SIZE);
    // stage 2
    Header header{};
    complete_header(response, header);
    // stage 3 is recieving the packet and then just returning the same data
    Question ques{};
    int que_offset = get_header_size(header);
    complete_question(response, ques, que_offset);
    // stage 4:
    Answer ans{};
    int ans_offset = que_offset + get_ques_size(ques);
    complete_answer(response, ans, ans_offset);

    // Send response
    if (sendto(udpSocket, response.data(), response.size(), 0,
               reinterpret_cast<struct sockaddr*>(&clientAddress),
               sizeof(clientAddress)) == -1) {
      perror("Failed to send response");
    }
  }
}
}  // namespace Slime
