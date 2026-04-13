#include "answer.hpp"

#include <arpa/inet.h>

#include <algorithm>

namespace Slime {

namespace {
const uint32_t DEFAULT_TTL = 60;
const std::vector<uint8_t> DATA_DEFAULT = {0x08, 0x08, 0x08, 0x08};

std::vector<uint8_t> encode_name(const std::string& name) {
  std::vector<uint8_t> res;
  res.reserve(name.size() + 2);
  // add a temp for the size variable
  size_t len_pos = 0;
  res.push_back(0);
  for (char chr : name) {
    if (chr == '.') {
      res[len_pos] = res.size() - len_pos + 1;
      len_pos = res.size();
      res.push_back(0);
    } else {
      res.push_back(chr);
    }
  }
  // fill in the last one
  res[len_pos] = res.size() - len_pos + 1;
  res.push_back('\0');
  res.shrink_to_fit();
  return res;
}
}  // namespace

void set_name(Slime::Answer& ans, const std::string& name) {
  ans.name = encode_name(name);
  set_rdlength(ans);
}

void set_type(Slime::Answer& ans, Slime::records value) {
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

void set_ans_data(Slime::Answer& ans) { ans.data = DATA_DEFAULT; }
void set_ans_data(Slime::Answer& ans, const std::vector<uint8_t>& addr) {
  ans.data = addr;
}
}  // namespace Slime
