import processing.serial.*;  

Serial python_port; 
Serial arduino_port;
Serial color_port; 

int cubie_size = 50;
int gap_size = 2;

int[] Ux = new int[9];
int[] Uy = new int[9];

int[] Rx = new int[9];
int[] Ry = new int[9];

int[] Fx = new int[9];
int[] Fy = new int[9];

int[] Dx = new int[9];
int[] Dy = new int[9];

int[] Lx = new int[9];
int[] Ly = new int[9];

int[] Bx = new int[9];
int[] By = new int[9];

int[] buttonx = new int[10];
int[] buttony = new int[10];

char[] up = {'U','U','U','U','U','U','U','U','U'};
char[] right = {'R','R','R','R','R','R','R','R','R'};
char[] front = {'F','F','F','F','F','F','F','F','F'};
char[] down = {'D','D','D','D','D','D','D','D','D'};
char[] left = {'L','L','L','L','L','L','L','L','L'};
char[] back = {'B','B','B','B','B','B','B','B','B'};

int side_indicator = 0; 

String[] scan_button_char_list = {"U", "F", "R", "L", "D", "B"};

String[] split_solvestring;
String[] scramblestring = new String[30];

boolean disabled; 

boolean solveflag_pressed = false;
boolean request_solve_flag = false;
boolean unsolved_solvestring = false;
boolean disableflag_pressed = false; 
boolean request_scan_flag = false; 

boolean button1_flag_pressed = false; 
boolean button2_flag_pressed = false; 
boolean button3_flag_pressed = false; 
boolean button4_flag_pressed = false; 
boolean button5_flag_pressed = false; 
boolean button6_flag_pressed = false; 

boolean scrambleflag_pressed = false; 
boolean request_scramble_flag = false; 
boolean unsolved_scramble = false;
boolean new_move_request = false; 
boolean request_disable = false; 

int solvestring_iterator = 0; 
int scramblestring_iterator = 0; 

int lowest_dist_index;
int lowest_dist_set;

int scan_button_size = 40; 

int last_millis;
boolean new_millis = true;

void setup() {  
  
  size(800,800);
  background(255);
  fill(255,0,0); 
  stroke(0);
  
  setupCoordinates();   
  drawStartCube();
  drawButtons(); 
  
  String arduino_port_id = "/dev/ttyACM0";
  try {
    arduino_port = new Serial(this, arduino_port_id, 115200); 
    arduino_port.clear(); 
  } catch (RuntimeException e) {
    println("Can't connect to the arduino's serial port: " + arduino_port_id);
  }

  // serial stuff:  
  String python_port_id = "/tmp/vserial2"; // permanent virtual port
  try {
     python_port = new Serial(this, python_port_id, 115200);  
     python_port.clear(); 
  } catch (RuntimeException e) {
    println("Can't connect to python serial port: " + python_port_id);
  }
  
  String color_port_id = "/tmp/vserial4";
  try {
     color_port = new Serial(this, color_port_id, 115200);  
     color_port.clear(); 
  } catch (RuntimeException e) {
    println("Can't connect to python opencv serial port: " + color_port_id);
  }
  //String portname2 = "/dev/ttyACM0";
  //arduino_port = python_port = new Serial(this, portname2, 115200);  // define input port 
  //arduino_port.clear();
  
  //setCubestate("DRLUUBFBRBLURRLRUBLRDDFDLFUFUFFDBRDUBRUFLLFDDBFLUBLRBD");
  parseCubestate();
  
}

void draw() {   
  color_receive(); 
  checkButtonFlags(); // button flags
  checkRequestFlags(); // request flags
  updateArduinoFlags(); 
  //checkMoveFlags(); // processing cube rotation flags
  checkMoveFlagsArduino();
  parseCubestate();  // apply any array manipulations to the cube graphic
}

void test() {
  checkRequestFlags();
  checkMoveFlags();
}

String python_receive() { // generic receive data from python script
  String inString = "";
  if (python_port.available () > 0) {
    inString = python_port.readString();
    inString.trim();
    //println(inString);
    //println(inString.length()); 
  }
  return inString;
}

String color_receive() { // generic receive data from python script
  String inString = "";
  if (color_port.available () > 0) {
    inString = color_port.readString();
    inString.trim();
    char[] invals = inString.toCharArray(); 
    switch(invals[0]) {
      case 'U':
        for (int i = 1; i < 10; i++) {
        up[i-1] = invals[i];
      }
      case 'F':
        for (int i = 1; i < 10; i++) {
        front[i-1] = invals[i];
      }
    
    println(inString);
    //println(inString.length()); 
  }
  return inString;
}



String arduino_receive() {
  String inString = "";
  if (arduino_port.available() > 0) {
    inString = arduino_port.readStringUntil('\n');
    if (inString != null) {
      inString = trim(inString); 
      //print(inString); 
    }
    else {
      inString = "";
    }
  }
  return inString; 
}

void updateArduinoFlags() {
  String confirmation = arduino_receive(); 
  if (confirmation.contains("X")) {
    new_move_request = true; 
    //print("new move request"); 
  }
  else {
    new_move_request = false;
  }
}


void request_solve() { // requests solve given current cubestate, returns solve string 
  String solvestring = ""; 
  python_port.write("solve"); 
  python_port.write('\n'); 
  for (int i = 0; i < 9; i++) {
    python_port.write(up[i]);
  }
  for (int i = 0; i < 9; i++) {
    python_port.write(right[i]);
  }
  for (int i = 0; i < 9; i++) {
    python_port.write(front[i]);
  }
  for (int i = 0; i < 9; i++) {
    python_port.write(down[i]);
  }
  for (int i = 0; i < 9; i++) {
    python_port.write(left[i]);
  }
  for (int i = 0; i < 9; i++) {
    python_port.write(back[i]);
  }
  python_port.write('\n');
  
  if (python_port.available() == 0) {
    delay(1000); 
  }
  
  solvestring = python_receive();
  split_solvestring = split(solvestring, " ");
  String converted_solvestring = convert_to_arduino_string(split_solvestring); 
  println("Received solution: " + solvestring);
  println("Solution length: " + split_solvestring.length);
  println("Converted solution: " + converted_solvestring); 
  
  unsolved_solvestring = true; 
  solvestring_iterator = 0;
  
  arduino_port.write(converted_solvestring); 
}

void request_scan() { // requests openCV scan from python script, returns 'state' string -- must still parse string into char arrays. 
  String state = "";
  
  color_port.write("scan");
  color_port.write('\n');
  delay(500); 
  
  state = color_receive();
  
  switch (side_indicator) {
    case 0:
      up = state.toCharArray(); 
      break;
    case 1:
      right = state.toCharArray(); 
      break;
    case 2:
      front = state.toCharArray(); 
      break;
    case 3:
      down = state.toCharArray(); 
      break;
    case 4:
      left = state.toCharArray(); 
      break;
    case 5:
      back = state.toCharArray(); 
      break;
  }
  
  
}

void checkMove(String move) { // takes in a move from a string array and performs it on the virtual cube
  switch(move) {
        case "R":
          right_turn(false); // R
          break;
        case "R'":
          right_turn(true); // R'
          break;
        case "U":
          up_turn(false); // U
          break;
        case "U'":
          up_turn(true); // U'
          break;
        case "F":
          front_turn(false); // F
          break;
        case "F'":
          front_turn(true); // F'
          break;
        case "L":
          left_turn(false); // L
          break;
        case "L'":
          left_turn(true); // L'
          break;
        case "D":
          down_turn(false); // D
          break;
        case "D'":
          down_turn(true); // D'
          break;
        case "B":
          back_turn(false); // B
          break;
        case "B'":
          back_turn(true); // B'
          break;
        case "R2":
          right_turn(false);
          right_turn(false);
          break;
        case "U2":
          up_turn(false);
          up_turn(false);
          break;
        case "F2":
          front_turn(false);
          front_turn(false);
          break;
        case "L2":
          left_turn(false);
          left_turn(false);
          break;
        case "D2":
          down_turn(false);
          down_turn(false);
          break;
        case "B2":
          back_turn(false);
          back_turn(false);
          break;
      }
}

String convert_to_arduino_string(String[] unconverted_string) {
  String outstring = "<";
  for (int i = 0; i < unconverted_string.length; i++) {
    if (unconverted_string[i].contains("2")) {
      outstring += unconverted_string[i].replace('2',unconverted_string[i].charAt(0));
    }
    else if (unconverted_string[i].contains("'")) {
      String append = unconverted_string[i].substring(0, unconverted_string[i].length()-1 );
      outstring += append.toLowerCase(); 
    }
    else {
      outstring += unconverted_string[i];
    }
  }
  outstring += ">";
  return outstring;
}

void setupCoordinates() {  
  Fx[0] = 250; Fy[0] = 300; // cubie cornerstone
  
  buttonx[0] = 40; buttony[0] = 40; // pushbutton cornerstone
  buttonx[1] = 200; buttony[1] = buttony[0];
  buttonx[2] = 450; buttony[2] = buttony[0]; 
  
  buttonx[3] = 40;
  int scanbutton_y_val = 720; 
  buttony[3] = scanbutton_y_val; 
  for (int i = 3; i < 8; i++) {
    buttonx[i+1] = buttonx[i] + scan_button_size;
    buttony[i+1] = scanbutton_y_val; 
  }
  
  Fx[1] = Fx[0] + cubie_size + gap_size;
  Fy[1] = Fy[0];
  Fx[2] = Fx[1] + cubie_size + gap_size;
  Fy[2] = Fy[0];
  Fx[3] = Fx[0];
  Fy[3] = Fy[0] + cubie_size + gap_size;
  Fx[4] = Fx[1];
  Fy[4] = Fy[3];
  Fx[5] = Fx[2];
  Fy[5] = Fy[3];
  Fx[6] = Fx[0];
  Fy[6] = Fy[3] + cubie_size + gap_size;
  Fx[7] = Fx[1];
  Fy[7] = Fy[6];
  Fx[8] = Fx[2];
  Fy[8] = Fy[6];
  
  Rx[0] = Fx[2] + cubie_size + gap_size; 
  Ry[0] = Fy[0];
  
  Rx[1] = Rx[0] + cubie_size + gap_size;
  Ry[1] = Ry[0];
  Rx[2] = Rx[1] + cubie_size + gap_size;
  Ry[2] = Ry[0];
  Rx[3] = Rx[0];
  Ry[3] = Ry[0] + cubie_size + gap_size;
  Rx[4] = Rx[1];
  Ry[4] = Ry[3];
  Rx[5] = Rx[2];
  Ry[5] = Ry[3];
  Rx[6] = Rx[0];
  Ry[6] = Ry[3] + cubie_size + gap_size;
  Rx[7] = Rx[1];
  Ry[7] = Ry[6];
  Rx[8] = Rx[2];
  Ry[8] = Ry[6];
  
  Bx[0] = Rx[2] + cubie_size + gap_size; 
  By[0] = Fy[0];

  Bx[1] = Bx[0] + cubie_size + gap_size;
  By[1] = By[0];
  Bx[2] = Bx[1] + cubie_size + gap_size;
  By[2] = By[0];
  Bx[3] = Bx[0];
  By[3] = By[0] + cubie_size + gap_size;
  Bx[4] = Bx[1];
  By[4] = By[3];
  Bx[5] = Bx[2];
  By[5] = By[3];
  Bx[6] = Bx[0];
  By[6] = By[3] + cubie_size + gap_size;
  Bx[7] = Bx[1];
  By[7] = By[6];
  Bx[8] = Bx[2];
  By[8] = By[6];
  
  Lx[0] = Fx[0] - cubie_size*3 - gap_size*3; 
  Ly[0] = Fy[0];
  
  Lx[1] = Lx[0] + cubie_size + gap_size;
  Ly[1] = Ly[0];
  Lx[2] = Lx[1] + cubie_size + gap_size;
  Ly[2] = Ly[0];
  Lx[3] = Lx[0];
  Ly[3] = Ly[0] + cubie_size + gap_size;
  Lx[4] = Lx[1];
  Ly[4] = Ly[3];
  Lx[5] = Lx[2];
  Ly[5] = Ly[3];
  Lx[6] = Lx[0];
  Ly[6] = Ly[3] + cubie_size + gap_size;
  Lx[7] = Lx[1];
  Ly[7] = Ly[6];
  Lx[8] = Lx[2];
  Ly[8] = Ly[6];
  
  Ux[0] = Fx[0];
  Uy[0] = Uy[0] = Fy[0] - cubie_size*3 - gap_size*3; 

  Ux[1] = Ux[0] + cubie_size + gap_size;
  Uy[1] = Uy[0];
  Ux[2] = Ux[1] + cubie_size + gap_size;
  Uy[2] = Uy[0];
  Ux[3] = Ux[0];
  Uy[3] = Uy[0] + cubie_size + gap_size;
  Ux[4] = Ux[1];
  Uy[4] = Uy[3];
  Ux[5] = Ux[2];
  Uy[5] = Uy[3];
  Ux[6] = Ux[0];
  Uy[6] = Uy[3] + cubie_size + gap_size;
  Ux[7] = Ux[1];
  Uy[7] = Uy[6];
  Ux[8] = Ux[2];
  Uy[8] = Uy[6];
  
  Dx[0] = Fx[0];
  Dy[0] = Fy[6] + cubie_size + gap_size; 
  
  Dx[1] = Dx[0] + cubie_size + gap_size;
  Dy[1] = Dy[0];
  Dx[2] = Dx[1] + cubie_size + gap_size;
  Dy[2] = Dy[0];
  Dx[3] = Dx[0];
  Dy[3] = Dy[0] + cubie_size + gap_size;
  Dx[4] = Dx[1];
  Dy[4] = Dy[3];
  Dx[5] = Dx[2];
  Dy[5] = Dy[3];
  Dx[6] = Dx[0];
  Dy[6] = Dy[3] + cubie_size + gap_size;
  Dx[7] = Dx[1];
  Dy[7] = Dy[6];
  Dx[8] = Dx[2];
  Dy[8] = Dy[6];  
}

void drawStartCube() { // draws cube from previously defined coordinates
  color_black(); 
  rect(Fx[0] - gap_size, Fy[0] - gap_size, cubie_size*9 + gap_size*10, cubie_size*3 + gap_size*4); 
  rect(Fx[0] - cubie_size*3 - gap_size * 4, Fy[0] - gap_size, cubie_size*3 + gap_size*4, cubie_size*3 + gap_size*4);
  rect(Fx[0] - gap_size, Fy[0] - cubie_size*3 - gap_size*4, cubie_size*3 + gap_size*4, cubie_size*3 + gap_size*4);  
  rect(Fx[0] - gap_size, Fy[0] + cubie_size*3 + gap_size*2, cubie_size*3 + gap_size*4, cubie_size*3 + gap_size*4);
  
  for (int i = 0; i < 9; i++) { 
    color_green(); 
    square(Fx[i], Fy[i], cubie_size);     
  }
  for (int i = 0; i < 9; i++) { 
    color_red();
    square(Rx[i], Ry[i], cubie_size);     
  }
  for (int i = 0; i < 9; i++) {
    color_blue();
    square(Bx[i], By[i], cubie_size);     
  }
  for (int i = 0; i < 9; i++) { 
    color_orange();
    square(Lx[i], Ly[i], cubie_size);     
  }
  for (int i = 0; i < 9; i++) { 
    color_white();
    square(Ux[i], Uy[i], cubie_size);     
  }
  for (int i = 0; i < 9; i++) { 
    color_yellow();
    square(Dx[i], Dy[i], cubie_size);     
  }  
}

void drawButtons() { // draws buttons from previously defined coordinates
  fill(0,0,0);
  textSize(50);
  textAlign(LEFT, TOP);
  text("SOLVE", buttonx[0], buttony[0]);
  text("SCRAMBLE", buttonx[1], buttony[1]);
  text("TOGGLE", buttonx[2], buttony[2]);
  
  textSize(20); 
  textAlign(CENTER, CENTER); 
  for (int i = 3; i < 9; i++) {
    switch_color(scan_button_char_list[i-3]);
    square(buttonx[i], buttony[i], scan_button_size);
    color_black(); 
    text(scan_button_char_list[i-3], buttonx[i]+0.5*scan_button_size, buttony[i]+0.5*scan_button_size);
  }
}


void parseCubestate() { // redraws cube based on current cube state char arrays
  for (int i = 0; i < 9; i++) {
    if (up[i] == 'U') {
        color_white();
        square(Ux[i], Uy[i], cubie_size);
    }
    else if (up[i] == 'R') {
        color_red();
        square(Ux[i], Uy[i], cubie_size);
    }
    else if (up[i] == 'F') {
        color_green();
        square(Ux[i], Uy[i], cubie_size);
    }
    else if (up[i] == 'D') {
        color_yellow();
        square(Ux[i], Uy[i], cubie_size);
    }
    else if (up[i] == 'L') {
        color_orange();
        square(Ux[i], Uy[i], cubie_size);
    }
    else if (up[i] == 'B') {
        color_blue();
        square(Ux[i], Uy[i], cubie_size);
    }
  }
  for (int i = 0; i < 9; i++) {
    if (right[i] == 'U') {
        color_white();
        square(Rx[i], Ry[i], cubie_size);
    }
    else if (right[i] == 'R') {
        color_red();
        square(Rx[i], Ry[i], cubie_size);
    }
    else if (right[i] == 'F') {
        color_green();
        square(Rx[i], Ry[i], cubie_size);
    }
    else if (right[i] == 'D') {
        color_yellow();
        square(Rx[i], Ry[i], cubie_size);
    }
    else if (right[i] == 'L') {
        color_orange();
        square(Rx[i], Ry[i], cubie_size);
    }
    else if (right[i] == 'B') {
        color_blue();
        square(Rx[i], Ry[i], cubie_size);
    }
  }
  for (int i = 0; i < 9; i++) {
    if (front[i] == 'U') {
        color_white();
        square(Fx[i], Fy[i], cubie_size);
    }
    else if (front[i] == 'R') {
        color_red();
        square(Fx[i], Fy[i], cubie_size);
    }
    else if (front[i] == 'F') {
        color_green();
        square(Fx[i], Fy[i], cubie_size);
    }
    else if (front[i] == 'D') {
        color_yellow();
        square(Fx[i], Fy[i], cubie_size);
    }
    else if (front[i] == 'L') {
        color_orange();
        square(Fx[i], Fy[i], cubie_size);
    }
    else if (front[i] == 'B') {
        color_blue();
        square(Fx[i], Fy[i], cubie_size);
    }
  }
  for (int i = 0; i < 9; i++) {
    if (down[i] == 'U') {
        color_white();
        square(Dx[i], Dy[i], cubie_size);
    }
    else if (down[i] == 'R') {
        color_red();
        square(Dx[i], Dy[i], cubie_size);
    }
    else if (down[i] == 'F') {
        color_green();
        square(Dx[i], Dy[i], cubie_size);
    }
    else if (down[i] == 'D') {
        color_yellow();
        square(Dx[i], Dy[i], cubie_size);
    }
    else if (down[i] == 'L') {
        color_orange();
        square(Dx[i], Dy[i], cubie_size);
    }
    else if (down[i] == 'B') {
        color_blue();
        square(Dx[i], Dy[i], cubie_size);
    }
  }
  for (int i = 0; i < 9; i++) {
    if (left[i] == 'U') {
        color_white();
        square(Lx[i], Ly[i], cubie_size);
    }
    else if (left[i] == 'R') {
        color_red();
        square(Lx[i], Ly[i], cubie_size);
    }
    else if (left[i] == 'F') {
        color_green();
        square(Lx[i], Ly[i], cubie_size);
    }
    else if (left[i] == 'D') {
        color_yellow();
        square(Lx[i], Ly[i], cubie_size);
    }
    else if (left[i] == 'L') {
        color_orange();
        square(Lx[i], Ly[i], cubie_size);
    }
    else if (left[i] == 'B') {
        color_blue();
        square(Lx[i], Ly[i], cubie_size);
    }
  }
  for (int i = 0; i < 9; i++) {
    if (back[i] == 'U') {
        color_white();
        square(Bx[i], By[i], cubie_size);
    }
    else if (back[i] == 'R') {
        color_red();
        square(Bx[i], By[i], cubie_size);
    }
    else if (back[i] == 'F') {
        color_green();
        square(Bx[i], By[i], cubie_size);
    }
    else if (back[i] == 'D') {
        color_yellow();
        square(Bx[i], By[i], cubie_size);
    }
    else if (back[i] == 'L') {
        color_orange();
        square(Bx[i], By[i], cubie_size);
    }
    else if (back[i] == 'B') {
        color_blue();
        square(Bx[i], By[i], cubie_size);
    }
  }
}

void shiftRight(char arr[], int n, int L) { // generic array shifting function, used in "turning" the sides of the cube
    char[] temp = new char[L];
    for (int i = 0; i < L; i++) {
        temp[(i+n)%L] = arr[i];
    }
    for (int i = 0; i < L; i++) {
        arr[i] = temp[i];
    }
}
void shiftLeft(char arr[], int n, int L) { // same as shift right, but to the left
    char[] temp = new char[L];  
    for (int i = 0; i < L; i++) {
        temp[(i-n+L)%L] = arr[i];
    }
    for (int i = 0; i < L; i++) {
        arr[i] = temp[i];
    }
}

void rotateSide(char arr[], boolean prime) { // rotates an array representation of a cube face
  char[] side = new char[9];  
  if (prime == false) { // CW
    for (int i = 0; i < 9; i++) {
      side[i] = arr[i];
    }
    arr[0] = side[6]; arr[1] = side[3]; arr[2] = side[0];  arr[5] = side[1]; arr[8] = side[2]; arr[7] = side[5]; arr[6] = side[8]; arr[3] = side[7]; 
    
  }
  if (prime == true) { // CCW
    for (int i = 0; i < 9; i++) {
      side[i] = arr[i]; 
    }
    arr[6] = side[0]; arr[3] = side[1]; arr[0] = side[2]; arr[1] = side[5]; arr[2] = side[8]; arr[5] = side[7]; arr[8] = side[6]; arr[7] = side[3]; 
  }    
}

/////////// turns ////////////////////////
// these are all very similar, they each perform their respective turn on the cube state arrays, prime = false for CW, prime = true for CCW

void right_turn(boolean prime) { 
  char[] slice = new char[12]; 
  
  slice[0] = front[8]; slice[1] = front[5]; slice[2] = front[2]; slice[3] = up[8]; slice[4] = up[5]; slice[5] = up[2]; slice[6] = back[0]; slice[7] = back[3]; slice[8] = back[6]; slice[9] = down[8]; slice[10] = down[5]; slice[11] = down[2];
  
  if (prime == false) { // normal R turn    
    shiftRight(slice, 3, 12);
    rotateSide(right, false); 
  }
  else if (prime == true) { // Rprime turn
    shiftLeft(slice, 3, 12); 
    rotateSide(right, true); 
  }
  
  front[8] = slice[0]; front[5] = slice[1]; front[2] = slice[2]; up[8] = slice[3]; up[5] = slice[4]; up[2] = slice[5]; back[0] = slice[6]; back[3] = slice[7]; back[6] = slice[8]; down[8] = slice[9]; down[5] = slice[10]; down[2] = slice[11];

}

void up_turn(boolean prime) {
  char[] slice = new char[12]; 
  
  for(int i = 0; i < 3; i++) {
    slice[i] = front[i];
  }
  for(int i = 3; i < 6; i++) {
    slice[i] = right[i%3];
  }
  for(int i = 6; i < 9; i++) {
    slice[i] = back[i%3];
  }
  for(int i = 9; i < 12; i++) {
    slice[i] = left[i%3];
  }
  
  if (prime == false) { // normal U turn
    shiftLeft(slice, 3, 12);
    rotateSide(up, false); 
  }
  else if (prime == true) {
    shiftRight(slice, 3, 12);
    rotateSide(up, true); 
  }
  
  for(int i = 0; i < 3; i++) {
    front[i] = slice[i];
  }
  for(int i = 3; i < 6; i++) {
    right[i%3] = slice[i];
  }
  for(int i = 6; i < 9; i++) {
    back[i%3] = slice[i];
  }
  for(int i = 9; i < 12; i++) {
    left[i%3] = slice[i];
  }  
}

void front_turn(boolean prime) {
  
  char[] slice = new char[12]; 
  
  slice[0] = up[6]; slice[1] = up[7]; slice[2] = up[8]; slice[3] = right[0]; slice[4] = right[3]; slice[5] = right[6]; slice[6] = down[2]; slice[7] = down[1]; slice[8] = down[0]; slice[9] = left[8]; slice[10] = left[5]; slice[11] = left[2]; 
  
  if (prime == false) { // normal R turn    
    shiftRight(slice, 3, 12);
    rotateSide(front, false); 
  }
  else if (prime == true) { // Rprime turn
    shiftLeft(slice, 3, 12); 
    rotateSide(front, true); 
  }
  
  up[6] = slice[0]; up[7] = slice[1]; up[8] = slice[2]; right[0] = slice[3]; right[3] = slice[4]; right[6] = slice[5]; down[2] = slice[6]; down[1] = slice[7]; down[0] = slice[8]; left[8] = slice[9]; left[5] = slice[10]; left[2] = slice[11]; 
  
}

void left_turn(boolean prime) {
  
  char[] twelve = new char[12];
  
  twelve[0]=front[6]; twelve[1]=front[3]; twelve[2]=front[0]; twelve[3]=up[6]; twelve[4]=up[3]; twelve[5]=up[0]; twelve[6]=back[2]; twelve[7]=back[5]; twelve[8]=back[8]; twelve[9]=down[6]; twelve[10]=down[3]; twelve[11]=down[0]; 
  
  if (prime == false) { // normal L turn    
    shiftLeft(twelve, 3, 12);
    rotateSide(left, false); 
  }
  else if (prime == true) { // Lprime turn
    shiftRight(twelve, 3, 12); 
    rotateSide(left, true); 
  }
  
  front[6]=twelve[0]; front[3]=twelve[1]; front[0]=twelve[2]; up[6]=twelve[3]; up[3]=twelve[4]; up[0]=twelve[5]; back[2]=twelve[6]; back[5]=twelve[7]; back[8]=twelve[8]; down[6]=twelve[9]; down[3]=twelve[10]; down[0]=twelve[11];
}

void down_turn(boolean prime) {
  
  char[] twelve = new char[12]; 
  
  twelve[0]=front[6]; twelve[1]=front[7]; twelve[2]=front[8]; twelve[3]=right[6]; twelve[4]=right[7]; twelve[5]=right[8]; twelve[6]=back[6]; twelve[7]=back[7]; twelve[8]=back[8]; twelve[9]=left[6]; twelve[10]=left[7]; twelve[11]=left[8];
  if (prime == false) { // normal D turn    
    shiftRight(twelve, 3, 12);
    rotateSide(down, false); 
  }
  else if (prime == true) { // Dprime turn
    shiftLeft(twelve, 3, 12); 
    rotateSide(down, true); 
  }  
  front[6]=twelve[0]; front[7]=twelve[1]; front[8]=twelve[2]; right[6]=twelve[3]; right[7]=twelve[4]; right[8]=twelve[5]; back[6]=twelve[6]; back[7]=twelve[7]; back[8]=twelve[8]; left[6]=twelve[9]; left[7]=twelve[10]; left[8]=twelve[11]; 
  
}

void back_turn(boolean prime) {
  
  char[] twelve = new char[12]; 
  twelve[0]=up[0]; twelve[1]=up[1]; twelve[2]=up[2]; twelve[3]=right[2]; twelve[4]=right[5]; twelve[5]=right[8]; twelve[6]=down[8]; twelve[7]=down[7]; twelve[8]=down[6]; twelve[9]=left[6]; twelve[10]=left[3]; twelve[11]=left[0];
    if (prime == false) { // normal B turn    
      shiftLeft(twelve, 3, 12);
      rotateSide(back, false); 
    }
    else if (prime == true) { // Bprime turn
      shiftRight(twelve, 3, 12); 
      rotateSide(back, true); 
    }  
    up[0]=twelve[0]; up[1]=twelve[1]; up[2]=twelve[2]; right[2]=twelve[3]; right[5]=twelve[4]; right[8]=twelve[5]; down[8]=twelve[6]; down[7]=twelve[7]; down[6]=twelve[8]; left[6]=twelve[9]; left[3]=twelve[10]; left[0]=twelve[11];
}

// generic color presets so I didn't get confused and set stuff to the wrong color 
void color_white() { fill(255,255,255); }
void color_green() { fill(0,255,0); }
void color_red() { fill(255,0,0); }
void color_yellow() { fill(255,255,0); }
void color_orange() { fill(255,165,0); }
void color_blue() { fill(0,255,255); }
void color_black() { fill(0,0,0); }

void switch_color(String color_in) {
  switch (color_in) {
      case "U":
        color_white(); 
        break;
      case "F":
        color_green();
        break; 
      case "R":
        color_red();
        break; 
      case "L":
        color_orange(); 
        break;
      case "D":
        color_yellow();
        break;
      case "B":
        color_blue();
        break;
    }
}

void setCubestate(String cubestate) { // takes in a string format cubestate and separates it into the 6 different char arrays 
  char[] temp_array = new char[63];
  temp_array = cubestate.toCharArray();   
  for (int i = 0; i < 9; i++) {
    up[i] = temp_array[i]; 
  }
  for (int i = 9; i < 18; i++) {
   right[i%9] = temp_array[i]; 
  }
  for (int i = 18; i < 27; i++) {
    front[i%9] = temp_array[i]; 
  }
  for (int i = 27; i < 36; i++) {
    down[i%9] = temp_array[i]; 
  }
  for (int i = 36; i < 45; i++) {
    left[i%9] = temp_array[i]; 
  }
  for (int i = 45; i < 54; i++) {
    back[i%9] = temp_array[i]; 
  }  
}

////////////////////////// GUI INTERACTIONS //////////////////////////////////////////////////

void mousePressed() { // change global flags depending on coordinates of mouse when clicked
  println("Mouse coordinates: " + mouseX + ", " + mouseY);
  if (mouseX > 40 && mouseX < 180 && mouseY > 50 && mouseY < 94) { // clicked on solve button
    solveflag_pressed = true;
    request_solve_flag = true; 
  }
  else if (mouseX > 200 && mouseX < 430 && mouseY > 50 && mouseY < 94) {
    scrambleflag_pressed = true;
    request_scramble_flag = true; 
  }
  else if (mouseX > 450 && mouseX < 620 && mouseY > 50 && mouseY < 94) {
    request_disable = true; 
    disableflag_pressed = true;
  }
  else if (mouseX > buttonx[3] && mouseX < buttonx[3] + scan_button_size && mouseY > buttony[3] && mouseY < buttony[3] + scan_button_size) {
    button1_flag_pressed = true;
    request_scan_flag = true; 
    side_indicator = 0;  
  }
  else if (mouseX > buttonx[4] && mouseX < buttonx[4] + scan_button_size && mouseY > buttony[4] && mouseY < buttony[4] + scan_button_size) {
    button2_flag_pressed = true;
    request_scan_flag = true; 
    side_indicator = 2; 
  }
  else if (mouseX > buttonx[5] && mouseX < buttonx[5] + scan_button_size && mouseY > buttony[5] && mouseY < buttony[5] + scan_button_size) {
    button3_flag_pressed = true; 
    request_scan_flag = true; 
    side_indicator = 1; 
  }
  else if (mouseX > buttonx[6] && mouseX < buttonx[6] + scan_button_size && mouseY > buttony[6] && mouseY < buttony[6] + scan_button_size) {
    button4_flag_pressed = true;
    request_scan_flag = true; 
    side_indicator = 4; 
  }
  else if (mouseX > buttonx[7] && mouseX < buttonx[7] + scan_button_size && mouseY > buttony[7] && mouseY < buttony[7] + scan_button_size) {
    button5_flag_pressed = true;
    request_scan_flag = true; 
    side_indicator = 3; 
  }
  else if (mouseX > buttonx[8] && mouseX < buttonx[8] + scan_button_size && mouseY > buttony[8] && mouseY < buttony[8] + scan_button_size) {
    button6_flag_pressed = true;
    request_scan_flag = true; 
    side_indicator = 5;
  }
}

void mouseReleased() { // reset all flags to false (nothing being pressed)
  solveflag_pressed = false; 
  scrambleflag_pressed = false; 
  disableflag_pressed = false; 
  button1_flag_pressed = false; 
  button2_flag_pressed = false; 
  button3_flag_pressed = false; 
  button4_flag_pressed = false; 
  button5_flag_pressed = false; 
  button6_flag_pressed = false; 
}

////////////////////////// FLAG CHECKING //////////////////////

void checkButtonFlags() { // checks global button flags
  stroke(255);
  fill(255,255,255); 
  rect(0,0,800,100);
  stroke(0); 
  
  for (int i = 3; i < 9; i++) {
    switch_color(scan_button_char_list[i-3]);
    square(buttonx[i], buttony[i], scan_button_size);
  }
  
  if (solveflag_pressed) {
    textSize(50);
    textAlign(LEFT, TOP);
    fill(0,255,0);
    text("SOLVE", buttonx[0], buttony[0]);
  }
  else {
    textSize(50);
    textAlign(LEFT, TOP);
    fill(0,0,0);
    text("SOLVE", buttonx[0], buttony[0]);
  }
  
  if (scrambleflag_pressed) {
    textSize(50);
    textAlign(LEFT, TOP);
    fill(0,255,0);
    text("SCRAMBLE", buttonx[1], buttony[1]);
  }
  else {
    textSize(50);
    textAlign(LEFT, TOP);
    fill(0,0,0);
    text("SCRAMBLE", buttonx[1], buttony[1]);
  }
  
  if (disableflag_pressed) {
    textSize(50);
    textAlign(LEFT, TOP);
    fill(0,255,0);
    text("TOGGLE", buttonx[2], buttony[2]); 
  }
  else {
    textSize(50);
    textAlign(LEFT, TOP);
    fill(0,0,0);
    text("TOGGLE", buttonx[2], buttony[2]); 
  }
  
  if (button1_flag_pressed) {
    fill(255,255,255);  
  }
  else {
    fill(0,0,0);
  }
  textSize(20);
  textAlign(CENTER, CENTER); 
  text(scan_button_char_list[0], buttonx[3] + 0.5*scan_button_size, buttony[3] + 0.5*scan_button_size);
  
  if (button2_flag_pressed) {
    fill(255,255,255);  
  }
  else {
    fill(0,0,0);
  }
  textSize(20);
  textAlign(CENTER, CENTER); 
  text(scan_button_char_list[1], buttonx[4] + 0.5*scan_button_size, buttony[4] + 0.5*scan_button_size);
  
  if (button3_flag_pressed) {
    fill(255,255,255);  
  }
  else {
    fill(0,0,0);
  }
  textSize(20);
  textAlign(CENTER, CENTER); 
  text(scan_button_char_list[2], buttonx[5] + 0.5*scan_button_size, buttony[5] + 0.5*scan_button_size);
  
  if (button4_flag_pressed) {
    fill(255,255,255);  
  }
  else {
    fill(0,0,0);
  }
  textSize(20);
  textAlign(CENTER, CENTER); 
  text(scan_button_char_list[3], buttonx[6] + 0.5*scan_button_size, buttony[6] + 0.5*scan_button_size);
  
  if (button5_flag_pressed) {
    fill(255,255,255);  
  }
  else {
    fill(0,0,0);
  }
  textSize(20);
  textAlign(CENTER, CENTER); 
  text(scan_button_char_list[4], buttonx[7] + 0.5*scan_button_size, buttony[7] + 0.5*scan_button_size);
  
  if (button6_flag_pressed) {
    fill(255,255,255);  
  }
  else {
    fill(0,0,0);
  }
  textSize(20);
  textAlign(CENTER, CENTER); 
  text(scan_button_char_list[5], buttonx[8] + 0.5*scan_button_size, buttony[8] + 0.5*scan_button_size);
    
}

void checkRequestFlags() {
  if (request_solve_flag) {
    thread("request_solve"); // runs the actual request in a different thread, so the GUI can update frames
    request_solve_flag = false; 
  }
  
  if (request_scramble_flag) {
    scramble();
    request_scramble_flag = false; 
  }
  
  if (request_disable) {
    disabled = !disabled; 
    if (disabled) {
      arduino_port.write("W"); 
    }
    else {
      arduino_port.write("E");
    }
    request_disable = false; 
  }
  
  if (request_scan_flag) {
    thread("request_scan");
    request_scan_flag = false; 
  }
}

void checkMoveFlags() { // delay version
  int delay = 100;
  if (new_millis) {
    last_millis = millis();
    new_millis = false; 
  }
  if (millis() - last_millis > delay) {
    if (unsolved_solvestring) {
      checkMove(split_solvestring[solvestring_iterator]);
      // serial print move to arduino
      // wait for feedback in separate thread
      if (solvestring_iterator < split_solvestring.length - 1) {
        solvestring_iterator++; 
      }
      else {
        unsolved_solvestring = false; // has iterated all the way through split_solvestring -- reset bool
      }
    }
    else if (unsolved_scramble) {
      checkMove(scramblestring[scramblestring_iterator]);
      if (scramblestring_iterator < scramblestring.length - 1) {
        scramblestring_iterator++; 
      }
      else {
        unsolved_scramble = false; 
      }
    }
    new_millis = true; 
  }
}

void checkMoveFlagsArduino() {
  if (unsolved_solvestring && new_move_request) {
    checkMove(split_solvestring[solvestring_iterator]);
      if (solvestring_iterator < split_solvestring.length - 1) {
        solvestring_iterator++; 
      }
      else {
        unsolved_solvestring = false; // has iterated all the way through split_solvestring -- reset bool
      }
     new_move_request = false; 
  }
  else if (unsolved_scramble && new_move_request) {
    checkMove(scramblestring[scramblestring_iterator]);
    if (scramblestring_iterator < scramblestring.length - 1) {
      scramblestring_iterator++; 
    }
    else {
      unsolved_scramble = false; 
    }
    new_move_request = false;
  }
}

void scramble() {  
  unsolved_scramble = true; 
  scramblestring_iterator = 0; 
  String[] list = {"R", "U", "F", "L", "D", "B", "R'", "U'", "F'", "L'", "D'", "B'"}; // list of possible moves (12)
  //String[] list2 = {"R", "R'", "F", "F'", "D", "D'", "L", "L'", "U", "U'", "B", "B'"}; // list of possible moves (12)
  int move_type = 69;
  for (int i = 0; i < scramblestring.length; i++) {
    int last_move_type = move_type; 
    move_type = int(random(0,5)); 
    while (last_move_type == move_type) {
      move_type = int(random(0,5)); 
    }
    int rand_multiplier = int(random(1,3)); 
    scramblestring[i] = list[move_type * rand_multiplier]; 
  }
  String converted_scramblestring = convert_to_arduino_string(scramblestring); 
  arduino_port.write(converted_scramblestring); 
}

  
