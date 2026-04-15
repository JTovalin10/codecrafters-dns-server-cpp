#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Slime {
// encodes a domain name as length-prefixed labels with a null terminator.
// name: the regular string ex: codecrafters.io
// Example: std::vector<uint8_t> bytes = encode_name("codecrafters.io");
// Result: \x0ccodecrafters\x02io\x00
std::vector<uint8_t> encode_name(const std::string& name);
}  // namespace Slime