#include "encryption.h"

void hexify(const unsigned char* buffer, unsigned int length)
{
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}

void generate_rsa_keys(RSAPrivateWrapper& rsapriv, std::string& pubkey, std::string& base64key)
{
	pubkey = rsapriv.getPublicKey();
	base64key = Base64Wrapper::encode(rsapriv.getPrivateKey());
}

std::string rsa_encryption(RSAPublicWrapper& rsapub, const unsigned char* plain, size_t length)
{
	return rsapub.encrypt((const char*)plain, length);
}

std::string rsa_decryption(RSAPrivateWrapper& rsapriv, const std::string& cipher)
{
	return rsapriv.decrypt(cipher);
}