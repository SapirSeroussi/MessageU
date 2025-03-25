#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <iomanip>
#include <winsock2.h>

#include "constants.h"
#include "clientsList.h"
#include "encryption.h"

bool receive_response(int sock, char client_id[ID_SIZE], ClientsList& clients_list, RSAPrivateWrapper& rsa_private);
bool receive_header(int sock, uint16_t& code, uint32_t& payload_size);

bool successful_registration_response(int sock, uint32_t& payload_size, char client_id[ID_SIZE]);
bool clients_list_response(int sock, uint32_t& payload_size, ClientsList& clients_list);
bool public_key_response(int sock, uint32_t& payload_size, ClientsList& clients_list);
bool message_sent_response(int sock, uint32_t& payload_size, ClientsList& clients_list);
bool pending_messages_response(int sock, uint32_t& payload_size, ClientsList& clients_list, RSAPrivateWrapper& rsa_private);

bool receive_message_header(int sock, char client_id[ID_SIZE], uint32_t& message_id, uint8_t& type, uint32_t& size);
void printMessage(const std::string name, uint8_t& type, const std::string content);
bool receive_data(int sock, char* buffer, size_t buffer_size);