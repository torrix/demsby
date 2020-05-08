#include <Wire.h>
#include <Bounce2.h>

const int channels = 2;
const int bounceLength = 100;
const byte servoControllerAddress = 2; // Set the servo controller address

const int button[] = {2, 3};
const int left[] = {22, 24};
const int right[] = {23, 25};

bool state[] = {0,0};

Bounce bounce[channels];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Initial state");
  sendToMegapoints();
    
  for(int i = 0; i < channels; i++) {
    bounce[i] = Bounce(button[i], bounceLength);
    pinMode(button[i], INPUT_PULLUP);
    pinMode(left[i], OUTPUT);
    pinMode(right[i], OUTPUT);
    digitalWrite(left[i], state[i]);
    digitalWrite(right[i], !state[i]); 
  }
}

void loop() {
  for(int i = 0; i < channels; i++) {
    bounce[i].update();
    if (bounce[i].fell() ) {
      state[i] = !state[i];
      digitalWrite(right[i], state[i]);
      digitalWrite(left[i], !state[i]);
      sendToMegapoints();
    }
  }
}

void sendToMegapoints() {
  // TODO: Split into two bytes
  // 8-7-6-5-4-3-2-1
  // X-X-X-X-9-10-11-12
  int foo = bitArrayToInt32(state, channels);
  Serial.println(foo);
  Wire.beginTransmission(servoControllerAddress);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission();
}

int bitArrayToInt32(bool arr[], int count)
{
    int ret = 0;
    int tmp;
    for (int i = 0; i < count; i++) {
        tmp = arr[i];
        ret |= tmp << (count - i - 1);
    }
    return ret;
}
