import uuid
import struct
import base64


def null_terminator_crop(string: str) -> str:
    name_end_index: int = string.find(b'\0')
    if name_end_index != -1:
        return string[:name_end_index]
    return string


def get_string(payload_data, request_format: str = "<255s") -> str:
    str1,  = struct.unpack(request_format, payload_data)
    return null_terminator_crop(str1).decode('utf-8')


def uuid_to_string(client_uuid) -> str:
    uuid_obj = uuid.UUID(bytes=client_uuid)
    return str(uuid_obj)


def decode_byte(bytes) -> str:
    return base64.b64encode(bytes).decode('utf-8')
