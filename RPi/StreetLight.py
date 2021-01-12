from datetime import datetime
import RPi.GPIO as GPIO


STATES = {
    "ON": 0xFFFF,
    "OFF": 0x0000, 
    }

def setup():
    global pins
    global p_R, p_G

    pins = (11, 12)  # pins is a dict

    GPIO.setmode(GPIO.BOARD)       # Numbers GPIOs by physical location
    GPIO.setup(pins, GPIO.OUT)   # Set pins' mode is output
    GPIO.output(pins, GPIO.LOW)  # Set pins to LOW(0V) to off led

    p_R = GPIO.PWM(pins[0], 2000)  # set Frequece to 2KHz
    p_G = GPIO.PWM(pins[1], 2000)

    p_R.start(0)      # Initial duty Cycle = 0(leds off)
    p_G.start(0)

def map(x, in_min, in_max, out_min, out_max):
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

def setColor(col):   # For example : col = 0x1122
	R_val = col  >> 8
	G_val = col & 0x00FF
	
	R_val = map(R_val, 0, 255, 0, 100)
	G_val = map(G_val, 0, 255, 0, 100)
	
	p_R.ChangeDutyCycle(R_val)     # Change duty cycle
	p_G.ChangeDutyCycle(G_val)

def destroy():
	p_R.stop()
	p_G.stop()
	GPIO.output(pins, GPIO.LOW)    # Turn off all leds
	GPIO.cleanup()


class StreetLight:
    def __init__(self):
        setup()
        self.date = datetime.now().strftime("%H:%M:%S")
        self.state = "OFF"
        setColor(STATES.get(self.state))

    def update_state(self, new_state):
        self.state = new_state
        setColor(STATES.get(self.state))
        self.date = datetime.now().strftime("%H:%M:%S")
        return "200"
    
    def get_state(self):
        return self.state
    
    def get_consumption(self):
        if self.state == "ON":
            return 73
        else:
            return 0
    
    def kill(self):
        destroy()
        self.state = "OFF"