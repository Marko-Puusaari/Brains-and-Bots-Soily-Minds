#include <Servo.h>
#include <CytronMotorDriver.h>

const int water_sensor_pin = A0;
const int high_water_level_light_pin = 12;
const int low_water_level_light_pin = 7;
const int servo_motor_pin = 2;
const int ultrasonic_sensor_trigger_pin = 8;
const int ultrasonic_sensor_echo_pin = 4;
const int on_off_test_pin = 13;
const int obstacle_detected_light_pin = 10;
const int move_time = 5000; //ms

const float stopping_distance = 10; //cm
int speed = 100;
const int wheel_diameter = 7.5; //cm
const int pi = 3.141592653589793238462643383;
const int wheel_circumference = pi * wheel_diameter;
const int wheel_separation = 20; //estimate: CHANGE THIS
const int turning_arc_circumference = pi * wheel_separation;

CytronMD left_motor(PWM_PWM, 6, 3);
CytronMD right_motor(PWM_PWM, 5, 11);

Servo servo_arm;

void setup() {
  //set pins to output for light bulbs
  pinMode(on_off_test_pin, OUTPUT);
  pinMode(high_water_level_light_pin, OUTPUT);
  pinMode(low_water_level_light_pin, OUTPUT);
  pinMode(obstacle_detected_light_pin, OUTPUT);

  //configure input and output pins for the ultrasonic sensor
  pinMode(4, INPUT);
  pinMode(8, OUTPUT);

  //Initialise for serial communication
  Serial.begin(9600);

  //Initialise the Servo arm
  servo_arm.attach(servo_motor_pin);
}

void blink(){
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
}

int water_level_sensor(){
  //water sensor reading
  int sensor_value = analogRead(water_sensor_pin);
  if (sensor_value > 350){
    digitalWrite(high_water_level_light_pin, HIGH);
    digitalWrite(low_water_level_light_pin, LOW);
  } else{
    digitalWrite(high_water_level_light_pin, LOW);
    digitalWrite(low_water_level_light_pin, HIGH);
  }
  return sensor_value;
}

void arm_down(){
  servo_arm.write(20);
  delay(2000);
}

void arm_up(){
  servo_arm.write(90);
  delay(2000);
}

float detect_distance(){
  //Send out the sound wave
  digitalWrite(ultrasonic_sensor_trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonic_sensor_trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonic_sensor_trigger_pin, LOW);

  //Detect the delay until the echo
  float duration = pulseIn(ultrasonic_sensor_echo_pin, HIGH);

  //Convert the echo time into a distance
  float distance = duration * 0.034 / 2;
  return distance;
}

void step_forwards(){
  float distance = detect_distance();
  unsigned long start_time = millis();
  digitalWrite(obstacle_detected_light_pin, LOW);
  if (distance > stopping_distance){
    right_motor.setSpeed(speed);
    left_motor.setSpeed(speed);
  } else{
    digitalWrite(obstacle_detected_light_pin, HIGH);
    return;
  }
  while (millis() - start_time < move_time){
    distance = detect_distance();
    if (distance < stopping_distance){
      digitalWrite(obstacle_detected_light_pin, HIGH);
      left_motor.setSpeed(0);
      right_motor.setSpeed(0);
      return;
    }
  }
  right_motor.setSpeed(0);
  left_motor.setSpeed(0);
}

void step_backwards(){
  float distance = detect_distance();
  unsigned long start_time = millis();
  if (distance > stopping_distance){
    digitalWrite(obstacle_detected_light_pin, LOW);
    right_motor.setSpeed(-speed);
    left_motor.setSpeed(-speed);
  } else{
    digitalWrite(obstacle_detected_light_pin, HIGH);
    return;
  }
  while (millis() - start_time < move_time){ //Estimating that detect_distance takes 30ms to run
    delay(5);
  }
  right_motor.setSpeed(0);
  left_motor.setSpeed(0);
}

void turn_left(){
  unsigned long start_time = millis();
  left_motor.setSpeed(-speed);
  right_motor.setSpeed(speed);
  while (millis() - start_time < 3000 * 0.6){ //3000 is an estimate: FINE TUNE!!!
    delay(5);
  }
  right_motor.setSpeed(0);
  left_motor.setSpeed(0);
}

void turn_right(){
  unsigned long start_time = millis();
  left_motor.setSpeed(speed);
  right_motor.setSpeed(-speed);
  while (millis() - start_time < 3000 * 0.6){ //3000 is an estimate: FINE TUNE!!!
    delay(5);
  }
  right_motor.setSpeed(0);
  left_motor.setSpeed(0);
}

void robo_dance(){
  for (int i = 0; i < 4; i++){
    delay(250);
    digitalWrite(on_off_test_pin, HIGH);
    delay (250);
    digitalWrite(on_off_test_pin, LOW);
  }
  for (int i = 0; i < 10; i++){
    delay(100);
    digitalWrite(on_off_test_pin, HIGH);
    delay (100);
    digitalWrite(on_off_test_pin, LOW);
  }
  arm_up();
  for (int i = 0; i < 4; i++){
    turn_left();
  }
  arm_down();
  for (int i = 0; i < 4; i++){
    turn_right();
  }
  step_forwards();
  step_backwards();
}

void run_commands(){
  int water_level = water_level_sensor();
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "ARM_DOWN"){
      arm_down();
      Serial.println("COMPLETE");
    }
    if (command == "ARM_UP"){{
      arm_up();
      Serial.println("COMPLETE");
    }}
    if (command == "ON") {
      digitalWrite(13, HIGH);
      Serial.println("COMPLETE");
    }
    if (command == "OFF") {
      digitalWrite(13, LOW);
      Serial.println("COMPLETE");
    }
    if (command == "SENSE_WATER"){
      Serial.println(water_level);
    }
    if (command == "DISTANCE"){
      float distance = detect_distance();
      Serial.println(distance);
    }
    if (command == "EXIT"){
      left_motor.setSpeed(0);
      right_motor.setSpeed(0);
      Serial.println("EXITED (no more commands can be run)");
      while(true){}
    }
    if (command == "WHAT_DO_YOU_LIKE"){
      Serial.println("I LIKE THE PEANUTS :>");
    } 
    if (command == "STEP_FORWARDS"){
      step_forwards();
      Serial.println("COMPLETE");
    } 
    if (command == "STEP_BACKWARDS"){
      step_backwards();
      Serial.println("COMPLETE");
    } 
    if (command == "QUARTER_TURN_LEFT"){
      turn_left();
      Serial.println("COMPLETE");
    }
    if (command == "QUARTER_TURN_RIGHT"){
      turn_right();
      Serial.println("COMPLETE");
    }
    if (command == "ROBO_DANCE"){
      robo_dance();
    }
  }
}

void loop() {
  run_commands();
  float distance = detect_distance();
  if (distance < stopping_distance){
      digitalWrite(obstacle_detected_light_pin, HIGH);
  } else {
      digitalWrite(obstacle_detected_light_pin, LOW);
  }

  //step_forwards();

  //digitalWrite(high_water_level_light_pin, HIGH);
}
