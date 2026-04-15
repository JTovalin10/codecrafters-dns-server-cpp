#include "header.hpp"

#include <stdexcept>

namespace Slime {

namespace {
const uint16_t QR_INDEX = 15;
const uint16_t OPCODE_INDEX = 11;
const uint16_t AA_INDEX = 10;
const uint16_t TC_INDEX = 9;
const uint16_t RD_INDEX = 8;
const uint16_t RA_INDEX = 7;
const uint16_t Z_INDEX = 4;
const uint16_t RCODE_INDEX = 0;
const uint16_t FOUR_MASK = 0x0f;
const uint16_t THREE_MASK = 0x7;
const uint16_t THREE_BIT_MAX = 7;
const uint16_t FOUR_BIT_MAX = 15;
const uint16_t OPCODE_MASK = 0x7800;
}  // namespace

void set_pid(Header& header, uint16_t value) { header.pid = value; }
uint16_t get_pid(const Header& header) { return header.pid; }

std::size_t get_header_size(const Header& header) { return sizeof(header); }

uint16_t get_qdcount(const Header& header) { return header.qdcount; }
uint16_t get_ancount(const Header& header) { return header.ancount; }
uint16_t get_nscount(const Header& header) { return header.nscount; }
uint16_t get_arcount(const Header& header) { return header.arcount; }

void increment_qdcount(Header& header) { header.qdcount += 1; }
void increment_ancount(Header& header) { header.ancount += 1; }
void increment_nscount(Header& header) { header.nscount += 1; }
void increment_arcount(Header& header) { header.arcount += 1; }

uint16_t get_flags_bit(const Header& header, uint16_t index) {
  return (header.flags >> index) & 1;
}

void set_flags_bit(Header& header, uint16_t value, uint16_t index) {
  if (value == 1) {
    header.flags |= (1 << index);
  } else {
    header.flags &= ~(1 << index);
  }
}

uint8_t get_qr(const Header& header) { return get_flags_bit(header, QR_INDEX); }

uint8_t get_opcode(const Header& header) {
  return (header.flags >> OPCODE_INDEX) & FOUR_MASK;
}

uint8_t get_aa(const Header& header) { return get_flags_bit(header, AA_INDEX); }

uint8_t get_tc(const Header& header) { return get_flags_bit(header, TC_INDEX); }

uint8_t get_rd(const Header& header) { return get_flags_bit(header, RD_INDEX); }

uint8_t get_ra(const Header& header) { return get_flags_bit(header, RA_INDEX); }

uint8_t get_z(const Header& header) {
  return (header.flags >> Z_INDEX) & THREE_MASK;
}

uint8_t get_rcode(const Header& header) {
  return (header.flags >> RCODE_INDEX) & FOUR_MASK;
}

void set_qr(Header& header, QR value) {
  set_flags_bit(header, static_cast<uint16_t>(value), QR_INDEX);
}

void set_aa(Header& header, AA value) {
  set_flags_bit(header, static_cast<uint16_t>(value), AA_INDEX);
}

void set_tc(Header& header, TC value) {
  set_flags_bit(header, static_cast<uint16_t>(value), TC_INDEX);
}

void set_rd(Header& header, RD value) {
  set_flags_bit(header, static_cast<uint16_t>(value), RD_INDEX);
}

void set_ra(Header& header, RA value) {
  set_flags_bit(header, static_cast<uint16_t>(value), RA_INDEX);
}

void set_z(Header& header, uint16_t value) {
  if (value > THREE_BIT_MAX) {
    throw std::out_of_range("set_z: value must be 0-7 (3 bits)");
  }
  header.flags = (header.flags & ~(THREE_MASK << Z_INDEX)) | (value << Z_INDEX);
}

void set_rcode(Header& header, RCODE value) {
  header.flags = (header.flags & ~(FOUR_MASK << RCODE_INDEX)) |
                 (static_cast<uint16_t>(value) << RCODE_INDEX);
}

void set_opcode(Header& header, OPCODE value) {
  header.flags = (header.flags & ~OPCODE_MASK) |
                 (static_cast<uint16_t>(value) << OPCODE_INDEX);
}

}  // namespace Slime
