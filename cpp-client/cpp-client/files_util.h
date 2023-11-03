#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include "config.h"

namespace client
{
    inline transfer get_transfer_read_form_file()
    {
        std::ifstream input_file(config::transfer_info_file_name);
        if (!input_file.is_open())
        {
            std::cerr << "Failed to open the file: " << config::transfer_info_file_name << std::endl;
            throw std::exception();
        }

        transfer transfer;
        std::string line;
        std::getline(input_file, line);
        {
            std::istringstream ss(line);
            std::getline(ss, transfer.ip_address, ':');
            std::getline(ss, line);
            transfer.port = static_cast<unsigned short int>(std::stoul(line));
        }
        std::getline(input_file, transfer.name);
        std::getline(input_file, transfer.file_address);

        input_file.close();

        return transfer;
    }


    inline identifier get_identifier_read_form_file()
    {
        std::ifstream input_file(config::me_info_file_name);
        // check if file exist
        if (!input_file.good())
        {
            std::ofstream fileCreate(config::me_info_file_name);
            if (fileCreate.is_open())
            {
                fileCreate.close();
            }
            return {};
        }
        if (!input_file.is_open())
        {
            return {};
        }

        identifier identifier;
        try
        {
            std::getline(input_file, identifier.name);
            std::getline(input_file, identifier.id);
            std::getline(input_file, identifier.private_key);
            identifier.is_available = true;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "file '" << config::me_info_file_name << "' reading error.\n" << ex.what() << "\n";
        }

        input_file.close();

        return identifier;
    }


    inline void set_identifier_write_to_file(const identifier& identifier)
    {
        std::ofstream me_info_file(config::me_info_file_name);
        me_info_file << identifier.name << std::endl;
        me_info_file << identifier.private_key << std::endl;
        me_info_file << identifier.id << std::endl;
        me_info_file.close();
    }



    /*
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
    */














}