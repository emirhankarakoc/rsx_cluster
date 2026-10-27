from .protocol import LAMPS, LIMITS

class Telemetry:
    def __init__(self, timeout=1.5):
        self.timeout = timeout
        self.reset()
    def reset(self):
        self.values = {}
        self.updated = {}
    def accept(self, values, now):
        for key,value in values.items():
            self.values[key] = value
            self.updated[key] = now
    def snapshot(self, now):
        return {key:self.values.get(key) if now-self.updated.get(key,float('-inf')) < self.timeout else None for key in (*LIMITS,*LAMPS)}
