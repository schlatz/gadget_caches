#include <JC_Button.h>          // https://github.com/JChristensen/JC_Button
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>
// Create the mp3 module object, 
//   Arduino Pin 8 is connected to TX of the JQ6500
//   Arduino Pin 9 is connected to one end of a  1k resistor, 
//     the other end of the 1k resistor is connected to RX of the JQ6500
//   If your Arduino is 3v3 powered, you can omit the 1k series resistor
JQ6500_Serial mp3(8,9);
TM1637Display display(11, 12); // CLK, DIO

enum SOUNDS {
    ALLE=1,
    VOGEL1,
    VOGEL2,
    VOGEL3,
    VOGEL4,
    
    FINAL, 
    FEHLER
};

long soll=4123;
long ist=0;
// pin assignments
const byte
    LED_PIN(13);                // the standard Arduino "pin 13" LED

Button btn_A(2);       // Alle Voegel
Button btn_1(3);
Button btn_2(4);
Button btn_3(5);
Button btn_4(6);
uint8_t con[] =  { 0b00111001, 0b01011100,0b01010100 , 0x00 };
uint8_t grat[] = { 0b01111101, 0b01010000, 0b01110111, 0x70 };
uint8_t ulat[] = { 0b00011100, 0b00111000, 0b01110111, 0x70 };
uint8_t ions[] = { 0x06, 0b01011100, 0b01010100, 0b01101101 };

void setup()
{
    btn_A.begin();              // initialize the button object
    btn_1.begin();
    btn_2.begin();
    btn_3.begin();
    btn_4.begin();
    
    pinMode(LED_PIN, OUTPUT);   // set the LED pin as an output

    mp3.begin(9600);
    mp3.reset();
    mp3.setVolume(20);
    mp3.setLoopMode(MP3_LOOP_NONE);
  
    Serial.begin(115200);

    display.setBrightness(6);
    
    display.showNumberDec(readVcc(), true);  // Expect: 0000
    delay(2000);
    //display.showNumberDec(ist, true);
   
}

void loop()
{
    static bool ledState;       // a variable that keeps the current LED status

    btn_A.read();     
    if (btn_A.wasPressed()) 
    { Serial.println(F("Btn A pressed"));  
     
      display.showNumberDec(1, true);
      mp3.playFileByIndexNumber(ALLE);  
    };      

    
    btn_1.read();       
    if (btn_1.isPressed()) 
    { Serial.println(F("Btn 1 pressed"));  
       ist = 10*ist + 1;
       display.showNumberDec(ist, false);
       mp3.playFileByIndexNumber(VOGEL1);  
    };            


    btn_2.read();
    if (btn_2.isPressed()) 
    { Serial.println(F("Btn 2 pressed")); 
      ist = 10*ist + 2;
      display.showNumberDec(ist, false);
       mp3.playFileByIndexNumber(VOGEL2);  
    };               

    
    btn_3.read();    
    if (btn_3.wasPressed()) 
    { Serial.println(F("Btn 3 pressed"));  
      ist = 10*ist + 3;
      display.showNumberDec(ist, false);
       mp3.playFileByIndexNumber(VOGEL3);  
    };               
            
     
    btn_4.read();          
    if (btn_4.wasPressed()) 
    { Serial.println(F("Btn 4 pressed"));  
      ist = 10*ist + 4;
      display.showNumberDec(ist, false);
       mp3.playFileByIndexNumber(VOGEL4);  
    };               
      
    if ( soll == ist )
    { Serial.println(F("Kombi richtig"));  
      mp3.playFileByIndexNumber(FINAL); 
      display.setSegments(con);
      delay(2000);
      display.setSegments(grat);
      delay(2000);
      display.setSegments(ulat);
      delay(2000);
      display.setSegments(ions);
      delay(2000);
      while (1)
      { display.showNumberDec(49, false);
        delay(1000);
        display.showNumberDec(12, false);
        delay(1000);
        display.showNumberDec(123, false);
        delay(1000);
        display.showNumberDec(8, false);
        delay(1000);
        display.showNumberDec(0, true,2,2);
        delay(1000);
        display.showNumberDec(456, false);
        delay(1000);
      };
    };
    if (ist >999)
    { mp3.playFileByIndexNumber(FEHLER);
      Serial.println(F("Kombi falsch"));
      Serial.println(ist);
      ist = 0;
      for (int i=0; i<5; i++)
      {
         display.setBrightness(7, false);  
         display.showNumberDec(0, true);       
         delay(100);
         display.setBrightness(7, true);
         display.showNumberDec(0, true);
         delay (100);
      };
      
    }
    
}

long readVcc() { long result; // Read 1.1V reference against AVcc 
 ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
 delay(2); // Wait for Vref to settle 
 ADCSRA |= _BV(ADSC); // Convert 
 while (bit_is_set(ADCSRA,ADSC)); 
 result = ADCL; result |= ADCH<<8; 
 result = 1126400L / result; // Back-calculate AVcc in mV 
 return result; }

