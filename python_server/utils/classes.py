if __name__ != "__main__":
    import struct
    import config

    class Header:
        version: int = None
        code: int = None
        payload_size: int = None

        def __init__(self, code, payload_size):
            self.version = config.server_version
            self.code = code
            self.payload_size = payload_size

        def serialize(self):
            return struct.pack("!BHL", self.version, self.code, self.payload_size)

    class Payload:
        client_id: str = None

        def __init__(self, client_id):
            self.client_id = client_id

        def serialize(self):
            return self.client_id.encode('utf-8')[:16].ljust(16, b'\0')
