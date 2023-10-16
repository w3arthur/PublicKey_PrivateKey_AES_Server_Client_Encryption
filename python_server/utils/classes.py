if __name__ != "__main__":
    import struct
    import config

    class Header:
        version: int = config.server_version
        code: int = None
        payload_size: int = None

        def __init__(self, code, payload_size):
            self.code = code
            self.payload_size = payload_size

        def serialize(self):
            return struct.pack("!BHL", self.version, self.code, self.payload_size)

    class Payload2100:
        client_id = None

        def __init__(self, client_id):
            self.client_id = client_id

        def serialize(self):
            return self.client_id[:16]

        def size(self):
            return 16  # len(self.client_id[:16])

    class Payload2102:
        client_id = None
        encrypted_aes_key = None

        def __init__(self, client_id, encrypted_aes_key):
            self.client_id = client_id
            self.encrypted_aes_key = encrypted_aes_key

        def serialize(self):
            count = f"{self.client_id}{self.encrypted_aes_key}"
            return count[:len(count)]

        def size(self):
            return 16 + len(self.encrypted_aes_key)  # len(self.client_id[:16])

# .encode('utf-8')[:16]
# .ljust(16, b'\0')
