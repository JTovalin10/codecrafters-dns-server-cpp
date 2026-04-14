#include "question.hpp"

#include <arpa/inet.h>

#include "../utils/my_utils.hpp"

namespace Slime {

size_t get_ques_size(const Question& ques) {
  return ques.name.size() + sizeof(ques.type) + sizeof(ques._class);
}

void set_que_name(Question& que, const std::string& name) {
  que.name = Slime::encode_name(name);
}

std::vector<uint8_t> get_que_name(const Question& que) { return que.name; }

void set_que_type(Question& que, Slime::records value) {
  que.type = htons(static_cast<uint16_t>(value));
}

Slime::records get_que_type(const Question& que) {
  return static_cast<Slime::records>(ntohs(que.type));
}

void set_que_class(Question& que, Slime::classes value) {
  que._class = htons(static_cast<uint16_t>(value));
}

Slime::classes get_que_class(const Question& que) {
  return static_cast<Slime::classes>(ntohs(que._class));
}

}  // namespace Slime
