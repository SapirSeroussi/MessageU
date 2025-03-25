#include "client.h"


void message_handler(SOCKET sock) {
    char client_id[ID_SIZE];
    uint16_t code;
    ClientsList clients_list;
    std::string name;

    std::string public_key, base64_key;
    RSAPrivateWrapper private_key;

    bool logged_in = false;
    int choice;
    std::vector<char> request_buffer;

    if (load_my_info(name, client_id, private_key, public_key)) {
        std::cout << "Welcome back " << name << std::endl;
        logged_in = true;
    }
    else {
        std::cout << "Welcome to MessageU" << std::endl;
        get_name(name, "Please enter your name: ");
    }


    while ((choice = menu()) != MENU_EXIT) {
        if (choice == MENU_REGISTER) {
            if (logged_in) {
                std::cout << "Client already registered." << std::endl;
            }
            else { 
                generate_rsa_keys(private_key, public_key, base64_key);

                if (public_key.size() != PUBLIC_KEY_SIZE) {
                    std::cerr << "Error: The public key size is incorrect. Please try again." << std::endl;
                    return;
                }

                request_buffer = register_request(name, public_key);
                send(sock, request_buffer.data(), request_buffer.size(), 0);
                request_buffer.clear();

                receive_response(sock, client_id, clients_list, private_key);

                save_my_info(client_id, name, private_key);
                generate_rsa_keys(private_key, public_key, base64_key);
                logged_in = true;
            }
        }
        if (!logged_in) {
            std::cerr << "Error: Client not registered. Please register before proceeding." << std::endl;
        }
        else { // Client is registered
            if (choice == MENU_CLIENTS_LIST) {

                request_buffer = clients_list_request(client_id);

                send(sock, request_buffer.data(), request_buffer.size(), 0);
                request_buffer.clear();

                receive_response(sock, client_id, clients_list, private_key);
            }
            else if (choice == MENU_PUBLIC_KEY) {
                char requested_id[ID_SIZE];
                if (get_recipient_id(clients_list, requested_id)) {
                    request_buffer = get_publicKey_request(client_id, requested_id);

                    send(sock, request_buffer.data(), request_buffer.size(), 0);
                    request_buffer.clear();


                    receive_response(sock, client_id, clients_list, private_key);
                }
            }
            else if (choice == MENU_PENDING_MESSAGES) {
                request_buffer = pending_messages_request(client_id);

                send(sock, request_buffer.data(), request_buffer.size(), 0);
                request_buffer.clear();

                receive_response(sock, client_id, clients_list, private_key);
            }
            else if (choice == MENU_SEND_MESSAGE) {
                char recipient_id[ID_SIZE];
                if (get_recipient_id(clients_list, recipient_id)) {
                    std::string message;
                    get_additional_info(message);
                    
                    request_buffer = send_message_request(clients_list, client_id, recipient_id, MESSAGE_TYPE_SEND_MESSAGE, message);

                    if (!request_buffer.empty()) {                      
                        send(sock, request_buffer.data(), request_buffer.size(), 0);
                        request_buffer.clear();

                        receive_response(sock, client_id, clients_list, private_key);
                    }
                }
            }
            else if (choice == MENU_GET_SYMMETRIC_KEY) {
                char recipient_id[ID_SIZE];
                if (get_recipient_id(clients_list, recipient_id)) {
                    request_buffer = send_message_request(clients_list, client_id, recipient_id, MESSAGE_TYPE_GET_SYMMETRIC_KEY);

                    if (!request_buffer.empty()) {
                        send(sock, request_buffer.data(), request_buffer.size(), 0);
                        request_buffer.clear();

                        receive_response(sock, client_id, clients_list, private_key);
                    }
                }
            }
            else if (choice == MENU_SEND_SYMMETRIC_KEY) {
                char recipient_id[ID_SIZE];
                if (get_recipient_id(clients_list, recipient_id)) {
                    request_buffer = send_message_request(clients_list,client_id ,recipient_id, MESSAGE_TYPE_SEND_SYMMETRIC_KEY);
                    if (!request_buffer.empty()) {
                        send(sock, request_buffer.data(), request_buffer.size(), 0);
                        request_buffer.clear();

                        receive_response(sock, client_id, clients_list, private_key);
                    }
                }
            }
        }
    }

    std::cout << "\nSee you next time :)" << std::endl;
}


int server_connection() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in serverAddr;

    std::string server_ip;
    int server_port;
    if (!getServerInfo(server_ip, server_port)) {
        return 1;
    }
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    message_handler(sock);

    closesocket(sock);
    WSACleanup();

    return 0;
}


int main() {
    return server_connection();
}
