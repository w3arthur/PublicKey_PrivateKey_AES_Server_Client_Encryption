#include <iostream>
#include <fstream>

#include "http_request.h"


int main() 
{

    client::client_handle_response_begin();

    std::cout << "\n Main running is done.\n";

#if defined(_DEBUG)
    std::cout << "\n\nDebugging mode\nPress [Enter] to run main() again\n";
    std::cin.get();
    client::config::config_reset();
    //client::delete_file(client::config::rsa_private_key_file);
    //client::delete_file(client::config::me_info_file_name);
    return main();
#endif // DEBUG
    std::cout << "Press enter to exit \n";
    std::cin.get();
    return 0;
}
