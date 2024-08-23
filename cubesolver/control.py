import time
import kociemba
import serial

port = "/tmp/vserial1"

main_flag = True

try:
    gui_serial = serial.Serial(port, 115200)
except:
    print("Could not connect to GUI - check serial connection")
    main_flag = False
else: 
    print("Successfully connected to GUI serial {}".format(port))

time.sleep(2) # wait a bit to establish serial connection, idk if this does anything lol

def processing_serial_write(arg): # just so I don't have to remember the stupid format
    gui_serial.write(arg.encode())

def receive_data(): # receive (properly formatted with endline char) data from serial
    line = ""
    flag = True
    while flag:
        for c in gui_serial.read().decode():
            if c == '\n':
                flag = False
            line = line + c
    trimmed_line = line.strip()
    return trimmed_line

def processing_serial_receive():

    print("\n") # line break

    input = receive_data()
    print("Received command: " + input)

    if input == "solve":
        current_cubestate = receive_data()
        print("Received cubestate: " + current_cubestate)
        if len(current_cubestate) == 54:
            print("Correct length, proceeding.")
        else:
            print("Incorrect length, check processing")
            return
        solvestring = kociemba.solve(current_cubestate)
        print("Solution: " + solvestring)
        processing_serial_write(solvestring)

############################### MAIN ###################################

while main_flag:

    processing_serial_receive() #check for data in the serial link and perform corresponding actions
    
############################## END MAIN ################################

print("exiting")