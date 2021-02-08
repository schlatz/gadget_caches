
 /*
 20150307 ekristoff  Adapted from SparkFun Inventor's Kit Example sketch 16
                     https://learn.sparkfun.com/tutorials/sik-experiment-guide-for-arduino---v32/experiment-16-simon-says
 http://geocaching.hyliston.net/2016/04/29/building-my-simon-says-smart-cache/
https://coord.info/GC5PTYV
                   
 Simon tones from Wikipedia
 - A (red) - 440Hz
 - a (green) - 880Hz
 - D (blue) - 587.33Hz
 - G (yellow) - 784Hz
*/
 #define MP3_DEBUG 1
 #include <SoftwareSerial.h>
//#include <JQ6500_Serial.h>
//JQ6500_Serial mp3(8,9);
SoftwareSerial mp3(8,9);
enum SOUNDS {
    WELCOME=1,
    SND1,
    SND2,
    SND3,
    SND4,
    WINNER, 
    LOSER  
};
 
/*************************************************
* Public Constants
*************************************************/
 
// NOTE FREQUENCIES
#define  C3  130.81
#define Db3 138.59
#define D3  146.83
#define Eb3 155.56
#define E3  164.81
#define F3  174.61
#define Gb3 185.00
#define G3  196.00
#define Ab3 207.65
#define LA3 220.00
#define Bb3 233.08
#define B3  246.94
#define C4  261.63
#define Db4 277.18
#define D4  293.66
#define Eb4 311.13
#define E4  329.63
#define F4  349.23
#define Gb4 369.99
#define G4  392.00
#define Ab4 415.30
#define LA4 440.00
#define Bb4 466.16
#define B4  493.88
#define C5  523.25
#define Db5 554.37
#define D5  587.33
#define Eb5 622.25
#define E5  659.26
#define F5  698.46
#define Gb5 739.99
#define G5  783.99
#define Ab5 830.61
#define LA5 880.00
#define Bb5 932.33
#define B5  987.77
 
// DURATION OF THE NOTES 
#define BPM 240     //you can change this value changing all the others
#define Q 60000/BPM //quarter 1/4 
#define H 2*Q       //half 2/4
#define T 3*Q       //three quarter 3/4
#define E Q/2       //eighth 1/8
#define S Q/4       // sixteenth 1/16
#define W 4*Q       // whole 4/4
 
// CHECKS FOR BUTTON AND LIGHT POSITIONS
#define CHOICE_OFF    0 //Used to control LEDs
#define CHOICE_NONE   0 //Used to check buttons
#define CHOICE_RED    (1 << 0)
#define CHOICE_GREEN  (1 << 1)
#define CHOICE_BLUE   (1 << 2)
#define CHOICE_YELLOW (1 << 3)
 
// DEFINE PIN LOCATIONS
#define LED_RED       2
#define LED_GREEN     3
#define LED_BLUE      4
#define LED_YELLOW    5
#define BUTTON_RED    14
#define BUTTON_GREEN  15
#define BUTTON_BLUE   16
#define BUTTON_YELLOW 17
#define BUZZER        11

 
// GAME PARAMETERS
#define ENTRY_TIME_LIMIT 3000 //Amount of time to press a button before game times out. 3000 ms = 3 sec
int ROUNDS_TO_WIN = 6; //Number of rounds to succeasfully remember before you win.
 
// GAME STATE
byte gameBoard[32]; //Contains the combination of buttons as we advance
byte gameRound = 0; //Counts the number of succesful rounds the player has made it through
 
//Servo servo;
int pos = 0;
 
void spiel_mp3 (byte nummer)
{
  mp3.write((byte)0x7E);
  mp3.write((byte)0x04);
  mp3.write((byte)0x03);
  mp3.write((byte)0x00);
  mp3.write((byte)nummer);   
  mp3.write((byte)0xEF);
  delay(300);
}

void setup() // Run once when power is connected
{    
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
 
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  Serial.begin(9600);
  mp3.begin(9600);
 // mp3.reset();
//  mp3.setVolume(30);
 // mp3.setLoopMode(MP3_LOOP_NONE);
 // mp3.playFileByIndexNumber(WELCOME);  
  mp3.write((byte)0x7E);
  mp3.write((byte)0x03);
  mp3.write((byte)0x06);
  mp3.write((byte)0x14);
  mp3.write((byte)0xEF);
  spiel_mp3(WELCOME);
 
      
}
 
 
void loop()
{
  attractMode(); // Blink lights while waiting for user to press a button
 
  // Indicate the start of game play
  setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
  delay(1000);
  setLEDs(CHOICE_OFF); // Turn off LEDs
  delay(250);
 
  // Play memory game and handle result
  if (play_memory() == true) 
    play_winner(); // Player won, play winner tones
  else 
    play_loser(); // Player lost, play loser tones
 
}
 
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//THE FOLLOWING FUNCTIONS CONTROL GAME PLAY
 
// Play the memory game
// Returns 0 if player loses, or 1 if player wins
boolean play_memory(void)
{
  randomSeed(millis()); // Seed the random generator with random amount of millis()
  gameRound = 0; // Reset the game to the beginning
 
  while (gameRound < ROUNDS_TO_WIN) 
  {
    add_to_moves(); // Add a button to the current moves, then play them back
    playMoves(); // Play back the current game board
 
    // Then require the player to repeat the sequence.
    for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++)
    {
      byte choice = wait_for_button(); // See what button the user presses
      if (choice == 0) return false; // If wait timed out, player loses
      if (choice != gameBoard[currentMove]) return false; // If the choice is incorect, player loses
    }
 
    delay(1000); // Player was correct, delay before playing moves
  }
 
  return true; // Player made it through all the rounds to win!
}
 
 
// Plays the current contents of the game moves
void playMoves(void)
{
  for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) 
  {
    toner(gameBoard[currentMove]);
 
    // Wait some amount of time between button playback
    // Shorten this to make game harder
    delay(150); // 150 works well. 75 gets fast.
  }
}
 
// Adds a new random button to the game sequence, by sampling the timer
void add_to_moves(void)
{
  byte newButton = random(0, 4); //min (included), max (exluded)
 
  // We have to convert this number, 0 to 3, to CHOICEs
  if(newButton == 0)      newButton = CHOICE_RED;
  else if(newButton == 1) newButton = CHOICE_GREEN;
  else if(newButton == 2) newButton = CHOICE_BLUE;
  else if(newButton == 3) newButton = CHOICE_YELLOW;
 
  gameBoard[gameRound++] = newButton; // Add this new button to the game array
}
 
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//THE FOLLOWING FUNCTIONS CONTROL THE HARDWARE
 
// Lights a given LEDs
// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc
void setLEDs(byte leds)
{
  if ((leds & CHOICE_RED) != 0)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);
 
  if ((leds & CHOICE_GREEN) != 0)
    digitalWrite(LED_GREEN, HIGH);
  else
    digitalWrite(LED_GREEN, LOW);
 
  if ((leds & CHOICE_BLUE) != 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);
 
  if ((leds & CHOICE_YELLOW) != 0)
    digitalWrite(LED_YELLOW, HIGH);
  else
    digitalWrite(LED_YELLOW, LOW);
}
 
// Wait for a button to be pressed. 
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
byte wait_for_button(void)
{
  long startTime = millis(); // Remember the time we started the this loop
 
  while ( (millis() - startTime) < ENTRY_TIME_LIMIT) // Loop until too much time has passed
  {
    byte button = checkButton();
 
    if (button != CHOICE_NONE)
    { 
      toner(button); // Play the button the user just pressed
      while(checkButton() != CHOICE_NONE) ;  // Now let's wait for user to release button
      delay(10); // This helps with debouncing and accidental double taps
      return button;
    }
 
  }
  return CHOICE_NONE; // If we get here, we've timed out!
}
 
// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte checkButton(void)
{
  if (digitalRead(BUTTON_RED) == 0) return(CHOICE_RED); 
  else if (digitalRead(BUTTON_GREEN) == 0) return(CHOICE_GREEN); 
  else if (digitalRead(BUTTON_BLUE) == 0) return(CHOICE_BLUE); 
  else if (digitalRead(BUTTON_YELLOW) == 0) return(CHOICE_YELLOW);
 
  return(CHOICE_NONE); // If no button is pressed, return none
}
 
// Light an LED and play tone
// Red, upper left:     440Hz - A4
// Green, upper right:  880Hz - A5
// Blue, lower left:    587.33Hz - D5
// Yellow, lower right: 784Hz - G5
void toner(byte which)
{
  setLEDs(which); //Turn on a given LED
 
  //Play the sound associated with the given LED
  switch(which) 
  {
  case CHOICE_RED:
     spiel_mp3(SND1);
    play(LA4, Q); 
    break;
  case CHOICE_GREEN:
      spiel_mp3(SND2);
    play(LA5, Q); 
    break;
  case CHOICE_BLUE:
    spiel_mp3(SND3);
    play(D5, Q); 
    break;
  case CHOICE_YELLOW:
    spiel_mp3(SND4); 
    play(G5, Q); 
    break;
  }
 
  setLEDs(CHOICE_OFF); // Turn off all LEDs
}
 
 
// Play the winner sound and lights
void play_winner(void)
{  
  spiel_mp3(WINNER);  
  winner_sound();

  attractMode();  
}
 
 
// Play the winner sound
// We are the Champions!
void winner_sound(void)
{
  play(F5,  W);
  play(E5,  Q);
  play(F5,  Q);
  play(E5,  Q);
  play(C5,  T);
  play(LA4, Q);
  play(D5,  H);
  play(LA4,  W); 
}
 
// Play the loser sound/lights
void play_loser(void)
{ spiel_mp3(LOSER); 
  setLEDs(CHOICE_RED | CHOICE_GREEN);
  play(B3,Q);
 
  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  play(B3,Q);
 
  setLEDs(CHOICE_RED | CHOICE_GREEN);
  play(B3,Q);
 
  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  play(B3,Q);
}
 
// Show an "attract mode" display while waiting for user to press button.
void attractMode(void)
{
  while(1) 
  {
    setLEDs(CHOICE_RED);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
 
    setLEDs(CHOICE_BLUE);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
 
    setLEDs(CHOICE_GREEN);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
 
    setLEDs(CHOICE_YELLOW);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
  }
}
 
 
 
 
void play(long note, int duration) {
  tone(BUZZER,note,duration);
  delay(1+duration);
}

 

