#pragma once

#include <cstdint>

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
  char* name;
  uint16_t type;
  uint16_t _class;
};

};  // namespace Slime
