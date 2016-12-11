#!/usr/bin/python2 -B
import socket
import sys
import random

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the socket to the address given on the command line
server_address = ('', 6565)
sock.bind(server_address)
sock.listen(1)

while True:
    print('\nWaiting for a connection')
    connection, client_address = sock.accept()
    try:
        print('------------------\nClient connected: ' + client_address[0])
        data = connection.recv(100)
        print('Client request:\r\n[\r\n' + data + '\r\n]');

        # Return a random number 
        ret_str = "200 OK\r\n[\"temp_F\":\"" + str(random.randint(0,99)) + "\"]"
        print('Server respose:\r\n[\r\n' + ret_str + '\r\n]')
        connection.sendall(ret_str)
    finally:
        print('Closing connection');
        connection.close()
