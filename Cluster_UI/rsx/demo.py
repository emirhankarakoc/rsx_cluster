import math
from .protocol import LAMPS

class Demo:
    def __init__(self):
        self.time = 0.0
        self.paused = False
        self.manual = False
        self.rpm = 3200
        self.speed = 45
        self.lamps = {key:False for key in LAMPS}
    def step(self, dt):
        if not self.paused:
            self.time += dt
        t = self.time % 30
        if self.manual:
            rpm,speed = self.rpm,self.speed
        elif t < 18:
            speed = t*5
            rpm = 1100 + (t%4.5)/4.5*6000
        elif t < 22:
            speed,rpm = 90,4200
        else:
            speed = max(0,90-(t-22)*11.25)
            rpm = 900+speed*35
        return dict(self.lamps,rpm=rpm,speed_mph=speed,odometer_mi=123456.7,trip_a_mi=123.4,trip_b_mi=56.7)
