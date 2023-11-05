if __name__ != "__main__":
    import struct
    import config  # as config

    # TODO: fix serialize function

    class Header:
        version: int = config.server_version
        code: int = None
        payload_size: int = None

        def __init__(self, code, payload_size):
            self.code = code
            self.payload_size = payload_size

        def serialize(self):
            return struct.pack("!BHL", self.version, self.code, self.payload_size)
        # def size(self): #set inside config

    class Payload2100:
        client_id: str = None

        def __init__(self, client_id):
            self.client_id = client_id

        def serialize(self):
            return struct.pack("16s", self.client_id)

        def size(self):
            return 16

    class Payload2101:
        # Registration failed
        pass

    class Payload2102(Payload2100):
        # client_id
        encrypted_aes_key = None

        def __init__(self, client_id, encrypted_aes_key):
            self.client_id = client_id
            self.encrypted_aes_key = encrypted_aes_key

        def serialize(self):
            count = struct.pack("16s", self.client_id) + self.encrypted_aes_key.encode(
                'utf-8')
            return count

        def size(self):
            return 16 + len(self.encrypted_aes_key.encode(
                'utf-8'))   # TODO: improve

    class Payload2103(Payload2100):
        # client_id
        content_size = None
        filename = None
        cksum = None

        def __init__(self, client_id, content_size, filename, cksum):
            self.client_id = client_id
            self.content_size = content_size
            self.filename = filename
            self.cksum = cksum

        def serialize(self):
            return struct.pack("16s", self.client_id) + struct.pack("!I", self.content_size) + struct.pack("255s", self.filename) + struct.pack("I", self.cksum) + struct.pack("I", 5)

        def size(self):
            return len(self.serialize())  # 16 + 4 + 255 + 4

    class Payload2104(Payload2100):
        # client_id

        def __init__(self, client_id):
            self.client_id = client_id

        def serialize(self):
            return struct.pack("!16s", self.client_id)

        def size(self):
            return 16

    class Payload2105(Payload2100):
        # client_id
        pass

    class Payload2106(Payload2100):
        # client_id
        pass

    class Payload2107:
        # Error
        pass
