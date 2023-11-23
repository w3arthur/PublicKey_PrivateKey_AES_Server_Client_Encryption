import base64
import struct
import hashlib
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import pad, unpad
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256


def calculate_numeric_checksum(data):
    sha256_hash = hashlib.sha256(data.encode()).digest()
    checksum = struct.unpack("I", sha256_hash[:4])[0]
    return checksum


def generate_aes_key():
    aes_key = get_random_bytes(32)
    return base64.urlsafe_b64encode(aes_key).decode('utf-8'), aes_key


def encrypt_aes_key(public_key_bytes, aes_key):
    # Load the public RSA key from bytes
    public_key = RSA.import_key(public_key_bytes)

    # Use PKCS1 OAEP for encryption with SHA-256 as the MGF function
    cipher_rsa = PKCS1_OAEP.new(public_key, hashAlgo=SHA256)

    # Encrypt the AES key
    encrypted_aes_key = cipher_rsa.encrypt(aes_key)

    return encrypted_aes_key, base64.urlsafe_b64encode(encrypted_aes_key).decode('utf-8')


def decrypt_with_aes(aes_key, ciphertext):
    aes_key_bytes = base64.urlsafe_b64decode(aes_key)
    iv_bytes = b'\x00' * 16
    cipher = AES.new(aes_key_bytes, AES.MODE_CBC, iv_bytes)

    ciphertext_bytes = base64.urlsafe_b64decode(ciphertext)
    decrypted_data = unpad(cipher.decrypt(ciphertext_bytes), AES.block_size)

    return decrypted_data.decode('utf-8', errors='ignore'), decrypted_data


def remove_pkcs7_padding(data):
    pad_byte = ord(data[-1])
    return data[:-pad_byte]
