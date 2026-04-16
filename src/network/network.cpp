#include "network.hpp"

#include <netinet/in.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

#include "../packet/packet.hpp"
#include "my_utils.hpp"

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

void set_header_to_host(Header& header) {
  header.pid = ntohs(header.pid);
  header.flags = ntohs(header.flags);
  header.qdcount = ntohs(header.qdcount);
  header.ancount = header.qdcount;
  header.arcount = 0;
  header.nscount = 0;
}

void set_header_to_network(Header& header) {
  header.pid = htons(header.pid);
  header.flags = htons(header.flags);
  header.qdcount = htons(header.qdcount);
  header.ancount = htons(header.ancount);
  header.arcount = htons(header.arcount);
  header.nscount = htons(header.nscount);
}

void populate_header(Header& header) {
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
}

uint16_t complete_header(std::array<uint8_t, BUFFER_SIZE>& buffer,
                         Header& header) {
  std::memcpy(&header, buffer.data(), sizeof(Header));
  set_header_to_host(header);
  populate_header(header);
  set_header_to_network(header);
  std::memcpy(buffer.data(), &header, sizeof(Header));
  return ntohs(header.qdcount);
}

size_t populate_questions(std::array<uint8_t, BUFFER_SIZE>& buffer,
                          Question& que, size_t offset) {
  auto [name, moved] = Slime::find_network_format_name(buffer, offset);
  set_que_name(que, std::move(name));
  set_que_class(que, Slime::classes::IN);
  set_que_type(que, Slime::records::A);
  return moved + sizeof(que._class) + sizeof(que.type);
}

void write_question_to_buffer(std::array<uint8_t, BUFFER_SIZE>& buffer,
                              Question& que, size_t offset) {
  const uint16_t type_n = ntohs(que.type);
  const uint16_t class_n = ntohs(que._class);
  uint8_t* cursor = buffer.data() + offset;
  std::memcpy(cursor, que.name.data(), que.name.size());
  cursor += que.name.size();
  std::memcpy(cursor, &type_n, sizeof(type_n));
  cursor += sizeof(type_n);
  std::memcpy(cursor, &class_n, sizeof(class_n));
}

void complete_question(std::array<uint8_t, BUFFER_SIZE>& buffer, Question& que,
                       size_t offset) {
  populate_questions(buffer, que, offset);
  write_question_to_buffer(buffer, que, offset);
}

void populate_answer(Answer& ans, std::vector<uint8_t>&& name) {
  Slime::set_ans_name(ans, std::move(name));
  Slime::set_ans_type(ans, Slime::records::A);
  Slime::set_class(ans, Slime::classes::IN);
  Slime::set_ttl(ans);
  Slime::set_data(ans);
}

void write_answer_to_buffer(std::array<uint8_t, BUFFER_SIZE>& buffer,
                            Answer& ans, size_t offset) {
  const uint16_t type_n = htons(ans.type);
  const uint16_t class_n = htons(ans._class);
  const uint32_t ttl_n = htonl(ans.ttl);
  const uint16_t rdlen_n = htons(ans.rdlength);

  uint8_t* cursor = buffer.data() + offset;
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

void complete_answer(std::array<uint8_t, BUFFER_SIZE>& buffer, Answer& ans,
                     size_t offset, std::vector<uint8_t> name) {
  populate_answer(ans, std::move(name));
  write_answer_to_buffer(buffer, ans, offset);
}

const size_t HEADER_SIZE = get_header_size(Header{});
void read_from(int& udpSocket, struct sockaddr_in& clientAddress) {
  ssize_t bytesRead;
  std::array<char, BUFFER_SIZE> buffer{};
  socklen_t clientAddrLen = sizeof(clientAddress);
  // empty buffer
  while (true) {
    // Receive data
    bytesRead = recvfrom(udpSocket, buffer.data(), buffer.size(), 0,
                         reinterpret_cast<struct sockaddr*>(&clientAddress),
                         &clientAddrLen);
    if (bytesRead == -1) {
      perror("Error receiving data");
      break;
    }

    if (bytesRead < HEADER_SIZE) {
      perror("Header is too small");
      break;
    }

    std::array<uint8_t, BUFFER_SIZE> request{};
    std::memcpy(&request, buffer.data(), BUFFER_SIZE);
    // stage 2
    Header header{};
    uint16_t num_questions = complete_header(request, header);
    size_t read_offset = HEADER_SIZE;
    std::vector<Question> questions{};
    questions.reserve(num_questions);
    for (int i{}; i < num_questions; i++) {
      Question que;
      read_offset += populate_questions(request, que, read_offset);
      questions.push_back(que);
    }

    std::array<uint8_t, BUFFER_SIZE> response{};
    std::memcpy(&response, &header, HEADER_SIZE);
    size_t write_offset = HEADER_SIZE;
    for (Question& que : questions) {
      write_question_to_buffer(response, que, write_offset);
      write_offset += get_ques_size(que);
    }
    for (Question& que : questions) {
      Answer ans;
      complete_answer(response, ans, write_offset, que.name);
      write_offset += get_ans_size(ans);
    }
    if (sendto(udpSocket, response.data(), write_offset, 0,
               reinterpret_cast<struct sockaddr*>(&clientAddress),
               sizeof(clientAddress)) == -1) {
      perror("Failed to send buffer");
    }
  }
}
}  // namespace Slime
