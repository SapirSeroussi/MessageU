#include "files.h"

// Load the ip and port from the file
bool getServerInfo(std::string& ip, int& port) {
    std::ifstream file(server_address_file);

    std::string line;
    std::string port_str;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        if (std::getline(iss, ip, ':') && std::getline(iss, port_str)) {
            try {
                port = std::stoi(port_str);
                return true;
            }
            catch (...) {
                std::cerr << "Error: Invalid port in file." << std::endl;
                return false;
            }
        }
    }

    std::cerr << "Error: Incorrect file format." << std::endl;
    return false;
}

// Save the name, ID and private key to a file
void save_my_info(const char client_id[ID_SIZE], const std::string& name, const RSAPrivateWrapper& private_key) {
    std::ofstream file(my_info_file);

    if (file.is_open()) {
        file << name << std::endl;

        for (size_t i = 0; i < ID_SIZE; i++) {
            file << std::setw(2) << std::setfill('0') << std::hex << (0xFF & static_cast<int>(client_id[i])) << " ";
        }
        file << std::endl;

        std::string private_key_base64 = Base64Wrapper::encode(private_key.getPrivateKey());
        file << private_key_base64 << std::endl;
        file.close();
    }
    else {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
}

// Load the name, ID and private key from the file
bool load_my_info(std::string& name, char client_id[ID_SIZE], RSAPrivateWrapper& private_key, std::string& public_key) {
    std::ifstream file(my_info_file);

    if (file.is_open()) { // my_info_file exist
        std::getline(file, name);

        for (size_t i = 0; i < ID_SIZE; i++) {
            int byte;
            file >> std::hex >> byte;
            client_id[i] = static_cast<char>(byte);
        }

        std::string private_key_base64;
        std::string line;

        while (std::getline(file, line)) {
            private_key_base64 += line; 
        }

        RSAPrivateWrapper private_key(Base64Wrapper::decode(private_key_base64));
        public_key = private_key.getPublicKey();

        file.close();

        return true;
    }
    else {
        return false;
    }
}
