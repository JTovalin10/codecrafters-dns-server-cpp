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
 * qdcount: number of questions in the question section
 * ancount: number of records in the answer section
 * nscount: number of records in the authority section
 * arcount: the number of records in the Additional section
 */

namespace Slime {

struct Header {
  uint16_t pid;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
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

// All Header fields (pid, flags, qdcount, ancount, nscount, arcount) are
// stored in HOST byte order. Caller owns htons/ntohs — call ntohs on every
// field after reading a Header off the wire, and htons on every field before
// writing it back out, otherwise values will be wrong.
//
// Example:
//   Header header{};
//   std::memcpy(&header, buffer.data(), sizeof(Header));
//   header.pid = ntohs(header.pid);
//   header.flags = ntohs(header.flags);
//   header.qdcount = ntohs(header.qdcount);
//   header.ancount = ntohs(header.ancount);
//   header.nscount = ntohs(header.nscount);
//   header.arcount = ntohs(header.arcount);
//
//   set_qr(header, QR::RESPONSE);
//   set_opcode(header, OPCODE::QUERY);
//   set_rcode(header, RCODE::NO_ERROR);
//   increment_ancount(header);
//
//   // before serializing back to the wire:
//   header.pid = htons(header.pid);
//   header.flags = htons(header.flags);
//   header.qdcount = htons(header.qdcount);
//   header.ancount = htons(header.ancount);
//   header.nscount = htons(header.nscount);
//   header.arcount = htons(header.arcount);
//   std::memcpy(buffer.data(), &header, sizeof(Header));

// pid is stored in host byte order. Caller must htons(header.pid) before
// writing to the wire or it will be wrong.
// Example: set_pid(header, 1234);
void set_pid(Header &header, uint16_t value);
// pid must be in host byte order (ntohs it after reading from the wire).
// Example: uint16_t id = get_pid(header);
uint16_t get_pid(const Header &header);

// size in bytes of the header section on the wire (always 12). No byte-order
// concern. Example: int offset = get_header_size(header);
std::size_t get_header_size(const Header &header);

// count must be in host byte order (ntohs it after reading from the wire).
// Example: uint16_t n = get_qdcount(header);
uint16_t get_qdcount(const Header &header);
// count must be in host byte order (ntohs it after reading from the wire).
// Example: uint16_t n = get_ancount(header);
uint16_t get_ancount(const Header &header);
// count must be in host byte order (ntohs it after reading from the wire).
// Example: uint16_t n = get_nscount(header);
uint16_t get_nscount(const Header &header);
// count must be in host byte order (ntohs it after reading from the wire).
// Example: uint16_t n = get_arcount(header);
uint16_t get_arcount(const Header &header);

// count must be in host byte order. Caller must htons(header.qdcount) before
// writing to the wire or it will be wrong.
// Example: increment_qdcount(header);
void increment_qdcount(Header &header);
/**
 * sets the qdcount to what was given. Caller must call htons(header.qdcount)
 */
void set_qdcount(Header &header, uint16_t val);
// count must be in host byte order. Caller must htons(header.ancount) before
// writing to the wire or it will be wrong.
// Example: increment_ancount(header);
void increment_ancount(Header &header);
// count must be in host byte order. Caller must htons(header.nscount) before
// writing to the wire or it will be wrong.
// Example: increment_nscount(header);
void increment_nscount(Header &header);
// count must be in host byte order. Caller must htons(header.arcount) before
// writing to the wire or it will be wrong.
// Example: increment_arcount(header);
void increment_arcount(Header &header);

// All flag getters and setters below operate on header.flags in HOST byte
// order — same rule as the full-header example above.

// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint16_t bit = get_flags_bit(header, 15);
uint16_t get_flags_bit(const Header &header, uint16_t index);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_flags_bit(header, 1, 15);
void set_flags_bit(Header &header, uint16_t value, uint16_t index);

// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t qr = get_qr(header);
uint8_t get_qr(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t op = get_opcode(header);
uint8_t get_opcode(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t aa = get_aa(header);
uint8_t get_aa(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t tc = get_tc(header);
uint8_t get_tc(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t rd = get_rd(header);
uint8_t get_rd(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t ra = get_ra(header);
uint8_t get_ra(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t z = get_z(header);
uint8_t get_z(const Header &header);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: uint8_t rc = get_rcode(header);
uint8_t get_rcode(const Header &header);

// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_qr(header, QR::RESPONSE);
void set_qr(Header &header, QR value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_aa(header, AA::AUTHORITATIVE);
void set_aa(Header &header, AA value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_tc(header, TC::NOT_TRUNCATED);
void set_tc(Header &header, TC value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_rd(header, RD::DESIRED);
void set_rd(Header &header, RD value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_ra(header, RA::AVAILABLE);
void set_ra(Header &header, RA value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_z(header, 0);  // value must be 0-7 (3 bits)
void set_z(Header &header, uint16_t value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_rcode(header, RCODE::NO_ERROR);
void set_rcode(Header &header, RCODE value);
// flags must be in host byte order. Caller owns ntohs/htons around this call.
// Example: set_opcode(header, OPCODE::QUERY);
void set_opcode(Header &header, OPCODE value);

}; // namespace Slime
