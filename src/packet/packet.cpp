#include "packet.hpp"

#include <stdexcept>

namespace Slime {

namespace PACKET_TOOLS {
const uint16_t QR_INDEX = 15;
const uint16_t OPCODE_INDEX = 11;
const uint16_t AA_INDEX = 10;
const uint16_t TC_INDEX = 9;
const uint16_t RD_INDEX = 8;
const uint16_t RA_INDEX = 7;
const uint16_t Z_INDEX = 4;
const uint16_t RCODE_INDEX = 0;
const char FOUR_MASK = 0x0f;
const char THREE_MASK = 0x7;
const uint16_t THREE_BIT_MAX = 7;
const uint16_t FOUR_BIT_MAX = 15;
}  // namespace PACKET_TOOLS

flags_bg::flags_bg(const struct Header& header) : flags(header.flags) {}

uint8_t flags_bg::get_qr() const {
  return get_flags_bit(PACKET_TOOLS::QR_INDEX);
}

uint8_t flags_bg::get_opcode() const {
  return (flags >> PACKET_TOOLS::OPCODE_INDEX) & PACKET_TOOLS::FOUR_MASK;
}

uint8_t flags_bg::get_aa() const {
  return get_flags_bit(PACKET_TOOLS::AA_INDEX);
}

uint8_t flags_bg::get_tc() const {
  return get_flags_bit(PACKET_TOOLS::TC_INDEX);
}

uint8_t flags_bg::get_rd() const {
  return get_flags_bit(PACKET_TOOLS::RD_INDEX);
}

uint8_t flags_bg::get_ra() const {
  return get_flags_bit(PACKET_TOOLS::RA_INDEX);
}

uint8_t flags_bg::get_z() const {
  return (flags >> PACKET_TOOLS::Z_INDEX) & PACKET_TOOLS::THREE_MASK;
}

uint8_t flags_bg::get_rcode() const {
  return (flags >> PACKET_TOOLS::RCODE_INDEX) & PACKET_TOOLS::FOUR_MASK;
}

uint16_t flags_bg::get_flags_bit(const uint16_t index) const {
  return (flags >> index) & 1;
}

void flags_bg::set_value(uint16_t value, const uint16_t index) {
  if (value == 1) {
    flags |= (1 << index);
  } else {
    flags &= ~(1 << index);
  }
}

void flags_bg::set_qr(uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("flags_bg::set_qr: value must be 0 or 1");
  }
  set_value(value, PACKET_TOOLS::QR_INDEX);
}

void flags_bg::set_aa(uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("flags_bg::set_aa: value must be 0 or 1");
  }
  set_value(value, PACKET_TOOLS::AA_INDEX);
}

void flags_bg::set_tc(uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("flags_bg::set_tc: value must be 0 or 1");
  }
  set_value(value, PACKET_TOOLS::TC_INDEX);
}

void flags_bg::set_rd(uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("flags_bg::set_rd: value must be 0 or 1");
  }
  set_value(value, PACKET_TOOLS::RD_INDEX);
}

void flags_bg::set_ra(uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("flags_bg::set_ra: value must be 0 or 1");
  }
  set_value(value, PACKET_TOOLS::RA_INDEX);
}

void flags_bg::set_z(uint16_t value) {
  if (value > PACKET_TOOLS::THREE_BIT_MAX) {
    throw std::out_of_range("flags_bg::set_z: value must be 0-7 (3 bits)");
  }
  flags = (flags & ~(PACKET_TOOLS::THREE_MASK << PACKET_TOOLS::Z_INDEX)) |
          (value << PACKET_TOOLS::Z_INDEX);
}

void flags_bg::set_rcode(uint16_t value) {
  if (value > PACKET_TOOLS::FOUR_BIT_MAX) {
    throw std::out_of_range("flags_bg::set_rcode: value must be 0-15 (4 bits)");
  }
  flags = (flags & ~(PACKET_TOOLS::FOUR_MASK << PACKET_TOOLS::RCODE_INDEX)) |
          (value << PACKET_TOOLS::RCODE_INDEX);
}

namespace SET_OPCODE_TOOLS {
const uint16_t QUERY = 0;
const uint16_t IQUERY = 1;
const uint16_t STATUS = 2;
const uint16_t NOTIFY = 4;
const uint16_t UPDATE = 5;
const uint16_t DSO = 6;

const uint16_t MASK = 0x7800;
}  // namespace SET_OPCODE_TOOLS

void flags_bg::set_opcode(uint16_t value) {
  if (value != SET_OPCODE_TOOLS::QUERY && value != SET_OPCODE_TOOLS::IQUERY &&
      value != SET_OPCODE_TOOLS::STATUS && value != SET_OPCODE_TOOLS::NOTIFY &&
      value != SET_OPCODE_TOOLS::UPDATE && value != SET_OPCODE_TOOLS::DSO) {
    return;
  }
  flags =
      (flags & ~SET_OPCODE_TOOLS::MASK) | (value << PACKET_TOOLS::OPCODE_INDEX);
}

void flags_bg::reset(const struct Header& header) { flags = header.flags; }

uint16_t flags_bg::release() const { return flags; }
};  // namespace Slime
