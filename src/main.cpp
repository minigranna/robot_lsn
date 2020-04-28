#include <Arduino.h>

// deklaracja wykorzystanych pinow
#define left_motor_forward_pin 4
#define left_motor_backward_pin 3
#define left_motor_pwm_pin 2

#define right_motor_forward_pin 5
#define right_motor_backward_pin 6
#define right_motor_pwm_pin 7

//ustawienie parametrow filtra
#define filter_size 10

//deklaracja zmiennych
float speeds = 0.6;
float turn = 0;

unsigned long t1 = millis(); // zmienna do obliczania roznic czasu w milisekundach
unsigned int wait_time = 1000;

int stage = 0; // etap algorytmu
/*
stage = 0 jazda prosto
stage = 1 jazda do tylu przez 0.6s
stage = 2 zakrecanie w lewo przez 0.2s
stage = 3 zatrzymanie sie
*/

int filter_values[filter_size];
int measure_counter = 0;  
int sensor_value = 0;     


// funkcja odczytujaca i filtrujaca dane z czujnikow
void readDistances(){
  filter_values[measure_counter]= analogRead(A0);

  // usrednienie wartosci pomiarow
  int suma = 0;
  for(int i=0; i<filter_size; i++) {
    suma += filter_values[i];
  }

  sensor_value = suma/filter_size;

  measure_counter++;
  // wyzerowanie numeru pomiaru filtra
  if (measure_counter == filter_size) {
    measure_counter = 0;
  }
}


// 
void SetSpeed(float l_speed, float r_speed){
  // wystreowanie kierunku krecenia lewego kola
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

  // wystreowanie kierunku krecenia prawego kola
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

  // trymowanie predkosci kol aby robot jechal prosto
  if(l_speed>0.2) l_speed -= 0.15;
  if(l_speed<-0.2) l_speed += 0.1;
  
  // skalowanie wartosci predkosci, odciecie wartosci min i max dla obu kol
  int l_pwm = abs(l_speed)*255;
  if(l_pwm > 255) l_pwm = 255;
  if(l_pwm < 0) l_pwm = 0;

  int r_pwm = abs(r_speed)*255;
  if(r_pwm > 255) r_pwm = 255;
  if(r_pwm < 0) r_pwm = 0;

  // ustawienie wspolczynnika wypelnienia generatorow pwm
  analogWrite(left_motor_pwm_pin, l_pwm );
  analogWrite(right_motor_pwm_pin, r_pwm);
}

void setup() {
  // inicjalizacja portu szeregowego
  Serial.begin(9600);

  pinMode(13, OUTPUT);
}


// glowna petla programu
void loop() {
  readDistances();
  
  // algorytm sterowania
  if(stage == 0) {
    if(sensor_value>980){
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

  float l_speed = speeds + turn;
  float r_speed = speeds - turn;
  SetSpeed(l_speed, r_speed);

  Serial.println(sensor_value); 
}