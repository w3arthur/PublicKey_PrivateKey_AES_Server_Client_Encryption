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




    inline std::vector<char> read_data_from_file(const std::string& file_path)
    {
        std::ifstream input_file(file_path, std::ios::binary);
        if (!input_file.is_open()) 
        {
            return {};
        }

        input_file.seekg(0, std::ios::end);
        const size_t file_size = input_file.tellg();
        input_file.seekg(0, std::ios::beg);

        std::vector<char> file_data(file_size);
        input_file.read(file_data.data(), file_size);
        input_file.close();

        return file_data;
    }



    inline void generate_rsa_keys(std::vector<unsigned char>& rsa_private_key, std::vector<unsigned char>& rsa_public_key)
    {
        if (rsa_private_key.empty() || rsa_public_key.empty()) {
            CryptoPP::AutoSeededRandomPool rng;

            // Generate or load private key
            CryptoPP::RSA::PrivateKey privateKey;
            if (rsa_private_key.empty()) {
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
            else {
                // Decode the loaded private key
                CryptoPP::ByteQueue privateKeyQueue;
                privateKeyQueue.Put(rsa_private_key.data(), rsa_private_key.size());
                privateKey.Load(privateKeyQueue);
            }

            // Generate or load public key
            CryptoPP::RSA::PublicKey publicKey;
            if (rsa_public_key.empty()) {
                publicKey = CryptoPP::RSA::PublicKey(privateKey);
                publicKey.Validate(rng, 3);

                // Save the public key to a vector
                CryptoPP::ByteQueue publicKeyQueue;
                publicKey.Save(publicKeyQueue);

                size_t publicKeySize = publicKeyQueue.MaxRetrievable();
                rsa_public_key.resize(publicKeySize);
                publicKeyQueue.Get(rsa_public_key.data(), publicKeySize);
            }
            else {
                // Decode the loaded public key
                CryptoPP::ByteQueue publicKeyQueue;
                publicKeyQueue.Put(rsa_public_key.data(), rsa_public_key.size());
                publicKey.Load(publicKeyQueue);
            }
        }
        else {
            // Keys exist, no need to generate new ones
            std::cout << "RSA keys already exist." << std::endl;
        }
    }




    std::vector<unsigned char> decode_base64(const std::string& input)
	{
        std::string decoded;
        CryptoPP::StringSource(input, true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StringSink(decoded)
            )
        );

        return std::vector<unsigned char>(decoded.begin(), decoded.end());
    }


    std::vector<unsigned char> decrypt_aes_key(const std::vector<unsigned char>& rsa_private_key, const std::vector<unsigned char>& encrypted_aes_key)
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

        try {
            CryptoPP::StringSource(encoded_aes_key, true,
                new CryptoPP::Base64Decoder(
                    new CryptoPP::StringSink(decrypted_aes_key)
                )
            );
        }
        catch (const std::exception& e) {
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



    inline CryptoPP::SecByteBlock vector_to_sec_block(const std::vector<unsigned char>& vec);
    std::string encrypt_with_aes(const std::vector<char>& plaintext, const std::string& aesKey)
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
            new CryptoPP::StreamTransformationFilter(cbcEncryption,
                new CryptoPP::Base64Encoder(
                    new CryptoPP::StringSink(ciphertextString),
                    false // Insert line breaks
                )
            )
        );

        return ciphertextString;
    }



    inline CryptoPP::SecByteBlock vector_to_sec_block(const std::vector<unsigned char>& vec)
    {
        CryptoPP::SecByteBlock secBlock(vec.data(), vec.size());
        return secBlock;
    }







    //TODO: to clean

    inline  std::string calculate_crc32(const std::vector<char>& data)
    {
        std::string result;
        CryptoPP::CRC32 hash;
        CryptoPP::FileSource(data.data(), true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::StringSink(result)));
        std::cout << "crc '" << result << "'" << std::endl;
        return result;
    }


    // Function to calculate MD5 checksum
    inline std::string calculateChecksum(const std::string& data)
    {
        CryptoPP::Weak1::MD5 md5;
        CryptoPP::byte digest[CryptoPP::Weak1::MD5::DIGESTSIZE];

        md5.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(data.c_str()), data.size());

        CryptoPP::HexEncoder encoder;
        std::string checksum;

        encoder.Attach(new CryptoPP::StringSink(checksum));
        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();

        return checksum;
    }

    //string rsaPrivateKey;
    //string rsaPublicKey;
    //string clientName = "YourClientName"; // Replace with the actual client name

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
