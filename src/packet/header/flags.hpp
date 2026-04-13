#pragma once

#include <cstdint>

#include "header.hpp"

namespace Slime {

class flags_bg {
 public:
  explicit flags_bg(const struct Header& header);

  uint8_t get_qr();
  uint8_t get_opcode();
  uint8_t get_aa();
  uint8_t get_tc();
  uint8_t get_rd();
  uint8_t get_ra();
  uint8_t get_z();
  uint8_t get_rcode();

  void set_qr(uint16_t value);

  void set_opcode(uint16_t value);

  void set_aa(uint16_t value);

  void set_tc(uint16_t value);

  void set_rd(uint16_t value);

  void set_ra(uint16_t value);

  void set_z(uint16_t value);

  void set_rcode(uint16_t value);

  void reset(const struct Header& header);
  // returns the flags so the user can use them (meant if they modify it)
  uint16_t release() const;

 private:
  uint16_t flags;
  uint16_t get_flags_bit(const uint16_t index);
  void set_value(uint16_t value, const uint16_t index);
};

};  // namespace Slime
