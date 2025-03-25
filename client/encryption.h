#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include "Base64Wrapper.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"

void hexify(const unsigned char* buffer, unsigned int length);

void generate_rsa_keys(RSAPrivateWrapper& rsapriv, std::string& pubkey, std::string& base64key);
std::string rsa_encryption(RSAPublicWrapper& rsapub, const unsigned char* plain, size_t length);
std::string rsa_decryption(RSAPrivateWrapper& rsapriv, const std::string& cipher);
