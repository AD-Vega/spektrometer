#include "stepper.h"
#include "qdec.h"
#include "progressmeter.h"
 
// Zasedene pinjole:
//     motor: 3 A1 7 6 13 10
//     vrata: 5
//     enkoder: 2 4

Stepper stepper;

using enc = ::SimpleHacks::QDec<4, 2, true>;
static int encoderStep = 8;

void go_to(float pos) {
  if (pos <= stepper.maxPosition && pos >= stepper.minPosition) {
      Serial.print("Moving from ");
      Serial.print(stepper.getPosition());
      Serial.print(" to ");
      Serial.print(pos);
      Serial.print(" ...");
      stepper.setDestination(pos);
      String input = "";
      BarIndicator indicator(stepper.getPosition(), pos);
      while (!stepper.partialMove()) {
          indicator.print(stepper.getPosition());
          while (Serial.available()) {
              char c = Serial.read();
              if (c == '\n') {
                  while (Serial.available()) {
                      Serial.read();
                  }
                  if (input.startsWith("stop")) {
                      stepper.setDestination(stepper.getPosition());
                      Serial.print(" interrupted at ");
                      Serial.print(stepper.getPosition());
                      Serial.println(".");
                      return;
                  } else {
                      input = "";
                      break;
                  }
              }
              input += c;
          }
      }
      indicator.print(stepper.getPosition(), true);
      Serial.println(" arrived.");
  } else {
      Serial.print("Position ");
      Serial.print(pos);
      Serial.println(" out of range.");
  }
}

void set_rate(int rate) {
  if (rate > 0 && rate <= 200) {
    stepper.setRate(rate);
    Serial.println("Rate set to " + String(rate) + ".");
  } else {
    Serial.println("Rate " + String(rate) + " out of range.");
  }
}

void set_encoder_step(int estep) {
  if (estep > 0 && estep <= 50) {
    encoderStep = estep;
    Serial.println("Encoder step set to " + String(encoderStep) + ".");
  } else {
    Serial.println("Encoder step " + String(estep) + " out of range.");
  }
}

void set_home_position() {
  Serial.print("Moving to home position ...");
  stepper.setHomePosition();
  Serial.println(" arrived to home position.");
}

// execute command
void execute(const String& input) {
  if (input == "home" || input == "h") {
    set_home_position();
  }
  else if (input == "red" || input == "r") {
    go_to(2500);
  }
  else if (input == "orange" || input == "o") {
    go_to(2310);
  }
  else if (input == "yellow" || input == "y") {
    go_to(2250);
  }
  else if (input == "green" || input == "g") {
    go_to(2000);
  }
  else if (input == "blue" || input == "b") {
    go_to(1750);
  }
  else if (input == "violet" || input == "v") {
    go_to(1600);
  }

  // Stepper motor rate
  else if (input.startsWith("rate")) {
    String rate_string = String(input);
    rate_string.remove(0, 4);
    int rate = rate_string.toInt();
    set_rate(rate);
  }
  // Encoder steps
  else if (input.startsWith("estep")) {
    String estep_string = String(input);
    estep_string.remove(0, 5);
    int estep = estep_string.toInt();
    set_encoder_step(estep);
  }

  else {
    const float value = input.toFloat();
    if (value > 0 || (value == 0 && input == "0")) {
      go_to(value);
    }
    else {
      Serial.println("Invalid command.");
    }
  }
}

void setup()
{
    enc::begin();
    Serial.begin(9600);
    delay(500);
    set_home_position();
}

void loop() 
{
    ::SimpleHacks::QDECODER_EVENT event = enc::update();

    int step = 0;
    if (event & ::SimpleHacks::QDECODER_EVENT_CW) {
        step = encoderStep;
    } else if (event & ::SimpleHacks::QDECODER_EVENT_CCW) {
        step = -encoderStep;
    }

    if (step) {
        stepper.setPosition(stepper.getPosition() + step);
        Serial.println(stepper.getPosition());
    }

    if (Serial.available()) {
      const String input = Serial.readStringUntil('\n');
      execute(input);
    }
  }
 
