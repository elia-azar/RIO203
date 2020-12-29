import socket 

host = ''
port = 40000

## SOCKET CREATION ##
def GET():
	reply = str(123)
	print("Nice automation dude!")
	return reply

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.bind((host, port)) #if it doesn't work remove one pair of parenthesis
	s.listen(1) #xallows one connection at the time 
	conn, addr = s.accept()
	with conn:
		print("Connected by: ", addr)
		while True:
			data = conn.recv(1024)
			data = data.decode('utf-8')
			dataMessage = data.split(' ', 1)
			command = dataMessage[0]
			if command ==  'GET':
				reply = GET()
				
			elif command == 'EXIT':
				print("Connection to client interrupted.")
			
			elif command == 'KILL': 
				print("Our server is shutting down.")
				s.close()
				break 
			else: 
				print( "Unknown command.")
			conn.sendall(reply.encode())	


