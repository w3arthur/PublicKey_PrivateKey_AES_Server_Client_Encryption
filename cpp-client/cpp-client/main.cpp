#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "files_util.h"
#include "structs.h"
#include "http_request.h"
#include <cstdint>




int main() 
{


    // TODO: add error handler
    client::transfer transfer = client::get_transfer();
    client::identyfier identyfier = client::get_identyfier();
    client::config::name = !identyfier.is_available ? transfer.name : transfer.name;
    /*
    client::request<client::request1025> request{};
    request.payload.set_name(client::config::name);

    //request.header.payload_size = htonl(name.length());
    //std::string serverResponse = client::SendMessageToServer(serverIP, serverPort, message);

    client::response register_request = client::send_request(transfer.ip_address, transfer.port, request);
    if (register_request.get_response_code() == client::response_code::response_error) 
    {
        std::cerr << "Message sending failed or no response received." << std::endl;
    }
    else 
    {
        std::cout << "Received: " << "ok" << std::endl;
        handel_response(register_request);
    }
    */


    std::vector<char> private_key{};
    std::vector<char> public_key{};

    client::generate_rsa_keys(private_key, public_key);

    std::cout << public_key.data() << std::endl;
    std::cout << public_key.size() << std::endl;

#if defined(_DEBUG)
    std::cout << "\n\nDebugging mode\nPress [Enter] to run main() again\n";
    std::cin.get();
    return main();
#endif // DEBUG
    return 0;
}
