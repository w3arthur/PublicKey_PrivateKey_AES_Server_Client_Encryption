#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "files_util.h"
#include "structs.h"
#include "http_request.h"

int main() 
{
    client::transfer transfer = client::get_transfer();
    client::identyfier identyfier = client::get_identyfier();
    if (!identyfier.is_available)
    {

    }

    //std::cout << client::SendHttpRequest("http://example.com/") << std::endl;


    std::string serverIP = "127.0.0.1"; // Change to your server's IP address
    std::string serverPort = "1234";   // Change to the server's listening port
    std::string message = "hi24242";


    std::string serverResponse = client::SendMessageToServer(serverIP, serverPort, message);
    if (!serverResponse.empty()) {
        std::cout << "Received: " << serverResponse << std::endl;
    }
    else {
        std::cerr << "Message sending failed or no response received." << std::endl;
    }



#if defined(_DEBUG)
    std::cout << "\n\nDebugging mode\nPress [Enter] to run main() again\n";
    std::cin.get();
    return main();
#endif // DEBUG
    return 0;
}
