#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include "constants.h"
#include "encryption.h"

bool getServerInfo(std::string& ip, int& port);
void save_my_info(const char client_id[ID_SIZE], const std::string& name, const RSAPrivateWrapper& private_key);
bool load_my_info(std::string& name, char client_id[ID_SIZE], RSAPrivateWrapper& private_key, std::string& public_key);
