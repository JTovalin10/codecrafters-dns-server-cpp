#pragma once

#include <cstdint>

#include "header.hpp"

namespace Slime {

enum class AA : uint8_t {
  NOT_AUTHORITATIVE = 0,
  AUTHORITATIVE = 1,
};

enum class OPCODE : uint8_t {
  QUERY = 0,
  IQUERY = 1,
  STATUS = 2,
  NOTIFY = 4,
  UPDATE = 5,
  DSO = 6,
};

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

void set_qr(Header& header, uint16_t value);
void set_aa(Header& header, AA value);
void set_tc(Header& header, uint16_t value);
void set_rd(Header& header, uint16_t value);
void set_ra(Header& header, uint16_t value);
void set_z(Header& header, uint16_t value);
void set_rcode(Header& header, uint16_t value);
void set_opcode(Header& header, OPCODE value);

};  // namespace Slime
