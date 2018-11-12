#include <SoftwareSerial.h>

// RX, TX for ESP8266
SoftwareSerial esp8266(2, 3);

bool DEBUG = true;
int TIMEOUT = 100;

#define BRAKE 0
#define CW 1
#define CCW 2
#define CS_THRESHOLD 15 // Definition of safety current (Check: "1.3 Monster Shield Example").

//MOTOR 1
#define MOTOR_A1_PIN 7
#define MOTOR_B1_PIN 8

//MOTOR 2
#define MOTOR_A2_PIN 4
#define MOTOR_B2_PIN 9

#define PWM_MOTOR_1 5
#define PWM_MOTOR_2 6

#define CURRENT_SEN_1 A2
#define CURRENT_SEN_2 A3

#define EN_PIN_1 A0
#define EN_PIN_2 A1

#define MOTOR_1 0
#define MOTOR_2 1

short usSpeed = 150; //default motor speed
unsigned short usMotor_Status = BRAKE;

void setup()
{
  // set build in led as output
  pinMode(13, OUTPUT);

  pinMode(MOTOR_A1_PIN, OUTPUT);
  pinMode(MOTOR_B1_PIN, OUTPUT);

  pinMode(MOTOR_A2_PIN, OUTPUT);
  pinMode(MOTOR_B2_PIN, OUTPUT);

  pinMode(PWM_MOTOR_1, OUTPUT);
  pinMode(PWM_MOTOR_2, OUTPUT);

  pinMode(CURRENT_SEN_1, OUTPUT);
  pinMode(CURRENT_SEN_2, OUTPUT);

  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);

          digitalWrite(EN_PIN_1, HIGH);
        digitalWrite(EN_PIN_2, HIGH);

  Serial.begin(9600);
//  esp8266.begin(9600); // Default baudrate (115200) causes issues
//  
//  sendToEsp8266("AT", TIMEOUT, DEBUG);
//  sendToEsp8266("AT+IPR=9600", TIMEOUT, DEBUG); // configure as access point
//  sendToEsp8266("AT+CWMODE=2", TIMEOUT, DEBUG); // configure as access point
//  sendToEsp8266("AT+CIFSR", TIMEOUT, DEBUG); // get ip address
//  sendToEsp8266("AT+CIPMUX=1", TIMEOUT, DEBUG); // configure for multiple connections
//  sendToEsp8266("AT+CIPSERVER=1,80", TIMEOUT, DEBUG); // turn on server on port 80
//
//  sendToUno("esp8266 connection is running!", TIMEOUT, DEBUG);
}


void loop()
{
        Forward();
delay(2000);
Stop();
delay(2000);
Reverse();
delay(2000);
Stop();
delay(2000);
  
//  if (Serial.available() > 0) {
//    String message = readSerialMessage();
//    if (find(message,"debugEsp8266:")){
//      String result = sendToEsp8266(message.substring(13,message.length()),TIMEOUT,DEBUG);
//      if(find(result,"OK"))
//        sendData("\nOK");
//      else
//        sendData("\nEr");
//    }
//  }
//  
//  if (esp8266.available() > 0) {
//    String message = readEsp8266Message();
//    Serial.println("RECEIVED: " + message);
//    
//    if (find(message, "L1")) {
//      digitalWrite(13, HIGH);
//    } else if (find(message, "L0")) {
//      digitalWrite(13, LOW);
//    }
//
//    // Motor control
//    digitalWrite(EN_PIN_1, HIGH);
//    digitalWrite(EN_PIN_2, HIGH);
//
//    if (find(message, "M0")) {
//        Stop();
//    } else if (find(message, "M1")) {
//        Forward();
//    } else if (find(message, "M2")) {
//        Reverse();
//    } else if (find(message, "M3")) {
//        IncreaseSpeed();
//    } else if (find(message, "M4")) {
//        DecreaseSpeed();
//    }
//  }
//
//  delay(TIMEOUT);
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

/////////////////////
/// Motor Control ///
/////////////////////

void Stop()
{
    Serial.println("Stop");
    usMotor_Status = BRAKE;
    motorGo(MOTOR_1, usMotor_Status, 0);
    motorGo(MOTOR_2, usMotor_Status, 0);
}

void Forward()
{
    Serial.println("Forward");
    usMotor_Status = CW;
    motorGo(MOTOR_1, usMotor_Status, usSpeed);
    motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void Reverse()
{
    Serial.println("Reverse");
    usMotor_Status = CCW;
    motorGo(MOTOR_1, usMotor_Status, usSpeed);
    motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void IncreaseSpeed()
{
    usSpeed = usSpeed + 10;
    if (usSpeed > 255) {
        usSpeed = 255;
    }

    Serial.print("Speed +: ");
    Serial.println(usSpeed);

    motorGo(MOTOR_1, usMotor_Status, usSpeed);
    motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void DecreaseSpeed()
{
    usSpeed = usSpeed - 10;
    if (usSpeed < 0) {
        usSpeed = 0;
    }

    Serial.print("Speed -: ");
    Serial.println(usSpeed);

    motorGo(MOTOR_1, usMotor_Status, usSpeed);
    motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void motorGo(uint8_t motor, uint8_t direct, uint8_t pwm) //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
    if (motor == MOTOR_1) {
        if (direct == CW) {
            digitalWrite(MOTOR_A1_PIN, LOW);
            digitalWrite(MOTOR_B1_PIN, HIGH);
        }
        else if (direct == CCW) {
            digitalWrite(MOTOR_A1_PIN, HIGH);
            digitalWrite(MOTOR_B1_PIN, LOW);
        }
        else {
            digitalWrite(MOTOR_A1_PIN, LOW);
            digitalWrite(MOTOR_B1_PIN, LOW);
        }

        analogWrite(PWM_MOTOR_1, pwm);
    }
    else if (motor == MOTOR_2) {
        if (direct == CW) {
            digitalWrite(MOTOR_A2_PIN, LOW);
            digitalWrite(MOTOR_B2_PIN, HIGH);
        }
        else if (direct == CCW) {
            digitalWrite(MOTOR_A2_PIN, HIGH);
            digitalWrite(MOTOR_B2_PIN, LOW);
        }
        else {
            digitalWrite(MOTOR_A2_PIN, LOW);
            digitalWrite(MOTOR_B2_PIN, LOW);
        }

        analogWrite(PWM_MOTOR_2, pwm);
    }
}
