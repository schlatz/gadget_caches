//Schorleautomat
//2 * I2C Display 1602
//Poti an A0
//GND-Taster an D4

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>

JQ6500_Serial mp3(8,9);// Pin 9 Arduino auf RX JQ6500 mit 1000 Ohm Widerstand !!!


// Umlaute auf LCD ü=\365 ä=\341 ö=\357 ß=\342
//
//
enum SOUNDS {
  
    KEIN_WEIN =1, //
    ZU_DUENN,
    WIRD_BESSER,
    HALWE_HALWE,
    ZU_FETT,
    NUR_WEIN,
    IMMER_NOCH_NIX,
    SO_ISSER_RECHT,
    WELCOME

};

const int analogInPin = A0; 
const int buttonPin   = 4;
const int dauer_status_anzeige_ms = 3000;
LiquidCrystal_I2C lcd_balken(0x27,16,2);  // set the lcd_balken address to 0x27 or 0x26 (solder bride A0) for a 16 chars and 2 line display
LiquidCrystal_I2C lcd_kopf  (0x3f,16,2);
uint8_t links_blk[8]  = {0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t rechts_blk[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f};

void setup()
{ 

  mp3.begin(9600);
  mp3.reset();
  mp3.setVolume(20);
  mp3.setLoopMode(MP3_LOOP_NONE);
  mp3.playFileByIndexNumber(WELCOME);
    
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);
  lcd_kopf.init(); 
  lcd_kopf.backlight();
  lcd_kopf.setCursor(0,0);
  lcd_kopf.print(F("*Schorleautomat*"));
  lcd_kopf.setCursor(0,1);
  lcd_kopf.print(F("Knopf n. "   "LINKS" "!"));
  
  lcd_balken.init();                      // initialize the lcd_balken 
  lcd_balken.createChar(1, links_blk);
  lcd_balken.createChar(2, rechts_blk);
  lcd_balken.backlight();
  lcd_balken.setCursor(0,0);
  lcd_balken.print(F("^^^^^^^^^^^^^^^^"));
  lcd_balken.setCursor(0,1);
  lcd_balken.print(F("||||||||||||||||"));
  //while ( analogRead(analogInPin) > 5 ) {}; // warten bis Poti ganz links gedreht.
  delay(3000);
  lcd_kopf.clear();
} // setup()

int sensorValue = 0;        // value read from the pot
int anzBlocks = 0; 
int prozent = 0;
long anz_timer = 0; // 0: keine Aufruf der Ausgabe der Kopfzeile; ansonsten Zeitpunkt der letzten Ausgabe von Statusinfo in ms

void einstellung_bewerten()
{
  Serial.print(anzBlocks);  Serial.print( ' ' );Serial.print( prozent );Serial.print( ' ' ); Serial.println(sensorValue);
  switch (anzBlocks)
         { 
          case 0: 
                   {
                       kopf_ausgabe(
                         F(" Wie, kein Wein?"),
                         F("Zweite Zeile der Aus")
                      );        
                      mp3.playFileByIndexNumber(KEIN_WEIN);
                    break;
                   }; // 0
          
          case 1: 
          case 2:
                   {
                      kopf_ausgabe(
                         F("Is awwer d\365nn"),
                         F("Zweite Zeile der Aus")
                      );                     
                      mp3.playFileByIndexNumber(ZU_DUENN);
                    break;
                   }; 
           
           case 3:
           case 4:
           case 5:
           case 9:
          case 10:
                   {
                      kopf_ausgabe(
                         F("Wird besser      "),
                         F("Zweite Zeile der Aus")
                      ); 
                      mp3.playFileByIndexNumber(WIRD_BESSER);
                    break;
                   }; 
          case 7:
          case 8:
          
            {
                      kopf_ausgabe(
                         F("Halwe Halwe     "),
                         F("Zweite Zeile der Aus")
                      );                      
                      mp3.playFileByIndexNumber(HALWE_HALWE);
                    break;
                   }; 
         
           case 11: 
           case 12: 
            {
                     kopf_ausgabe(
                         F("So ischer recht     "),
                         F("Zweite Zeile der Aus")
                      );                      
                     mp3.playFileByIndexNumber(SO_ISSER_RECHT);
                    break;
                   }; 
           
           case 14: 
           case 15:                       
                    {
                     kopf_ausgabe(
                         F("Zu fetter Schorle!     "),
                         F("Zweite Zeile der Aus")
                      );                      
                     mp3.playFileByIndexNumber(ZU_FETT);
                    break;
                   };  
           case 16:   
                    {
                     kopf_ausgabe(
                         F("Nur Wein"),
                         F("")
                      );     
                     mp3.playFileByIndexNumber(NUR_WEIN);
                    break;
                   };    
           default:                   
                    {
                     kopf_ausgabe(
                         F("Immer noch NIX"),
                         F("Zweite Zeile der Aus")
                      );                    
                     mp3.playFileByIndexNumber(IMMER_NOCH_NIX);
                    }; // default            
         }; // switch
} // einstellung_bewerten()

void kopf_ausgabe(const __FlashStringHelper* z1, const __FlashStringHelper* z2)
{
  lcd_kopf.setCursor(0,0);
  lcd_kopf.print(z1);
  lcd_kopf.setCursor(0,1);
  lcd_kopf.print(z2);
};


void loop()
{ 
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
// map it to the range of the analog out:
  anzBlocks = map(sensorValue, 0, 1022, 0, 16);
  prozent = map(sensorValue, 0, 1022, 0, 100);

  // Linke Seite Balken
  lcd_balken.setCursor(0,0);
  for (int i=1; i<=anzBlocks; i++)
  {
     lcd_balken.write(255);
  };
  for (int i=1; i<=16-anzBlocks; i++)
  {
     lcd_balken.write(1);
  };

  // Rechte Seite Balken
  lcd_balken.setCursor(0,1);
  for (int i=1; i<=anzBlocks; i++)
  {
     lcd_balken.write(2);
  };
  for (int i=1; i<=16-anzBlocks; i++)
  {
     lcd_balken.write(255);
  };

  lcd_kopf.setCursor(0,0);
  lcd_kopf.print(F("   Wein   "));
  lcd_kopf.print(prozent);
  lcd_kopf.print(F("                "));
  lcd_kopf.setCursor(0,1);
  lcd_kopf.print(F("   Wasser "));
  lcd_kopf.print(100 - prozent);
  lcd_kopf.print(F("                "));

  if (digitalRead(buttonPin) == LOW)  {
      lcd_kopf.clear();
      einstellung_bewerten ();
      anz_timer=millis();
      while (digitalRead(buttonPin) == LOW);     
  };
 
  
} // loop()
