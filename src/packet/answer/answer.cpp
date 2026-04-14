#include "answer.hpp"

#include <arpa/inet.h>

#include <algorithm>

#include "../utils/my_utils.hpp"

namespace Slime {

namespace {
const uint32_t DEFAULT_TTL = 60;
const std::vector<uint8_t> DATA_DEFAULT = {0x08, 0x08, 0x08, 0x08};
}  // namespace

void set_ans_name(Slime::Answer& ans, const std::string& name) {
  ans.name = Slime::encode_name(name);
  set_rdlength(ans);
}

void set_ans_type(Slime::Answer& ans, Slime::records value) {
  ans.type = htons(static_cast<uint16_t>(value));
}

void set_class(Slime::Answer& ans, Slime::classes value) {
  ans._class = htons(static_cast<uint16_t>(value));
}

void set_ttl(Slime::Answer& ans) { ans.TTL = DEFAULT_TTL; }
void set_ttl(Slime::Answer& ans, uint32_t value) { ans.TTL = value; }

void decrement_ttl(Slime::Answer& ans) {
  ans.TTL = std::max(static_cast<uint32_t>(0), ans.TTL - 1);
}

void set_rdlength(Slime::Answer& ans) { ans.RDLENGTH = ans.data.size(); }

void set_data(Slime::Answer& ans) {
  ans.data = DATA_DEFAULT;
  set_rdlength(ans);
}
void set_data(Slime::Answer& ans, const std::vector<uint8_t>& addr) {
  ans.data = addr;
  set_rdlength(ans);
}

size_t get_ans_size(const Slime::Answer& ans) {
  return ans.name.size() + sizeof(ans.type) + sizeof(ans._class) +
         sizeof(ans.TTL) + sizeof(ans.RDLENGTH) + ans.data.size();
}
}  // namespace Slime
