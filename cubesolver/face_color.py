import serial
import kociemba
import time
import cv2
import numpy as np
from pynput import keyboard

# The currently active modifiers
current = set()

def on_press(key):
    pass

def on_release(key):
    pass


port = "/tmp/vserial3"

try:
    gui_serial = serial.Serial(port, 115200, timeout=0.01)
except:
    print("Could not connect to GUI - check serial connection")
    main_flag = False

stickers = {
        'detectpixel' : [[188,132], [320,132], [453,132],
                         [188,260], [320,260], [453,260],
                         [188,376], [320,376], [453,376]]                    
    }

side_vals = ["","","","","","","","",""]

url = "http://10.104.63.18:8080/video"
capture = cv2.VideoCapture(url)
if not capture.isOpened():
    raise IOError("Cannot open livestream")

def colordetect(x,y,frame,i):
    
    roi = frame[y-20:y+20, x-20:x+20]

    avg_color_row = np.average(roi, axis = 0)
    avgcolor = np.average(avg_color_row, axis = 0)

    hsv_value = np.uint8([[avgcolor]])
    hsv = cv2.cvtColor(hsv_value,cv2.COLOR_BGR2HSV)

    h = hsv[0][0][0]
    s = hsv[0][0][1]
    v = hsv[0][0][2]

        
    cv2.imshow('stream',frame)
    #ORDER: B-G-R
    if h >= 0 and h < 50 and s < 80:
        white = (255,255,255)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), white, 5)
        side_vals[i] = "U"
        return 'W'
    elif h >= 0 and h < 20:
        orange = (0,165,255)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), orange, 5)
        side_vals[i] = "L"
        return 'O'
    elif h > 80 and h < 120:
        blue = (226,181,0)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), blue, 5)
        side_vals[i] = "B"
        return 'B'
    elif h > 20 and h < 50 and s > 80 and s < 230:
        yellow = (0,233,255)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), yellow, 5)
        side_vals[i] = "D"
        return 'Y'
    elif h > 140:
        red = (0,0,255)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), red, 5)
        side_vals[i] = "R"
        return 'R'
    elif h > 50 and h < 80:
        green = (0,255,0)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), green, 5)
        side_vals[i] = "F"
        return 'G'
    
def colorRGB(x, y, frame, i):

    roi = frame[y-20:y+20, x-20:x+20]
    avg_color_row = np.average(roi, axis = 0)
    avgcolor = np.average(avg_color_row, axis = 0)
    B = avgcolor[0]
    G = avgcolor[1]
    R = avgcolor[2]

    if R > 140 and R < 190 and G > 120 and G < 200 and B > 100 and B < 180:
        white = (255,255,255)
        frame = cv2.rectangle(frame, (x-40,y-40), (x+40,y+40), white, 5)


    
def grid(frame):
    for i in range(0, width, 20):
        frame = cv2.line(frame, (i,0), (i,height), (0,0,0), 1)
    for i in range(0, height, 20):
        frame = cv2.line(frame, (0,i), (width,i), (0,0,0), 1)

def mouseRGB(event,x,y,flags,param):
    
    if event == cv2.EVENT_LBUTTONDOWN: #checks mouse left button down condition
        
        roi = frame[y-20:y+20, x-20:x+20]

        avg_color_row = np.average(roi, axis = 0)
        avgcolor = np.average(avg_color_row, axis = 0)

        hsv_value = np.uint8([[avgcolor]])
        hsv = cv2.cvtColor(hsv_value,cv2.COLOR_BGR2HSV)
        print ("HSV : " , hsv)
        print("Coordinates of pixel: X: ",x,"Y: ",y)

keylist = [117, 102, 114, 100, 108, 98]
letterlist = ['U','F','R','D','L','B']

cv2.namedWindow('stream')
cv2.setMouseCallback('stream',mouseRGB)


while True:
        
    ret,frame = capture.read()

    frame = cv2.resize(frame, (960,540), interpolation = cv2.INTER_LINEAR)
    
    height = frame.shape[0]
    width = frame.shape[1]

    cc=str(gui_serial.readline())
    command = cc[2:][:-5]
    
    i = 0
    for x,y in stickers['detectpixel']:
        colordetect(x,y,frame, i)
        #colorRGB(x,y,frame,i)
        i = i+1
        
    cv2.imshow('stream',frame)       


    key = cv2.waitKey(1)

    if key in keylist:
        index = keylist.index(key)
        out_string = letterlist[index]
        for string in side_vals:
            out_string += string
        gui_serial.write(out_string.encode())
        print("Sending string: " + out_string)

capture.release()
cv2.destroyAllWindows()

