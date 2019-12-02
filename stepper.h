#define BD63847EFV
#include <ROHM_Steppers.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

enum class Direction: byte {
  red = CW,
  blue = CCW,
  zero = CCW,
};

class Stepper {
public:
    static constexpr byte gatePin = 5;
    static constexpr byte steppingMode = SIXTEENTH_STEP;
    static constexpr float maxPosition = 3300;
    static constexpr float minPosition = 0;  // enforced by inGate()
    int stepsPerSecond = 100;

private:
    ROHM_Stepper RS{ONE};
    float position = minPosition;
    float remainingSteps = 0;
    Direction direction = Direction::zero;

public:
    Stepper() {
        // Optical gate pullup
        pinMode(gatePin, INPUT_PULLUP);
        RS.ENABLE(ACTIVE);
        RS.PS(ACTIVE);
        delayMicroseconds(40); // wait 40us after PS->High as recommended in data sheet
        RS.MODE(steppingMode);
        RS.setCLK_Hz(stepsPerSecond * steppingMode);
    }

    bool inGate() const {
        return digitalRead(gatePin) == HIGH;
    }

    void step(float numSteps, Direction dir) {
        remainingSteps = 0;
        RS.CW_CCW(int(dir));
        performSteps(numSteps, dir);
    }

    bool partialMove() {
        if (remainingSteps <= 0) {
            return true;
        }

        float steps = MIN(1, remainingSteps);
        remainingSteps -= steps;
        performSteps(steps, direction);

        if (remainingSteps <= 0) {
            return true;
        } else {
            return false;
        }
    }

    float getPosition() const {
        return position;
    }

    void setRate(int rate) {
      RS.setCLK_Hz(rate * steppingMode);
    }

    void setDestination(float pos) {
        remainingSteps = pos - position;
        direction = remainingSteps > 0 ? Direction::red : Direction::blue;
        RS.CW_CCW(int(direction));
        remainingSteps = ABS(remainingSteps);
    }

    void setPosition(float pos) {
        setDestination(pos);
        step(remainingSteps, direction);
    }

    void setHomePosition() {
        if (!inGate()) {
            RS.CW_CCW(int(Direction::blue));
            while (!inGate()) {
                RS.CLK(1);
            }
        }
        RS.CW_CCW(int(Direction::red));

        const unsigned int maxStepsRed = 100;
        unsigned int stepsDone = 0;
        while (!inGate() && stepsDone < maxStepsRed) {
            RS.CLK(1);
            ++stepsDone;
        }

        if (stepsDone >= maxStepsRed) {
          blockForeverOnError();
        }

        position = 0;
        remainingSteps = 0;
    }

private:
    void performSteps(float numSteps, Direction dir) {
        float inc = (dir == Direction::zero ? -1. : +1.) / steppingMode;
        for (unsigned int i = 0; i < (steppingMode * numSteps); ++i, position += inc) {
            if ((dir == Direction::blue && (position < minPosition || inGate())) ||
                (dir == Direction::red && (position > maxPosition))) {
                break;
            }
            RS.CLK(1);
        }
    }

    void blockForeverOnError() {
      // Switching the direction blinks the LED.
      while (true) {
        RS.CW_CCW(int(Direction::blue));
        delay(200);
        RS.CW_CCW(int(Direction::red));
        delay(200);
      }
    }
};
