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


    inline void handel_response(response& response)
    {
        switch (response.get_response_code())
        {
            case response_code::register_success:   //response2100
            {
                std::cout << "2100 Registered successfully\n";
                response_payload<response2100> payload;
                std::memcpy(&payload, response.payload.data(), sizeof(payload));
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
                std::memcpy(&payload, response.payload.data(), sizeof(payload));

                identifier identifier;
                identifier.name = config::name; //set inside main
                identifier.private_key = convert_vector_to_ascii_string(config::private_key);
                identifier.id = convert_uuid_bytes_to_string(config::client_id);
                set_identifier_write_to_file(identifier);


                std::string aes_encrypted_64(response.payload.begin() + config::client_id_size, response.payload.end());
                const std::string file_data =  read_data_from_file(config::read_from_file_name);

                request<request1028> request_send_a_file;
                request_send_a_file.payload.message_content =(encrypt_with_aes(file_data, aes_encrypted_64)) ;
                request_send_a_file.payload.content_size = static_cast<uint32_t>(request_send_a_file.payload.message_content.size());

                std::cout << "file content:" << std::endl;
                std::cout << file_data << std::endl;
                std::cout << ":end" << std::endl;
                config::file_checksum = calculate_numeric_checksum(file_data);
                config::file_content_size = request_send_a_file.payload.content_size;

                    // serialize custom_payload
                request_send_a_file.custom_payload.append(std::string(reinterpret_cast<char*>(&request_send_a_file.payload.content_size), sizeof(request_send_a_file.payload.content_size)));
                request_send_a_file.custom_payload.append(request_send_a_file.payload.message_content);
                client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_send_a_file);
                handel_response(res);
            }
            break;
            case response_code::file_received_successfully_with_crc:  //response2103
            {
                std::cout << "2103 Accepted receiving File with CRC\n";
                response_payload<response2103> payload;
                std::memcpy(&payload, response.payload.data(), sizeof(payload));

                


                if(config::file_content_size == payload.content_size && config::file_checksum == payload.cksum)
                {
                    request<request1029> request_inform_checksum_ok_filename;
                    strncpy_s(request_inform_checksum_ok_filename.payload.file_name, config::filename, sizeof(request_inform_checksum_ok_filename.payload.file_name));
                    auto aa2a = request_inform_checksum_ok_filename.payload.file_name;
                    client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_inform_checksum_ok_filename);
                    handel_response(res);
                }
                else
                {
                    if (config::is_login_attempt_available())
                    {
                        request<request1030> request_inform_wrong_crc;
                        strncpy_s(request_inform_wrong_crc.payload.file_name, config::filename, sizeof(request_inform_wrong_crc.payload.file_name));
                        auto aa1a = request_inform_wrong_crc.payload.file_name;
                        client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_inform_wrong_crc);
                        handel_response(res);
                    }
                    else
                    {
                        request<request1031> request_inform_wrong_crc_for_4th_time;
                        strncpy_s(request_inform_wrong_crc_for_4th_time.payload.file_name, config::filename, sizeof(request_inform_wrong_crc_for_4th_time.payload.file_name));
                        auto aa3a = request_inform_wrong_crc_for_4th_time.payload.file_name;
                        client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_inform_wrong_crc_for_4th_time);
                        handel_response(res);
                    }
                }



            }
            break;
            case response_code::approval_message_receiving:  //response2104
            {   //response to 1029 / 1031
                std::cout << "2104 Accepted receiving Thanks message\n";
                response_payload<response2104> payload;
            }
            break;
            case response_code::approval_reconnection_request_send_crypted_aes:  //response2105
            {
                std::cout << "2105 Accept registered attempt, send encrypted AES again\n";
                response_payload<response2105> payload;
            }
            break;
            case response_code::denined_reconnection_request_client_should_register_again:  //response2106
            {
                std::cerr << "2106 all requests to login failed, \n"
                    "The client is not registered or there is an issue with public key, \n"
                    "The client have to registered.\n";
                response_payload<response2106> payload;
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






