#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "../shared_enums.hpp"

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

// All Question fields are stored in HOST byte order. Caller owns htons/ntohs
// — call ntohs on que.type and que._class after reading the Question off the
// wire, and htons on them before writing back out, otherwise values will be
// wrong.
//
// Example:
//   Question que{};
//   set_que_name(que, "codecrafters.io");
//   set_que_type(que, Slime::records::A);
//   set_que_class(que, Slime::classes::IN);
//   // before serializing to the wire:
//   que.type = htons(que.type);
//   que._class = htons(que._class);

// size in bytes of the question section on the wire:
// name (length-prefixed labels + terminating 0) + type + class
// Example: size_t n = get_ques_size(que);
size_t get_ques_size(const Question& ques);

// passes in the already network notation to move into the thing.
// Example: set_que_name(que, encode_name("codecrafters.io"));
void set_que_name(Question& que, std::vector<uint8_t>&& name);
// returns the encoded name bytes (length-prefixed labels + null terminator).
// Example: std::vector<uint8_t> n = get_que_name(que);
std::vector<uint8_t> get_que_name(const Question& que);

// type is stored in host byte order. Caller must htons(que.type) before
// writing the Question to the wire or it will be wrong.
// Example: set_que_type(que, Slime::records::A);
void set_que_type(Question& que, Slime::records value);
// type must be in host byte order (ntohs it after reading from the wire).
// Example: Slime::records t = get_que_type(que);
Slime::records get_que_type(const Question& que);

// class is stored in host byte order. Caller must htons(que._class) before
// writing the Question to the wire or it will be wrong.
// Example: set_que_class(que, Slime::classes::IN);
void set_que_class(Question& que, Slime::classes value);
// class must be in host byte order (ntohs it after reading from the wire).
// Example: Slime::classes c = get_que_class(que);
Slime::classes get_que_class(const Question& que);

};  // namespace Slime
