#include <Arduino.h>
#include <Wire.h>

const int claw_motor_STEP = 2;
const int claw_motor_DIR = 3;
const int claw_motor_ENA = 4;
const int R_motor_STEP = 5;
const int R_motor_DIR = 6;
const int R_motor_ENA = 7;
const int L_motor_STEP = 8;
const int L_motor_DIR = 9;
const int L_motor_ENA = 10;
const int stopper_motor_STEP = 11; 
const int stopper_motor_DIR = 12; 
const int stopper_motor_ENA = 13; 

int steps_per_rev = 3200; 
int steps_per_90 = 800; 
int global_claw_offset = 280; 

int claw_delay = 150; // delay in microseconds
int RL_delay = 100; 
int stopper_delay = 200;

float target_claw_resting_angles[4] = {319.65, 237.041, 145.98, 57.04};

bool is_stopper_up = true; 

int position = 0; // pos = 0 denotes green face in front
int claw_position = 0; // claw_position = 0 means tape on top, increases ccw

bool sequential = false; 

bool unsolved_solvestring = false;
int solvestring_iterator = 0;  

void setup() {
  for (int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
  }
  digitalWrite(claw_motor_ENA, HIGH);
  digitalWrite(R_motor_ENA, HIGH);
  digitalWrite(L_motor_ENA, HIGH);
  digitalWrite(stopper_motor_ENA, HIGH);

  Wire.begin();
  Wire.setClock(800000L);

  Serial.begin(115200);
  Serial.println("Serial open");
}

void loop() {
  /*
  if (Serial.available() > 0) {
    char inval = Serial.read();
    seq = int(Serial.read()); 
    checkMove(inval); 
  }
 */
  custom_serial_read(); 
}

void custom_serial_read() {
  if (Serial.available() > 0) {
    char inchar = Serial.read();
    if (inchar == '<') {
      unsolved_solvestring = true; 
      solvestring_iterator = 0; 
      
      Serial.println("Received solvestring: "); 
      String solvestring = Serial.readStringUntil('>');
      char solve_array[solvestring.length() + 1];
      solvestring.toCharArray(solve_array, solvestring.length() + 1);
      for (int i = 0; i < solvestring.length(); i++) {
        Serial.print(solve_array[i]); 
      }
      for (int i = 0; i < solvestring.length(); i++) {
        if (solvestring[i] == solvestring[i+1] && i < solvestring.length() - 1) {
          sequential = true; 
        }
        else {
          sequential = false; 
        }
        checkMove(solvestring[i]);
        delay(10); 
        Serial.println("X"); 
      }
    }
    else {
      checkMove(inchar); 
    }
  }
}

void checkMove(char input_move) {
  switch (input_move) {
    case 'R':
      right_turn(false);
      break;
    case 'r':
      right_turn(true);
      break;
    case 'L':
      left_turn(true);
      break;
    case 'l':
      left_turn(false);
      break;
    case 'B':
      back_turn(position, false);
      break;
    case 'b':
      back_turn(position, true);
      break;
    case 'U':
      up_turn(position, false);
      break;
    case 'u':
      up_turn(position, true);
      break;
    case 'F':
      front_turn(position, false);
      break;
    case 'f':
      front_turn(position, true);
      break;
    case 'D':
      down_turn(position, false);
      break;
    case 'd':
      down_turn(position, true);
      break;
    case 'S':
      stopper_up(); 
      break; 
    case 's':
      stopper_down(); 
      break; 
    case 'Z':
      rotate_entire_cube(true);
      break;
    case 'z':
      rotate_entire_cube(false);
      break;
    case 'W':
      digitalWrite(claw_motor_ENA, LOW);
      digitalWrite(R_motor_ENA, LOW);
      digitalWrite(L_motor_ENA, LOW);
      digitalWrite(stopper_motor_ENA, LOW);
      break;
    case 'E':
      digitalWrite(claw_motor_ENA, HIGH); 
      digitalWrite(R_motor_ENA, HIGH);
      digitalWrite(L_motor_ENA, HIGH);
      digitalWrite(stopper_motor_ENA, HIGH);
      break;
    case 'C':
      correct_claw(); 
      break; 
    case 'M':
      Serial.println(ReadRawAngle());
      target_claw_resting_angles[0] = ReadRawAngle();  
      break; 
  }
}

void motorstep(int motor_pin, int delay) {
  int half_delay = delay * 0.5;
  digitalWrite(motor_pin, HIGH);
  delayMicroseconds(half_delay);
  digitalWrite(motor_pin, LOW);   
  delayMicroseconds(half_delay);
}

void rotate_entire_cube(bool prime) {
  digitalWrite(R_motor_DIR, prime);
  digitalWrite(L_motor_DIR, !prime);
  for (int i = 0; i < steps_per_90; i++) {
    motorstep(R_motor_STEP, RL_delay);
    motorstep(L_motor_STEP, RL_delay);
  }
}

/*
void rotate_claw(bool prime, int claw_offset) {
  digitalWrite(claw_motor_DIR, prime);
  for (int i = 0; i < steps_per_90; i++) {
    motorstep(claw_motor_STEP, claw_delay);
  }
  for (int i = 0; i < claw_offset; i++) {
    motorstep(claw_motor_STEP, claw_delay);
  }
  digitalWrite(claw_motor_DIR, !prime);
  delay(100);
  for (int i = 0; i < claw_offset; i++) {
    motorstep(claw_motor_STEP, claw_delay);
  }
}
*/

void rotate_claw(bool prime, int useless_val) {
  digitalWrite(claw_motor_DIR, prime);
  int steps_per_120 = 1066; 
  for (int i = 0; i < steps_per_120; i++) {
    motorstep(claw_motor_STEP, claw_delay);
  }
  digitalWrite(claw_motor_DIR, !prime);
  delay(100);
  int retrace_steps = 266; 
  for (int i = 0; i < retrace_steps; i++) {
    motorstep(claw_motor_STEP, claw_delay);
  }
  if (prime) {
    if (claw_position < 3) {
      claw_position--; 
    }
    else {
      claw_position = 0; 
    }
  }
  else {
    if (claw_position > 0) {
      claw_position++; 
    }
    else {
      claw_position = 3; 
    }
  }
  Serial.println(claw_position); 
  //correct_claw(); 
}

void correct_claw() {
  float cur_angle = ReadRawAngle(); 
  float target_angle = target_claw_resting_angles[0]; 
  float diff = cur_angle - target_angle;
  float tol = 0.225; // max allowed error in deg
  bool local_prime;  
  if (diff < 0) {
    local_prime = true; 
  }
  else {
    local_prime = false; 
  }
  digitalWrite(claw_motor_DIR, local_prime); 
  while (abs(diff) > tol) { 
    motorstep(claw_motor_STEP, claw_delay);
    diff = ReadRawAngle() - target_angle;  
  }
  Serial.println(ReadRawAngle()); 
}

void back_turn(int pos, bool prime) {
  stopper_down();
  int passed_claw_offset; 
  if (sequential) {
    passed_claw_offset = 0;
  }
  else {
    passed_claw_offset = global_claw_offset; 
  }
  
  if (pos == 0) {
    rotate_claw(prime, passed_claw_offset); 
  }
  else if (pos == 1) {
    rotate_entire_cube(false);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 2) {
    rotate_entire_cube(true);
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else {
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  position = 0; // set global position variable to indicate new position
}

void up_turn(int pos, bool prime) {
  stopper_down();
  int passed_claw_offset; 
  if (sequential) {
    passed_claw_offset = 0;
  }
  else {
    passed_claw_offset = global_claw_offset; 
  }
  
  if (pos == 0) {
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 1) {
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 2) {
    rotate_entire_cube(false);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else {
    rotate_entire_cube(false);
    rotate_entire_cube(false);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  position = 1; // set global position variable to indicate new position
}

void front_turn(int pos, bool prime) {
  stopper_down();
  int passed_claw_offset; 
  if (sequential) {
    passed_claw_offset = 0;
  }
  else {
    passed_claw_offset = global_claw_offset; 
  }
  
  if (pos == 0) {
    rotate_entire_cube(true);
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 1) {
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 2) {
    rotate_claw(prime, passed_claw_offset);
  }
  else {
    rotate_entire_cube(false);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  position = 2; // set global position variable to indicate new position
}

void down_turn(int pos, bool prime) {
  stopper_down();
  int passed_claw_offset; 
  if (sequential) {
    passed_claw_offset = 0;
  }
  else {
    passed_claw_offset = global_claw_offset; 
  }
  
  if (pos == 0) {
    rotate_entire_cube(false);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 1) {
    rotate_entire_cube(true);
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else if (pos == 2) {
    rotate_entire_cube(true);
    delay(50);
    rotate_claw(prime, passed_claw_offset);
  }
  else {
    rotate_claw(prime, passed_claw_offset);
  }
  position = 3; // set global position variable to indicate new position
}

void right_turn(bool prime) {
  stopper_up();
  delay(50);  
  digitalWrite(R_motor_DIR, !prime);
  for (int i = 0; i < steps_per_90; i++) {
    motorstep(R_motor_STEP, RL_delay);
  }
  delay(50);
  if (!sequential) { 
    stopper_down();
  } 
}

void left_turn(bool prime) {
  stopper_up(); 
  delay(50); 
  digitalWrite(L_motor_DIR, prime);
  for (int i = 0; i < steps_per_90; i++) {
    motorstep(L_motor_STEP, RL_delay); 
  }
  delay(50); 
  if (!sequential) { 
    stopper_down();
  }  
}

void stopper_up() {
  if (!is_stopper_up) {
    digitalWrite(stopper_motor_DIR, is_stopper_up);
    is_stopper_up = !is_stopper_up; 
    for (int i = 0; i < steps_per_90; i++) {
      motorstep(stopper_motor_STEP, stopper_delay); 
    }
  }
}

void stopper_down() {
  if (is_stopper_up) {
    digitalWrite(stopper_motor_DIR, is_stopper_up);
    is_stopper_up = !is_stopper_up; 
    for (int i = 0; i < steps_per_90+2; i++) {
      motorstep(stopper_motor_STEP, stopper_delay); 
    }
  }
}

float ReadRawAngle() { // call takes ~130 microseconds
    //----- read low-order bits 7:0
    Wire.beginTransmission(0x36); // connect to the sensor
    // read both low + high order bits (0x0C and 0x0D, autoincrementin address)
    // figure 21 - register map: Raw angle (11:8)
    // figure 21 - register map: Raw angle (7:0)
    Wire.write(0x0C);
    Wire.endTransmission();       // end transmission
    uint8_t ret = Wire.requestFrom(0x36, 2);    // request from the sensor
    //Serial.println("Wire.requestFrom(): " + String(ret));
    while (Wire.available() != 2) {} // wait until it becomes available
    int highbyte = Wire.read(); // Reading the data after the request
    int lowbyte = Wire.read();

    //Serial.println("Received from Wire.read(): " + String(highbyte) +  ", " + String(lowbyte));

    uint16_t rawAngle = (uint16_t)((highbyte & 0xffu)<< 8u) | (lowbyte & 0xffu);
    //Serial.println("Raw angle: " + String(rawAngle));
    float degAngle = rawAngle * 0.087890625; // 360/4096 = 0.087890625
    //Serial.println("Degree Angle: " + String(degAngle, 4));
    return degAngle;
}
