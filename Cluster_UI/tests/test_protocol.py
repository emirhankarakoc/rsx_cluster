import unittest
from rsx.protocol import Framer, decode, LAMPS
from rsx.model import Telemetry
from rsx.demo import Demo

class ProtocolTests(unittest.TestCase):
    def test_fragmented_and_multiple(self):
        f = Framer()
        self.assertEqual(f.feed(b'{"version":1,"rp'),[])
        lines = f.feed(b'm":3000}\r\n{"version":1,"rpm":4000}\n')
        self.assertEqual([decode(x)['rpm'] for x in lines],[3000,4000])
    def test_bad_then_good(self):
        f = Framer(40)
        lines = f.feed(b'x'*100+b'\n\xff\nno json\n{"version":1,"rpm":1}\n')
        self.assertIsNone(decode(lines[0]))
        self.assertEqual(decode(lines[1]),{'rpm':1})
    def test_invalid_values(self):
        for fragment in ['"rpm":NaN','"rpm":Infinity','"rpm":-1','"rpm":true','"rpm":30001','"left":1','"speed_mph":"40"','"seq":false']:
            self.assertIsNone(decode('{"version":1,'+fragment+'}'),fragment)
        self.assertIsNone(decode('{"version":true,"rpm":3}'))
        self.assertIsNone(decode('[]'))
        self.assertIsNone(decode('{"version":1,"rpm":' + '9'*400 + '}'))
    def test_legacy(self):
        value = decode('500,50,3000,25,50.00,UNCALIBRATED','Legacy CSV')
        self.assertEqual(value,{'rpm':3000,'speed_mph':None})
        for line in ['elapsed_ms,rpm_pulses,rpm,vss_pulses,vss_hz,speed_mph','# LIGHT ON','Starting UNO','500,50,nan,25,50,3']:
            self.assertIsNone(decode(line,'Legacy CSV'))
        self.assertEqual(decode('500,50,3000,25,50,46.2','Legacy CSV')['speed_mph'],46.2)
    def test_field_timeout(self):
        m = Telemetry()
        m.accept({'rpm':3000,'left':False},10)
        m.accept({'speed_mph':42},11)
        self.assertIs(m.snapshot(11)['left'],False)
        self.assertIsNone(m.snapshot(11)['right'])
        self.assertIsNone(m.snapshot(11.5)['rpm'])
        self.assertEqual(m.snapshot(11.5)['speed_mph'],42)
        self.assertIsNone(m.snapshot(12.5)['speed_mph'])
    def test_reconnect_resets_partial_and_values(self):
        f,m = Framer(),Telemetry()
        f.feed(b'{"version":1,"rpm":')
        m.accept({'rpm':8000,'cel':True},1)
        f.reset(); m.reset()
        self.assertIsNone(decode(f.feed(b'1000}\n')[0]))
        self.assertTrue(all(x is None for x in m.snapshot(1).values()))
        self.assertEqual(decode(f.feed(b'{"version":1,"rpm":2000}\n')[0])['rpm'],2000)
    def test_demo_exclusion_and_pause(self):
        d = Demo(); a = d.step(1)
        d.paused = True
        self.assertEqual(a,d.step(10))
        d.manual = True; d.rpm = 1234; d.speed = 25
        self.assertEqual(d.step(1)['rpm'],1234)
        self.assertEqual(d.step(1)['speed_mph'],25)
        self.assertEqual(set(d.lamps),set(LAMPS))

if __name__ == '__main__': unittest.main()
