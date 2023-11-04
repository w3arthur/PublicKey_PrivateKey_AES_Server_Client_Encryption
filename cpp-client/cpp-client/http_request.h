#pragma once
//#include <iostream>
#include "encrypting_util.h"
#include "http_request_util.h"
#include "files_util.h"
#include "uuid_util.h"
//#include "structs.h"
//#include "config.h"
namespace client
{

    template <class Request_Class>
    response send_request(const std::string& serverIP, unsigned short int serverPort, request<Request_Class>& request, const uint32_t payload_custom_size = 0);

    inline void handel_response(response& response)
    {
        switch (response.get_response_code())
        {
            case response_code::register_success:   //response2100
            {
                std::cout << "2100 Registered successfully\n";
                response_payload<response2100> payload;
                std::memcpy(&payload.client_id, response.payload.data(), config::client_id_size);   //response.header.payload_size

                config::set_client_id(payload.client_id);   //sizeof(payload.client_id)

                request<request1026> request_send_public_key;
                std::vector<unsigned char> public_key;
                generate_rsa_keys(config::private_key, public_key);
                request_send_public_key.payload.set_name(config::name);
                request_send_public_key.payload.set_public_key(public_key.data());
                client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_send_public_key);
                handel_response(res);
            }
            break;
            case response_code::register_fail:  //response2101
            {
                    //TODO: create x3 time try
                std::cerr << "user name: '" << config::name << "' already exist\n End the program.\n"
                    "Server not response.\n";
            }
            break;
            case response_code::public_key_received_sending_aes:  //response2102
            {
                std::cout << "2102 Accepted, Public key received, sending encrypted AES\n";
                response_payload<response2102> payload;
                
                
                std::memcpy(&payload.client_id, response.payload.data(), config::client_id_size); //identifier.id set inside response2100

                identifier identifier;
                identifier.name = config::name; //set inside main
                identifier.private_key = convert_vector_to_ascii_string(config::private_key);

                identifier.id = convert_uuid__bytes_to_string(config::client_id);
                ////TODO: fix, bytes_to_hex_string, 
                
                set_identifier_write_to_file(identifier);

                char new_client_id[16];
                convert_uuid_string_to_bytes(config::identyfier.id, new_client_id);


                std::string aes_encrypted_64(response.payload.begin() + config::client_id_size, response.payload.end());
                auto aes_encrypted = decode_base64(aes_encrypted_64);




                const std::vector<char> file_data =  read_data_from_file(config::read_from_file_name);



                //std::vector<unsigned char> aaaaaa(response.payload.begin() + config::client_id_size, response.payload.end());
                //auto aaa = decrypt_with_aes(aaaaaa, identifier.private_key);

                //const std::vector<unsigned char> aes = decrypt_aes_key(config::private_key, aaa);

                request<request1028> request_send_a_file;
                request_send_a_file.payload.message_content = encrypt_with_aes(file_data, aes_encrypted_64);

                //auto dfile = decrypt_with_aes(request_send_a_file.payload.message_content, aes_encrypted_64);
                request_send_a_file.payload.content_size = static_cast<uint32_t>(request_send_a_file.payload.message_content.size());
                const uint32_t payload_custom_size = request_send_a_file.payload.message_content.size() + static_cast<uint32_t>(sizeof(request_send_a_file.payload.content_size));
                client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_send_a_file, payload_custom_size);
                    //TODO: handle response
                handel_response(res);
            }
            break;
            case response_code::file_received_successfully_with_crc:  //response2103
            {
                std::cout << "2103 Accepted receiving File with CRC\n";
                response_payload<response2103> response{};
            }
            break;
            case response_code::approval_message_receiving:  //response2104
            {   //response to 1029 / 1031
                std::cout << "2104 Accepted receiving Thanks message\n";
                response_payload<response2104> response{};
            }
            break;
            case response_code::approval_reconnection_request_send_crypted_aes:  //response2105
            {
                std::cout << "2105 Accept registered attempt, send encrypted AES again\n";
                response_payload<response2105> response{};
            }
            break;
            case response_code::denined_reconnection_request_client_should_register_again:  //response2106
            {
                std::cerr << "2106 all requests to login failed, \n"
                    "The client is not registered or there is an issue with public key, \n"
                    "The client have to registered.\n";
                response_payload<response2106> response{};
            }
            break;
            case response_code::global_server_error:  //response2107
            // fall through
            default:
            {
                //response_payload<response2107> response{};
                std::cerr << "2107 Global error that can't handle in any case before.\n"
            		"Try restart the server.\n";
            }
            break;
        }
    }


}






