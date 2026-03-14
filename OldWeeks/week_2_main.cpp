#include <Arduino.h>

// put function declarations here:
String morseTable[] = {
    // Letters A-Z (indices 0-25)
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....",
    "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.",
    "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",
    "-.--", "--..",
    // Digits start here (indices 26-35)
    ".----", "..--", "...--", "....-", "....",
    "-...", "--...", "---..", "----.", "-----"};

#define LED 13 // LED pin number

String input;

int lookup(char c)
{
  // If the character is not a valid letter or digit then return -1
  if (!isAlphaNumeric(c))
  {
    return -1;
  }
  // Now we do a bit of ASCII trickery to convert letters & digits
  // into positions in our lookup table. See https://www.arduino.cc/reference/en/

  // Letters are easier
  if (isAlpha(c))
  {
    return c - 'A';
  }
  if (isDigit(c))
  {
    // add on 25 because the digits come after the letters in the table, so we skip far
    return (c - '0') + 25;
  }
}

// Converts  the morse code string to LED blinking
void morseToLED(String code)
{
  for (char i : code)
  { // loops over each morse charecter to blink led accordingly
    if (i == '-')
    {
      digitalWrite(LED, HIGH);
      delay(200);
      digitalWrite(LED, LOW);
    }
    else
    {
      digitalWrite(LED, HIGH);
      delay(50);
      digitalWrite(LED, LOW);
    }
    delay(500);
  }
}

void setup()
{
  Serial.begin(115200); // Initialize Serial Monitor
  pinMode(LED, OUTPUT); // Set LED to output pin
}
void loop()
{

  if (Serial.available())
  {                                       // Checks if something new in serial monitor
    input = Serial.readStringUntil('\n'); // reads till new line
    input.trim();                         // Trims off unnecessary stuff around the sting
    input.toUpperCase();                  // Converts to only uppercase
    for (char s : input)
    { // Loops through each character in input
      if (isspace(s))
      {
        delay(300); // Generates delay if its a space
        continue;
      }
      int index = lookup(s); // Else looks up the correct index based on character
      if (index != -1)
      { // if index is != -1 then its a valid index
        morseToLED(morseTable[index]);
      }
    }
  }
  delay(1000);
}