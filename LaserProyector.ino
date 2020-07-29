/*
 * Author: Ezequiel Nieva Pérez
 * Title: Laser Proyector - Computing Systems Project (PoliMi)
 * Description:
 * This program consist on the control of two stepper motors 
 * in order to follow a path which is configured by some inputs generated
 * by push-buttons, then by powering a laser diode the path configured 
 * creates a pattern in a 9 points grid or uses preconfigured 
 * patterns already programed (letters). 
 */
#include<Stepper.h>

//Start up variables
int buttonval = 0;                  //analog input buttons
int laserpin = 12;                  //pin laser
int revstep = 15;                   //Range of steps per cycle (1.32°)
int stepcount = 0;                  //number of steps in diagonal function
int st = 1;                         //Min. step. (0.1°);
const int stepsRev =512;            //Max. number of steps per cycle
Stepper motorA(stepsRev, 6,7,8,9);  // Motor A setup
Stepper motorB(stepsRev, 2,4,3,5);  // Motor B setup

//UI
int startpin = 10;                  //pin of start button
int startbutton = 0;                //Read input from start pin
int state = 0;                      //state of the system on/off

//Mode selection
int selectval = 0;                  //value given due to the analog input for mode selection (up or down)
int insertMode = 0;                 //insert button to define the mode selected
int pushcountM = 0;                 //counter of the number of times the button up or down is selected
int selectcount = 0;                //counter for the number of times the insert button is pressed
int actmode = 0;                    //actual mode of the system
int modes [3] = {0,1,2};            //number of modes that the system have (0-automatic,1-user-defined path,2-pattern selection)

// Path Definition variables
int insertpin = 11;                 //pin definition of the insert button
int insert = 0;                     //value of pin insertpin
int numseq= 20;                     //max. number of points selection on a 9 point grid
int pushcount = 0;                  //counter for the number of times the pathselection buttons had been pressed
int seq [20];                       //array containing the sequence defined by the user
int countvert = 0;                  // counter for the vertical steps taken
int counthorz = 0;                  // counter for the horizontal steps taken

// Pattern selection variables
int numpat = 3;                     //number of patterns available
int insertpat = 0;                  //insert pattern variable
int patselectval = 0;               //pattern selection variable
int pushcountpat = 0;               //counter for how many times the pattern buttons are pressed
int pattern [3];                    //indexes for pattern selection
int letter = 1;                     //type of letter selected


void setup() {
  pinMode(laserpin,OUTPUT);         //Laser pin enable
  pinMode(startpin,INPUT_PULLUP);   //start Button pin enable
  pinMode(insertpin,INPUT_PULLUP);  //insert button pin enable
  motorA.setSpeed(45);              //vertical direction - speed (rpm)
  motorB.setSpeed(45);              //horizontal direction
  Serial.begin(9600);               
}
/* The main loop contains the start of the system and the mode selection, 
 *  for that different functions were used in order to mantain or select 
 *  a state. The system consist in 3 modes (automatic, user-defined,
 *  pattern selection).
 *  Functions:
 *  onoffButton()
 *  Mode()
 *  polimi()
 *  pathdef()
 *  Routine()
 *  patternselect()
 *  alphabet()
*/
void loop() {
  // Turn on reading
  startbutton = digitalRead(startpin); 
  onoffButton(); 
  delay(100);
  if (state == 1){
    // current state reading
    while (startbutton = digitalRead(startpin)==HIGH){
      digitalWrite(laserpin,HIGH);
      Mode(); // Mode selection
      if (modes[actmode] == 0){
        Serial.println("Mode 1");
        polimi();
        selectcount = 0;
      } else if (modes[actmode]==1){
        Serial.println("Mode 2");
        for (int j=0;j<numseq;j++){
          seq[j] = 0;
        }
        pathdef();
        for (int i=0;i<50;i++){
          Routine();
        }
        selectcount = 0;
      } else if (modes[actmode]==2){
        Serial.println("Mode 3");
        patternselect();
        for (int i=0;i<numpat;i++){
          alphabet(pattern[i]);
          delay(500);
        }
        selectcount = 0;
      }
      
    }
    delay(500);
    onoffButton();// Turn off reading
  } else{
    digitalWrite(laserpin,LOW); //Turn off laser
  }
}
/* This function consist on the mode selection of the system, 
 *  it is composed by 3 inputs (3 buttons), insert, next mode, 
 *  and past mode. The output will be the selection of a mode. 
*/
void Mode(){
    while(selectcount == 0){
      delay(3000);
      insertMode = digitalRead(insertpin);
      Serial.println(insertMode);
      if(insertMode == 1){
        pushcountM =0;
        while (pushcountM==0){
             selectval = analogRead(A0);
             Serial.println(selectval);
             if (selectval>=970 && selectval<=990){
                if (actmode != 0){
                  actmode = actmode - 1;
                }else {
                  actmode = actmode;
                }
                Serial.println("Past mode");
                pushcountM = pushcountM + 1;
             }else if (selectval>=950 && selectval<=965){
                if (actmode != 3){
                  actmode = actmode + 1;
                }else {
                  actmode = actmode;
                }
                Serial.println("Next mode");
                pushcountM = pushcountM + 1;
             }
          }
      } else {
          selectcount = selectcount + 1;
          pushcountM = 0;
      }
      
    }
}
/* This fuction consists on the path definition given by 
 *  a user (of size numseq). For that the function has two inputs one digital 
 *  and one analog (divided in 4 ranges in order to use 4 buttons), 
 *  the inputs are insert button (digital), left button (analog), 
 *  up button (analog), down button (analog), right button (analog). 
 *  Then the output is a sequence selected and inserted. 
*/
void pathdef(){
  for (int i=0;i<numseq;i++){
      delay(3000);
      insert = digitalRead(insertpin);
      if(insert == 1){
        while (pushcount==0){
             buttonval = analogRead(A0);
             Serial.println(buttonval);
             if (buttonval>=990 && buttonval<=1010){  
                seq [i] = 1;
                Serial.println(1);
                pushcount = pushcount + 1;
             }else if (buttonval>=965 && buttonval<=970){
                seq [i] = 2;
                Serial.println(2);
                pushcount = pushcount + 1;
             }else if (buttonval>=950 && buttonval<=960){
                seq [i] = 3;
                Serial.println(3);
                pushcount = pushcount + 1;
             }else if (buttonval>=910 && buttonval<=920){
                seq [i] = 4;
                Serial.println(4);
                pushcount = pushcount + 1;
             }
          }
      } else {
        i = numseq;    
      }
    pushcount = 0;
  }
  for(int i=0;i<numseq;i++){
    Serial.println(seq[i]);
  }
}

/* This function consist in following a routine defined 
 *  by a sequence of steps that the motors have to achieve,
 *  so its input its an array composed by the sequence of steps
 *  to follow and the output is the motors movement.
*/
void Routine(){
  for (int i = 0; i<=numseq-1;i++){
    if (seq[i]==1){
      motorB.step(revstep);
      counthorz = counthorz + revstep;
    }else if (seq[i]==4){
      motorB.step(-revstep);
      counthorz = counthorz - revstep;
    }else if (seq[i]==2){
      motorA.step(-revstep);
      countvert = countvert - revstep;
    }else if (seq[i]==3){
      motorA.step(revstep);
      countvert = countvert + revstep;
    } else {
      i = numseq;
    }
    //seq[i]=0;
    delay(5);
  }
}
/* This function consist on the pattern selection of 
 *  preconfigured paths for the motors, the inputs are given by 
 *  a digital input and an analog input (divided in 4 buttons, left,
 *  right, up, down). 
 *  The system draws the pattern authomatically depending on the 
 *  variable letter, then if the button up is pressed the pattern 
 *  is selected and save in the array pattern. If not pressed the 
 *  pattern showed can be changed by pressing the right or left button, 
 *  and for exit without selecting the down button can be pressed. 
*/
void patternselect(){
  for (int i=0;i<numpat;i++){
      delay(3000);
      insertpat = digitalRead(insertpin);
      if(insertpat == 1){
        while (pushcountpat==0){
          alphabet(letter);
          patselectval = analogRead(A0);
          Serial.println(patselectval);
          if (patselectval>=995 && patselectval<=1010){  
            if (letter != 1){
              letter = letter - 1;
            } else {
              letter = letter;
            }
            Serial.println("left");
          }else if (patselectval>=915 && patselectval<=925){
            if (letter != 3){
              letter = letter + 1;
            } else {
              letter = letter;
            }
            Serial.println("right");
          }else if (patselectval>=975 && patselectval<=990){
            pattern [i] = letter;
            Serial.println("selected");
            pushcountpat = pushcountpat + 1;
          }else if (patselectval>=955 && patselectval<=965){
            Serial.println("exit");
            pushcountpat = pushcountpat + 1;
            i = numpat;
          }
        }
      } else {
        i = numpat;    
      }
    pushcountpat = 0;
  }
}
/* This function is a collection of alphabet letters paths that 
 *  depend on the input variable letter to select the pattern. 
 */
void alphabet(int letter){
  switch (letter){
    case 1:
      for (int i=0;i<20;i++){
        letterA();
      }
      break;
    case 2:
      for (int i=0;i<20;i++){
        letterB();
      }
      break;
    case 3:
      for (int i=0;i<20;i++){
        letterC();
      }
      break;
    default:
      break;
  }
}

/* This function consist in the definition of the start button 
 *  state. 
*/
void onoffButton(){
  if (startbutton == HIGH && state == 0){
    //stay OFF 
    state = 1-startbutton;
  }else if (startbutton == HIGH && state ==1){
    //stay ON
    state = 1;
  }else if (startbutton == LOW && state == 0){
    //ON
    state = 1;
  }else{
    //OFF
    state = startbutton;
  }
}

/* The following functions are preconfigured sequences
 *  of paths that the motors can follow in order to draw 
 *  a pattern with the laser. 
 */
void diagonal(){
  stepcount = stepcount + 1;
  if (stepcount <=revstep){
    motorA.step(-st);
    motorB.step(-st);
  } else {
    stepcount = 0;
    st = -st;
  }
  
}

void polimi(){
  //letter P
  for (int i=0;i<30;i++){
    motorA.step(-20);
    motorB.step(-15);
    motorA.step(10);
    motorB.step(15);
    motorA.step(10);
  }
  delay(50);
  //letter O
  for (int j=0;j<30;j++){
    motorA.step(-20);
    motorB.step(-15);
    motorA.step(20);
    motorB.step(15);
  }
  delay(50);
  //letter L
  for (int i=0;i<30;i++){
    motorA.step(-18);
    delay(10);
    motorA.step(18);
    motorB.step(-15);
    motorB.step(15);
  }
  delay(50);
  for (int i=0;i<30;i++){
    motorB.step(-8);
    motorA.step(-20);
    motorB.step(-7);
    motorB.step(7);
    motorB.step(8);
    motorB.step(-8);
    motorA.step(20);
    motorB.step(-7);
    motorB.step(7);
    motorB.step(8);
  }
  delay(50);
  for (int i=0;i<30;i++){
    motorA.step(-20);
    motorB.step(-8);
    motorA.step(10);
    motorA.step(-10);
    motorB.step(-7);
    motorA.step(20);
    digitalWrite(laserpin,LOW);
    motorB.step(15);
    delay(10);
    digitalWrite(laserpin,HIGH);
  }
  delay(50);
    for (int i=0;i<30;i++){
    motorB.step(-8);
    motorA.step(-20);
    motorB.step(-7);
    motorB.step(7);
    motorB.step(8);
    motorB.step(-8);
    motorA.step(20);
    motorB.step(-7);
    motorB.step(7);
    motorB.step(8);
  }
  delay(50);
}

void letterA(){
  motorA.step(-20);
  motorB.step(-10);
  motorA.step(20);
  motorA.step(-10);
  motorB.step(10);
  motorA.step(10);
}

void letterB(){
  motorA.step(-20);
  motorB.step(-10);
  motorA.step(20);
  motorB.step(10);
  motorB.step(-10);
  motorA.step(20);
  motorB.step(10);
  motorA.step(-20);
}

void letterC(){
  motorA.step(-20);
  motorB.step(-10);
  motorB.step(10);
  motorA.step(20);
  motorA.step(20);
  motorB.step(-10);
  motorB.step(10);
  motorA.step(-20);
}
