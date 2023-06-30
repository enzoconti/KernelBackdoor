import socket
import time
import struct
import os

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 65432  # The port used by the server
f = open("out.ppm", "wb")
f.write(b"P6\n")
keys_size_bytes = bytes()
screensize_bytes = bytes()
data = bytes()
pixels = bytes()


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    while True:
        try:
            while len(keys_size_bytes) != 4:
                keys_size_bytes = s.recv(4-len(keys_size_bytes))
                if not keys_size_bytes:
                    print("Serve closed connection")
                    break
            keys_size = struct.unpack("i", keys_size_bytes)[0]


            print(f"Got keys_size={keys_size!r} and going to get the keys")
            while len(data) != keys_size:
                data = s.recv(keys_size-len(data))
                if not data:
                    print("Serve closed connection")
                    break
            print(f"Received {data!r}")
            
        except ConnectionResetError:
            print("Server closed connection")
            break
        except ConnectionRefusedError:
            print("Connection refused. Server is unreachable")
        except socket.timeout:
            print("Connection timeout. Server is unreachable")
        time.sleep(5)

    f.close()
    s.close()