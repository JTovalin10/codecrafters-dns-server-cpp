#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Slime {
std::vector<uint8_t> encode_name(const std::string& name);
}  // namespace Slime