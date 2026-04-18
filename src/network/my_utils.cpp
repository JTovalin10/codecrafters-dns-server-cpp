#include "my_utils.hpp"

#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace Slime {

std::vector<uint8_t> encode_name(const std::string &name) {
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
  res.push_back('\0'); // 0x00
  res.shrink_to_fit();
  return res;
}

const uint8_t IS_POINTER = 0xC0;
const uint8_t GET_SIZE = 0x3f;
const uint8_t ADD_BYTE = 8;
std::pair<std::vector<uint8_t>, size_t>
find_network_format_name(std::array<uint8_t, BUFFER_SIZE> arr, size_t offset) {
  std::vector<uint8_t> res{};
  res.reserve(BUFFER_SIZE);
  std::unordered_set<size_t> visited{};
  const size_t soffset = offset;
  size_t wire_bytes{};
  bool followed_pointer = false;
  while (true) {
    const uint8_t byte = arr[offset];
    if (byte == '\0') {
      if (!followed_pointer) {
        wire_bytes = offset - soffset + 1;
      }
      break;
    }

    if ((byte & IS_POINTER) == IS_POINTER) {
      // this is a pointer so we must jump to there length byte
      if (visited.find(offset) != visited.end()) {
        throw std::runtime_error("Infinite loop");
      }
      if (!followed_pointer) {
        wire_bytes = offset - soffset + 2; // +2 for 2-byte pointer
        followed_pointer = true;
      }
      visited.insert(offset);
      offset = (((byte & GET_SIZE) << ADD_BYTE) | arr[offset + 1]);
    } else {
      // its pointing to a length byte
      uint8_t length = byte;
      res.push_back(length);
      offset++;
      for (size_t i = 0; i < length; i++) {
        res.push_back(arr[offset++]);
      }
    }
  }
  res.push_back('\0');
  res.shrink_to_fit();
  return {res, wire_bytes};
}

} // namespace Slime
