#pragma once
#include <iostream>
#include <exception>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link against the Winsock library

namespace client
{

    bool ParseUrl(const std::string& fullUrl, std::string& hostname, std::string& port, std::string& path);

    std::string SendHttpRequest(const std::string& fullUrl)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // Initialize Winsock
        {
            std::cerr << "WSAStartup failed." << std::endl;
            return {}; // Return an empty string on failure
        }

        std::string hostname, port, path;
        if (!ParseUrl(fullUrl, hostname, port, path))  // Parse the URL into components
        {
            WSACleanup();
            return {}; // Return an empty string on failure
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

        try
        {
            if (clientSocket == INVALID_SOCKET) // Create a socket
            {
                throw std::exception("Socket creation failed.");
            }

            struct addrinfo hints, * result = NULL;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result) != 0) // Resolve the server's IP address
            {
                throw std::exception("Address resolution failed.");
            }

            if (connect(clientSocket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR)     // Connect to the server
            {
                throw std::exception("Connection failed.");
            }

            std::string request = "GET " + path + " HTTP/1.1\r\n"
                "Host: " + hostname + "\r\n"
                "Connection: close\r\n\r\n";    // Send the HTTP request

            if (send(clientSocket, request.c_str(), static_cast<int>(request.length()), 0) == SOCKET_ERROR)
            {
                throw std::exception("Send failed.");
            }
        }
        catch (std::exception ex)
        {
            std::cerr << ex.what() << std::endl;
            if (clientSocket != INVALID_SOCKET)
            {
                closesocket(clientSocket);
            }
            WSACleanup();
            return {}; // Return an empty string on failure
        }


        // Receive and store the response in httpResponse
        char buffer[4096];
        int bytesRead;
        std::string httpResponse;
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
        {
            buffer[bytesRead] = '\0';
            httpResponse += buffer;
        }

        // Cleanup and close the socket
        closesocket(clientSocket);
        WSACleanup();

        return httpResponse; // Return the HTTP response
    }



    // Function to send a message to the server
    bool SendMessage(const std::string& serverIP, int port, const std::string& message) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock" << std::endl;
            return false;
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create socket" << std::endl;
            WSACleanup();
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, serverIP.c_str(), &(serverAddr.sin_addr));

        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Failed to connect to server" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        int bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Failed to send message to server" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        closesocket(clientSocket);
        WSACleanup();
        return true;
    }





    std::string SendMessageToServer(const std::string& serverIP, const std::string& serverPort, const std::string& message) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            return "";
        }

        // Create a socket
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            WSACleanup();
            return "";
        }

        // Resolve the server's IP address
        struct addrinfo hints, * result = NULL;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &result) != 0) {
            std::cerr << "Address resolution failed." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return "";
        }

        // Connect to the server
        if (connect(clientSocket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
            std::cerr << "Connection failed." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return "";
        }

        // Send the message to the server
        if (send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return "";
        }

        std::cout << "Sent: " << message << std::endl;

        // Receive and store the response from the server
        char buffer[1024];
        int bytesRead;
        std::string response = "";

        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }

        // Close the socket and cleanup
        closesocket(clientSocket);
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