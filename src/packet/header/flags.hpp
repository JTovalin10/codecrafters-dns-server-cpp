#pragma once

#include <cstdint>
#include <stdexcept>

#include "header.hpp"

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
const uint16_t FOUR_MASK = 0x0f;
const uint16_t THREE_MASK = 0x7;
const uint16_t THREE_BIT_MAX = 7;
const uint16_t FOUR_BIT_MAX = 15;
}  // namespace PACKET_TOOLS

namespace SET_OPCODE_TOOLS {
const uint16_t QUERY = 0;
const uint16_t IQUERY = 1;
const uint16_t STATUS = 2;
const uint16_t NOTIFY = 4;
const uint16_t UPDATE = 5;
const uint16_t DSO = 6;

const uint16_t MASK = 0x7800;
}  // namespace SET_OPCODE_TOOLS

inline uint16_t get_flags_bit(const Header& header, const uint16_t index) {
  return (header.flags >> index) & 1;
}

inline void set_flags_bit(Header& header, uint16_t value,
                          const uint16_t index) {
  if (value == 1) {
    header.flags |= (1 << index);
  } else {
    header.flags &= ~(1 << index);
  }
}

inline uint8_t get_qr(const Header& header) {
  return get_flags_bit(header, PACKET_TOOLS::QR_INDEX);
}

inline uint8_t get_opcode(const Header& header) {
  return (header.flags >> PACKET_TOOLS::OPCODE_INDEX) & PACKET_TOOLS::FOUR_MASK;
}

inline uint8_t get_aa(const Header& header) {
  return get_flags_bit(header, PACKET_TOOLS::AA_INDEX);
}

inline uint8_t get_tc(const Header& header) {
  return get_flags_bit(header, PACKET_TOOLS::TC_INDEX);
}

inline uint8_t get_rd(const Header& header) {
  return get_flags_bit(header, PACKET_TOOLS::RD_INDEX);
}

inline uint8_t get_ra(const Header& header) {
  return get_flags_bit(header, PACKET_TOOLS::RA_INDEX);
}

inline uint8_t get_z(const Header& header) {
  return (header.flags >> PACKET_TOOLS::Z_INDEX) & PACKET_TOOLS::THREE_MASK;
}

inline uint8_t get_rcode(const Header& header) {
  return (header.flags >> PACKET_TOOLS::RCODE_INDEX) & PACKET_TOOLS::FOUR_MASK;
}

inline void set_qr(Header& header, uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("set_qr: value must be 0 or 1");
  }
  set_flags_bit(header, value, PACKET_TOOLS::QR_INDEX);
}

inline void set_aa(Header& header, uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("set_aa: value must be 0 or 1");
  }
  set_flags_bit(header, value, PACKET_TOOLS::AA_INDEX);
}

inline void set_tc(Header& header, uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("set_tc: value must be 0 or 1");
  }
  set_flags_bit(header, value, PACKET_TOOLS::TC_INDEX);
}

inline void set_rd(Header& header, uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("set_rd: value must be 0 or 1");
  }
  set_flags_bit(header, value, PACKET_TOOLS::RD_INDEX);
}

inline void set_ra(Header& header, uint16_t value) {
  if (value != 0 && value != 1) {
    throw std::out_of_range("set_ra: value must be 0 or 1");
  }
  set_flags_bit(header, value, PACKET_TOOLS::RA_INDEX);
}

inline void set_z(Header& header, uint16_t value) {
  if (value > PACKET_TOOLS::THREE_BIT_MAX) {
    throw std::out_of_range("set_z: value must be 0-7 (3 bits)");
  }
  header.flags =
      (header.flags & ~(PACKET_TOOLS::THREE_MASK << PACKET_TOOLS::Z_INDEX)) |
      (value << PACKET_TOOLS::Z_INDEX);
}

inline void set_rcode(Header& header, uint16_t value) {
  if (value > PACKET_TOOLS::FOUR_BIT_MAX) {
    throw std::out_of_range("set_rcode: value must be 0-15 (4 bits)");
  }
  header.flags =
      (header.flags & ~(PACKET_TOOLS::FOUR_MASK << PACKET_TOOLS::RCODE_INDEX)) |
      (value << PACKET_TOOLS::RCODE_INDEX);
}

inline void set_opcode(Header& header, uint16_t value) {
  if (value != SET_OPCODE_TOOLS::QUERY && value != SET_OPCODE_TOOLS::IQUERY &&
      value != SET_OPCODE_TOOLS::STATUS && value != SET_OPCODE_TOOLS::NOTIFY &&
      value != SET_OPCODE_TOOLS::UPDATE && value != SET_OPCODE_TOOLS::DSO) {
    return;
  }
  header.flags = (header.flags & ~SET_OPCODE_TOOLS::MASK) |
                 (value << PACKET_TOOLS::OPCODE_INDEX);
}

};  // namespace Slime
