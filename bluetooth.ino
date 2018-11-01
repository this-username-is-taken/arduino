// Writes 'a' to Bluetooth chip every second
// Tested on HC-05, default pairing password is 1234
// See output with: screen /dev/tty.HC-05-DevB
// https://www.instructables.com/id/Remotely-Control-LED-using-HC-05-Bluetooth-Arduino/

void setup()
{
  // set the baud rate to 9600, same should be of your Serial Monitor
  Serial.begin(9600);
}

void loop()
{
  Serial.write('a');
  if (Serial.available()) {
    char ch = Serial.read();
    Serial.write(ch);
  }
  delay(1000);
}
