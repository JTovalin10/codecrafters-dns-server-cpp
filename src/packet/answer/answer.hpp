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
  uint32_t TTL;
  uint16_t RDLENGTH;
  std::vector<uint8_t> data;
};

// sets the name, it will parse the string for the . and write it in network
// notion. also calls set_rdlength for you
// name: the regular string ex: codecrafters.io
void set_ans_name(Slime::Answer& ans, const std::string& name);

// sets the type, DO NOT CALL HTONS ON VALUE
void set_ans_type(Slime::Answer& ans, Slime::records value);

// sets the class, DO NOT CALL HTONS ON VALUE
void set_class(Slime::Answer& ans, Slime::classes value);

void set_ttl(Slime::Answer& ans);
void set_ttl(Slime::Answer& ans, uint32_t value);

void decrement_ttl(Slime::Answer& ans);

void set_rdlength(Slime::Answer& ans);

void set_data(Slime::Answer& ans);
void set_data(Slime::Answer& ans, const std::vector<uint8_t>& addr);

// size in bytes of the answer section on the wire:
// name + type(2) + class(2) + TTL(4) + RDLENGTH(2) + data
size_t get_ans_size(const Slime::Answer& ans);
}  // namespace Slime
