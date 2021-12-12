#include "stdio.h"
#include "wiringPi.h"
#include "stdlib.h"
#include "string.h"

#define DELAY         100
#define VALUE_IN_MIN  121
#define VALUE_IN_MAX  466
#define VALUE_OUT_MIN 0
#define VALUE_OUT_MAX 1023
#define BUFFER_SIZE   5

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main(void)
{
  system("dos2unix waveData.txt > /dev/null 2>&1");

  wiringPiSetup();

  int val = 0, valPrev = VALUE_IN_MIN, mapVal = 0;
  char fileBuff[BUFFER_SIZE] = { '\0' };
  FILE* f;

  pinMode(1, PWM_OUTPUT);
  pwmWrite(1, VALUE_OUT_MIN);

  while (1) {
    f = fopen("/home/pi/LED_Strip/waveData.txt", "r");

    while (fgets(fileBuff, sizeof(fileBuff), f) != NULL) {
      val = atoi(fileBuff);
      // printf("current value: %u\n", val);

      if (val > valPrev) {
        for (int i = valPrev; i <= val; i++)
        {
          // printf("increase: %u\n", i);
          mapVal = map(i, VALUE_IN_MIN, VALUE_IN_MAX, VALUE_OUT_MIN, VALUE_OUT_MAX);
          pwmWrite(1, mapVal);
          delay(DELAY);
        }
      }
      else if (val < valPrev) {
        for (int i = valPrev; i >= val; i--)
        {
          // printf("decrease: %u\n", i);
          mapVal = map(i, VALUE_IN_MIN, VALUE_IN_MAX, VALUE_OUT_MIN, VALUE_OUT_MAX);
          pwmWrite(1, mapVal);
          delay(DELAY);
        }
      }
      else if (val == valPrev) {
        // printf("same: %u\n", val);
        mapVal = map(val, VALUE_IN_MIN, VALUE_IN_MAX, VALUE_OUT_MIN, VALUE_OUT_MAX);
        pwmWrite(1, mapVal);
        delay(DELAY);
      }

      valPrev = val;
      // printf("previous value: %u\n\n", valPrev);

      delay(DELAY);
    }
    fclose(f);
    memset(fileBuff, 0, BUFFER_SIZE);
  }

  return 0;
}
