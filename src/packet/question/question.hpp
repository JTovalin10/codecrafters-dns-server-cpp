#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "../utils/shared_enums.hpp"

/**
 * name: domain name represented as a sequence of labels
 * ex: google.com = \x06google\x03com\0x00 (null byte) where each \x.. is the
 * length and also represents the .
 *
 * type: the type of record, we usually deal with 1 and 5
 * Type, Value, Meaning
 * A, 1, a hot address
 * NS, 2, an authority name server
 * MD, 3, a mail destination (use MX)
 * MF, 4, A mail forwarder (use MX)
 * CNAME, 5, the canonical name for an alias
 * SOA, 6, marks teh start of a zone of authority
 * MB, 7, a mailbox domain name (experimental)
 * MG, 8, a mail group member (experimental)
 * MR, 9, a mail rename domain name (experimental)
 * NULL, 10, a null RR (experimental)
 * WKS, 11, a well known serveice description
 * PTR, 12, a domain name pointer
 * HINFO, 14, mailbox or mail list information
 * MX, 15, mail exchange
 * TXT, 16, text strings
 *
 * class: usually set to 1
 * type, Value, Meaning
 * IN, 1, the internet
 * CS, 2, the CSNET calss (obsolete)
 * CH, 3, the choas class
 * HS, 4, Hesiod
 *
 */

namespace Slime {

struct Question {
  std::vector<uint8_t> name;
  uint16_t type;
  uint16_t _class;
};

// size in bytes of the question section on the wire:
// name (length-prefixed labels + terminating 0) + type + class
size_t get_ques_size(const Question& ques);

// parses the string for '.' and writes it in network notation
// name: the regular string ex: codecrafters.io
void set_que_name(Question& que, const std::string& name);
std::vector<uint8_t> get_que_name(const Question& que);

// DO NOT CALL HTONS ON VALUE
void set_que_type(Question& que, Slime::records value);
Slime::records get_que_type(const Question& que);

// DO NOT CALL HTONS ON VALUE
void set_que_class(Question& que, Slime::classes value);
Slime::classes get_que_class(const Question& que);

};  // namespace Slime
