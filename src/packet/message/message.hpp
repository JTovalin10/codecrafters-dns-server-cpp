#pragma once

#include "../header/header.hpp"
#include "../question/question.hpp"

namespace Slime {

struct Message {
  Header header;
  Question question;
  /**
  Answer answer;
  Authority auth;
  Additional additional;
  */
};

};  // namespace Slime
