#include <Servo.h> //used to generate PWM signal for motor control
Servo servoL;
Servo servoR;

//ultrasound pins
#define echoPin 2
#define trigPin 3

//turn and timer indexing
int route_idx = 0;
int turn_idx = -1;
int turn_idx_prev = -2;
float t_start = millis(); 

//straightaway running times
float t[] = {0,0,0,0,0};

//parameters to calibrate for proper turning and movement
int Lf = 3000;
int Rf = 1750;
int Lb = 1750;
int Rb = 2250;
int off = 2000;
int turn_delay = 430;

float sense(){
  //send pulse out
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //read return wave and calculate distance
  return pulseIn(echoPin, HIGH) * (0.0343/2);
}

void Turn(){
  //order of operations for turning
  switch(turn_idx){
    case 0:
      Left();
      break;
    case 1:
      Right();
      break;
    case 2:
      Right();
      break;
    case 3:
      Left();
      break;
    case 4:
      Stop();
      route_idx = 1;
      break;
  }
}

//Writing speeds to servomotors
void Fwd(){
  //both servos forwards
  servoL.writeMicroseconds(Lf);
  servoR.writeMicroseconds(Rf);}
void Bck(){
  //both servos backwards
  servoL.writeMicroseconds(Lb);
  servoR.writeMicroseconds(Rb);}
void Stop(){
  //both servos off
  servoL.writeMicroseconds(off);
  servoR.writeMicroseconds(off);
  delay(200);}
void Left(){
  //left backwards, right forwards
  servoL.writeMicroseconds(Lb);
  servoR.writeMicroseconds(Rf);
  delay(turn_delay);
  Stop();}
void Right(){
  //right backwards, left forwards
  servoL.writeMicroseconds(Lf);
  servoR.writeMicroseconds(Rb);
  delay(turn_delay);
  Stop();}

void setup() {
  Serial.begin(9600);
  //initialize and select Servo pins
  servoL.attach(13);
  servoR.attach(12);
  //initialize and select ultrasound sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  t_start = millis(); 
}

void loop() {
  //if on initial (blue) route
  if(route_idx == 0){
    //check whether to start timer for straightaway
    if(turn_idx != turn_idx_prev){
      t_start = millis();
      turn_idx_prev = turn_idx;
    }
    //move forwards
    Fwd();
    //check ultrasound for obstacle
    if(sense() < 5){
      turn_idx = turn_idx + 1;
      //save straightaway time
      t[turn_idx] = millis() - t_start;
      Stop();
      //call function to institute turning order given
      Turn();
      //increase route indicies
    }
  }
  //repeat all initial movements except backwards
  else if(route_idx == 1){
    Bck();
    delay(t[4]);
    Stop();
    Right();
    Bck();
    delay(t[3]);
    Stop();
    Left();
    Bck();
    delay(t[2]);
    Stop();
    Left();
    Bck();
    delay(t[1]);
    Stop();
    Right();
    Bck();
    delay(t[0]);
    Stop();
  }
  //delay while waiting for straightaway
  delay(10);
}
