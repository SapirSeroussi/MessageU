#pragma once

#include <iostream>
#include <vector>

#include "constants.h"
#include "clientsList.h"

std::vector<char> create_header(const char id[ID_SIZE], uint16_t code, uint32_t msg_size);

std::vector<char> register_request(const std::string name, const std::string& publicKey);
std::vector<char> clients_list_request(const char id[ID_SIZE]);
std::vector<char> get_publicKey_request(const char my_id[ID_SIZE], const char requested_id[ID_SIZE]);
std::vector<char> send_message_request(ClientsList& clients_list, const char client_id[ID_SIZE], const char id[ID_SIZE], const uint8_t& message_type, const std::string& message_content = "");
std::vector<char> pending_messages_request(const char id[ID_SIZE]);

bool check_type(const uint8_t& message_type, const uint32_t& contentSize);
std::string encryptMessage(ClientsList& clients_list, const char recipient_id[ID_SIZE], const std::string& message_content);
std::vector<char> combine_buffers(const std::vector<char>& header, const std::vector<char>& payload);