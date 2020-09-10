# import the socket module to allow us to setup socket communication
import socket
host = '192.168.16.2'
# Pick a port that is unassigned
port = 5560

storedValue = "Hello from the Server!"

def setupServer():
    # type of connection established, specify TCP or UDP
    # in this case SOCK_STREAM is TCP
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Socket has been created.")

    # try to bind host and port
    try:
        s.bind((host, port)) # if unbound, no data can be received
    # print the error message if binding failed
    except socket.error as msg:
        print(msg)
    print("Socket bind complete.")

    # return, so that other functions can access the socket
    return s

def setupConnection():
    s.listen(1) # Allows 1 connection at a time.

    # accept whatever connection and address the socket listened to
    conn, address = s.accept()
    print("Connected to: " + address[0] + ":" + str(address[1]))

    # connection value will be needed later
    return conn

def GET():
    reply = storedValue
    return reply

def REPEAT(dataMessage):
    reply = dataMessage[1]
    return reply

def dataTransfer(conn):
    # loop to allow transmission and receives until told not to.
    while True:
        data = conn.recv(1024) # receives the data; 1024 = buffer size
        # note: python 3 requires us to differentiate between bytes and strings
        # decode to receive data, encode to transmit data
        
        data = data.decode('utf-8') # utf-8 strings

        # Split the data such that we separate the command
        # from the rest of the data
        dataMessage = data.split(' ', 1)
        command = dataMessage[0]

        if command == 'GET':
            reply = GET()
        elif command == 'REPEAT':
            reply = REPEAT(dataMessage)
        elif command == 'EXIT':
            print("Client has disconnected.")
            # sends us back to the while loop below.
            # allows us to be able to reconnect when needed
            break
        elif command == 'KILL':
            print("Server is shutting down.")
            s.close() # close the socket
            break
        else:
            reply = 'Unknown Command'
            
        # send the reply back to the client
        conn.sendall(str.encode(reply))
        print("Reply from the server has been sent.")
    conn.close()

s = setupServer()

while True:
    try:
        conn = setupConnection()
        dataTransfer(conn)
    except:
        break
