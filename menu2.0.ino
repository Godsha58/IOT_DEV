int buttonSemaforo = 11;
int buttonProgram = 8;
int red = 6;
int red2 = 7;
int yellow = 4;
int yellow2 = 5;
int green = 2;
int green2 = 3;
int state = 0;
int lastState = 0;
int countintermitente = 0;
int menu_secuencia = 0;
int menu = 1;
int x = 0;
bool active = true;
long lastChange = 0;
const long duration = 1000;
const long intermitente = 2000;
const long yellowOn = 200;
enum StateTL { RED, YELLOW, GREEN };
StateTL stateTL = GREEN;
bool buttonstat = false;
bool buttonlast = false;

// JoyStick
int yPin = A0;
int xPin = A1;
int xPos, yPos = 0;

void setup() {
  Serial.begin(9600);

  pinMode(red, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(green2, OUTPUT);
  pinMode(buttonSemaforo, INPUT);
  pinMode(buttonProgram, INPUT);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  DDRD = B11111100;
}

void loop() {
  
  buttonstat = digitalRead(buttonProgram);

  if (buttonstat != buttonlast) {
    if (buttonstat == HIGH) {
      
      menu++;
      
      if (menu > 3) { 
        menu = 1;        
      }
      delay(50); 
    }
    buttonlast = buttonstat;
  }  
  
  xPos = analogRead(xPin);

  if (xPos < 100) {
    changeSequence();

    while (analogRead(xPin) < 100) {
      delay(10);
    }
  }

  switch (menu) {
    case 1:
      programaSemaforo();
      break;
    case 2:
      switch (menu_secuencia) {
        case 1:
          wave();
          break;
        case 2:
          xmas();
          break;
        case 3:
          binary();
          break;
      }
      break;
    case 3:
      registro();
      break;
  }
}

void programaSemaforo() {
  state = digitalRead(buttonSemaforo);

  if ((state == HIGH) && (lastState == LOW)) {
    active = !active;
    lastChange = millis();
    delay(20);
  }
  lastState = state;

  if (active) {
    semaforo();
  } else {
    pasopeatonal();
  }
}

void pasopeatonal() {
  Serial.println("Paso Peatonal");
  long goingTime = millis() - lastChange;
  if (goingTime < intermitente) {
    if ((goingTime / yellowOn) % 2 == 0) {
      digitalWrite(yellow, HIGH);
      digitalWrite(yellow2, HIGH);
      digitalWrite(red, LOW);
      digitalWrite(red2, LOW);
      digitalWrite(green, LOW);
      digitalWrite(green2, LOW);
    } else {
      digitalWrite(yellow, LOW);
      digitalWrite(yellow2, LOW);
      digitalWrite(red, HIGH);
      digitalWrite(red2, HIGH);
    }
  } else {
    digitalWrite(yellow, LOW);
    digitalWrite(yellow2, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(red2, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(green2, LOW);
  }
}

void semaforo() {
  long goingTime = millis() - lastChange;
  switch (stateTL) {
    case RED:
      digitalWrite(red, HIGH);
      digitalWrite(red2, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(yellow2, LOW);
      digitalWrite(green, LOW);
      digitalWrite(green2, LOW);
      if (goingTime >= duration) {
        stateTL = GREEN;
        lastChange = millis();
      }
      Serial.println("RED");
      break;
    case YELLOW:
      digitalWrite(red, LOW);
      digitalWrite(red2, LOW);
      digitalWrite(green, LOW);
      digitalWrite(green2, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(yellow2, HIGH);
      if (goingTime >= duration) {
        stateTL = RED;
        lastChange = millis();
        countintermitente = 0;
      }
      Serial.println("YELLOW");
      break;
    case GREEN:
      digitalWrite(red, LOW);
      digitalWrite(red2, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(yellow2, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(green2, HIGH);
      if (goingTime < intermitente) {
        digitalWrite(green, LOW);
        digitalWrite(green2, LOW);
        if ((goingTime / yellowOn) % 2 == 0) {
          digitalWrite(green, HIGH);
          digitalWrite(green2, HIGH);
        } else {
          digitalWrite(green, LOW);
          digitalWrite(green2, LOW);
        }
      } else {
        digitalWrite(green, LOW);
        digitalWrite(green2, LOW);
        countintermitente++;
        if (countintermitente >= 1) {
          stateTL = YELLOW;
          lastChange = millis();
        }
      }
      Serial.println("GREEN");
      break;
  }
}

void xmas() {
  Serial.println("ñavidah");
  digitalWrite(green, HIGH);
  digitalWrite(yellow, HIGH);
  digitalWrite(red, HIGH);
  digitalWrite(green2, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(red2, LOW);
  delay(200);
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green2, HIGH);
  digitalWrite(yellow2, HIGH);
  digitalWrite(red2, HIGH);
  delay(200);
}

void binary() {
  Serial.println("Binary");
    for (int num = 0; num < 256; num++) {
        xPos = analogRead(xPin);
        PORTD = num;
        delay(100);
        if (xPos < 342 || xPos > 682) {
            break;
        }
    }
}

void wave() {
  Serial.println("Wave");
  for (int num = 2; num <= 7; num++) {
    digitalWrite(num, HIGH);
    digitalWrite(num - 1, LOW);
    delay(75);
    digitalWrite(num, LOW);
  }
}

void changeSequence() {
  if (xPos < 342) { 
        menu_secuencia++;
        if (menu_secuencia > 3) {
            menu_secuencia = 1; 
        }
    } else if (xPos > 682) { 
        menu_secuencia--;
        if (menu_secuencia < 1) {
            menu_secuencia = 3; 
        }
    }
}

//no se que verga estoy haciendo


void cheat1(){
      Serial.println("tanque sovietico spawned");
      digitalWrite(red, LOW);
      digitalWrite(red2, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(yellow2, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(green2, HIGH);
}

void cheat2(){
      Serial.println("na na gente volando");
      digitalWrite(red, LOW);
      digitalWrite(red2, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(yellow2, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(green2, LOW);

}

void cheat3(){
      Serial.println("kamehameha");
      digitalWrite(red, HIGH);
      digitalWrite(red2, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(yellow2, LOW);
      digitalWrite(green, LOW);
      digitalWrite(green2, LOW);

}


  //direcciones
  //left = x < 500;
  //rigth = X > 700;
  //up= Y < 500;
  //down = Y > 700;
 
  //A = digitalRead(buttonSemaforo);


//oiuyt

 char actualcombo[5];
  int leds[6] = {2,3,4,5,6,7};

void resetValuesRegister(){
 x = 0;
  for(int i = 0; i < 5; i++) actualcombo[i] = ' ';
  for(int i = 0; i < 3; i++) pinMode(leds[i], LOW);
}

void detectjoystick(int axis, char cord, char cord2){
  int axisBtn = analogRead(axis);
  if(axisBtn >= 1000){
    if(x == 5) resetValuesRegister();
    actualcombo[x] = cord;
    x++;
    for(int i = 0; i < 5; i++) Serial.println(actualcombo[i]);
    for(int i = 0; i < 20; i++) delay(20);
  }
  
  if(axisBtn <= 300){
    if(x == 5) resetValuesRegister();
    actualcombo[x] = cord2;
    x++;
    for(int i = 0; i < 5; i++) Serial.println(actualcombo[i]);
    for(int i = 0; i < 20; i++){ 
      delay(20);
    }
   }
}

void registro(){
  Serial.println("REGISTRO");
  if(digitalRead(buttonSemaforo)){
    if(x == 5) resetValuesRegister();
    actualcombo[x] = 'A';
    x++;
    for(int i = 0; i < 5; i++) Serial.println(actualcombo[i]);
    for(int i = 0; i < 20; i++) {delay(10);}
  }

  detectjoystick(xPin, 'L', 'R');
  detectjoystick(yPin, 'U', 'D');

if(actualcombo[0] == 'U' && actualcombo[1] == 'U' && actualcombo[2] == 'D' && actualcombo[3] == 'D' && actualcombo[4] == 'L'){

   cheat1();

  }else if(actualcombo[0] == 'D' && actualcombo[1] == 'R' && actualcombo[2] == 'D' && actualcombo[3] == 'R' && actualcombo[4] == 'A'){

    cheat2();

  }else if(actualcombo[0] == 'L' && actualcombo[1] == 'D' && actualcombo[2] == 'R' && actualcombo[3] == 'L' && actualcombo[4] == 'A'){  

    cheat3();
  }
 }

