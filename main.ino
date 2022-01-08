#include <PWMServo.h>
#include "ArduinoESPAT.h"

#define SERVO 9
#define PRESS 1
#define R1 10

ESPAT espat("SSID","PASSWORD");
PWMServo g_servo;

void setup() {
  pinMode(7,INPUT);
  g_servo.attach(SERVO);

  Serial.begin(9600);
  espat.begin();
  espat.tryConnectAP();
  espat.clientIP();
}

void mail(int flag) {
  if(flag == 0){
    espat.get("maker.ifttt.com","/trigger/weight_mail/with/key/KEY",80);
    delay(1000);
  }else{
    espat.get("maker.ifttt.com","/trigger/finish_mail/with/key/KEY",80);
    delay(1000);
  }
}

double weight(int data) {
  double v,r,fg,kg,g;
  v = data * 5.0 / 1024;
  r = R1 * v / (5.0 - v);
  fg = 880.79/r + 47.96;
  kg = (fg / 4448) * 10 - 0.1;
  g = kg * 1000 - 8.0;

  return g;
}

int count = 0;
int count_final = 0;
int flag = 0;
double food_after;
double abs_data;

void loop() {
  int data,i;
  double food_weight;               //お皿のエサの重さ
  double food_servo;                //サーボから出した餌の重さ

  if(flag == 0) {
    food_weight = weight(analogRead(PRESS));
    Serial.print("weight:");
    Serial.println(food_weight);
    Serial.print("count:");
    Serial.println(count);
    Serial.print("\n");
    abs_data = food_weight - food_after;
    if(abs_data > 5) {
      count++;
    }
    food_after = food_weight;
    delay(1000);
  }

  if(count == 3 && count_final != 2 && flag == 0) {
    mail(flag);
    delay(4000);
    food_weight = weight(analogRead(PRESS));
    //delay(600000);
    delay(2000);
    g_servo.write(0);
    delay(300);
    g_servo.write(90);
    delay(1000);
    food_servo = weight(analogRead(PRESS));
    Serial.print("food_weight:");
    Serial.println(food_weight);
    Serial.print("food_servo:");
    Serial.println(food_servo);
    food_servo -= food_weight;
    Serial.print("Output food:");
    Serial.println(food_servo);
    Serial.print("\n");
    count = 0;
    count_final++;
    delay(1000);
  }

  if(count_final == 2) {
    flag = 1;
    count_final = 0;
    mail(flag);
  }
}


/*体重と餌のカロリーを元に一日の適切量を求める
  その適切量を何分割かに分けて出す
  サーボの閉じる速さで出す量を調節
  countとcount_finalの条件分岐の値を変更
  count++の条件分岐の値を変更*/
