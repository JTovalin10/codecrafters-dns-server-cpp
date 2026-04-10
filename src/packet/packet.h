#include <cstdint>

/**
 *
 */
struct Header {
  uint16_t pid;
  uint16_t flags;
  uint16_t QDCOUNT;
  uint16_t ANDCOUNT;
  uint16_t NSCOUNT;
  uint16_t ARCOUNT;
};

struct Message {
  Header header;
  Question question;
  Answer answer;
  Authority auth;
  Additional addtioanl;
};
