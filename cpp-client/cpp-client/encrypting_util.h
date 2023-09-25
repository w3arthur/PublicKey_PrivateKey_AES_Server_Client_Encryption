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



    void generateOrLoadRSAKeys(std::string& rsaPrivateKey, std::string& rsaPublicKey, const std::string& clientName) {
        // Check if private and public keys already exist and match the clientName
        if (rsaPrivateKey.empty() || rsaPublicKey.empty()) {
            CryptoPP::AutoSeededRandomPool rng;
            CryptoPP::InvertibleRSAFunction params;
            params.GenerateRandomWithKeySize(rng, 1024);

            CryptoPP::RSA::PrivateKey privateKey(params);
            CryptoPP::RSA::PublicKey publicKey(params);

            // Save the private key to the string
            CryptoPP::StringSink privateKeySink(rsaPrivateKey);
            privateKey.Save(privateKeySink);

            // Save the public key to the string
            CryptoPP::StringSink publicKeySink(rsaPublicKey);
            publicKey.Save(publicKeySink);

            // Save the keys to files
            //saveRSAKeysToFile(privateKey, "private_key.pem");
            //saveRSAKeysToFile(publicKey, "public_key.pem");
        }
        else
        {
            // Keys exist, no need to generate new ones
            std::cout << "RSA keys already exist." << std::endl;
        }
    }





}