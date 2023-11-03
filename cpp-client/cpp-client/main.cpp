#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "files_util.h"
//#include "structs.h"
//#include "config.h"
#include "http_request.h"



int main() 
{
    using namespace client;

    // TODO: add error handler
    config::transfer = client::get_transfer();
    config::identyfier = client::get_identyfier();
    client::config::name = !config::identyfier.is_available ? config::transfer.name : config::transfer.name;
   
    client::request<client::request1025> request{};
    request.payload.set_name(client::config::name);

    client::response res = client::send_request(config::transfer.ip_address, config::transfer.port, request);
    handel_response(res);
    



#if defined(_DEBUG)
    std::cout << "\n\nDebugging mode\nPress [Enter] to run main() again\n";
    std::cin.get();
    return main();
#endif // DEBUG
    return 0;
}
