#include "question.hpp"

#include <cstring>

namespace Slime {

size_t get_ques_size(const Question& ques) {
  size_t name_len = (ques.name == nullptr) ? 0 : std::strlen(ques.name) + 1;
  return name_len + sizeof(ques.type) + sizeof(ques._class);
}

}  // namespace Slime
