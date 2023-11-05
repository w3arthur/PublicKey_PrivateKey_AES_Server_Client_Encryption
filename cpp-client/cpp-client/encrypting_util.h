#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <utility>

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/crc.h>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <cryptopp/modes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/oaep.h>

#include "http_request.h"

namespace client
{


    uint32_t calculate_numeric_checksum(const std::string& data) {
        CryptoPP::SHA256 hash;
        std::vector<uint8_t> digest(CryptoPP::SHA256::DIGESTSIZE);

        // Calculate the SHA-256 hash of the input data
        hash.CalculateDigest(digest.data(), reinterpret_cast<const CryptoPP::byte*>(data.c_str()), data.size());

        // Extract the first 4 bytes of the hash as a uint32_t
        const uint32_t checksum = *(reinterpret_cast<uint32_t*>(digest.data()));

        return checksum;
    }
    /*
    inline uint32_t calculate_checksum(const std::string& data)
	{
        uint32_t checksum = 0;
        for (char c : data) {
            checksum = (checksum * 31) + static_cast<uint8_t>(c);  // Simple hash function
        }
        return checksum;
    }
    */

    inline std::string read_data_from_file(const std::string& file_path)
    {
        std::ifstream file(file_path);
        if (file) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            if(file.is_open())
            {
                file.close();
            }
            
            return content;
        }
        else {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            if (file.is_open())
            {
                file.close();
            }
            return "";
        }
    }



    inline void generate_rsa_keys(std::vector<unsigned char>& rsa_private_key, std::vector<unsigned char>& rsa_public_key)
    {
        if (rsa_private_key.empty() || rsa_public_key.empty()) 
        {
            CryptoPP::AutoSeededRandomPool rng;

            // Generate or load private key
            CryptoPP::RSA::PrivateKey privateKey;
            if (rsa_private_key.empty()) 
            {
                rsa_private_key.reserve(config::security_asymmetric_key_size);
                CryptoPP::InvertibleRSAFunction params;
                params.GenerateRandomWithKeySize(rng, config::security_asymmetric_key_size); // Set key size to 1024 bits
                privateKey = CryptoPP::RSA::PrivateKey(params);
                privateKey.Validate(rng, 3);

                // Save the private key to a vector
                CryptoPP::ByteQueue privateKeyQueue;
                privateKey.Save(privateKeyQueue);

                size_t privateKeySize = privateKeyQueue.MaxRetrievable();
                rsa_private_key.resize(privateKeySize);
                privateKeyQueue.Get(rsa_private_key.data(), privateKeySize);
            }
            else 
            {
                // Decode the loaded private key
                CryptoPP::ByteQueue privateKeyQueue;
                privateKeyQueue.Put(rsa_private_key.data(), rsa_private_key.size());
                privateKey.Load(privateKeyQueue);
            }

            // Generate or load public key
            CryptoPP::RSA::PublicKey publicKey;
            if (rsa_public_key.empty()) 
            {
                publicKey = CryptoPP::RSA::PublicKey(privateKey);
                publicKey.Validate(rng, 3);

                // Save the public key to a vector
                CryptoPP::ByteQueue publicKeyQueue;
                publicKey.Save(publicKeyQueue);

                size_t publicKeySize = publicKeyQueue.MaxRetrievable();
                rsa_public_key.resize(publicKeySize);
                publicKeyQueue.Get(rsa_public_key.data(), publicKeySize);
            }
            else 
            {
                // Decode the loaded public key
                CryptoPP::ByteQueue publicKeyQueue;
                publicKeyQueue.Put(rsa_public_key.data(), rsa_public_key.size());
                publicKey.Load(publicKeyQueue);
            }
        }
        else 
        {
            // Keys exist, no need to generate new ones
            std::cout << "RSA keys already exist." << std::endl;
        }
    }




    inline std::vector<unsigned char> decode_base64(const std::string& input)
	{
        std::string decoded;
        CryptoPP::StringSource
    	(
            input,
            true,
            new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded))
        );

        std::vector<unsigned char> decoded_string(decoded.begin(), decoded.end());
        return decoded_string;
    }


    inline std::vector<unsigned char> decrypt_aes_key(const std::vector<unsigned char>& rsa_private_key, const std::vector<unsigned char>& encrypted_aes_key)
    {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::RSA::PrivateKey privateKey;

        // Load the private RSA key from the vector
        CryptoPP::ByteQueue privateKeyQueue;
        privateKeyQueue.Put(rsa_private_key.data(), rsa_private_key.size());
        privateKey.Load(privateKeyQueue);

        // Create an RSAES_OAEP_SHA_Decryptor with the private key
        CryptoPP::RSAES_OAEP_SHA_Decryptor rsaDecryptor(privateKey);

        // Decode the base64-encoded encrypted AES key
        std::string encoded_aes_key(encrypted_aes_key.begin(), encrypted_aes_key.end());
        std::string decrypted_aes_key;

        try 
        {
            CryptoPP::StringSource
        	(
                encoded_aes_key, 
                true,
                new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decrypted_aes_key))
            );
        }
        catch (const std::exception& e) 
        {
            // Handle base64 decoding error
            std::cerr << "Error decoding base64: " << e.what() << std::endl;
            return std::vector<unsigned char>();
        }

        // Convert the decrypted AES key to a vector of unsigned char
        std::vector<unsigned char> decrypted_aes_key_bytes(decrypted_aes_key.begin(), decrypted_aes_key.end());

        return decrypted_aes_key_bytes;
    }




	inline std::string convert_vector_to_ascii_string(const std::vector<unsigned char>& privateKeyData)
	{
	    CryptoPP::ByteQueue privateKeyQueue;
	    privateKeyQueue.Put(privateKeyData.data(), privateKeyData.size());

	    std::string encodedPrivateKey;
	    CryptoPP::Base64URLEncoder base64Encoder;
	    privateKeyQueue.CopyTo(base64Encoder);
	    base64Encoder.MessageEnd();
	    
	    if (const CryptoPP::word64 size = base64Encoder.MaxRetrievable(); size) 
	    {
	        encodedPrivateKey.resize(size);
	        base64Encoder.Get(reinterpret_cast<CryptoPP::byte*>(&encodedPrivateKey[0]), encodedPrivateKey.size());
	    }
	    return encodedPrivateKey;
	}


    


    std::vector<unsigned char> decode_aes_key(const std::string& aesKeyBase64)
	{
        std::string encodedString(aesKeyBase64);

        // Replace URL-safe characters back to the base64 encoding
        std::replace(encodedString.begin(), encodedString.end(), '-', '+');
        std::replace(encodedString.begin(), encodedString.end(), '_', '/');

        // Calculate the required padding
        size_t padding = 4 - (encodedString.size() % 4);

        // Append padding characters if needed
        if (padding < 4) {
            encodedString.append(padding, '=');
        }

        std::string decodedString;
        CryptoPP::StringSource(encodedString, true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StringSink(decodedString)
            )
        );

        std::vector<unsigned char> aesKey(decodedString.begin(), decodedString.end());
        return aesKey;
    }




    std::string encrypt_with_aes(const std::string& plaintext, const std::string& aesKey)
    {
        // Use the AES key directly (no additional hashing)

        std::vector<unsigned char> aesKeyBin = decode_aes_key(aesKey);
        if(aesKeyBin.size() < 32) 
        { 
            aesKeyBin.resize(32);
        }
        CryptoPP::SecByteBlock key(reinterpret_cast<const CryptoPP::byte*>(aesKeyBin.data()), aesKeyBin.size());

        // Initialization Vector (IV) of all zeros (16 bytes)
        CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };

        // Create an AES encryption object with CBC mode
        CryptoPP::AES::Encryption aesEncryption(key, key.size());
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

        std::string ciphertextString;

        // Encrypt the plaintext
        CryptoPP::StringSource((CryptoPP::byte*)plaintext.data(), plaintext.size(), true,
            new CryptoPP::StreamTransformationFilter
            (
                cbcEncryption,
                new CryptoPP::Base64Encoder
                (
                    new CryptoPP::StringSink(ciphertextString),
                    false // Insert line breaks
                )
            )
        );

        return ciphertextString;
    }




    //TODO: check if requires

    inline void saveRSAKeysToFile(const CryptoPP::RSA::PublicKey& key, const std::string& filename)
    {
        CryptoPP::FileSink file(filename.c_str());
        key.DEREncode(file);
        file.MessageEnd();
    }

    inline void saveRSAKeysToFile(const CryptoPP::RSA::PrivateKey& key, const std::string& filename)
    {
        CryptoPP::FileSink file(filename.c_str());
        key.DEREncode(file);
        file.MessageEnd();
    }








}
