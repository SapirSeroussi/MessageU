#include "responses.h"

// Receives data from the server and routes it to the appropriate response
bool receive_response(int sock, char client_id[ID_SIZE], ClientsList& clients_list, RSAPrivateWrapper& rsa_private) {
    uint16_t code;
    uint32_t payload_size;
    
    if (!receive_header(sock, code, payload_size))
        return false;

    if (code == ERROR_RESPONSE) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }
    
    switch (code) {
    case SUCCESSFUL_REGISTER_RESPONSE:
        return successful_registration_response(sock, payload_size, client_id);
    case CLIENTS_LIST_RESPONSE:
        return clients_list_response(sock, payload_size, clients_list);
    case PUBLIC_KEY_RESPONSE:
        return public_key_response(sock, payload_size, clients_list);
    case MESSAGE_SENT_RESPONSE:
        return message_sent_response(sock, payload_size, clients_list);
    case PENDING_MESSAGES_RESPONSE:
        return pending_messages_response(sock, payload_size, clients_list, rsa_private);
    default:
        std::cerr << "Error: Server Error.\n";
        return false;
    }
}

// Read the Header from the responses
// Header format: version, code, payload size
bool receive_header(int sock, uint16_t& code, uint32_t& payload_size) {
    char header[VERSION_SIZE + CODE_SIZE + SIZE_SIZE]; // header of the server response
    
    if (!receive_data(sock, header, sizeof(header))) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }
    
    if (header[0] != VERSION) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }
    std::memcpy(&code, &header[VERSION_SIZE], CODE_SIZE);
    std::memcpy(&payload_size, &header[VERSION_SIZE + CODE_SIZE], SIZE_SIZE);
    return true;
}


// Register successful responce - return the id the server created for this client
bool successful_registration_response(int sock, uint32_t& payload_size, char client_id[ID_SIZE]) {
    if (payload_size != ID_SIZE) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    if (recv(sock, client_id, ID_SIZE, 0) != ID_SIZE) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    std::array<uint8_t, ID_SIZE> buffer;
    std::cout << "Registration successful." << std::endl;
    return true;
}


// User list response - Add clients IDs and names to clients_list and print their names
bool clients_list_response(int sock, uint32_t& payload_size, ClientsList& clients_list) {
    if (payload_size % (ID_SIZE + NAME_SIZE) != 0) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    std::vector<char> payload(payload_size);
    size_t received = 0;

    if (!receive_data(sock, payload.data(), payload_size)) {
        return false;
    }

    size_t offset = 0;
    while (offset < payload_size) {
        clients_list.addClient(payload.data() + offset, payload.data() + offset + ID_SIZE);
        offset += ID_SIZE + NAME_SIZE;
    }

    clients_list.printClientsNames();

    return true;;
}

// Public key response - get the requested public key and save it
bool public_key_response(int sock, uint32_t& payload_size, ClientsList& clients_list) {
    if (payload_size != (ID_SIZE + PUBLIC_KEY_SIZE)) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    std::vector<char> payload(payload_size);

    if (!receive_data(sock, payload.data(), payload_size)) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    char client_id[ID_SIZE];
    std::memcpy(client_id, payload.data(), ID_SIZE);
    std::vector<char> public_key(payload.begin() + ID_SIZE, payload.end());
    if (!clients_list.setClientPublicKey(client_id, public_key)) {
        std::cout << "Error: A problem occure while saving the public key, please try again.";
    }

    return true;
}


// Message_sent_response - print the recepient client name and the message ID the server sent
bool message_sent_response(int sock, uint32_t& payload_size, ClientsList& clients_list) {
    if (payload_size != (ID_SIZE + SIZE_SIZE)) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    std::vector<char> payload(payload_size);
    if (!receive_data(sock, payload.data(), payload_size)) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    char client_id[ID_SIZE];
    std::memcpy(client_id, payload.data(), ID_SIZE);

    uint32_t message_id;
    std::memcpy(&message_id, payload.data() + ID_SIZE, MESSAGE_ID_SIZE);

    std::string name = clients_list.getNameById(client_id);
    if (name.empty()) {
        std::cerr << "Message successfully sent to unrecognized client. Please try again.";
        return false;
    }

    std::cout << "Message to " << name << " has been sent successfully." << std::endl;
    std::cout << "Message ID: " << message_id << std::endl;

    return true;
}


// Pending messages response - get the messages, decrypt if necessary and print them.
bool pending_messages_response(int sock, uint32_t& payload_size, ClientsList& clients_list, RSAPrivateWrapper& rsa_private) {
    char client_id[ID_SIZE];
    uint32_t message_id;
    uint8_t type;
    uint32_t size;
    size_t total_received = 0;

    std::cout << "\nPending messages:" << std::endl;

    while (total_received < payload_size && receive_message_header(sock, client_id, message_id, type, size)) {
        total_received += MESSAGE_HEADER_SIZE;
        std::string recieived_name = clients_list.getNameById(client_id);
        if (recieived_name.empty()) {
            std::cerr << "Error: Sender is not recognized.\n"; 
            continue;
        }

        std::vector<char> content(size);

        if (!receive_data(sock, content.data(), size)) {
            std::cerr << "Error: Server Error.\n";
            continue;  // Skip to the next message if the content can't be received
        }
        total_received += size;

        std::string message = "";
        if (type == MESSAGE_TYPE_GET_SYMMETRIC_KEY) {
            message = "Request for symmetric key.\n";
        }
        else if (type == MESSAGE_TYPE_SEND_SYMMETRIC_KEY) {
            try {
                std::string encrypted_aes_key(content.begin(), content.end());
                std::string decrypted_aes_key = rsa_decryption(rsa_private, encrypted_aes_key);

                if (decrypted_aes_key.size() != AESWrapper::DEFAULT_KEYLENGTH) {
                    std::cerr << "Error: A problem occure while reading the message.\n";
                    continue;
                }

                unsigned char aes_key[AESWrapper::DEFAULT_KEYLENGTH];
                std::memcpy(aes_key, decrypted_aes_key.c_str(), AESWrapper::DEFAULT_KEYLENGTH);

                if (clients_list.setClientSymmetricKey(client_id, aes_key))
                    message = "Symmetric key received.\n";
                else {
                    std::cerr << "Error: Symmetric key already exists. Cannot overwrite.\n";
                    continue;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error: A problem occure while reading the message.\n";
                continue;
            }
        }
        else if (type == MESSAGE_TYPE_SEND_MESSAGE) {
            try {
                std::string content_str(content.begin(), content.end());
                const unsigned char* retrievedKey = clients_list.getClientSymmetricKey(client_id);
                if (!retrievedKey) {
                    std::cerr << "Error: A problem occure while reading the message.\n";
                    continue;
                }

                AESWrapper aes(retrievedKey, AESWrapper::DEFAULT_KEYLENGTH);
                message = aes.decrypt(content_str.c_str(), content_str.size());
            }
            catch (const std::exception& e) {
                std::cerr << "Error: A problem occure while reading the message.\n";
                continue;
            }
        }

        printMessage(recieived_name, type, message);
    }

    if (total_received < MESSAGE_HEADER_SIZE) {
        std::cout << "- No messages pending -\n" << std::endl;
    }
    return true;
}

// Print the messages pending
void printMessage(const std::string name, uint8_t& type, const std::string content) {
    std::cout << "From: " << name << std::endl;
    std::cout << "Content:\n" << content << std::endl;
    std::cout << "-----<EOM>-----\n" << std::endl;
}

// Receive the header of each message in the pending messages response
bool receive_message_header(int sock, char client_id[ID_SIZE], uint32_t& message_id, uint8_t& type, uint32_t& size) {
    char msg_header[MESSAGE_HEADER_SIZE];

    if (recv(sock, msg_header, MESSAGE_HEADER_SIZE, 0) != MESSAGE_HEADER_SIZE) {
        std::cerr << "Error: Server Error.\n";
        return false;
    }

    std::memcpy(client_id, msg_header, ID_SIZE);
    std::memcpy(&message_id, &msg_header[ID_SIZE], MESSAGE_ID_SIZE);
    type = static_cast<uint8_t>(msg_header[ID_SIZE + MESSAGE_ID_SIZE]);
    std::memcpy(&size, &msg_header[ID_SIZE + MESSAGE_ID_SIZE + MESSAGE_TYPE_SIZE], SIZE_SIZE);

    return true;
}

// Helper function to receive content of large size from the server.
bool receive_data(int sock, char* buffer, size_t buffer_size) {
    size_t total_received = 0;
    while (total_received < buffer_size) {
        int bytes_received = recv(sock, buffer + total_received, buffer_size - total_received, 0);
        if (bytes_received <= 0) {
            std::cerr << "Error: Server Error.\n";
            return false;
        }
        total_received += bytes_received;
    }
    return total_received == buffer_size;
}
