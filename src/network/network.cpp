#include "network.hpp"

#include <cstddef>
#include <netinet/in.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <utility>
#include <vector>

#include "../packet/packet.hpp"
#include "my_utils.hpp"

namespace Slime {

int irudp(int &udpSocket, struct sockaddr_in &clientAddress, char *resolver) {
  std::string tmp(resolver);
  size_t split = tmp.find(":");
  if (split == std::string::npos) {
    std::cerr << "Could not find port and ip\n";
    return -1;
  }
  uint32_t ip = std::stoul(tmp.substr(0, split));
  uint16_t port = static_cast<uint16_t>(std::stoul(tmp.substr(split + 1)));
  return iudp(udpSocket, clientAddress, port, ip);
}
}

int iudp(int &udpSocket, struct sockaddr_in &clientAddress, uint8_t port,
         uint32_t ip) {
  udpSocket = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL);
  if (udpSocket < 0) {
    std::cerr << "Socket creation failed: " << strerror(errno) << "..." << '\n';
    return -1;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << '\n';
    return -1;
  }

  sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr = {htonl(port)},
  };

  if (bind(udpSocket, reinterpret_cast<struct sockaddr *>(&serv_addr),
           sizeof(serv_addr)) != 0) {
    std::cerr << "Bind failed: " << strerror(errno) << '\n';
    return -1;
  }
  return 0;
}

uint8_t check_if_rd(std::array<uint8_t, BUFFER_SIZE> request) {
  Slime::Header header;
  std::memcpy(&header, &request, sizeof(header));
  header.flags = ntohs(header.flags);
  uint8_t rd = get_rd(header);
  header.flags = htons(header.flags);
  return rd;
}

void set_header_to_host(Slime::Header &header) {
  header.pid = ntohs(header.pid);
  header.flags = ntohs(header.flags);
  header.qdcount = ntohs(header.qdcount);
  header.ancount = header.qdcount;
  header.arcount = 0;
  header.nscount = 0;
}

void set_header_to_network(Slime::Header &header) {
  header.pid = htons(header.pid);
  header.flags = htons(header.flags);
  header.qdcount = htons(header.qdcount);
  header.ancount = htons(header.ancount);
  header.arcount = htons(header.arcount);
  header.nscount = htons(header.nscount);
}

void populate_header(Slime::Header &header) {
  set_qr(header, Slime::QR::RESPONSE);
  uint8_t opcode = get_opcode(header);
  if (opcode != 0) {
    set_rcode(header, Slime::RCODE::NOT_IMPLEMENTED);
  } else {
    set_rcode(header, Slime::RCODE::NO_ERROR);
  }
  set_ra(header, Slime::RA::AVAILABLE);
  set_aa(header, Slime::AA::NOT_AUTHORITATIVE);
  set_tc(header, Slime::TC::NOT_TRUNCATED);
  // mimic RD set by the user so this is fine as is
  set_ra(header, Slime::RA::NOT_AVAILABLE);
  set_z(header, 0);
}

uint16_t complete_header(std::array<uint8_t, BUFFER_SIZE> &buffer,
                         Slime::Header &header) {
  std::memcpy(&header, buffer.data(), sizeof(header));
  set_header_to_host(header);
  populate_header(header);
  set_header_to_network(header);
  std::memcpy(buffer.data(), &header, sizeof(header));
  return ntohs(header.qdcount);
}

size_t populate_questions(const std::array<uint8_t, BUFFER_SIZE> &buffer,
                          Slime::Question &que, size_t offset) {
  auto [name, moved] = Slime::find_network_format_name(buffer, offset);
  set_que_name(que, std::move(name));
  set_que_class(que, Slime::classes::IN);
  set_que_type(que, Slime::records::A);
  return moved + sizeof(que._class) + sizeof(que.type);
}

void write_question_to_buffer(std::array<uint8_t, BUFFER_SIZE> &buffer,
                              Slime::Question &que, size_t offset) {
  const uint16_t type_n = ntohs(que.type);
  const uint16_t class_n = ntohs(que._class);
  uint8_t *cursor = buffer.data() + offset;
  std::memcpy(cursor, que.name.data(), que.name.size());
  cursor += que.name.size();
  std::memcpy(cursor, &type_n, sizeof(type_n));
  cursor += sizeof(type_n);
  std::memcpy(cursor, &class_n, sizeof(class_n));
}

void complete_question(std::array<uint8_t, BUFFER_SIZE> &buffer,
                       Slime::Question &que, size_t offset) {
  populate_questions(buffer, que, offset);
  write_question_to_buffer(buffer, que, offset);
}

void populate_answer(Slime::Answer &ans, std::vector<uint8_t> &&name) {
  Slime::set_ans_name(ans, std::move(name));
  Slime::set_ans_type(ans, Slime::records::A);
  Slime::set_class(ans, Slime::classes::IN);
  Slime::set_ttl(ans);
  Slime::set_data(ans);
}

void write_answer_to_buffer(std::array<uint8_t, BUFFER_SIZE> &buffer,
                            Slime::Answer &ans, size_t offset) {
  const uint16_t type_n = htons(ans.type);
  const uint16_t class_n = htons(ans._class);
  const uint32_t ttl_n = htonl(ans.ttl);
  const uint16_t rdlen_n = htons(ans.rdlength);

  uint8_t *cursor = buffer.data() + offset;
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

void complete_answer(std::array<uint8_t, BUFFER_SIZE> &buffer,
                     Slime::Answer &ans, size_t offset,
                     std::vector<uint8_t> name) {
  populate_answer(ans, std::move(name));
  write_answer_to_buffer(buffer, ans, offset);
}

void find_all_questions(std::vector<Slime::Question> &questions,
                        const std::array<uint8_t, BUFFER_SIZE> &request,
                        size_t num_questions, size_t &read_offset) {
  for (int i{}; i < num_questions; i++) {
    Slime::Question que;
    read_offset += populate_questions(request, que, read_offset);
    questions.push_back(que);
  }
}

void populate_response(std::array<uint8_t, BUFFER_SIZE> &response,
                       size_t &write_offset,
                       std::vector<Slime::Question> &questions) {
  // write questions and answers
  for (Slime::Question &que : questions) {
    write_question_to_buffer(response, que, write_offset);
    write_offset += get_ques_size(que);
  }
  for (Slime::Question &que : questions) {
    Slime::Answer ans;
    complete_answer(response, ans, write_offset, que.name);
    write_offset += get_ans_size(ans);
  }
}

const size_t HEADER_SIZE = 12;

void execr(const std::array<uint8_t, BUFFER_SIZE> &request,
           std::array<uint8_t, BUFFER_SIZE> &response, size_t &write_offset) {
  return;
}

// request is for read-only
// response is where we will store our response
void execnr(const std::array<uint8_t, BUFFER_SIZE> &request,
            std::array<uint8_t, BUFFER_SIZE> &response, size_t &write_offset) {
  // find all questions
  Slime::Header header{};
  uint16_t num_questions = complete_header(response, header);
  size_t read_offset = HEADER_SIZE;
  std::vector<Slime::Question> questions{};
  questions.reserve(num_questions);
  find_all_questions(questions, request, num_questions, read_offset);
  populate_response(response, write_offset, questions);
}

void read_from(int &udpSocket, struct sockaddr_in &clientAddress) {
  ssize_t bytesRead;
  std::array<char, BUFFER_SIZE> buffer{};
  socklen_t clientAddrLen = sizeof(clientAddress);
  // empty buffer
  while (true) {
    // Receive data
    bytesRead = recvfrom(udpSocket, buffer.data(), buffer.size(), 0,
                         reinterpret_cast<struct sockaddr *>(&clientAddress),
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
    bool recursive = check_if_rd(request);
    // make the response contain the same header as request
    std::array<uint8_t, BUFFER_SIZE> response = request;
    size_t write_offset{HEADER_SIZE};
    if (recursive) {
      execr(request, response, write_offset);
    } else {
      execnr(request, response, write_offset);
    }
    if (sendto(udpSocket, response.data(), write_offset, 0,
               reinterpret_cast<struct sockaddr *>(&clientAddress),
               sizeof(clientAddress)) == -1) {
      perror("Failed to send buffer");
    }
  }
}
} // namespace Slime
