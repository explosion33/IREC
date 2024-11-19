import RPi.GPIO as GPIO
from time import sleep

class Encoder:
    def __init__(self, pin_a, pin_b, counts_per_rev=64, gear_ratio=1):
        self.pin_a = pin_a
        self.pin_b = pin_b
        self.counts_per_rev = counts_per_rev * gear_ratio
        self.counter = 0
        self.last_a = 0
        self.last_b = 0
        
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin_a, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(pin_b, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        
        GPIO.add_event_detect(pin_a, GPIO.BOTH, callback=self._encoder_callback)
        GPIO.add_event_detect(pin_b, GPIO.BOTH, callback=self._encoder_callback)
    
    def _encoder_callback(self, channel):
        a_current = GPIO.input(self.pin_a)
        b_current = GPIO.input(self.pin_b)

        if channel == self.pin_a:
            if a_current != self.last_a:
                if a_current == 1:  # Rising edge of A
                    if b_current == 0:
                        self.counter += 1  # Clockwise
                    else:
                        self.counter -= 1  # Counterclockwise
                else:  # Falling edge of A
                    if b_current == 1:
                        self.counter += 1  # Clockwise
                    else:
                        self.counter -= 1  # Counterclockwise
                self.last_a = a_current

        if channel == self.pin_b:
            if b_current != self.last_b:
                if b_current == 1:  # Rising edge of B
                    if a_current == 1:
                        self.counter += 1  # Clockwise
                    else:
                        self.counter -= 1  # Counterclockwise
                else:  # Falling edge of B
                    if a_current == 0:
                        self.counter += 1  # Clockwise
                    else:
                        self.counter -= 1  # Counterclockwise
                self.last_b = b_current
    
    def get_position(self):
        return self.counter
    
    def get_angle(self):
        return (self.counter * 360.0) / self.counts_per_rev
    
    def reset(self):
        self.counter = 0

if __name__ == "__main__":
    try:
        # Set a and b pins 
        encoder = Encoder(pin_a=14, pin_b=15)
        
        print("Monitoring encoder position. Press Ctrl+C to exit.")
        while True:
            position = encoder.get_position()
            angle = encoder.get_angle()
            print(f"Position: {position} counts, Angle: {angle:.1f}°")
            sleep(0.1)
    
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        GPIO.cleanup()