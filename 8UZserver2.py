# 8. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong

# 3.1. TCP-Server:

import socket
from threading import Thread
import random
HOST = "127.0.0.1"
PORT2 = 65433

answers = ["Go on.", "Fine, please go on.", "Come, elucidate your thoughts.", "Damn that's crazy!", "Please enrich your active vocabulary.", "You haven't learnt much English in school, have you?", "Is this a new abbreviation I don't know about?", 
           "That's too brief.", "Do you suffer from sleeplessness?", "Why did you redeem it?", "Do you experience problems to express yourself sometimes?", "Not an easy question ...", "Is this an important question for you?", "Neither ...", "But you just said that."]

def run_server2():
    
     with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT2))
        s.listen()
        
        print(f"Server listening on {HOST}:{PORT2}")
        

        while True:
            conn, addr = s.accept()
            # erstelle Thread für jeden Client
            Thread(
                target=handle_client,
                args=(conn, addr),
                daemon=True
            ).start()

def handle_client(conn, addr):
        
        with conn:
            print(f"Connected by {addr}")
            conn.sendall("Hello! What do you want us to talk about? (Type \"exit\" to  end))".encode())

            while True:
                question = conn.recv(1024)
                if not question:
                    print(f"Disconnected from {addr}")
                    break
                text = question.decode().strip()
                if text.lower() == 'exit':
                    conn.sendall("Goodbye!".encode())
                    print(f"Disconnected from {addr}")
                    break
                conn.sendall(answers[random.randrange(len(answers))].encode()) # Zufällig vom Array ausgewählte Antwort

run_server2()