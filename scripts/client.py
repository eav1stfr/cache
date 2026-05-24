import socket


HOST = "127.0.0.1"
PORT = 6379


def send_command(sock, command: str) -> str:
    sock.sendall((command + "\n").encode())
    response = ""
    while not response.endswith("\n"):
        response += sock.recv(1024).decode()
    return response.strip()


def run_tests(sock):
    print("=== SET ===")
    print(send_command(sock, "SET name alex"))        # OK
    print(send_command(sock, "SET city astana"))      # OK

    print("\n=== GET ===")
    print(send_command(sock, "GET name"))             # alex
    print(send_command(sock, "GET city"))             # astana
    print(send_command(sock, "GET missing"))          # NOT FOUND

    print("\n=== DEL ===")
    print(send_command(sock, "DEL name"))             # DELETED
    print(send_command(sock, "GET name"))             # NOT FOUND
    print(send_command(sock, "DEL name"))             # NOT FOUND

    print("\n=== TTL ===")
    print(send_command(sock, "SET session abc123 3")) # OK — expires in 3 seconds
    print(send_command(sock, "GET session"))          # abc123

    print("Waiting 4 seconds for TTL to expire...")
    import time
    time.sleep(4)

    print(send_command(sock, "GET session"))          # NOT FOUND

    print("\n=== INVALID COMMANDS ===")
    print(send_command(sock, "SET"))                  # ERROR
    print(send_command(sock, "GET"))                  # ERROR
    print(send_command(sock, "FOOBAR key"))           # ERROR
    print(send_command(sock, "SET key value badttl")) # ERROR


def interactive(sock):
    print("Connected. Type commands (SET key value, GET key, DEL key). Ctrl+C to quit.\n")
    try:
        while True:
            command = input("> ").strip()
            if not command:
                continue
            print(send_command(sock, command))
    except KeyboardInterrupt:
        print("\nDisconnected.")


if __name__ == "__main__":
    import sys

    mode = sys.argv[1] if len(sys.argv) > 1 else "interactive"

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))
        print(f"Connected to {HOST}:{PORT}\n")

        if mode == "test":
            run_tests(sock)
        else:
            interactive(sock)