#pragma once

#include <string>

const std::string my_info_file = "my.info";
const std::string server_address_file = "server.info";

const char VERSION = 2;

const size_t MAX_BUFFER_SIZE = 1024; 

const size_t ID_SIZE = 16;
const size_t VERSION_SIZE = 1;
const size_t CODE_SIZE = 2;
const size_t SIZE_SIZE = 4;
const size_t HEADER_SIZE = ID_SIZE + VERSION_SIZE + CODE_SIZE + SIZE_SIZE;

const size_t MAX_MESSAGE_SIZE = (1UL << (SIZE_SIZE * 8)) - 1;
const size_t BASE_MESSAGE_SIZE = 1024;
const size_t INITIAL_MESSAGE_SIZE = (BASE_MESSAGE_SIZE > MAX_MESSAGE_SIZE) ? MAX_MESSAGE_SIZE : BASE_MESSAGE_SIZE;

const size_t NAME_SIZE = 255;
const size_t PUBLIC_KEY_SIZE = 160;

const size_t MESSAGE_TYPE_SIZE = 1;
const size_t MESSAGE_ID_SIZE = 4;
const size_t MESSAGE_HEADER_SIZE = ID_SIZE + MESSAGE_ID_SIZE + MESSAGE_TYPE_SIZE + SIZE_SIZE;

const size_t RSA_ENCRYPTED_MESSAGE_LENGTH = 128;	// 128 bytes = 1024 bits


const size_t REGISTER_REQUEST = 600;
const size_t CLIENTS_LIST_REQUEST = 601;
const size_t PUBLIC_KEY_REQUEST = 602;
const size_t SEND_MESSAGE_REQUEST = 603;
const size_t PENDING_MESSAGES_REQUEST = 604;

const size_t SUCCESSFUL_REGISTER_RESPONSE = 2100;
const size_t CLIENTS_LIST_RESPONSE = 2101;
const size_t PUBLIC_KEY_RESPONSE = 2102;
const size_t MESSAGE_SENT_RESPONSE = 2103;
const size_t PENDING_MESSAGES_RESPONSE = 2104;
const size_t ERROR_RESPONSE = 9000;

const size_t MESSAGE_TYPE_GET_SYMMETRIC_KEY = 1;
const size_t MESSAGE_TYPE_SEND_SYMMETRIC_KEY = 2;
const size_t MESSAGE_TYPE_SEND_MESSAGE = 3;
const size_t MESSAGE_TYPE_SEND_FILE = 4;

const size_t MENU_REGISTER = 110;
const size_t MENU_CLIENTS_LIST = 120;
const size_t MENU_PUBLIC_KEY = 130;
const size_t MENU_PENDING_MESSAGES = 140;
const size_t MENU_SEND_MESSAGE = 150;
const size_t MENU_GET_SYMMETRIC_KEY = 151;
const size_t MENU_SEND_SYMMETRIC_KEY = 152;
const size_t MENU_EXIT = 0;

