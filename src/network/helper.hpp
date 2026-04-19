#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "../packet/packet.hpp"
#include "shared_vars.hpp"

namespace Slime {

inline constexpr size_t HEADER_SIZE = 12;

// ─── Name encoding ───────────────────────────────────────────────────────────

std::vector<uint8_t> encode_name(const std::string& name);

std::pair<std::vector<uint8_t>, size_t> find_network_format_name(
    std::array<uint8_t, BUFFER_SIZE> arr, size_t offset);

// ─── Header helpers ──────────────────────────────────────────────────────────

void set_header_to_host(Header& header);
void set_header_to_network(Header& header);
void populate_header(Header& header);
uint16_t complete_header(std::array<uint8_t, BUFFER_SIZE>& buffer,
                         Header& header);

// ─── Question helpers ────────────────────────────────────────────────────────

size_t populate_questions(const std::array<uint8_t, BUFFER_SIZE>& buffer,
                          Question& que, size_t offset);

void write_question_to_buffer(std::array<uint8_t, BUFFER_SIZE>& buffer,
                              Question& que, size_t offset);

void find_all_questions(std::vector<Question>& questions,
                        const std::array<uint8_t, BUFFER_SIZE>& request,
                        size_t num_questions, size_t& read_offset);

// ─── Answer helpers ──────────────────────────────────────────────────────────

void populate_answer(Answer& ans, std::vector<uint8_t>&& name);

void write_answer_to_buffer(std::array<uint8_t, BUFFER_SIZE>& buffer,
                            Answer& ans, size_t offset);

void complete_answer(std::array<uint8_t, BUFFER_SIZE>& buffer, Answer& ans,
                     size_t offset, std::vector<uint8_t> name);

// ─── Response helpers ────────────────────────────────────────────────────────

void populate_response(std::array<uint8_t, BUFFER_SIZE>& response,
                       size_t& write_offset, std::vector<Question>& questions);

/**
 * executes a forward request. returns the response of after all fowards have
 * been compelete via the passed in reference to the request
 *
 * ARGS:
 * request: the clients request for the DNS server
 * response: our response that will be sent back to the client
 * write_offset: offset that will be used for the write offset
 *
 */
void execf(const std::array<uint8_t, BUFFER_SIZE>& request,
           std::array<uint8_t, BUFFER_SIZE>& response, size_t& write_offset,
           int& fSocket, struct sockaddr_in& fAddr);

/**
 *Executes a non forward request to the client which is just echoing the request
 * in the proper format
 *
 * ARGS:
 * request: the clients request for the DNS server
 * response: our response that will be sent back to the client
 * write_offset: offset that will be used for the write offset
 *
 */
void execnf(const std::array<uint8_t, BUFFER_SIZE>& request,
            std::array<uint8_t, BUFFER_SIZE>& response, size_t& write_offset);

}  // namespace Slime
