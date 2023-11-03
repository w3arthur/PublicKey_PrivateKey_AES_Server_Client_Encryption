#include <iostream>
#include <fstream>
#include <string>

#include "files_util.h"
#include "http_request.h"


int main() 
{
    using namespace client;

    // TODO: add error handler
    config::transfer = client::get_transfer_read_form_file();
    config::identyfier = client::get_identifier_read_form_file();
    config::name = !config::identyfier.is_available ? config::transfer.name : config::transfer.name;
   
    request<client::request1025> request;
    request.payload.set_name(config::name);

    response res = send_request(config::transfer.ip_address, config::transfer.port, request);
    handel_response(res);


#if defined(_DEBUG)
    std::cout << "\n\nDebugging mode\nPress [Enter] to run main() again\n";
    std::cin.get();
    return main();
#endif // DEBUG
    return 0;
}
