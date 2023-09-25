if __name__ != "__main__":
    import socket
    import threading
    from Crypto.Cipher import AES
    import hashlib

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


else:
    from ..main import main
    main()
