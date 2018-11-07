// Based on code from: https://www.hackster.io/jeffpar0721/add-wifi-to-arduino-uno-663b9e

#include <SoftwareSerial.h>

// RX, TX for ESP8266
SoftwareSerial esp8266(2, 3);

bool DEBUG = true;
int TIMEOUT = 100;

void setup()
{
  // set build in led as output
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(9600); // Default baudrate (115200) causes issues
  
  sendToEsp8266("AT", TIMEOUT, DEBUG);
  sendToEsp8266("AT+IPR=9600", TIMEOUT, DEBUG); // configure as access point
  sendToEsp8266("AT+CWMODE=2", TIMEOUT, DEBUG); // configure as access point
  sendToEsp8266("AT+CIFSR", TIMEOUT, DEBUG); // get ip address
  sendToEsp8266("AT+CIPMUX=1", TIMEOUT, DEBUG); // configure for multiple connections
  sendToEsp8266("AT+CIPSERVER=1,80", TIMEOUT, DEBUG); // turn on server on port 80

  sendToUno("esp8266 connection is running!", TIMEOUT, DEBUG);
}


void loop()
{
  if (Serial.available() > 0) {
    String message = readSerialMessage();
    if (find(message,"debugEsp8266:")){
      String result = sendToEsp8266(message.substring(13,message.length()),TIMEOUT,DEBUG);
      if(find(result,"OK"))
        sendData("\nOK");
      else
        sendData("\nEr");
    }
  }
  
  if (esp8266.available() > 0) {
    String message = readEsp8266Message();
    Serial.println("RECEIVED: " + message);

    if (find(message, "ON")) {
      digitalWrite(13, HIGH);
    } else if (find(message, "OFF")) {
      digitalWrite(13, LOW);
    }
  }

  delay(TIMEOUT);
}


/*
* Name: sendData
* Description: Function used to send string to tcp client using cipsend
* Params: 
* Returns: void
*/
void sendData(String str) {
  String len = "";
  len += str.length();
  sendToEsp8266("AT+CIPSEND=0,"+len, TIMEOUT, DEBUG);
  delay(100);
  sendToEsp8266(str, TIMEOUT, DEBUG);
  delay(100);
  sendToEsp8266("AT+CIPCLOSE=5", TIMEOUT, DEBUG);
}


/*
* Name: find
* Description: Function used to match two string
* Params: 
* Returns: true if match else false
*/
boolean find(String string, String value){
  if(string.indexOf(value)>=0)
    return true;
  return false;
}

/*
* Name: readSerialMessage
* Description: Function used to read data from Arduino Serial.
* Params: 
* Returns: The response from the Arduino (if there is a reponse)
*/
String readSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(Serial.available()>0){
    value[index_count]=Serial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}



/*
* Name: readEsp8266Message
* Description: Function used to read data from ESP8266 Serial.
* Params: 
* Returns: The response from the esp8266 (if there is a reponse)
*/
String readEsp8266Message() {
  String message = "";   
  while (esp8266.available() > 0) {
    message += (char)esp8266.read();
  }
  return message;
}



/*
* Name: sendToEsp8266
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendToEsp8266(String command, const int timeout, boolean debug) {
  esp8266.println(command); // send the read character to the esp8266
  
  String response = "";
  long int time = millis();
  while (time + TIMEOUT > millis()) {
    while (esp8266.available()) {
      response += (char)esp8266.read();
    }
  }
  
  if (debug) {
    Serial.println("DEBUG (ESP): " + response);
  }
  return response;
}

String sendToUno(String command, const int timeout, boolean debug){
  Serial.println(command); // send the read character to the esp8266

  String response = "";
  long int time = millis();
  while (time + timeout > millis()) {
    while (Serial.available()) {
      response += (char)Serial.read();
    }
  }
  
  if (debug) {
    Serial.println("DEBUG (UNO): " + response);
  }
  return response;
}
