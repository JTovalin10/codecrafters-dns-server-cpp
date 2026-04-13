#pragma once

#include <cstddef>
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

enum class QR : uint8_t {
  QUERY = 0,
  RESPONSE = 1,
};

enum class AA : uint8_t {
  NOT_AUTHORITATIVE = 0,
  AUTHORITATIVE = 1,
};

enum class TC : uint8_t {
  NOT_TRUNCATED = 0,
  TRUNCATED = 1,
};

enum class RD : uint8_t {
  NOT_DESIRED = 0,
  DESIRED = 1,
};

enum class RA : uint8_t {
  NOT_AVAILABLE = 0,
  AVAILABLE = 1,
};

enum class OPCODE : uint8_t {
  QUERY = 0,
  IQUERY = 1,
  STATUS = 2,
  NOTIFY = 4,
  UPDATE = 5,
  DSO = 6,
};

enum class RCODE : uint8_t {
  NO_ERROR = 0,
  FORMAT_ERROR = 1,
  SERVER_FAILURE = 2,
  NAME_ERROR = 3,
  NOT_IMPLEMENTED = 4,
  REFUSED = 5,
  YX_DOMAIN = 6,
  YX_RR_SET = 7,
  NX_RR_SET = 8,
  NOT_AUTH = 9,
  NOT_ZONE = 10,
  DSO_TYPE_NI = 11,
};

// sets the packet id, DO NOT CALL HTONS on value
void set_pid(Header& header, uint16_t value);
uint16_t get_pid(const Header& header);

// size in bytes of the header section on the wire (always 12)
std::size_t get_header_size(const Header& header);

// count fields are stored in network byte order, re-applies ntohs for you
uint16_t get_qdcount(const Header& header);
uint16_t get_ancount(const Header& header);
uint16_t get_nscount(const Header& header);
uint16_t get_arcount(const Header& header);

// increments the count by 1, DO NOT CALL HTONS, handled internally
void increment_qdcount(Header& header);
void increment_ancount(Header& header);
void increment_nscount(Header& header);
void increment_arcount(Header& header);

uint16_t get_flags_bit(const Header& header, uint16_t index);
void set_flags_bit(Header& header, uint16_t value, uint16_t index);

uint8_t get_qr(const Header& header);
uint8_t get_opcode(const Header& header);
uint8_t get_aa(const Header& header);
uint8_t get_tc(const Header& header);
uint8_t get_rd(const Header& header);
uint8_t get_ra(const Header& header);
uint8_t get_z(const Header& header);
uint8_t get_rcode(const Header& header);

void set_qr(Header& header, QR value);
void set_aa(Header& header, AA value);
void set_tc(Header& header, TC value);
void set_rd(Header& header, RD value);
void set_ra(Header& header, RA value);
void set_z(Header& header, uint16_t value);
void set_rcode(Header& header, RCODE value);
void set_opcode(Header& header, OPCODE value);

};  // namespace Slime
