#pragma once
#include "encrypting_util.h"
#include "http_request_util.h"
#include "files_util.h"
#include "uuid_util.h"
//#include "structs.h"
//#include "config.h"
namespace client
{
    inline void handel_response(response& response);

    inline void client_handle_response_begin()
    {
        config::identyfier = get_identifier_read_form_file();
        config::transfer = get_transfer_read_form_file();
        const bool is_private_key_file = is_file_exists(config::rsa_private_key_file);

        if (config::identyfier.is_available)
        {
            config::name = config::identyfier.name;
            config::set_client_id(uuid_string_to_bytes(config::identyfier.id).c_str());

            auto aa =config::client_id;
        }
        else
        {
            config::name = config::transfer.name;
        }

        if (is_file_exists(config::rsa_private_key_file) && config::identyfier.is_available)
        {
            std::cerr << "The user registered in the past, request aes key, the prv.key and me.info files available\n";
            request<request1027> request_for_aes_key;
            request_for_aes_key.payload.set_name(config::name);
            response res = send_request(config::transfer.ip_address, config::transfer.port, request_for_aes_key);
            handel_response(res);
        }

        else
        {
            request<request1025> request_login;
            request_login.payload.set_name(config::name);
            response res = send_request(config::transfer.ip_address, config::transfer.port, request_login);

            while (res.get_response_code() == response_code::register_fail && config::is_login_attempt_available() - 1) //2
            {
                std::cerr << "2100 Accept registered attempt, try to login again\n";
                request<request1025> request_login_attempt;
                request_login_attempt.payload.set_name(config::name);
                res = send_request(config::transfer.ip_address, config::transfer.port, request_login_attempt);
                handel_response(res);
            }

            handel_response(res);   // response_code::register_fail  //response2101}
        }
    }

    inline void handel_response(response& response)
    {
        switch (response.get_response_code())
        {
            case response_code::register_success:   //response2100
	            {
	                std::cout << "2100 Registered successfully, send the public key\n";
	                response_payload<response2100> payload;
	                std::memcpy(&payload, response.payload.data(), sizeof(payload));
	                config::set_client_id(payload.client_id);   //sizeof(payload.client_id)

	                request<request1026> request_send_public_key;
	                std::vector<unsigned char> public_key;
	                std::vector<unsigned char> private_key;
	                generate_rsa_keys(private_key, public_key);
	                save_rsa_private_keys_to_file(private_key, config::rsa_private_key_file);
	                request_send_public_key.payload.set_name(config::name);
	                request_send_public_key.payload.set_public_key(public_key.data());
	                client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_send_public_key);
	                handel_response(res);
	            } break;
            case response_code::register_fail:
	            {
		            std::cerr << "user name: '" << config::name << "' failed registration for the 4th time or the name is exist in the database\n\n End the program.\n";
	            } break;   //response2101  //Finish the program
            case response_code::public_key_received_sending_aes:  //response2102
	            {
	                std::cout << "2102 Accepted, sending encrypted AES\n";
	                //response_payload<response2102> payload2102;
	                //std::memcpy(&payload2102, response.payload.data(), sizeof(payload2102));  //not in use
	                std::vector<unsigned char> private_key = load_rsa_private_key_from_file(config::rsa_private_key_file); //config::private_key;

	                    //store identifier inside a file
	                identifier identifier;
	                identifier.name = config::name; //set inside main
	                identifier.private_key = convert_vector_to_ascii_string(private_key);
	                identifier.id = convert_uuid_bytes_to_string(config::client_id);
	                set_identifier_write_to_file(identifier);

	                std::vector<unsigned char> aes_encrypted(response.payload.begin() + sizeof(config::client_id), response.payload.end());

	                std::vector<unsigned char> decrypted_aes_byte = decrypt_aes_key(private_key, aes_encrypted);
	                std::string decrypted_aes = convert_vector_to_base64(decrypted_aes_byte);
	                const std::string file_data = read_data_from_file(config::transfer.file_address);

	                // 3 times retry to send the encrypt with aes
	                client::response res;
            		do
	                {
	                    request<request1028> request_send_a_file;
	                    request_send_a_file.payload.set_file_name(config::transfer.file_address);
	                    request_send_a_file.payload.message_content = encrypt_with_aes(file_data, decrypted_aes);
	                    // serialize serialized_payload
	                    serialize_request1028(request_send_a_file, config::prv_file_content_size);

	                    config::prv_file_checksum = calculate_numeric_checksum(file_data);   //set checksum for next 2103 response 

	                    res = send_request(config::transfer.ip_address, config::transfer.port, request_send_a_file);

	                    if(res.get_response_code() == response_code::file_received_successfully_with_crc)
	                    {
                    		std::cout << "2103 Accepted receiving File with CRC\n";
                    		response_payload<response2103> payload2103;
                    		std::memcpy(&payload2103, res.payload.data(), sizeof(payload2103));

                    		//payload.client_id

                    		//compare size and check sum
                    		if (config::prv_file_content_size == payload2103.content_size && config::prv_file_checksum == payload2103.cksum)
                    		{
	                            std::cout << "Checksum matches\n";
                    			request<request1029> request_inform_checksum_ok_filename;
                    			request_inform_checksum_ok_filename.payload.set_file_name(config::transfer.file_address);
                    			res = send_request(config::transfer.ip_address, config::transfer.port, request_inform_checksum_ok_filename);
	                            break;  //handel_response(res); 
                    		}
                    		else if(config::is_file_send_attempt_available())
                    		{
	                            std::cout << "Checksum not matches try to encrypt and calculate it again\n";
	                            std::cerr << "2105 Accept registered attempt, send encrypted AES again\n";
                    			request<request1030> request_inform_wrong_crc;
                    			request_inform_wrong_crc.payload.set_file_name(config::transfer.file_address);
                    			res = send_request(config::transfer.ip_address, config::transfer.port, request_inform_wrong_crc);
                    			// don't handel_response(res);
                    		}
	                        else
	                        {
	                            std::cout << "Cant get right checksum for the 4th time\n";
	                            request<request1031> request_inform_wrong_crc_for_4th_time;
	                            request_inform_wrong_crc_for_4th_time.payload.set_file_name(config::transfer.file_address);
	                            res = send_request(config::transfer.ip_address, config::transfer.port, request_inform_wrong_crc_for_4th_time);
	                            break;  //handel_response(res); 
	                        }
	                    }
	                    else
	                    {
	                        break;
	                    }
	                } while (res.get_response_code() != response_code::approval_message_receiving);

            		handel_response(res); // response_code::approval_message_receiving or approval_message_receiving
		            
	            } break;
            case response_code::file_received_successfully_with_crc: {} break;  //response2103 //will retry inside response2102
            case response_code::approval_message_receiving:
	            {
		            std::cout << "2104 Accepted receiving Thanks message\n";
	            } break;   //response2104  //response to 1029, 1030, 1031  //Finish the program
            case response_code::approval_reconnection_request_send_crypted_aes: {} break; //response2105 (same as response2102)//will retry inside response2102
            case response_code::denied_reconnection_request_client_should_register_again:  //response2106
	            {
	                std::cerr << "2106 The client is not registered or there is an issue with public key, \n"
	                    "The client have to register again.\n";
	            } break;       // Finish the program, will return from response2102
            case response_code::global_server_error:  //response2107  // fall through
              default:
            {
                  std::cerr << "2107 Global error.\n"
                      "Try restart the server.\n";
            } break;   // Finish the program, Global error
        }
    }

}






