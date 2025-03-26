#include "hardware/Buzzer.h"

// Define the buzzer GPIO pin
#define BUZZER_PIN 14

//--------------------------------------------------
// Helper function: Simple beep
//--------------------------------------------------
static void doBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);               // Buzzer on for 200 ms
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);               // Pause 200 ms between beeps
}

//--------------------------------------------------
// Helper function: Cat Meow (frequency sweep)
//--------------------------------------------------
static void catMeow() {
  // Up-sweep (approx. 400 Hz -> 700 Hz)
  for (int freq = 400; freq <= 700; freq += 10) {
    tone(BUZZER_PIN, freq, 40);  // beep at 'freq' Hz for 40ms
    delay(40);                   // short wait so tones don't overlap
  }
  delay(100);  // short pause

  // Down-sweep (approx. 700 Hz -> 400 Hz)
  for (int freq = 700; freq >= 400; freq -= 10) {
    tone(BUZZER_PIN, freq, 40);
    delay(40);
  }
  // Turn buzzer off
  noTone(BUZZER_PIN);
}

//--------------------------------------------------
// Public function: Initialize buzzer
//--------------------------------------------------
void initBuzzer(void)
{
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

//--------------------------------------------------
// Public function: Buzzer Action
//--------------------------------------------------
void buzzerAction(int times, bool speakNumber, bool meow)
{
  // If meow is true, we ignore speakNumber and beep times times with a cat meow
  if (meow) {
    for (int i = 0; i < times; i++) {
      catMeow();
      delay(300);  // Optional pause between meows
    }
    return;
  }

  // If not meow, proceed with beep logic
  if (!speakNumber) {
    // beep 'times' times
    for (int i = 0; i < times; i++) {
      doBeep();
    }
  } else {
    // "Speak" the integer by beeping each digit
    String digits = String(times);  // Convert to string
    for (unsigned int i = 0; i < digits.length(); i++) {
      char c = digits[i];
      // Convert char to integer digit
      int digit = c - '0';
      // beep 'digit' times
      for (int j = 0; j < digit; j++) {
        doBeep();
      }
      // optional: short pause between digits
      delay(500);
    }
  }
}
