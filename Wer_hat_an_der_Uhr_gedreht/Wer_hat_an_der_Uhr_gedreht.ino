#include <Stepper.h> // Hinzufügen der Programmbibliothek.
#include <JQ6500_Serial.h>
int SPU = 2048; // Schritte pro Umdrehung.
int STEP_FUER_STD = 173; // Schritte um einse Stunde auf Ziffernbaltt abzufahren
Stepper Motor(SPU, 3,5,4,6); // Der Schrittmotor erhält die Bezeichnung "Motor" und es wird angegeben an welchen Pins der Motor angeschlossen ist.
JQ6500_Serial mp3(8,9);

const int button_start_Pin = 2;     // Startet den Stepper (gelber Draht)
const int button_set_Pin = 10;     // Stellt die Uhr (grauer Draht)
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status



void uhr_drehen () {
   // 2048 Schritte entspricht einer Umdrehung.
   Motor.step(1*STEP_FUER_STD); //um eine Stunde vor Start bei 2 Uhr
   delay(1000); // 3 Uhr
   Motor.step(2*STEP_FUER_STD); 
   delay(1000); // 5 Uhr
   Motor.step(2*STEP_FUER_STD); 
   delay(1000); // 7 Uhr
   Motor.step(-3*STEP_FUER_STD);
   Motor.step (-12); // 4 Uhr
    delay(1000);
   Motor.step(1*STEP_FUER_STD);
   delay(1000); //5 Uhr
   Motor.step(3*STEP_FUER_STD);
   Motor.step (12); 
   delay(1000);  // 8 Uhr
   Motor.step(-6*STEP_FUER_STD); 
   delay(1000); // 2 UHR
   // A=3 B=5 C=7 D=4 E=5 F=8   
   }

void uhr_stellen () {
   Motor.step(10); 
   delay(100); 
}



void setup() //Hier beginnt das Setup.
{
    Motor.setSpeed(4); // Angabe der Geschwindigkeit in Umdrehungen pro Minute.
    // initialize the pushbutton pin as an input:
    pinMode(button_start_Pin, INPUT_PULLUP);
    pinMode(button_set_Pin, INPUT_PULLUP);
    mp3.begin(9600);
    mp3.reset();
    mp3.setVolume(20);
    mp3.setLoopMode(MP3_LOOP_NONE);
    mp3.setEqualizer(MP3_EQ_BASS);
};

void loop() {
  // check if the start-button is pressed. If it is, the buttonState is LOW:
  buttonState = digitalRead(button_start_Pin);
  if (buttonState == LOW) { 
    mp3.playFileByIndexNumber(1);
    uhr_drehen();
  };

  buttonState = digitalRead(button_set_Pin);
  if (buttonState == LOW) { 
     uhr_stellen();
  };
} // end loop
