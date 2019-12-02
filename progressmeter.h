#ifndef PROGRESSMETER_H
#define PROGRESSMETER_H

/* An abstract progress indicator. It provides the core of a progress indicator
 * that prints the current state at predetermined time intervals.
*/
class ProgressIndicator
{
public:
  using Milliseconds = unsigned long;
  using TimePoint = Milliseconds;

   ProgressIndicator(float initial_, float target_) :
      initial(0), target(0)
   {
      reset(initial_, target_);
   }

   virtual ~ProgressIndicator() = default;

   // Print the current progress if either enough time has elapsed from the
   // previous printing or the forcePrint parameter is true.
   void print(float value, bool forcePrint = false)
   {
      TimePoint now = millis();
      if (!forcePrint && now < previousPrint + printPeriod)
      {
         return;
      }
      previousPrint = now;
      printProgress(value);
   }

   // Set new initial and target values.
   void reset(float initial_, float target_)
   {
      initial = initial_;
      target = target_;
      previousPrint = millis() - printPeriod;
   }

   // Finalize the output (for example, by printing a final newline).
   virtual void finalize() = 0;

protected:
   // Do the actual printing.
   virtual void printProgress(float value) = 0;

   float initial;
   float target;
   TimePoint previousPrint;

   static constexpr Milliseconds printPeriod{100};
};


// An ASCII progress bar.
class BarIndicator : public ProgressIndicator
{
public:
   using ProgressIndicator::ProgressIndicator;

   virtual void finalize()
   {
      Serial.write('\n');
   }

private:
   virtual void printProgress(float value)
   {
      constexpr size_t stringLength = 32;
      char formattedValue[stringLength];
      snprintf(formattedValue, stringLength, "\r\033[K%4d.%02d ", int(value), int(100 * (value - int(value))));
      Serial.write(formattedValue);

      int position = round(length * (value - initial)/(target - initial));
      position = min(max(position, 0), length - 1);
      for (int i = 0; i < length; ++i)
      {
         Serial.write(
            i < position ? '=' :
            i == position ? '>' :
            '-');
      }
   }

   // Length of the bar in characters.
   static constexpr int length = 30;
};


/* An indicator with simple numeric output suitable for further processing.
 * It outputs lines with the format:
 *
 * <current value> <progress percent>
*/
class PercentIndicator : public ProgressIndicator
{
public:
   using ProgressIndicator::ProgressIndicator;
   virtual void finalize() {}

private:
   virtual void printProgress(float value)
   {
      constexpr size_t stringLength = 32;
      char formatted[stringLength];
      snprintf(formatted, stringLength, "%4d.%02d %d\r\n",
               int(value), int(100 * (value - int(value))),
               (int)round(100 * (value - initial)/(target - initial)));
      Serial.write(formatted);
   }
};

#endif  // PROGRESSMETER_H
