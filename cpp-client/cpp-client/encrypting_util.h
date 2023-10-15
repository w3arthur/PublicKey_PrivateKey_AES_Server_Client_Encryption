#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <cryptopp/modes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>


namespace client
{

    // Function for AES encryption

    std::string encrypt_aes(const std::string& plaintext, const CryptoPP::SecByteBlock& key, const CryptoPP::SecByteBlock& iv)
    {
        try
        {
            std::string ciphertext;
            CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
            encryptor.SetKeyWithIV(key, key.size(), iv);

            CryptoPP::StringSource(plaintext, true,
                new CryptoPP::StreamTransformationFilter(encryptor,
                    new CryptoPP::StringSink(ciphertext)
                )
            );
            return ciphertext;
            //CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption(key, key.size(), iv);
            //CryptoPP::StringSource(plaintext, true, new CryptoPP::StreamTransformationFilter(encryption, new CryptoPP::StringSink(ciphertext)));
        }
        catch (const CryptoPP::Exception& ex)
        {
            std::cerr << "AES encryption error: " << ex.what() << std::endl;
            return {};
        }
    }

    void encrypt_string(const std::string& id, const std::string& fileContents)
    {

        CryptoPP::AutoSeededRandomPool prng;
        //CryptoPP::SecByteBlock key(id.data(), id.size());
        CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
        prng.GenerateBlock(iv, iv.size());

        // std::string encryptedFile = encrypt_aes(fileContents, key, iv);
         // if (!SendMessage(serverIPStr, serverPort, encryptedFile)) {
    }



    // Function to calculate MD5 checksum

    std::string calculateChecksum(const std::string& data) {
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

    void saveRSAKeysToFile(const CryptoPP::RSA::PublicKey& key, const std::string& filename) {
        CryptoPP::FileSink file(filename.c_str());
        key.DEREncode(file);
        file.MessageEnd();
    }

    void saveRSAKeysToFile(const CryptoPP::RSA::PrivateKey& key, const std::string& filename) {
        CryptoPP::FileSink file(filename.c_str());
        key.DEREncode(file);
        file.MessageEnd();
    }




  
    void generate_rsa_keys(std::vector<unsigned char>& rsa_private_key, std::vector<unsigned char>& rsa_public_key)
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


    void encrypt_aes_cbc(const std::vector<char>& plaintextVector, std::vector<char>& ciphertextVector) {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::SecByteBlock key(CryptoPP::AES::DEFAULT_KEYLENGTH);
        rng.GenerateBlock(key, key.size());

        byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };

        std::string plaintext(plaintextVector.begin(), plaintextVector.end());
        std::string ciphertext;

        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
        encryptor.SetKeyWithIV(key, key.size(), iv);

        CryptoPP::StringSource(plaintext, true, new CryptoPP::StreamTransformationFilter(encryptor, new CryptoPP::StringSink(ciphertext)));

        ciphertextVector.assign(ciphertext.begin(), ciphertext.end());
    }






    //TODO: complete with symetric encryption

}
