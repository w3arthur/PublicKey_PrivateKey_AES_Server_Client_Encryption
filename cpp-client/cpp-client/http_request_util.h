#pragma once
#include <iostream>
#include <exception>
#include <ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link against the Win sock library
#include "structs.h"
//#include "config.h"



namespace client
{
	

	inline bool parse_url(const std::string& fullUrl, std::string& hostname, std::string& port, std::string& path);


	template <class Request_Class>
	response send_request(const std::string& serverIP, unsigned short int serverPort, request<Request_Class>& request, const uint32_t payload_custom_size) // = 0
	{
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

	    // Send the header to the server
	    request.header.payload_size = payload_custom_size == 0 ? sizeof(request.payload) : payload_custom_size; // send payload_custom_size if sett
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
	    const int bytes_received = recv(client_socket, received_data.data(), received_data.size(), 0);

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





	inline bool parse_url(const std::string& fullUrl, std::string& hostname, std::string& port, std::string& path)
	{
	    try
	    {
	        size_t pos = fullUrl.find("://");
	        if (pos == std::string::npos)
	        {
	            throw std::exception("Invalid URL format.");
	        }

	        std::string url_without_protocol = fullUrl.substr(pos + 3);
	        pos = url_without_protocol.find('/');     // Find the first '/' after "://"
	        if (pos == std::string::npos)
	        {
	            throw std::exception("Invalid URL format.");
	        }

	        hostname = url_without_protocol.substr(0, pos);
	        const size_t colonPos = hostname.find(':');   // Check if the port is specified
	        if (colonPos != std::string::npos)
	        {
	            port = hostname.substr(colonPos + 1);
	            hostname = hostname.substr(0, colonPos);
	        }
	        else
	        {
	            port = "80"; // Default HTTP port
	        }

	        path = url_without_protocol.substr(pos);
	        if (path.empty())
	        {
	            path = "/";
	        }
	    }
	    catch (const std::exception& ex)
	    {
	        std::cerr << ex.what() << std::endl;
	        return false;
	    }

	    return true;
	}

}

