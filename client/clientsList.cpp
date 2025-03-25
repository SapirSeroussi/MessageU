#include "clientsList.h"

// ClientInfo:

ClientInfo::ClientInfo() {
    std::memset(this->id, 0, ID_SIZE);  
    std::memset(this->name, 0, NAME_SIZE);
    std::memset(this->public_key, 0, PUBLIC_KEY_SIZE);
    std::memset(this->symmetric_key, 0, AESWrapper::DEFAULT_KEYLENGTH);  
}

ClientInfo::ClientInfo(const char* id, const char* name) {
    std::memcpy(this->id, id, ID_SIZE);    
    std::memcpy(this->name, name, NAME_SIZE);
    std::memset(this->public_key, 0, PUBLIC_KEY_SIZE);
    std::memset(this->symmetric_key, 0, AESWrapper::DEFAULT_KEYLENGTH);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
 
// ClientsList:

void ClientsList::addClient(const char* id, const char* name) {
    ClientInfo client(id, name);
    idMap.insert({ std::string(client.id, ID_SIZE), client });
    nameToIdMap[std::string(client.name, NAME_SIZE)] = std::string(client.id, ID_SIZE);
}

void ClientsList::printClientsNames() const {
    if (nameToIdMap.empty()) {
        std::cout << "No clients available." << std::endl;
        return;
    }

    std::cout << "Clients List:" << std::endl;
    for (const auto& entry : nameToIdMap) {
        std::cout << "- " << entry.first << std::endl; 
    }
}

bool ClientsList::getIdByName(const std::string& name, char requested_id[ID_SIZE]) const {
    std::string padded_name = name;
    if (name.empty())
        return false;

    if (padded_name.size() < NAME_SIZE) {
        padded_name.resize(NAME_SIZE, '\0'); 
    }
    
    auto requested_name = nameToIdMap.find(padded_name);
    if (requested_name != nameToIdMap.end()) {
        std::memcpy(requested_id, requested_name->second.c_str(), ID_SIZE);

        return true; 
    }
    return false;
}

std::string ClientsList::getNameById(const char* id) const {
    std::string id_str(id, ID_SIZE);
    auto requested_name = idMap.find(id_str);
    if (requested_name != idMap.end()) {
        return std::string(requested_name->second.name);
    }
    return "";  
}

bool ClientsList::getClient(const char* id, ClientInfo& clientInfo) const {
    std::string id_str(id, ID_SIZE);
    auto client = idMap.find(id_str);
    if (client != idMap.end()) {
        clientInfo = client->second;  // Return the ClientInfo object
        return true;
    }
    return false;
}


const unsigned char* ClientsList::getClientSymmetricKey(const char* id) const {
    if (!id) {
        return nullptr;
    }

    std::string id_str(id, ID_SIZE);
    auto client = idMap.find(id_str);

    if (client == idMap.end()) {
        return nullptr;
    }

    // Check that the key isnt empty
    bool isKeySet = false;
    for (size_t i = 0; i < AESWrapper::DEFAULT_KEYLENGTH; i++) {
        if (client->second.symmetric_key[i] != 0) {
            isKeySet = true;
            break;
        }
    }

    if (!isKeySet) {
        return nullptr;
    }

    return client->second.symmetric_key;
}

std::string ClientsList::getClientPublicKey(const char* id) const {
    if (!id) {
        return "";
    }

    std::string id_str(id, ID_SIZE);
    auto client = idMap.find(id_str);

    if (client == idMap.end()) {
        return "";
    }

    // check that the key isnt empty
    bool isKeySet = false;
    for (size_t i = 0; i < PUBLIC_KEY_SIZE; i++) {
        if (client->second.public_key[i] != 0) {
            isKeySet = true;
            break;
        }
    }

    if (!isKeySet) {
        return "";
    }

    return std::string(client->second.public_key, PUBLIC_KEY_SIZE);
}


bool ClientsList::setClientPublicKey(const char* id, const std::vector<char>& publicKey) {
    if (!id || publicKey.size() != PUBLIC_KEY_SIZE) {
        return false; 
    }

    std::string id_str(id, ID_SIZE);
    auto client = idMap.find(id_str);

    if (client == idMap.end()) {
        return false; 
    }

    std::memcpy(client->second.public_key, publicKey.data(), PUBLIC_KEY_SIZE);
    return true;
}

bool ClientsList::setClientSymmetricKey(const char* id, const unsigned char key[AESWrapper::DEFAULT_KEYLENGTH]) {
    if (!id) {
        return false;
    }

    std::string id_str(id, ID_SIZE);
    auto client = idMap.find(id_str);

    if (client == idMap.end()) {
        return false;
    }

    // check that the key isnt empty
    bool isEmpty = true;
    for (size_t i = 0; i < AESWrapper::DEFAULT_KEYLENGTH; i++) {
        if (key[i] != 0) {
            isEmpty = false;
            break;
        }
    }
    if (isEmpty) {
        return false;
    }

    std::memcpy(client->second.symmetric_key, key, AESWrapper::DEFAULT_KEYLENGTH);

    return true;
}
