import time
import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 65432  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while True:
        s.sendall(b"DEL this is a message")
        try:
            data = s.recv(1024)
            if not data:
                print("Server closed connection")
                break
            print(f"Received {data!r}")
            time.sleep(1)
        except ConnectionResetError:
            print("Server closed connection")
        except ConnectionRefusedError:
            print("Connection refused. Server is unreachable")
        except socket.timeout:
            print("Connection timeout. Server is unreachable")