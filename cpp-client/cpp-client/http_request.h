#pragma once
#include <iostream>
#include <exception>
#include <ws2tcpip.h>
#include <cstring>
#include "json_utils.h"
#include "structs.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link against the Winsock library

#include "encrypting_util.h"
#include <algorithm>



std::vector<char> createSubVector(const std::vector<char>& dataVector, size_t start, size_t end) {
    // Check if the start and end indices are valid
    if (start >= dataVector.size() || start >= end) {
        return std::vector<char>(); // Return an empty vector for invalid indices
    }

    end = min(end, dataVector.size()); // Ensure end is within bounds

    // Create a new std::vector<char> from the specified range
    std::vector<char> result(dataVector.begin() + start, dataVector.begin() + end);

    return result;
}



namespace client
{

    template <class Request_Class>
    response send_request(const std::string& serverIP, unsigned short int serverPort, request<Request_Class>& request);


    void handel_response(response& response)
    {
        switch (response.get_response_code())
        {
            case response_code::register_success:   //response2100
            {
                std::cout << "2100 Registered successfully\n";
                response_payload<response2100> payload;
                std::memcpy(&payload.client_id, response.payload.data(), response.header.payload_size);   //16
                

                config::set_client_id(payload.client_id, sizeof(payload.client_id));

                request<request1026> request_send_public_key{};
                std::vector<unsigned char> public_key{};
                client::generate_rsa_keys(config::private_key, public_key);
                request_send_public_key.payload.set_name(config::name);
                request_send_public_key.payload.set_public_key(public_key.data());
                client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_send_public_key);
                handel_response(res);
            }
            break;
            case response_code::register_fail:  //response2101
            {
                std::cerr << "user name: '" << config::name << "' already exist\n End the program.\n";
            }
            break;
            case response_code::public_key_received_sending_aes:  //response2102
            {
                std::cout << "2102 Accepted, Public key received, sending encrypted AES\n";
                response_payload<response2102> payload;
                std::memcpy(&payload.client_id, response.payload.data(), 16);

                
                //response.payload.aes_key = std::vector<unsigned char>(response.payload.begin() + 16, response.payload.end());


                std::vector<unsigned char> aes(response.payload.begin() + 16, response.payload.end());
                std::vector<char> file_data =  read_data_from_file("file.txt");
                auto file_aes = encrypt_with_aes(file_data, aes);
                auto dfile = dencrypt_with_aes(file_aes, aes);
                //auto file_check_sum = calculate_crc32(file_data);
               // auto aaa = encryptFileWithAES(payload.aes_key, "file.txt");

                request<request1028> request_send_a_file{};
                //request_send_a_file.payload.set_content_size(aaa.first);
                //request_send_a_file.payload.
                //client::response res = send_request(config::transfer.ip_address, config::transfer.port, request_send_public_key);
                //handel_response(res);
                //payload.aes_key.resize(response.header.payload_size);
                //std::memcpy(&payload.aes_key, response.payload.data() + 16, response.header.payload_size);
                //payload.aes_key = a;


            }
            break;
            case response_code::file_received_successfully_with_crc:
            {
                std::cout << "2103 Accepted receiving File with CRC\n";
                response_payload<response2103> response{};
            }
            break;
            case response_code::approval_message_receiving:
            {   //response to 1029 / 1031
                std::cout << "2104 Accepted receiving Thanks message\n";
                response_payload<response2104> response{};
            }
            break;
            case response_code::approval_reconnection_request_send_crypted_aes:
            {
                std::cout << "2105 Accept recconection attempt, send encrypted AES again\n";
                response_payload<response2105> response{};
            }
            break;
            case response_code::denined_reconnection_request_client_should_register_again: // 106
            {
                std::cerr << "2106 all requests to loging failed, \n"
                    "The client is not registerd or there is an issue with public key, \n"
                    "The client have to reregister.\n";
                response_payload<response2106> response{};
            }
            break;
            case response_code::global_server_error:    // 2107
                //response_payload<response2107> response{};
            //break; // fall through
            default:
            {
                std::cerr << "2107 Global error that can't handle in any case before.\n";
            }
            break;
        }
    }




    bool ParseUrl(const std::string& fullUrl, std::string& hostname, std::string& port, std::string& path);


    template <class Request_Class>
    response send_request(const std::string& serverIP, unsigned short int serverPort, request<Request_Class>& request) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            return {};
        }

        // Create a socket
        SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            WSACleanup();
            return {};
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

        // Connect to the server
        if (connect(client_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Connection to server failed." << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return {};
        }

        // Prepare the request header
        //RequestHeader header;

        // Send the header to the server

        request.header.payload_size = sizeof(request.payload);
        if (send(client_socket, reinterpret_cast<char*>(&request.header), sizeof(request.header), 0) == SOCKET_ERROR) {
            std::cerr << "Header send failed." << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return {};
        }

        // Send the payload (name) to the server
        if (send(client_socket, reinterpret_cast<char*>(&request.payload), request.header.payload_size, 0) == SOCKET_ERROR) {
            std::cerr << "Payload send failed." << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return {};
        }

        // Receive data from the server
        std::vector<char> received_data(1024); // Adjust buffer size as needed
        int bytes_received = recv(client_socket, received_data.data(), received_data.size(), 0);

        if (bytes_received == SOCKET_ERROR) {
            std::cerr << "Error receiving data." << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return {};
        }

        // Resize the received data buffer to the actual size received
        received_data.resize(bytes_received);
        // Deserialize the header
        if (received_data.size() < sizeof(response_header)) {
            std::cerr << "Received data is too small for the header." << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return {};
        }

        response response(received_data);

        // Extract the payload based on the payload_size from the header
        if (received_data.size() < sizeof(response_header) + response.header.payload_size) {
            std::cerr << "Received data is too small for the payload." << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return {};
        }

        // Payload payload = DeserializePayload(received_data);
        response.payload = std::vector<char>(received_data.begin() + sizeof(response_header), received_data.begin() + sizeof(response_header) + response.header.payload_size);

        // Close the socket and cleanup
        closesocket(client_socket);
        WSACleanup();

        return response;
    }






    bool ParseUrl(const std::string& fullUrl, std::string& hostname, std::string& port, std::string& path)
    {
        try
        {
            size_t pos = fullUrl.find("://");
            if (pos == std::string::npos)
            {
                throw std::exception("Invalid URL format.");
            }

            std::string urlWithoutProtocol = fullUrl.substr(pos + 3);
            pos = urlWithoutProtocol.find('/');     // Find the first '/' after "://"
            if (pos == std::string::npos)
            {
                throw std::exception("Invalid URL format.");
            }

            hostname = urlWithoutProtocol.substr(0, pos);
            size_t colonPos = hostname.find(':');   // Check if the port is specified
            if (colonPos != std::string::npos)
            {
                port = hostname.substr(colonPos + 1);
                hostname = hostname.substr(0, colonPos);
            }
            else
            {
                port = "80"; // Default HTTP port
            }

            path = urlWithoutProtocol.substr(pos);
            if (path.empty())
            {
                path = "/";
            }
        }
        catch (std::exception ex)
        {
            std::cerr << ex.what() << std::endl;
            return false;
        }

        return true;
    }



}






