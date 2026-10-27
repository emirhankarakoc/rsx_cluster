"""Strict wire decoding and bounded streaming framing; no Qt dependency."""
import json
import math

LAMPS = ('left','right','low_beam','high_beam','cel','srs','charge','low_fuel','brake','seatbelt_warning')
LIMITS = {'rpm':(0,30000),'speed_mph':(0,400),'odometer_mi':(0,1e9),'trip_a_mi':(0,1e7),'trip_b_mi':(0,1e7)}

def number(value, low, high):
    return type(value) in (int,float) and low <= value <= high and math.isfinite(value)

def decode(line, mode='JSON'):
    if mode == 'Legacy CSV':
        parts = line.strip().split(',')
        if len(parts) != 6 or not parts[0].strip().isdigit():
            return None
        try:
            elapsed, pulses, rpm, vss, hz = map(float, parts[:5])
            if not all(number(v,0,1e12) for v in (elapsed,pulses,vss,hz)) or not number(rpm,0,30000):
                return None
            speed = None if parts[5].strip() == 'UNCALIBRATED' else float(parts[5])
            if speed is not None and not number(speed,0,400):
                return None
            return {'rpm':rpm,'speed_mph':speed}
        except ValueError:
            return None
    try:
        obj = json.loads(line, parse_constant=lambda s: (_ for _ in ()).throw(ValueError(s)))
        if not isinstance(obj,dict) or type(obj.get('version')) is not int or obj['version'] != 1:
            return None
        if 'seq' in obj and (type(obj['seq']) is not int or not 0 <= obj['seq'] <= 2**53-1):
            return None
        result = {}
        for key, limits in LIMITS.items():
            if key in obj:
                if not number(obj[key],*limits):
                    return None
                result[key] = obj[key]
        for key in LAMPS:
            if key in obj:
                if type(obj[key]) is not bool:
                    return None
                result[key] = obj[key]
        return result or None
    except (ValueError,TypeError,RecursionError,OverflowError):
        return None

class Framer:
    def __init__(self, limit=4096):
        self.limit = limit
        self.reset()
    def reset(self):
        self.buffer = bytearray()
        self.dropping = False
    def feed(self, data):
        lines = []
        for byte in data:
            if byte == 10:
                if not self.dropping:
                    try:
                        lines.append(self.buffer.decode('utf-8').rstrip('\r'))
                    except UnicodeDecodeError:
                        pass
                self.reset()
            elif not self.dropping:
                self.buffer.append(byte)
                if len(self.buffer) > self.limit:
                    self.buffer.clear()
                    self.dropping = True
        return lines
