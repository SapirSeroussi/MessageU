#include "ui.h"

std::map<int, std::string> initMenuOptions() {
    return {
        {MENU_REGISTER, "Register"},
        {MENU_CLIENTS_LIST, "Request for clients list"},
        {MENU_PUBLIC_KEY, "Request for public key"},
        {MENU_PENDING_MESSAGES, "Request for waiting messages"},
        {MENU_SEND_MESSAGE, "Send a text message"},
        {MENU_GET_SYMMETRIC_KEY, "Send a request for symmetric key"},
        {MENU_SEND_SYMMETRIC_KEY, "Send your symmetric key"},
        {MENU_EXIT, "Exit client"}
    };
}

void printMenu(const std::map<int, std::string>& menuOptions) {
    std::cout << "\nMessageU client at your service:" << std::endl;
    for (const auto& option : menuOptions) {
        std::cout << option.first << ") " << option.second << std::endl;
    }
}

// Generate the menu and velidate the choice
int menu() {
    std::map<int, std::string> menuOptions = initMenuOptions();
    printMenu(menuOptions);

    int choice;

    while (true) {
        std::cout << "\nEnter your choice: ";
        if (std::cin >> choice && menuOptions.count(choice))
            return choice;
        else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid option. Please choose again." << std::endl;
        }   
    }
    return choice;
}

// Ask the client for a name according to a prompt, validate the name length
void get_name(std::string& recipient_name, const std::string& prompt) {
    char buffer[NAME_SIZE];
    
    while (true) {
        std::cout << prompt;

        std::cin.clear();
        if (std::cin.peek() == '\n') {
            std::cin.ignore(); 
        }
        std::cin.getline(buffer, NAME_SIZE);

        if (std::cin.fail()) {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

            std::cerr << "Error: Name is too long. Please enter a name with at most "
                << (NAME_SIZE - 1) << " characters." << std::endl;
            continue;  
        }

        if (buffer[0] == '\0') {
            std::cerr << "Error: Please enter a valid name." << std::endl;
            continue;  
        }

        recipient_name = buffer;
        return;
    }
}

// get the recipient name from the client and return its id
bool get_recipient_id(ClientsList& clients_list, char recipient_id[ID_SIZE]) {
    std::string name;
    get_name(name, "\nEnter recipient name: ");

    if (!clients_list.getIdByName(name, recipient_id)) {
        std::cout << "[ " << name << " ] not found. Try option 120 to update the list." << std::endl;
        return false;
    }
    return true;
}

void get_additional_info(std::string& additional_info) {
    std::vector<char> buffer;
    buffer.reserve(INITIAL_MESSAGE_SIZE);

    char c;
    std::cout << "Enter your message content: ";

    while (std::cin.get(c)) {
        if (c == '\n')
            break;
        if (buffer.size() >= MAX_MESSAGE_SIZE) {
            std::cerr << "\nError: Message size exceeds the limit of " << MAX_MESSAGE_SIZE << " bytes!\n";
            return;
        }
        buffer.push_back(c);
    }

    additional_info = std::string(buffer.begin(), buffer.end());
}

