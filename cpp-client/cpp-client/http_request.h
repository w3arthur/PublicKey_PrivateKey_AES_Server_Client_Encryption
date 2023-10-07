#pragma once
#include <iostream>
#include <exception>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include "json_utils.h"
#include "structs.h"
#pragma comment(lib, "ws2_32.lib") // Link against the Winsock library

namespace client
{

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



    void handel_response(response& response)
    {
        switch (response.get_response_code())
        {
        case response_code::register_success:
        {
            response_payload<response2100> payload(response);
            std::memcpy(&payload, response.payload.data(), response.header.payload_size);
            payload.client_id[sizeof(payload) - 1] = '\0';

            config::set_client_id(payload.client_id);

            std::cout << config::client_id << std::endl;
            std::cout << sizeof(config::client_id) << std::endl;
            //client::request<client::request1026> request{};
            //request.payload.set_name(config::name);


        }
        break;
        case response_code::register_fail:
        {
            //retry counter
            exit(1);
            //response_payload<response2101> response{};
        }
        break;
        case response_code::public_key_received_sending_aes:
        {
            response_payload<response2102> response{};
        }
        break;
        case response_code::file_received_successfully_with_crc:
        {
            response_payload<response2103> response{};
        }
        break;
        case response_code::approval_message_receiving:
        {
            response_payload<response2104> response{};
        }
        break;
        case response_code::approval_reconnection_request_send_crypted_aes:
        {
            response_payload<response2105> response{};
        }
        break;
        case response_code::denined_reconnection_request_client_should_register_again:
        {
            response_payload<response2106> response{};
        }
        break;
        case response_code::global_server_error:
        {
            response_payload<response2107> response{};
        }
        break;
        }



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