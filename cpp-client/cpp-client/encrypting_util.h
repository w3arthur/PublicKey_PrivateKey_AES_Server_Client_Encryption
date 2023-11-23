#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include "config.h"

namespace client
{

    std::string uuid_string_to_bytes(const std::string& uuid_string) {

        std::string str;
        CryptoPP::StringSource ss(uuid_string, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::StringSink(str)
            )
        );

        return str;
    }


    uint32_t calculate_numeric_checksum(const std::string& data)
	{
        CryptoPP::SHA256 hash;
        std::vector<uint8_t> digest(CryptoPP::SHA256::DIGESTSIZE);

        // Calculate the SHA-256 hash of the input data
        hash.CalculateDigest(digest.data(), reinterpret_cast<const CryptoPP::byte*>(data.c_str()), data.size());

        // Extract the first 4 bytes of the hash as a uint32_t
        const uint32_t checksum = *(reinterpret_cast<uint32_t*>(digest.data()));
        return checksum;
    }

    inline std::vector<unsigned char> decode_to_base64(const std::string& input)
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

    inline std::string convert_vector_to_base64(const std::vector<unsigned char>& privateKeyData)   //with = in the end
	{
        CryptoPP::ByteQueue privateKeyQueue;
        privateKeyQueue.Put(privateKeyData.data(), privateKeyData.size());

        std::string encodedPrivateKey;
        CryptoPP::Base64Encoder base64Encoder(new CryptoPP::StringSink(encodedPrivateKey), false);
        privateKeyQueue.CopyTo(base64Encoder);
        base64Encoder.MessageEnd();

        return encodedPrivateKey;
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

    inline void save_rsa_private_keys_to_file(const std::vector<unsigned char>& rsa_private_key, const char* filename)
    {
        CryptoPP::FileSink file(filename);

        // Decode the private key from the vector and save it to the file
        CryptoPP::ByteQueue privateKeyQueue;
        privateKeyQueue.Put(rsa_private_key.data(), rsa_private_key.size());

        CryptoPP::RSA::PrivateKey privateKey;
        privateKey.Load(privateKeyQueue);

        privateKey.DEREncode(file);
        file.MessageEnd();
    }

    inline std::vector<unsigned char> load_rsa_private_key_from_file(const std::string& filename)
	{
        std::vector<unsigned char> rsa_private_key;
        CryptoPP::FileSource file(filename.c_str(), true);
        size_t keySize = file.MaxRetrievable();
        rsa_private_key.resize(keySize);
        file.Get(rsa_private_key.data(), keySize);
        return rsa_private_key;
    }

    std::vector<unsigned char> decrypt_aes_key(const std::vector<unsigned char>& rsa_private_key1, const std::vector<unsigned char>& encrypted_aes_key) {
        // Convert the encoded RSA private key to a std::string
        std::string rsa_private_key(rsa_private_key1.begin(), rsa_private_key1.end());

        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::RSAES_OAEP_SHA256_Decryptor decryptor; // Use SHA-256

        // Load the private RSA key from the std::string
        CryptoPP::ByteQueue privateKeyQueue;
        privateKeyQueue.Put(reinterpret_cast<const CryptoPP::byte*>(rsa_private_key.data()), rsa_private_key.size());
        decryptor.AccessKey().Load(privateKeyQueue);

        // Decrypt the AES key
        std::string aes_key;
        CryptoPP::StringSource(encrypted_aes_key.data(), encrypted_aes_key.size(), true,
            new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::StringSink(aes_key)
            ));

        std::vector<unsigned char> aes_key_vector(aes_key.begin(), aes_key.end());
        return aes_key_vector;
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

}
