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

void complete_header(std::array<char, BUFFER_SIZE>& response, Header& header) {
  std::memcpy(&header, response.data(), sizeof(Header));
  header.pid = ntohs(header.pid);
  header.flags = ntohs(header.flags);
  header.ancount = 0;
  header.arcount = 0;
  header.nscount = 0;
  header.qdcount = 0;
  // we already got the piud from the request packet
  set_qr(header, QR::RESPONSE);
  uint8_t opcode = get_opcode(header);
  if (opcode != 0) {
    set_rcode(header, Slime::RCODE::NOT_IMPLEMENTED);
  } else {
    set_rcode(header, Slime::RCODE::NO_ERROR);
  }
  set_aa(header, Slime::AA::NOT_AUTHORITATIVE);
  set_tc(header, Slime::TC::NOT_TRUNCATED);
  // mimic RD set by the user so this is fine as is
  set_ra(header, Slime::RA::NOT_AVAILABLE);
  set_z(header, 0);
  increment_qdcount(header);
  increment_ancount(header);
  header.pid = htons(header.pid);
  header.flags = htons(header.flags);
  header.ancount = htons(header.ancount);
  header.qdcount = htons(header.qdcount);
  header.arcount = htons(header.arcount);
  header.nscount = htons(header.nscount);
  std::memcpy(response.data(), &header, sizeof(Header));
}

void complete_question(std::array<char, BUFFER_SIZE>& response, Question& que,
                       int offset) {
  set_que_name(que, "codecrafters.io");
  set_que_class(que, Slime::classes::IN);
  set_que_type(que, Slime::records::A);
  const uint16_t type_n = ntohs(que.type);
  const uint16_t class_n = ntohs(que._class);
  char* cursor = response.data() + offset;
  std::memcpy(cursor, que.name.data(), que.name.size());
  cursor += que.name.size();
  std::memcpy(cursor, &type_n, sizeof(type_n));
  cursor += sizeof(type_n);
  std::memcpy(cursor, &class_n, sizeof(class_n));
}

void complete_answer(std::array<char, BUFFER_SIZE>& response, Answer& ans,
                     int offset) {
  Slime::set_ans_name(ans, "codecrafters.io");
  Slime::set_ans_type(ans, Slime::records::A);
  Slime::set_class(ans, Slime::classes::IN);
  Slime::set_ttl(ans);
  Slime::set_data(ans);
  const uint16_t type_n = htons(ans.type);
  const uint16_t class_n = htons(ans._class);
  const uint32_t ttl_n = htonl(ans.ttl);
  const uint16_t rdlen_n = htons(ans.rdlength);

  char* cursor = response.data() + offset;
  std::memcpy(cursor, ans.name.data(), ans.name.size());
  cursor += ans.name.size();
  std::memcpy(cursor, &type_n, sizeof(type_n));
  cursor += sizeof(type_n);
  std::memcpy(cursor, &class_n, sizeof(class_n));
  cursor += sizeof(class_n);
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
    size_t que_offset = get_header_size(header);
    complete_question(response, ques, que_offset);
    // stage 4:
    Answer ans{};
    size_t ans_offset = que_offset + get_ques_size(ques);
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
