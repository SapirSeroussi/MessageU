#pragma once

#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "constants.h"
#include "requests.h"
#include "responses.h"
#include "ui.h"
#include "encryption.h"
#include "clientsList.h"
#include "files.h"


int server_connection();
void message_handler(SOCKET sock);
int main();