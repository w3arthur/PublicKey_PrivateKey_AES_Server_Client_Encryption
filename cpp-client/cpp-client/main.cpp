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

    client::transfer transfer = client::get_transfer();
    client::identyfier identyfier = client::get_identyfier();
    if (!identyfier.is_available)
    {

    }

    //std::cout << client::SendHttpRequest("http://example.com/") << std::endl;


    std::string serverIP = "127.0.0.1"; // Change to your server's IP address
    std::string serverPort = "1234";   // Change to the server's listening port
    std::string name = "hi24242";

    client::request<client::request1025> request;
    strncpy_s(request.header.client_id, client::config::client_id, sizeof(request.header.client_id));
    //request.header.payload_size = htonl(name.length());

    strncpy_s(request.payload.name, name.c_str(), sizeof(request.payload.name));
    request.payload.name[name.size()/* - 1*/] = '\0';

    //std::string serverResponse = client::SendMessageToServer(serverIP, serverPort, message);

    client::response res = client::SendRequest(serverIP, serverPort, request);
    if (res.header.code != 0 ) { //!serverResponse.empty()
        std::cout << "Received: " << "ok" << std::endl;

        using namespace client;
        

        switch (res.header.code)
        {
        case client::register_success:
        {
            response_payload<response2100> payload{};

            
            const char* nullTerminatorPos = std::strchr(res.payload.data(), '\0');

            //auto aaa = std::find(payload.client_id, sizeof(payload), '\0');

            std::memcpy(&payload.client_id, res.payload.data(), sizeof(payload));
            payload.client_id[sizeof(payload) - 1] = '\0';

            std::cout << payload.client_id << std::endl;

            std::cin.get();
        }
        break;
        case register_fail:
        {
            response_payload<response2101> response{};
        }
        break;
        case public_key_received_sending_aes:
        {
            response_payload<response2102> response{};
        }
        break;
        case file_received_successfully_with_crc:
        {
            response_payload<response2103> response{};
        }
        break;
        case approval_message_receiving:
        {
            response_payload<response2104> response{};
        }
        break;
        case approval_reconnection_request_send_crypted_aes:
        {
            response_payload<response2105> response{};
        }
        break;
        case denined_reconnection_request_client_should_register_again:
        {
            response_payload<response2106> response{};
        }
        break;
        case global_server_error:
        {
            response_payload<response2107> response{};
        }
        break;
        }






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
