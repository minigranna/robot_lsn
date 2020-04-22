#include <Arduino.h>

#define left_motor_forward_pin 4
#define left_motor_backward_pin 3
#define left_motor_pwm_pin 2

#define right_motor_forward_pin 5
#define right_motor_backward_pin 6
#define right_motor_pwm_pin 7

#define liczba_pomiarow 10

float l_speed = 0;
float r_speed = 0;
float speeds = 0.6;
float turn = 0;

unsigned long t1 = millis(); 
unsigned int wait_time = 1000;

int stage = 0; 

int sensorValue[liczba_pomiarow];
int n = 0;  
int d1 = 0;     

void zmierzDystans(){
    if (n == liczba_pomiarow) {
    n = 0;
  }
  
  sensorValue[n]= analogRead(A0);

  d1 = 0;
  for(int i=0; i<liczba_pomiarow; i++) {
    d1 += sensorValue[i];
  }
  d1 = d1/liczba_pomiarow;
  n++;
}

void SetSpeed(float l_speed, float r_speed){
  if(l_speed>0.3) {
    digitalWrite(left_motor_forward_pin, HIGH);
    digitalWrite(left_motor_backward_pin, LOW);
  }
  else if(l_speed<-0.3) {
    digitalWrite(left_motor_forward_pin, LOW);
    digitalWrite(left_motor_backward_pin, HIGH);
  }
  else {
    digitalWrite(left_motor_forward_pin, LOW);
    digitalWrite(left_motor_backward_pin, LOW);
  }

 
   if(r_speed>0.3) {
    digitalWrite(right_motor_forward_pin, HIGH);
    digitalWrite(right_motor_backward_pin, LOW);
  }
  else if(r_speed<-0.3) {
    digitalWrite(right_motor_forward_pin, LOW);
    digitalWrite(right_motor_backward_pin, HIGH);
  }
  else {
    digitalWrite(right_motor_forward_pin, LOW);
    digitalWrite(right_motor_backward_pin, LOW);
  }

  if(l_speed>0.2) l_speed -= 0.15;
  

  if(l_speed<-0.2) l_speed += 0.1;
  
  int l_pwm = abs(l_speed)*255;
  if(l_pwm > 255) l_pwm = 255;
  if(l_pwm < 0) l_pwm = 0;

  int r_pwm = abs(r_speed)*255;
  if(r_pwm > 255) r_pwm = 255;
  if(r_pwm < 0) r_pwm = 0;

  Serial.print("r: ");
  Serial.print(r_pwm);
  Serial.print(" l: ");
  Serial.println(l_pwm);

  analogWrite(left_motor_pwm_pin, l_pwm );
  analogWrite(right_motor_pwm_pin, r_pwm);
}

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  zmierzDystans();
  
  if(stage == 0) {
    if(d1>980){
      digitalWrite(13, HIGH);
      t1 = millis();
      wait_time = 0;
      stage = 1;
    } else{
      digitalWrite(13, LOW);
      speeds = 0.6;
    }    
  }

  if(millis()-t1 > wait_time){
    t1 = millis();
    if(stage == 1) {
      speeds = -0.6;
      wait_time = 600;
      stage = 2;
    } else if(stage == 2){
      turn = -0.6;
      speeds = 0;
      wait_time = 200;
      stage = 3;
    } else if(stage == 3){
      turn = 0;
      stage = 0;      
    }
  }

  SetSpeed(speeds + turn, speeds - turn);

  Serial.println(d1); 
}