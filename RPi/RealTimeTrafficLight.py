import sys
sys.path.append(".")
import RPi.GPIO as GPIO
import time
import _thread
from TrafficLight import TrafficLight

ObstaclePin = 31
Destroy = [False]
Obstacle = [False]

def setup():
	GPIO.setmode(GPIO.BOARD)       # Numbers GPIOs by physical location
	GPIO.setup(ObstaclePin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def loop():
    while not Destroy[0]:
        time.sleep(0.25)
        if (0 == GPIO.input(ObstaclePin)):
            Obstacle[0] = True
        else:
            Obstacle[0] = False
			

def destroy():
    Destroy[0] = True
    GPIO.cleanup() # Release resource


class RealTimeTrafficLight:

    def __init__(self):
        setup()
        _thread.start_new_thread(loop, ())
        self.state = "RED"
        self.light = TrafficLight()

        def run(self):
            while not Destroy[0]:
                self.update_state("RED")
                #time.sleep(80)
                for _ in range(80):
                    if Obstacle[0] == True:
                        break
                    time.sleep(1)
                self.update_state("GREEN")
                time.sleep(50)
                self.update_state("YELLOW")
                time.sleep(10)

        _thread.start_new_thread(run, (self,))

    def update_state(self, new_state):
        self.state = new_state
        return self.light.update_state(new_state)
    
    def get_name(self):
        return "traffic_light"
    
    def get_value(self):
        if (Obstacle[0]):
            return 1
        return 0
    
    def get_state(self):
        return self.state
    
    def get_consumption(self):
        return self.light.get_consumption()
    
    def kill(self):
        destroy()
        self.light.kill()