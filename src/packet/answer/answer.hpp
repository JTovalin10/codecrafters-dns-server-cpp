#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace Slime {

namespace TOOLS {
const uint8_t TYPE_LIMIT = 16;
const uint8_t CLASS_LIMIT = 4;
const uint8_t DEFAULT_TTL = 60;
const uint8_t UI8_MASK = 0xFF;
const std::vector<uint8_t> DATA_DEFAULT = {0x08, 0x08, 0x08, 0x08};

std::vector<uint8_t> encode_name(const std::string& name) {
  std::vector<uint8_t> res;
  res.reserve(name.size() + 2);
  // add a temp for the size variable
  size_t len_pos = 0;
  res.push_back(0);
  for (char c : name) {
    if (c == '.') {
      res[len_pos] = res.size() - len_pos + 1;
      len_pos = res.size();
      res.push_back(0);
    } else {
      res.push_back(c);
    }
  }
  // fill in the last one
  res[len_pos] = res.size() - len_pos + 1;
  res.push_back('\0');
  res.shrink_to_fit();
  return res;
}
}  // namespace TOOLS

enum class ans_types : uint8_t {
  A = 1,
  NS = 2,
  MD = 3,
  MF = 4,
  CNAME = 5,
  SOA = 6,
  MB = 7,
  MG = 8,
  MR = 9,
  NULL_ = 10,
  WKS = 11,
  PTR = 12,
  HINFO = 13,
  MINFO = 14,
  MX = 15,
  TXT = 16,
};

/**
  *A               1 a host address

NS              2 an authoritative name server

MD              3 a mail destination (Obsolete - use MX)

MF              4 a mail forwarder (Obsolete - use MX)

CNAME           5 the canonical name for an alias

SOA             6 marks the start of a zone of authority

MB              7 a mailbox domain name (EXPERIMENTAL)

MG              8 a mail group member (EXPERIMENTAL)

MR              9 a mail rename domain name (EXPERIMENTAL)

NULL            10 a null RR (EXPERIMENTAL)

WKS             11 a well known service description

PTR             12 a domain name pointer

HINFO           13 host information

MINFO           14 mailbox or mail list information

MX              15 mail exchange

TXT             16 text strings
  */
struct Answer {
  std::vector<uint8_t> name;
  uint16_t type;
  uint16_t _class;
  uint32_t TTL;
  uint16_t RDLENGTH;
  std::vector<uint8_t> data;
};

void set_name(Slime::Answer& ans, const std::string& name) {
  std::vector<uint8_t> pname = TOOLS::encode_name(name);
  ans.name = std::move(pname);
}

void set_type(Slime::Answer& ans, uint8_t value) {
  if (value > TOOLS::TYPE_LIMIT) {
    throw std::invalid_argument("set_type: value must be between 1-16");
  }
  ans.type = value;
}

void set_class(Slime::Answer& ans, uint8_t value) {
  if (value > TOOLS::CLASS_LIMIT) {
    throw std::invalid_argument("set_class: value must be between 1-4");
  }
  ans._class = value;
}

void set_ttl(Slime::Answer& ans, int value = TOOLS::DEFAULT_TTL) {
  ans.TTL = value;
}

void decrement_ttl(Slime::Answer& ans) {
  ans.TTL = std::max(static_cast<uint32_t>(0), ans.TTL - 1);
}

void set_rdlength(Slime::Answer& ans) { ans.RDLENGTH = ans.data.size(); }

void set_ans_data(Slime::Answer& ans,
                  const std::vector<uint8_t>& addr = TOOLS::DATA_DEFAULT) {
  ans.data = addr;
}
}  // namespace Slime
