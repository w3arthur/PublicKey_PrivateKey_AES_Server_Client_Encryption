if __name__ != "__main__":
    # TODO: clear comment
    import base64
    import os
    import secrets
    from Crypto.Cipher import AES
    import hashlib
    from cryptography.fernet import Fernet
    from cryptography.hazmat.primitives import serialization, hashes
    from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
    from cryptography.hazmat.primitives.asymmetric import padding, rsa
    from cryptography.hazmat.backends import default_backend
    from cryptography.hazmat.primitives.padding import PKCS7

    def generate_aes_key():
        aes_key = os.urandom(32)  # 24 bytes for AES-192
        return base64.urlsafe_b64encode(aes_key).decode('utf-8'), aes_key

    # Function to decrypt ciphertext with AES key
    def decrypt_with_aes(aes_key, ciphertext):
        aes_key_bytes = aes_key  # base64.urlsafe_b64decode(aes_key)
        iv_bytes = b'\x00' * 16  # IV of all zeros (16 bytes)
        cipher = Cipher(algorithms.AES(aes_key_bytes), modes.CBC(
            iv_bytes), backend=default_backend())
        decryptor = cipher.decryptor()

        # Decode the base64 string and decrypt it
        ciphertext_bytes = base64.urlsafe_b64decode(ciphertext)
        decrypted_data = decryptor.update(ciphertext_bytes)

        try:
            decrypted_data += decryptor.finalize()
        except ValueError as e:
            print("Error:", e)
            return None

        # Add errors='ignore' to handle decoding errors
        return decrypted_data.decode('utf-8', errors='ignore')

    # aes_key = generate_aes_key()
    # plaintext = "Hello, AES encryption!"
    # ciphertext = encrypt_with_aes(aes_key, plaintext)
    # decrypted_text = decrypt_with_aes(aes_key, ciphertext)

    # print("Generated AES Key:", aes_key)
    # print("Plaintext:", plaintext)
    # print("Ciphertext:", ciphertext)
    # print("Decrypted Text:", decrypted_text)
