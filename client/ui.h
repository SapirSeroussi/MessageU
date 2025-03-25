#pragma once

#include <iostream>
#include <map>
#include <limits>
#include <vector>

#include "constants.h"
#include "clientsList.h"

std::map<int, std::string> initMenuOptions();
void printMenu(const std::map<int, std::string>& menuOptions);
int menu();

void get_name(std::string& recipient_name, const std::string& prompt);
bool get_recipient_id(ClientsList& clients_list, char recipient_id[ID_SIZE]);
void get_additional_info(std::string& additional_info);