if __name__ != "__main__":
    # TODO: clear comment
    import base64
    import socket
    import threading
    from Crypto.Cipher import AES
    import hashlib
    from cryptography.fernet import Fernet
    from cryptography.hazmat.primitives import serialization
    from cryptography.hazmat.primitives.asymmetric import padding, rsa
    from cryptography.hazmat.primitives import hashes

    def decrypt_with_aes_ecb(ciphertext, aes_key):
        # Create an AES cipher object in ECB mode with the provided key
        cipher = AES.new(aes_key, AES.MODE_ECB)

        # Decrypt the ciphertext
        decrypted_data = cipher.decrypt(ciphertext)

        # Remove any padding (if used during encryption)
        return decrypted_data.rstrip(b"\0")

    # TODO: delete:

    def generate_and_encrypt_aes_key(public_key_bytes):
        public_key = serialization.load_der_public_key(public_key_bytes)
        aes_key = Fernet.generate_key()
        # Encrypt the AES key with the recipient's public key
        encrypted_aes_key = public_key.encrypt(
            aes_key,
            padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None
            )
        )

        return base64.b64encode(encrypted_aes_key).decode('utf-8'), base64.urlsafe_b64encode(aes_key).decode('utf-8')

    # פונקציה להצפנת נתונים בAES

    def encrypt(data, key):
        cipher = AES.new(key, AES.MODE_EAX)
        ciphertext, tag = cipher.encrypt_and_digest(data)
        return ciphertext

    def calculate_checksum(data):
        return hashlib.md5(data).hexdigest()

    def handle_client(client_socket, clients):
        # הודעה מהלקוח
        request = client_socket.recv(1024)
        # מימוש הפרוטוקול כאן
        # ...
        # תגובה ללקוח
        response = "תשובת השרת"
        client_socket.send(response.encode())
        client_socket.close()
