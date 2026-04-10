#pragma once

#include <cstdint>

/**
 * each are 16 bits for a total of 12 bytes
 * pid = A random ID assigned to query packets. Response packets must reply with
 * the same PID
 * flags {
 * bits, name, description
 * 1, QR, 1 for a reply packet, 0 for a question packet
 *
 * 4, OPCODE, specifies the kind of query in a Message
 *
 * 1, AA, 1 if the responding server "owns" the domain queried
 *
 * 1, TC, 1 if the message is larger than 512 bytes (should default to 0 in UDP
 * responses)
 *
 * 1, RD, Sender sets this to 1 if the server should recursively resolve this
 * query otherwise iterative (0)
 *
 * 1, RA, server sets this to 1 to indicate that recursion is available
 *
 * 3, Z, used by DNSSEC queries. At inception, it was reserved for future use
 *
 * 4, RCODE, Response code indicating the status of the response
 * }
 * QDCOUNT: number of questions in the question section
 * ANCOUNT: number of records in the answer section
 * NSCOUNT: number of records in the authority section
 * ARCOUNT: the number of records in the Additional section
 */

namespace Slime {

struct Header {
  uint16_t pid;
  uint16_t flags;
  uint16_t QDCOUNT;
  uint16_t ANCOUNT;
  uint16_t NSCOUNT;
  uint16_t ARCOUNT;
};

/**
 * name: domain name represented as a sequence of labels
 * ex: google.com = \x06google\x03com\0x00 (null byte) where each \x.. is the
 * length and also represents the .
 *
 * type: the type of record, we usually deal with 1 and 5
 * Type, Value, Meaning
 * A, 1, a hot address
 * NS, 2, an authority name server
 * MD, 3, a mail destination (use MX)
 * MF, 4, A mail forwarder (use MX)
 * CNAME, 5, the canonical name for an alias
 * SOA, 6, marks teh start of a zone of authority
 * MB, 7, a mailbox domain name (experimental)
 * MG, 8, a mail group member (experimental)
 * MR, 9, a mail rename domain name (experimental)
 * NULL, 10, a null RR (experimental)
 * WKS, 11, a well known serveice description
 * PTR, 12, a domain name pointer
 * HINFO, 14, mailbox or mail list information
 * MX, 15, mail exchange
 * TXT, 16, text strings
 *
 * class: usually set to 1
 * type, Value, Meaning
 * IN, 1, the internet
 * CS, 2, the CSNET calss (obsolete)
 * CH, 3, the choas class
 * HS, 4, Hesiod
 *
 */
struct Question {
  char* name;
  uint16_t type;
  uint16_t _class;
};

struct Message {
  Header header;
  Question question;
  /**
  Answer answer;
  Authority auth;
  Additional additional;
  */
};

class flags_bg {
 public:
  explicit flags_bg(const struct Header& header);

  uint8_t get_qr() const;
  uint8_t get_opcode() const;
  uint8_t get_aa() const;
  uint8_t get_tc() const;
  uint8_t get_rd() const;
  uint8_t get_ra() const;
  uint8_t get_z() const;
  uint8_t get_rcode() const;

  void set_qr(uint16_t value);

  void set_opcode(uint16_t value);

  void set_aa(uint16_t value);

  void set_tc(uint16_t value);

  void set_rd(uint16_t value);

  void set_ra(uint16_t value);

  void set_z(uint16_t value);

  void set_rcode(uint16_t value);

  void reset(const struct Header& header);
  // returns the flags so the user can use them (meant if they modify it)
  uint16_t release() const;

 private:
  uint16_t flags;
  uint16_t get_flags_bit(const uint16_t index) const;
  void set_value(uint16_t value, const uint16_t index);
};

};  // namespace Slime
