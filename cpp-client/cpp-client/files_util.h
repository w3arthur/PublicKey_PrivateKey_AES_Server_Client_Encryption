#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include "config.h"
#include "structs.h"

namespace client
{

    transfer get_transfer()
    {
        std::ifstream inputFile(config::transfer_info_file_name);
        if (!inputFile.is_open())
        {
            std::cerr << "Failed to open the file: " << config::transfer_info_file_name << std::endl;
            throw std::exception();
        }

        transfer transfer;
        std::string line;
        std::getline(inputFile, line);
        {
            std::istringstream ss(line);
            std::getline(ss, transfer.url, ':');
            std::getline(ss, line);
            transfer.port = std::stoi(line);
        }
        std::getline(inputFile, transfer.sender_name);
        std::getline(inputFile, transfer.file_address);

        inputFile.close();

        return transfer;
    }


    identyfier get_identyfier()
    {
        std::ifstream inputFile(config::me_info_file_name);
        if (!inputFile.good())
        {
            std::ofstream fileCreate(config::me_info_file_name);
            if (fileCreate.is_open())
            {
                fileCreate.close();
            }
            return {}; //identyfier.is_available = false;
        }
        if (!inputFile.is_open())
        {
            return {};  //identyfier.is_available = false;
        }

        identyfier identyfier;
        try
        {
            std::string line;
            std::getline(inputFile, identyfier.name);
            std::getline(inputFile, identyfier.unique);
            std::getline(inputFile, identyfier.private_key);
            identyfier.is_available = true;
        }
        catch (std::exception)
        {
            //identyfier.is_available = false;
        }

        inputFile.close();

        return identyfier;
    }


    void set_identyfier(const identyfier& identyfieri)
    {
        std::ofstream rsaKeyFile("info.me");
        rsaKeyFile << identyfieri.name << std::endl;
        rsaKeyFile << identyfieri.private_key << std::endl;
        rsaKeyFile << identyfieri.unique << std::endl;
        rsaKeyFile.close();
    }

    std::string get_file_content(const std::string& file_path)
    {

        std::ifstream fileStream(file_path, std::ios::binary);
        if (!fileStream.is_open()) {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return {};
        }
        std::string fileContents;
        fileContents.assign((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
        fileStream.close();
        return fileContents;
    }















}