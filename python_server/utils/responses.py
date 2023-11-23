import utils.classes as classes


def response2100(name, client_id):
    code: int = 2100
    payload = classes.Payload2100(client_id)
    header = classes.Header(code, payload.size())
    header_bytes = header.serialize()
    payload_bytes = payload.serialize()
    message = header_bytes + payload_bytes
    return message


def response2101():
    code: int = 2101
    payload_size = 0
    header = classes.Header(code, payload_size)
    header_bytes = header.serialize()
    message = header_bytes
    return message


def response2102(client_id, encrypted_aes_key):
    code: int = 2102
    payload = classes.Payload2102(client_id, encrypted_aes_key)
    header = classes.Header(code, payload.size())
    header_bytes = header.serialize()
    payload_bytes = payload.serialize()
    message = header_bytes + payload_bytes
    return message


def response2103(client_id, content_size, filename, cksum):
    code: int = 2103
    payload = classes.Payload2103(
        client_id, content_size, filename.encode('utf-8'), cksum)
    header = classes.Header(code, payload.size())
    header_bytes = header.serialize()
    payload_bytes = payload.serialize()
    message = header_bytes + payload_bytes
    return message


def response2014_accept(client_id):
    code: int = 2104
    payload = classes.Payload2104(client_id)
    header = classes.Header(code, payload.size())
    header_bytes = header.serialize()
    payload_bytes = payload.serialize()
    message = header_bytes + payload_bytes
    return message


def response2105(client_id):
    code: int = 2105
    payload = classes.Payload2105(client_id)
    header = classes.Header(code, payload.size())
    header_bytes = header.serialize()
    payload_bytes = payload.serialize()
    message = header_bytes + payload_bytes
    return message


def response2106(client_id):
    code: int = 2106
    payload = classes.Payload2106(client_id)
    header = classes.Header(code, payload.size())
    header_bytes = header.serialize()
    payload_bytes = payload.serialize()
    message = header_bytes + payload_bytes
    return message


def response2017():
    code: int = 2107
    payload_size = 0
    header = classes.Header(code, payload_size)
    header_bytes = header.serialize()
    message = header_bytes
    return message
