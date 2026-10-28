//analog input 
// a22 - 24k - 24k - arduino analog input - 24k - gnd (b18)
//2002 rsx , cel kontak acik cel acik 333, motor acik cel acik 340 , cel kapali motor kapali 427 , cel kapali motor acik 426.
//safe range'de ayarlandi. 360 ve 400. +-30 seklinde.


const byte CEL_PIN = A0;

const int CEL_ON_THRESHOLD = 360;
const int CEL_OFF_THRESHOLD = 400;

bool celOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(CEL_PIN, INPUT_PULLUP);
}

void loop() {
  long total = 0;

  // Average 10 readings
  for (int i = 0; i < 10; i++) {
    total += analogRead(CEL_PIN);
    delay(2);
  }

  int raw = total / 10;

  // Active LOW with hysteresis
  if (raw <= CEL_ON_THRESHOLD) {
    celOn = true;
  }
  else if (raw >= CEL_OFF_THRESHOLD) {
    celOn = false;
  }

  // JSON v1 - RSX Cluster UI
  Serial.print("{\"version\":1,\"cel\":");
  Serial.print(celOn ? "true" : "false");

  Serial.print(",\"raw\":");
  Serial.print(raw);

  Serial.println("}");

  delay(80);
}
