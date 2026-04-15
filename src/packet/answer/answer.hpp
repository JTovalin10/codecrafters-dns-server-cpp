#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../utils/shared_enums.hpp"

namespace Slime {

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
  uint32_t ttl;
  uint16_t rdlength;
  std::vector<uint8_t> data;
};

// All Answer numeric fields (type, _class, ttl, rdlength) are stored in HOST
// byte order. Caller owns htons/htonl/ntohs/ntohl — call ntohs/ntohl on these
// fields after reading an Answer off the wire, and htons/htonl on them before
// writing it back out, otherwise values will be wrong.
//
// Example:
//   Answer ans{};
//   set_ans_name(ans, "codecrafters.io");
//   set_ans_type(ans, Slime::records::A);
//   set_class(ans, Slime::classes::IN);
//   set_ttl(ans, 60);
//   set_data(ans, {8, 8, 8, 8});
//   // before serializing to the wire:
//   ans.type = htons(ans.type);
//   ans._class = htons(ans._class);
//   ans.ttl = htonl(ans.ttl);
//   ans.rdlength = htons(ans.rdlength);

// parses the string for '.' and writes it in network notation. Also calls
// set_rdlength for you. name: the regular string ex: codecrafters.io
// Example: set_ans_name(ans, "codecrafters.io");
void set_ans_name(Slime::Answer& ans, const std::string& name);

// type is stored in host byte order. Caller must htons(ans.type) before
// writing to the wire or it will be wrong.
// Example: set_ans_type(ans, Slime::records::A);
void set_ans_type(Slime::Answer& ans, Slime::records value);

// class is stored in host byte order. Caller must htons(ans._class) before
// writing to the wire or it will be wrong.
// Example: set_class(ans, Slime::classes::IN);
void set_class(Slime::Answer& ans, Slime::classes value);

// sets ttl to the default value in host byte order. Caller must htonl(ans.ttl)
// before writing to the wire or it will be wrong.
// Example: set_ttl(ans);
void set_ttl(Slime::Answer& ans);
// sets ttl to an explicit value in host byte order. Caller must htonl(ans.ttl)
// before writing to the wire or it will be wrong.
// Example: set_ttl(ans, 60);
void set_ttl(Slime::Answer& ans, uint32_t value);

// decrements ttl by 1. ttl must be in host byte order (ntohl it after reading
// from the wire) or result will be wrong.
// Example: decrement_ttl(ans);
void decrement_ttl(Slime::Answer& ans);

// sets rdlength to ans.data.size() in host byte order. Caller must
// htons(ans.rdlength) before writing to the wire or it will be wrong.
// Example: set_rdlength(ans);
void set_rdlength(Slime::Answer& ans);

// sets data to a default value and updates rdlength.
// Example: set_data(ans);
void set_data(Slime::Answer& ans);
// sets data to addr bytes and updates rdlength.
// Example: set_data(ans, {8, 8, 8, 8});
void set_data(Slime::Answer& ans, const std::vector<uint8_t>& addr);

// size in bytes of the answer section on the wire:
// name + type(2) + class(2) + ttl(4) + rdlength(2) + data
// Example: size_t n = get_ans_size(ans);
size_t get_ans_size(const Slime::Answer& ans);
}  // namespace Slime
