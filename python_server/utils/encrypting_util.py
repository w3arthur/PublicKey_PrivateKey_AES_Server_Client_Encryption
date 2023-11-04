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

    def generate_aes_key():
        aes_key = os.urandom(24)  # 24 bytes for AES-192
        return base64.urlsafe_b64encode(aes_key).decode('utf-8')

    # Function to encrypt plaintext with AES key
    def encrypt_with_aes(aes_key, plaintext):
        aes_key_bytes = base64.urlsafe_b64decode(aes_key)
        cipher = Cipher(algorithms.AES(aes_key_bytes),
                        modes.ECB(), backend=default_backend())
        encryptor = cipher.encryptor()
        ciphertext = encryptor.update(
            plaintext.encode('utf-8')) + encryptor.finalize()
        return base64.urlsafe_b64encode(ciphertext).decode('utf-8')

    # Function to decrypt ciphertext with AES key
    def decrypt_with_aes(aes_key, ciphertext):
        aes_key_bytes = base64.urlsafe_b64decode(aes_key)
        cipher = Cipher(algorithms.AES(aes_key_bytes),
                        modes.ECB(), backend=default_backend())
        decryptor = cipher.decryptor()

        # Decrypt the ciphertext
        ciphertext_bytes = base64.urlsafe_b64decode(ciphertext)
        decrypted_data = decryptor.update(
            ciphertext_bytes) + decryptor.finalize()

        return decrypted_data.decode('utf-8')
