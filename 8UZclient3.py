# 8. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong

import socket
import time 
from threading import Thread
import uuid 
import random 
import json
import os

# 4.1. TCP-Client:
HOST = "127.0.0.1"
PORT3 = 65434

lamport = 0
client_id = str(uuid.uuid4())[:8] # kürze ID's zur Länge 8
time_offset = random.randint(-10, 10) # lokale Abweichung

def time_calc():

    return time.strftime("%H:%M:%S", time.localtime(time.time() + time_offset)) # lokale Zeit in %H:%M:%S

# lösche alten Chat
def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear') 

def receive_message(sock):
    global lamport
    
    while True:
        try:
            data = sock.recv(4096)
            if not data:
                break

            messages = json.loads(data.decode())

            # Lamport Zähler max(L, L_r) + 1
            max_lamport = max(m["lamport"] for m in messages)
            lamport = max(lamport, max_lamport) + 1

            clear_screen()
            
            for m in messages:
                print(f"{m['lamport']}, {m['local_time']} {m['client_id']}: {m['message']} ")
            print("> ", end="", flush=True)
        except:
            break

def run_client3():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT3))

        print(f"Connected as Client {client_id}")

        Thread(target=receive_message, args=(s,), daemon=True).start()

        print("> ", end="", flush=True)

        while True:
            text = input("Say something (or type 'exit' to leave): ")
            global lamport
            lamport += 1

            msg = {"lamport" : lamport, "client_id" : client_id, "local_time" : time_calc(), "message" : text}
        
            s.sendall(json.dumps(msg).encode()) # von Dictionary zum String konvertieren 

            if text.lower() == "exit":
                raise KeyboardInterrupt 
                
            
run_client3()


