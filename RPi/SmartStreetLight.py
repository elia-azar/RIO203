import sys
sys.path.append(".")
import _thread
from StreetLight import StreetLight
import PCF8591 as ADC
import RPi.GPIO as GPIO
import time

DO = 17
GPIO.setmode(GPIO.BCM)
Destroy = [False]

def setup():
    global lux
    lux = 255
    ADC.setup(0x48)
    GPIO.setup(DO, GPIO.IN)


def loop():
    while True:
        lux = ADC.read(0)
        time.sleep(1)

def destroy():
    Destroy[0] = True
    GPIO.cleanup()

class SmartStreetLight:

    def __init__(self):
        setup()
        _thread.start_new_thread(loop, ())
        self.light = StreetLight()
        self.state = self.light.state

        def run(self):
            while not Destroy[0]:
                if lux < 100:
                    self.update_state("ON")
                else:
                    self.update_state("OFF")
                time.sleep(10*60)

        _thread.start_new_thread(run, (self,))

    def update_state(self, new_state):
        self.state = new_state
        return self.light.update_state(new_state)
    
    def get_value(self):
        return lux

    def get_state(self):
        return self.state
    
    def get_consumption(self):
        return self.light.get_consumption()
    
    def kill(self):
        destroy()
        self.light.kill()