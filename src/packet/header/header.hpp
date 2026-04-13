#pragma once

#include <cstdint>

/**
 * each are 16 bits for a total of 12 bytes
 * pid = A random ID assigned to query packets. Response packets must reply with
 * the same PID
 * flags {
 * bits, name, description
 * 1, QR, 1 for a reply packet, 0 for a question packet
 *
 * 4, OPCODE, specifies the kind of query in a Message
 *
 * 1, AA, 1 if the responding server "owns" the domain queried
 *
 * 1, TC, 1 if the message is larger than 512 bytes (should default to 0 in UDP
 * responses)
 *
 * 1, RD, Sender sets this to 1 if the server should recursively resolve this
 * query otherwise iterative (0)
 *
 * 1, RA, server sets this to 1 to indicate that recursion is available
 *
 * 3, Z, used by DNSSEC queries. At inception, it was reserved for future use
 *
 * 4, RCODE, Response code indicating the status of the response
 * }
 * QDCOUNT: number of questions in the question section
 * ANCOUNT: number of records in the answer section
 * NSCOUNT: number of records in the authority section
 * ARCOUNT: the number of records in the Additional section
 */

namespace Slime {

struct Header {
  uint16_t pid;
  uint16_t flags;
  uint16_t QDCOUNT;
  uint16_t ANCOUNT;
  uint16_t NSCOUNT;
  uint16_t ARCOUNT;
};

};  // namespace Slime
