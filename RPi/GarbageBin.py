import RPi.GPIO as GPIO
import time 
import _thread 


TRIG = 11
ECHO = 12
threshold = 10

Destroy = [False]

states = ["FULL","EMPTY"]

def setup():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(TRIG, GPIO.OUT)
    GPIO.setup(ECHO, GPIO.IN)

def distance():
    GPIO.output(TRIG, 0)
    time.sleep(0.000002)

    GPIO.output(TRIG, 1)
    time.sleep(0.00001)
    GPIO.output(TRIG, 0)

    while GPIO.input(ECHO) == 0:
        a = 0
    time1 = time.time()
    while GPIO.input(ECHO) == 1:
        a = 1
    time2 = time.time()

    during = time2 - time1
    return during * 340 / 2 * 100

def loop():
    global emptyspace
    emptyspace = distance()
    while True:
        emptyspace = distance()
        time.sleep(60)

def destroy():
    GPIO.cleanup()

class GarbageBin:
    def __init__(self):
        setup()
        _thread.start_new_thread(loop, ()) # threading.Thread(target=loop).start() would be the best practise way of threading ? 
        self.state = "EMPTY"
        
        def run(self):
            while not Destroy[0]:
                self.update_state()
                time.sleep(10 * 60)

        _thread.start_new_thread(run, (self,))
    
    def get_name(self):
        return "garbage"

    def get_value(self):
        return emptyspace

    def update_state(self): # the states were defined earlier in the code. 
        if emptyspace < threshold:
            if self.state == states[1]:
                print("WARNING: Garbage is Full")  
            self.state = states[0]
        else:
            if self.state == states[0]:
                print("INFO: Garbage has been emptied") 
            self.state = states[1]
    
    def get_state(self):
        return self.state
    
    def get_consumption(self):
        return 0

    def kill(self):
        destroy()
        Destroy[0] = True
        GPIO.cleanup()  # Release of the resource

