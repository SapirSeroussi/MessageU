# MessageU - Secure Messaging Application

MessageU is a secure client-server messaging application that allows users to exchange encrypted messages. The project implements various encryption techniques including RSA for asymmetric encryption and AES for symmetric encryption to ensure secure communication.

## Architecture

The project consists of two main components:

1. **Client** - Written in C++ using Winsock for network communication
2. **Server** - Written in Python with SQLite database for persistence

## Features

- User registration with RSA key pair generation
- Listing available clients
- Requesting public keys of other users
- Symmetric key exchange using RSA encryption
- Sending encrypted text messages using AES
- Message history persistence
- Data storage in both RAM and an SQLite database
- Multi-client support using threads for concurrency

## 🔐 Security Features

- **RSA Encryption**: Used for public/private key operations (1024-bit)
- **AES Encryption**: Used for symmetric encryption of messages
- **Key Exchange Protocol**: Secure exchange of symmetric keys using asymmetric encryption
- **Message Encryption**: Messages between clients are encrypted end-to-end. However, as specified in the assignment, messages between the client and server are not encrypted

## Prerequisites

### Client
- C++ compiler with C++11 support
- Crypto++ library
- Winsock2 library (Windows)

### Server
- Python 3.x
- SQLite3 library

## 🛠️ Installation

### Installing Crypto++
1. Download the cryptopp library
2. Open `cryptest.sln` file
3. Build the solution
4. Link `cryptlib.lib` to the project
    - Add `C:\Crypto++` to Include Directories
    - Add `cryptlib.lib` to Additional Dependencies
    - Set Runtime Library to `Multi-threaded Debug (/MTd)`

### Client Setup
1. Clone the repository
2. Configure server port in `server.info`
3. Compile the client code:
```bash
g++ -o messageu_client client/*.cpp -lcryptopp -lws2_32
```

### Server Setup
1. Configure server port in `myport.info` (default: 1357)
2. Start the server:
```bash
python server/server.py
```

## 🗃️ Configuration Files
- `server.info`: The client reads the server’s IP and port from this file.
- `my.info`: The client stores its client details in this file.
- `myport.info`: The server reads its listening port from this file.

## 🚀 Usage

### First-time Setup
1. Start the client application
2. Register a new user (option 110)
3. After registration, your user information is stored in `my.info`

### Basic Operations
- **Register (110)**: Register as a new client
- **Request clients list (120)**: Fetch the list of registered clients
- **Request public key (130)**: Get the public key of another client
- **Check pending messages (140)**: Retrieve waiting messages
- **Send text message (150)**: Send an encrypted message (requires symmetric key)
- **Request symmetric key (151)**: Ask another client for a symmetric key
- **Send symmetric key (152)**: Send your symmetric key to another client (requires public key)

### Message Exchange Process
1. Get the clients list (option 120)
2. Request another client's public key (option 130)
3. Send a request for a symmetric key (option 151)
4. When they log in, they'll see your request and send their key (option 152)
5. Once you have their symmetric key, you can exchange encrypted messages (option 150)

## 📡 Protocol

- The protocol is **fixed** and cannot be changed
- Error handling is intentionally **vague for security reasons**
- Client-server communication messages **are not encrypted**, as specified in the assignment
- Requests consist of:
    - **Header**: Client ID (16 bytes), Version (1 byte), Code (2 bytes), Payload Size (4 bytes)
    - **Payload**: Varies based on request type

## ⚠️ Message Handling & Error Reporting

- After messages are successfully delivered to the client, they are removed from storage
- If the server encounters an error, the client displays "server responded with an error" and waits for further input

## 📝 Notes

- The application uses version 2 of the protocol
- The server does not store session data and handles each request independently (stateless design)
  
