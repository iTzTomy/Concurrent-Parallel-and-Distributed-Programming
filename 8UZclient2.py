# 8. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong

import socket

# 3.1. TCP-Client:
HOST = "127.0.0.1"
PORT2 = 65433

def run_client2():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT2))

        print("Connected to ELIZA Chat AI.")
        greetingMSG = s.recv(1024)
        print(f"ELIZA: {greetingMSG.decode()}")

        while True:
            question = input("You: ")

            if question == "exit":
                s.sendall(question.encode())
                goodByeMSG = s.recv(1024)
                print(f"ELIZA: {goodByeMSG.decode()}")
                break
                
            if question:
                s.sendall(question.encode())
                answer = s.recv(1024)
                
                print(f"ELIZA: {answer.decode()}")

run_client2()


