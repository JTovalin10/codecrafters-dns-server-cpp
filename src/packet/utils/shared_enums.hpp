#pragma once
#include <cstdint>

namespace Slime {
enum class records : uint16_t {
  A = 1,
  NS = 2,
  MD = 3,
  MF = 4,
  CNAME = 5,
  SOA = 6,
  MB = 7,
  MG = 8,
  MR = 9,
  NULL_ = 10,
  WKS = 11,
  PTR = 12,
  HINFO = 13,
  MINFO = 14,
  MX = 15,
  TXT = 16,
};

enum class classes : uint16_t {
  IN = 1,
  CS = 2,
  CH = 3,
  HS = 4,
};
}  // namespace Slime