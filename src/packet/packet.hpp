#pragma once

#include "answer/answer.hpp"
#include "header/header.hpp"
#include "message/message.hpp"
#include "question/question.hpp"

struct packet {
  Slime::Header h;
  Slime::Question q;
  Slime::Answer m;
};
