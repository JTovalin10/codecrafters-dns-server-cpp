#include "my_utils.hpp"

#include <algorithm>
#include <vector>

namespace Slime {

std::vector<uint8_t> encode_name(const std::string& name) {
  std::vector<uint8_t> res;
  res.reserve(name.size() + 2);
  // add a temp for the size variable
  size_t len_pos = 0;
  res.push_back(0);
  for (char chr : name) {
    if (chr == '.') {
      res[len_pos] = res.size() - len_pos - 1;
      len_pos = res.size();
      res.push_back(0);
    } else {
      res.push_back(chr);
    }
  }
  // fill in the last one
  res[len_pos] = res.size() - len_pos - 1;
  res.push_back('\0');  // 0x00
  res.shrink_to_fit();
  return res;
}

std::vector<uint8_t> find_name(std::array<uint8_t, BUFFER_SIZE> arr,
                               int offset) {
  int name_start = offset;
  while (arr[offset] != '\0') {
    uint8_t length = arr[offset];
    offset += length + 1;
  }
  offset++;  // skip nullterminator
  int name_length = offset - name_start;
  return {arr.begin() + name_start, arr.begin() + name_start + name_length};
}

}  // namespace Slime
