#include <TheThingsNetwork.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
const char *appEui = "8CF9572000029B91";
const char *appKey = "9E88FBA619EA2777431108A54D2EF0F4";
#define loraSerial Serial1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *myMotor2 = AFMS.getStepper(200, 1);
int current_step = 0;
const int solarPin = 0;
void setup() {
   loraSerial.begin(57600);
  debugSerial.begin(115200);
  Serial.begin(9600);           // set up Serial library at 9600 bps
  ttn.reset(true);
  delay(50);
  Serial.println("-- STATUS");
  ttn.showStatus();
  Serial.println("-- JOIN");
  ttn.join(appEui, appKey, 8, 10);
  ttn.onMessage(callbackLora);
  AFMS.begin();    
  myMotor->setSpeed(10);
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
}
void loraSend(uint32_t payload) {
  Serial.println(payload);
  Serial.println(sizeof(payload));
  byte envoi[2];
  envoi[0] = highByte(payload);
  envoi[1] = lowByte(payload);
  ttn.sendBytes(envoi, sizeof(envoi));
  delay(5000);
}
void callbackLora(const uint8_t *payload, size_t size, port_t port) {
  Serial.println("Message received");
  if(payload[0] == 1){
    routine();
  }
  Serial.print("more");
  delay(500);
}

void loop(){
  loraSend(analogRead(solarPin));
  delay(10000);
}

bool safe_step(int steps) {
  if( steps > 200 ) return false;
  if ( current_step + steps  > 200 ){
    int close_gap = 200 - current_step;
    myMotor->step(close_gap, FORWARD, MICROSTEP);
    full_circle();
    myMotor->step(steps-close_gap, FORWARD, MICROSTEP);
    current_step = steps-close_gap;
  }
  else {
    myMotor->step(steps, FORWARD, MICROSTEP);
    current_step += steps;
  }
  return true;
}

void full_light_check(){
  //TODO
}

void full_circle(){
  myMotor->setSpeed(1);
  myMotor->step(200, BACKWARD, MICROSTEP);
  myMotor->setSpeed(1);
}

void routine() {
  long v = analogRead(solarPin);
   myMotor->step(20, FORWARD, MICROSTEP);
  long v2 = analogRead(solarPin);
  //Serial.println(v);
  //Serial.println(v2);
  if( v > v2) {
    search_light(BACKWARD);
  }
  else{
    search_light(FORWARD);
  }
}

void search_light(int DIRECTION) {
  long v, v2;
  do {
    v = analogRead(solarPin);
    myMotor->step(10, DIRECTION, MICROSTEP);
    v2 = analogRead(solarPin); 
   // Serial.println(v);
    //Serial.println(v2);
  } 
  while( v < v2 );
  myMotor->step(5, DIRECTION == FORWARD ? BACKWARD : FORWARD, MICROSTEP);
}
