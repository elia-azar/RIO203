import sys
sys.path.append(".")
import _thread
from StreetLight import StreetLight
import time
from datetime import datetime

Destroy = [False]

daily_light = [0.1, 0.1, 0.2, 0.7, 2, 10, 50, 80, 107, 200, 300, 500,
700, 750, 690, 580, 500, 450, 340, 250, 50, 10, 1, 0.5]

def compute_lux():
    global lux
    now = datetime.now()
    hour = int(now.strftime("%H"))
    minute = int(now.strftime("%M"))
    percentage = 1.0 - minute/60.0
    lux = percentage * daily_light[hour] + (1.0 - percentage) * daily_light[(hour + 1) % 24]
    return

class SmartStreetLightb:

    def __init__(self):
        compute_lux()
        self.light = StreetLight()
        self.state = self.light.state

        def run(self):
            now = datetime.now()
            hour = int(now.strftime("%H"))
            minute = int(now.strftime("%M"))
            percentage = 1.0 - minute/60.0
            lux = percentage * daily_light[hour] + (1.0 - percentage) * daily_light[(hour + 1) % 24]

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
    
    def get_name(self):
        return "street_light"
    
    def get_value(self):
        return lux

    def get_state(self):
        return self.state
    
    def get_consumption(self):
        return self.light.get_consumption()
    
    def kill(self):
        self.light.kill()