#include <Arduino.h>

// deklaracja wykorzystanych pinow
#define left_motor_forward_pin 4
#define left_motor_backward_pin 3
#define left_motor_pwm_pin 2

#define right_motor_forward_pin 5
#define right_motor_backward_pin 6
#define right_motor_pwm_pin 7

int port_number[6] = {A4, A5, A3, A1, A2, A0};

//ustawienie parametrow filtra
#define filter_size 10

#define sensor_level 800

//deklaracja zmiennych
float speeds = 0.35;
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

int filter_values[6][filter_size];
int measure_counter = 0;  
int sensor_value[6];     


// funkcja odczytujaca i filtrujaca dane z czujnikow
void readDistances(){
  for(int a=0; a<6; a++){
    delay(1);
    filter_values[a][measure_counter]= analogRead(port_number[a]);

    // usrednienie wartosci pomiarow
    int suma = 0;
    for(int i=0; i<filter_size; i++) {
      suma += filter_values[a][i];
    }

    sensor_value[a] = suma/filter_size;

    measure_counter++;
    // wyzerowanie numeru pomiaru filtra
    if (measure_counter > filter_size) {
      measure_counter = 0;
    }
  }
}


bool isFrontOffTable(){
  return (sensor_value[0]>sensor_level or sensor_value[1]>sensor_level or sensor_value[5]>sensor_level);
}

bool isBackOffTable(){
  return (sensor_value[2]>sensor_level or sensor_value[3]>sensor_level or sensor_value[4]>sensor_level);
}


// 
void SetSpeed(float l_speed, float r_speed){
  // wystreowanie kierunku krecenia lewego kola
  if(l_speed>0.1) {
    digitalWrite(left_motor_forward_pin, HIGH);
    digitalWrite(left_motor_backward_pin, LOW);
  }
  else if(l_speed<-0.1) {
    digitalWrite(left_motor_forward_pin, LOW);
    digitalWrite(left_motor_backward_pin, HIGH);
  }
  else {
    digitalWrite(left_motor_forward_pin, LOW);
    digitalWrite(left_motor_backward_pin, LOW);
  }

  // wysterowanie kierunku krecenia prawego kola
  if(r_speed>0.1) {
    digitalWrite(right_motor_forward_pin, HIGH);
    digitalWrite(right_motor_backward_pin, LOW);
  }
  else if(r_speed<-0.1) {
    digitalWrite(right_motor_forward_pin, LOW);
    digitalWrite(right_motor_backward_pin, HIGH);
  }
  else {
    digitalWrite(right_motor_forward_pin, LOW);
    digitalWrite(right_motor_backward_pin, LOW);
  }

  // trymowanie predkosci kol aby robot jechal prosto
  if(l_speed>0.1) l_speed -= 0.08;
  if(l_speed<-0.1) l_speed += 0.05;
  
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


void logic(){
  // algorytm sterowania
  if(stage == 0) {
    digitalWrite(13, HIGH);
    if(isFrontOffTable()){
      t1 = millis();
      speeds = -0.35;
      turn = 0;
      wait_time = 800;
      stage = 1;
    } else{
      speeds = 0.35;
      turn = 0;
    }    
  } else{
    digitalWrite(13, LOW);

  }
  
  if(millis()-t1 > wait_time){
    if(stage == 1){
      t1 = millis();
      speeds = 0;
      turn = -0.35;
      wait_time = 400;
      stage = 2;
    } else if (stage == 2){
      t1 = millis();
      turn = 0;
      speeds = 0;
      wait_time = 0;
      stage = 0;
    } 
  } else{
    if(stage == 1){
      if(isBackOffTable()){
        t1 = millis();
        speeds = 0.35;
        turn = 0;
        wait_time = 100;
        stage = 1;
      } 
    }
  }
}


void setup() {
  // inicjalizacja portu szeregowego
  Serial.begin(9600);

  pinMode(13, OUTPUT);
}


// glowna petla programu
void loop() {
  readDistances();
  logic();

  float l_speed = speeds + turn;
  float r_speed = speeds - turn;
  SetSpeed(l_speed, r_speed);

  // for(int a=0; a<6; a++){
  //   Serial.print(sensor_value[a]>800);
  //   Serial.print(" ");
  // }  
  // Serial.print(isFrontOffTable());
  // Serial.print(" ");
  // Serial.print(isBackOffTable());
  // Serial.println("");
}