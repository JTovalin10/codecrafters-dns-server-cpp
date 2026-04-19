#include "helper.hpp"

#include <iostream>
#include <netinet/in.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "shared_vars.hpp"

namespace Slime {

// ─── Name encoding ───────────────────────────────────────────────────────────

std::vector<uint8_t> encode_name(const std::string &name) {
  std::vector<uint8_t> res;
  res.reserve(name.size() + 2);
  size_t len_pos = 0;
  res.push_back(0);
  for (char chr : name) {
    if (chr == '.') {
      res[len_pos] = res.size() - len_pos - 1;
      len_pos = res.size();
      res.push_back(0);
    } else {
      res.push_back(chr);
    }
  }
  res[len_pos] = res.size() - len_pos - 1;
  res.push_back('\0');
  res.shrink_to_fit();
  return res;
}

const uint8_t IS_POINTER = 0xC0;
const uint8_t GET_SIZE = 0x3f;
const uint8_t ADD_BYTE = 8;

std::pair<std::vector<uint8_t>, size_t>
find_network_format_name(std::array<uint8_t, BUFFER_SIZE> arr, size_t offset) {
  std::vector<uint8_t> res{};
  res.reserve(BUFFER_SIZE);
  std::unordered_set<size_t> visited{};
  const size_t soffset = offset;
  size_t wire_bytes{};
  bool followed_pointer = false;

  while (true) {
    const uint8_t byte = arr[offset];
    if (byte == '\0') {
      if (!followed_pointer) {
        wire_bytes = offset - soffset + 1;
      }
      break;
    }

    if ((byte & IS_POINTER) == IS_POINTER) {
      if (visited.find(offset) != visited.end()) {
        throw std::runtime_error("Infinite loop");
      }
      if (!followed_pointer) {
        wire_bytes = offset - soffset + 2;
        followed_pointer = true;
      }
      visited.insert(offset);
      offset = (((byte & GET_SIZE) << ADD_BYTE) | arr[offset + 1]);
    } else {
      uint8_t length = byte;
      res.push_back(length);
      offset++;
      for (size_t i = 0; i < length; i++) {
        res.push_back(arr[offset++]);
      }
    }
  }
  res.push_back('\0');
  res.shrink_to_fit();
  return {res, wire_bytes};
}

// ─── Header helpers ──────────────────────────────────────────────────────────

void set_header_to_host(Header &header) {
  header.pid = ntohs(header.pid);
  header.flags = ntohs(header.flags);
  header.qdcount = ntohs(header.qdcount);
  header.ancount = header.qdcount;
  header.arcount = 0;
  header.nscount = 0;
}

void set_header_to_network(Header &header) {
  header.pid = htons(header.pid);
  header.flags = htons(header.flags);
  header.qdcount = htons(header.qdcount);
  header.ancount = htons(header.ancount);
  header.arcount = htons(header.arcount);
  header.nscount = htons(header.nscount);
}

void populate_header(Header &header) {
  set_qr(header, QR::RESPONSE);
  uint8_t opcode = get_opcode(header);
  if (opcode != 0) {
    set_rcode(header, RCODE::NOT_IMPLEMENTED);
  } else {
    set_rcode(header, RCODE::NO_ERROR);
  }
  set_aa(header, AA::NOT_AUTHORITATIVE);
  set_tc(header, TC::NOT_TRUNCATED);
  set_ra(header, RA::NOT_AVAILABLE);
  set_z(header, 0);
}

uint16_t complete_header(std::array<uint8_t, BUFFER_SIZE> &buffer,
                         Header &header) {
  std::memcpy(&header, buffer.data(), sizeof(header));
  set_header_to_host(header);
  populate_header(header);
  set_header_to_network(header);
  std::memcpy(buffer.data(), &header, sizeof(header));
  return ntohs(header.qdcount);
}

// ─── Question helpers ────────────────────────────────────────────────────────

size_t populate_questions(const std::array<uint8_t, BUFFER_SIZE> &buffer,
                          Question &que, size_t offset) {
  auto [name, moved] = find_network_format_name(buffer, offset);
  set_que_name(que, std::move(name));
  set_que_class(que, classes::IN);
  set_que_type(que, records::A);
  return moved + sizeof(que._class) + sizeof(que.type);
}

void write_question_to_buffer(std::array<uint8_t, BUFFER_SIZE> &buffer,
                              Question &que, size_t offset) {
  const uint16_t type_n = htons(que.type);
  const uint16_t class_n = htons(que._class);
  uint8_t *cursor = buffer.data() + offset;
  std::memcpy(cursor, que.name.data(), que.name.size());
  cursor += que.name.size();
  std::memcpy(cursor, &type_n, sizeof(type_n));
  cursor += sizeof(type_n);
  std::memcpy(cursor, &class_n, sizeof(class_n));
}

void find_all_questions(std::vector<Question> &questions,
                        const std::array<uint8_t, BUFFER_SIZE> &request,
                        size_t num_questions, size_t &read_offset) {
  for (size_t i = 0; i < num_questions; i++) {
    Question que;
    read_offset += populate_questions(request, que, read_offset);
    questions.push_back(que);
  }
}

// ─── Answer helpers ──────────────────────────────────────────────────────────

void populate_answer(Answer &ans, std::vector<uint8_t> &&name) {
  set_ans_name(ans, std::move(name));
  set_ans_type(ans, records::A);
  set_class(ans, classes::IN);
  set_ttl(ans);
  set_data(ans);
}

void copy_answer(Answer &ans, std::vector<uint8_t> &&name, size_t pos,
                 std::array<uint8_t, BUFFER_SIZE> &buf) {
  ans.name = std::move(name);
  uint16_t type_n, class_n, rdlen_n;
  uint32_t ttl_n;
  std::memcpy(&type_n, buf.data() + pos, 2);
  pos += 2;
  std::memcpy(&class_n, buf.data() + pos, 2);
  pos += 2;
  std::memcpy(&ttl_n, buf.data() + pos, 4);
  pos += 4;
  std::memcpy(&rdlen_n, buf.data() + pos, 2);
  pos += 2;

  ans.type = ntohs(type_n);
  ans._class = ntohs(class_n);
  ans.rdlength = ntohs(rdlen_n);
  ans.ttl = ntohl(ttl_n);
  ans.data.assign(buf.data() + pos, buf.data() + pos + ans.rdlength);
}

void write_answer_to_buffer(std::array<uint8_t, BUFFER_SIZE> &buffer,
                            Answer &ans, size_t offset) {
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

void complete_answer(std::array<uint8_t, BUFFER_SIZE> &buffer, Answer &ans,
                     size_t offset, std::vector<uint8_t> name) {
  populate_answer(ans, std::move(name));
  write_answer_to_buffer(buffer, ans, offset);
}

// ─── Response helpers ────────────────────────────────────────────────────────

void populate_response(std::array<uint8_t, BUFFER_SIZE> &response,
                       size_t &write_offset, std::vector<Question> &questions) {
  for (Question &que : questions) {
    write_question_to_buffer(response, que, write_offset);
    write_offset += get_ques_size(que);
  }
  for (Question &que : questions) {
    Answer ans;
    complete_answer(response, ans, write_offset, que.name);
    write_offset += get_ans_size(ans);
  }
}

int execf(const std::array<uint8_t, BUFFER_SIZE> &request,
          std::array<uint8_t, BUFFER_SIZE> &response, size_t &write_offset,
          int &fSocket, struct sockaddr_in &fAddr, socklen_t fAddrLen) {
  Header header{};
  uint16_t num_questions = complete_header(response, header);
  size_t read_offset = HEADER_SIZE;
  std::vector<Question> questions{};
  questions.reserve(num_questions);
  find_all_questions(questions, request, num_questions, read_offset);
  // we need to send one question at a time
  std::vector<Answer> answers{};
  answers.reserve(num_questions);
  // init the header for each question packet
  Header fwd_header{};
  std::memcpy(&fwd_header, request.data(), HEADER_SIZE);
  fwd_header.qdcount = ntohs(fwd_header.qdcount);
  set_qdcount(fwd_header, 1);
  fwd_header.qdcount = htons(fwd_header.qdcount);
  for (Question &que : questions) {
    std::array<uint8_t, BUFFER_SIZE> fwd{};
    std::memcpy(fwd.data(), &fwd_header, HEADER_SIZE);
    write_question_to_buffer(fwd, que, HEADER_SIZE);

    if (sendto(fSocket, fwd.data(), fwd.size(), 0,
               reinterpret_cast<struct sockaddr *>(&fAddr), fAddrLen) < 0) {
      std::cerr << "Failed to send data\n";
      return -1;
    }

    std::array<uint8_t, BUFFER_SIZE> res{};
    if (recvfrom(fSocket, res.data(), res.size(), 0,
                 reinterpret_cast<struct sockaddr *>(&fAddr), &fAddrLen) < 0) {
      std::cerr << "Failed to receive data\n";
      return -1;
    }

    // now we need to parse the data to find the answer
    size_t offset = HEADER_SIZE + get_ques_size(que);
    auto [name, name_size] = find_network_format_name(res, offset);
    Answer ans{};
    copy_answer(ans, std::move(name), name_size + offset, res);
    answers.push_back(ans);

    // now we want to write the question to the response
    write_question_to_buffer(response, que, write_offset);
    write_offset += get_ques_size(que);
  }

  // now we want to write the answers to them
  for (Answer &ans : answers) {
    write_answer_to_buffer(response, ans, write_offset);
    write_offset += get_ans_size(ans);
  }
  return 0;
}

void execnf(const std::array<uint8_t, BUFFER_SIZE> &request,
            std::array<uint8_t, BUFFER_SIZE> &response, size_t &write_offset) {
  Header header{};
  uint16_t num_questions = complete_header(response, header);
  size_t read_offset = HEADER_SIZE;
  std::vector<Question> questions{};
  questions.reserve(num_questions);
  find_all_questions(questions, request, num_questions, read_offset);
  populate_response(response, write_offset, questions);
}

} // namespace Slime
