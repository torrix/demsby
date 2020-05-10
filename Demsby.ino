#include <Wire.h>

#include <Bounce2.h>

const int channels = 12;
const int bounceLength = 100;
const byte servoControllerAddress = 2;

const int button[] = {
    2, // 0
    3, // 1
    4, // 2
    5, // 3
    6, // 4
    7, // 5
    8, // 6
    9, // 7
    10, // 8
    11, // 9
    12, // 10
    13 // 11
};

const int point[] = {
    22, // 0
    24, // 1
    26, // 2
    28, // 3
    30, // 4
    32, // 5
    34, // 6
    36, // 7
    38, // 8
    40, // 9
    42, // 10
    44  // 11
};

bool state[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

Bounce bounce[channels];

void setup() {
    Wire.begin();
    Serial.begin(9600);
    Serial.println("RESTARTING");
    sendToMegapoints();
    outputState();

    for (int i = 0; i < channels; i++) {
        bounce[i] = Bounce(button[i], bounceLength);
        pinMode(button[i], INPUT_PULLUP);
        pinMode(point[i], OUTPUT);
        pinMode(point[i] + 1, OUTPUT);
        digitalWrite(point[i], state[i]);
        digitalWrite(point[i] + 1, !state[i]);
    }
}

void loop() {
    for (int i = 0; i < channels; i++) {
        bounce[i].update();
        if (bounce[i].fell()) {
            state[i] = !state[i];
            digitalWrite(point[i] + 1, state[i]);
            digitalWrite(point[i], !state[i]);
            sendToMegapoints();
            outputState();
        }
    }
}

void sendToMegapoints() {
    // Megapoints expects to receive two bytes of data in this order:
    // 8-7-6-5-4-3-2-1
    // X-X-X-X-9-10-11-12
    bool bitArrayOne[] = {
      state[7],
      state[6],
      state[5],
      state[4],
      state[3],
      state[2],
      state[1],
      state[0],
    };
    
    bool bitArrayTwo[] = {
      0,
      0,
      0,
      0,
      state[8],
      state[9],
      state[10],
      state[11],
    };
    
    int byteOne = bitArrayToInt32(bitArrayOne);
    int byteTwo = bitArrayToInt32(bitArrayTwo);
    
    // Debugging
    Serial.print("Megapoints: ");
    Serial.print(byteOne);
    Serial.print(",");
    Serial.println(byteTwo);
    // End debugging
    
    Wire.beginTransmission(servoControllerAddress);
    Wire.write(byteOne);
    Wire.write(byteTwo);
    Wire.endTransmission();
}

void outputState() {
    // Outputs a JSON-compatible array of the current state
    Serial.print('{');
    for (int i = 0; i < channels; i++) {
        Serial.print(state[i]);
        if (i != channels - 1) {
            Serial.print(',');
        }
    }
    Serial.println('}');    
}

int bitArrayToInt32(bool arr[]) {
    int ret = 0;
    int tmp;
    for (int i = 0; i < 8; i++) {
        tmp = arr[i];
        ret |= tmp << (8 - i - 1);
    }

    return ret;
}
