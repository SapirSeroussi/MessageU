#include "requests.h"

// header format : id, version, code, payload size  
std::vector<char> create_header(const char id[ID_SIZE], uint16_t code, uint32_t msg_size) {
    std::vector<char> header(HEADER_SIZE, 0);

    std::memcpy(header.data(), id, ID_SIZE);

    header[ID_SIZE] = VERSION;
    std::memcpy(header.data() + ID_SIZE + VERSION_SIZE, &code, CODE_SIZE);
    std::memcpy(header.data() + ID_SIZE + VERSION_SIZE + CODE_SIZE, &msg_size, SIZE_SIZE);

    return header;
}


// Register request
// payload contains name and public key
std::vector<char> register_request(const std::string name, const std::string& publicKey) {
    char empty_id[ID_SIZE] = { 0 };

    std::vector<char> payloadBuffer(NAME_SIZE + PUBLIC_KEY_SIZE, 0);
    std::memcpy(payloadBuffer.data(), name.c_str(), std::min(name.size(), size_t(NAME_SIZE - 1)));
    std::memcpy(payloadBuffer.data() + NAME_SIZE, publicKey.data(), PUBLIC_KEY_SIZE);

    std::vector<char> headerBuffer = create_header(empty_id, REGISTER_REQUEST, payloadBuffer.size());

    return combine_buffers(headerBuffer, payloadBuffer);
}


// Users list request
// no payload, payload size = 0
std::vector<char> clients_list_request(const char id[ID_SIZE]) {
    return create_header(id, CLIENTS_LIST_REQUEST, 0);
}


// Public key request
// payload contains the ID of the requested public key 
std::vector<char> get_publicKey_request(const char my_id[ID_SIZE], const char requested_id[ID_SIZE]) {
    std::vector<char> headerBuffer = create_header(my_id, PUBLIC_KEY_REQUEST, ID_SIZE);
    std::vector<char> payloadBuffer(requested_id, requested_id + ID_SIZE);

    return combine_buffers(headerBuffer, payloadBuffer);
}


// Send message request
// Payload contains recipient ID, message type, content size and the content
std::vector<char> send_message_request(ClientsList& clients_list, const char client_id[ID_SIZE], const char recipient_id[ID_SIZE], const uint8_t& message_type, const std::string& content) {
    std::string message_content = content;
    
    // for normal messages - encrypt the message with symmetric key before sending it
    if (message_type == MESSAGE_TYPE_SEND_MESSAGE) {
        if (message_content.empty())
            return {};
        message_content = encryptMessage(clients_list, recipient_id, message_content);
    }
    // for symmetric key messages - encrypt the symmetric key with the recipirnt public key
    else if (message_type == MESSAGE_TYPE_SEND_SYMMETRIC_KEY) {
        try {
            unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
            AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH);
            clients_list.setClientSymmetricKey(recipient_id, key);

            std::string public_key = clients_list.getClientPublicKey(recipient_id);
            if (public_key.empty()) {
                std::string name = clients_list.getNameById(recipient_id);
                std::cerr << "Error: no public key is saved for client: " << name << "." << std::endl;
                return {};
            }
            RSAPublicWrapper rsa_pubic(public_key);
            message_content = rsa_encryption(rsa_pubic, key, AESWrapper::DEFAULT_KEYLENGTH);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: Unable to send the symmetric key. Please try again." << std::endl;
            return {};
        }   
    }
    uint32_t contentSize = message_content.size();

    if (!check_type(message_type, contentSize)) {
        return {};
    }
    std::vector<char> payloadBuffer(ID_SIZE + MESSAGE_TYPE_SIZE + SIZE_SIZE + contentSize, 0);

    std::memcpy(payloadBuffer.data(), recipient_id, ID_SIZE);
    std::memcpy(&payloadBuffer[ID_SIZE], &message_type, MESSAGE_TYPE_SIZE);
    std::memcpy(&payloadBuffer[ID_SIZE+MESSAGE_TYPE_SIZE], &contentSize, SIZE_SIZE);

    if (message_type != MESSAGE_TYPE_GET_SYMMETRIC_KEY) {
        std::memcpy(&payloadBuffer[ID_SIZE + MESSAGE_TYPE_SIZE + SIZE_SIZE], message_content.data(), contentSize);
    }
    
    std::vector<char> headerBuffer = create_header(client_id, SEND_MESSAGE_REQUEST, payloadBuffer.size());

    return combine_buffers(headerBuffer, payloadBuffer);
}    

// Pending messages request
// no payload, payload size = 0
std::vector<char> pending_messages_request(const char id[ID_SIZE]) {
    return create_header(id, PENDING_MESSAGES_REQUEST, 0);
}

// Returns true if the type is correct and the content size matches it.
bool check_type(const uint8_t& message_type, const uint32_t& contentSize) {
    if (message_type != MESSAGE_TYPE_GET_SYMMETRIC_KEY && contentSize == 0)
        return false;
    if (message_type == MESSAGE_TYPE_SEND_SYMMETRIC_KEY && contentSize != RSA_ENCRYPTED_MESSAGE_LENGTH)
        return false;
    if (message_type == MESSAGE_TYPE_SEND_MESSAGE || message_type == MESSAGE_TYPE_SEND_FILE)
        if (contentSize % 16 != 0)  // message encrypted with AES-BCB encryption always divide by 16
            return false;
    return true;
}

// encrypt regular messages with symmetric key 
std::string encryptMessage(ClientsList& clients_list, const char recipient_id[ID_SIZE], const std::string& message_content) {
    const unsigned char* recipient_key = clients_list.getClientSymmetricKey(recipient_id);

    if (!recipient_key) {
        std::string name = clients_list.getNameById(recipient_id);
        std::cerr << "Error: no symmetric key is saved for client: " << name << ", message can't be encrypted." << std::endl;
        return "";
    }

    AESWrapper aes(recipient_key, AESWrapper::DEFAULT_KEYLENGTH);
    std::string ciphertext = aes.encrypt(message_content.c_str(), message_content.size());

    return ciphertext;
}

// combine 2 vectors
std::vector<char> combine_buffers(const std::vector<char>& header, const std::vector<char>& payload) {
    std::vector<char> combinedBuffer;
    combinedBuffer.reserve(header.size() + payload.size());
    combinedBuffer.insert(combinedBuffer.end(), header.begin(), header.end());
    combinedBuffer.insert(combinedBuffer.end(), payload.begin(), payload.end());
    return combinedBuffer;
}

