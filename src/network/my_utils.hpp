#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace Slime {
const int BUFFER_SIZE = 512;

// encodes a domain name as length-prefixed labels with a null terminator.
// name: the regular string ex: codecrafters.io
// Example: std::vector<uint8_t> bytes = encode_name("codecrafters.io");
// Result: \x0ccodecrafters\x02io\x00
std::vector<uint8_t> encode_name(const std::string& name);

// returns an array of bytes that contains the network notation name from a
// array of bytes
std::vector<uint8_t> find_name(std::array<uint8_t, BUFFER_SIZE> arr,
                               int offset);

}  // namespace Slime
