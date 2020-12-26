import socket 

host = '192.168.1.154'
port = 40000 

print("commands available: \n 1) EXIT\n 2) GET\n 3) KILL")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

while True: 
	command = input("Enter your command: ")
	if command == 'EXIT':
		# Sends exit request to the server
		s.send(command.encode())
		break
	elif command == 'KILL': 
		s.send(command.encode())
		break
	elif command == 'GET':
		s.send(command.encode())
	else: 
		s.send(command.encode())
	reply = s.recv(1024)
	print(reply.decode('utf-8'))

s.close()
