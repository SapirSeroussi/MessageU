#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstring>  

#include "constants.h"
#include "encryption.h" // for DEFAULT_KEYLENGTH


struct ClientInfo {
    char id[ID_SIZE];
    char name[NAME_SIZE];
    char public_key[PUBLIC_KEY_SIZE];
    unsigned char symmetric_key[AESWrapper::DEFAULT_KEYLENGTH];

    ClientInfo();
    ClientInfo(const char* id, const char* name);
};

class ClientsList {
private:
    std::unordered_map<std::string, ClientInfo> idMap;          // Key: client_id (string) -> Value: ClientInfo
    std::unordered_map<std::string, std::string> nameToIdMap;   // Key: name -> Value: client_id (string)

public:
    ClientsList() = default;
    void addClient(const char* id, const char* name);
    void printClientsNames() const;
    
    // Getters
    bool getIdByName(const std::string& name, char requested_id[ID_SIZE]) const;
    std::string getNameById(const char* id) const;
    bool getClient(const char* id, ClientInfo& clientInfo) const;
    const unsigned char* getClientSymmetricKey(const char* id) const;
    std::string getClientPublicKey(const char* id) const;

    // Setters
    bool setClientPublicKey(const char* id, const std::vector<char>& publicKey);
    bool setClientSymmetricKey(const char* id, const unsigned char key[AESWrapper::DEFAULT_KEYLENGTH]);
    
};

