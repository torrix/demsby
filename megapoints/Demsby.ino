#include <Wire.h>

#include <Bounce2.h>

const long interval = 1000;
const int channels = 12;
const int routes = 8;
const int bounceLength = 100;
const byte servoControllerAddress = 2;

const int routeButtons[] = {
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8
};

const int button[] = {
    2, // 1
    3, // 2
    4, // 3
    5, // 4
    6, // 5
    7, // 6
    8, // 7
    9, // 8
    10, // 9
    11, // 10
    12, // 11
    13 // 12
};

const int led[] = {
    22, // 1
    24, // 2
    26, // 3
    28, // 4
    30, // 5
    32, // 6
    34, // 7
    36, // 8
    38, // 9
    40, // 10
    42, // 11
    44, // 12
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
Bounce routeBounce[routes];

unsigned long previousMillis = 0;

void setup() {
    Wire.begin();
    Serial.begin(9600);
    //    Serial.println("RESTARTING");
    sendToMegapoints();
    outputState();

    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);
    pinMode(A4, INPUT_PULLUP);
    pinMode(A5, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);
    pinMode(A7, INPUT_PULLUP);
    pinMode(A8, INPUT_PULLUP);

    for (int i = 0; i < channels; i++) {
        bounce[i] = Bounce(button[i], bounceLength);
        pinMode(button[i], INPUT_PULLUP);
        pinMode(led[i], OUTPUT);
        pinMode(led[i] + 1, OUTPUT);
        digitalWrite(led[i], !state[i]);
        digitalWrite(led[i] + 1, state[i]);
    }
    for (int i = 0; i < routes; i++) {
        routeBounce[i] = Bounce(routeButtons[i], bounceLength);
        pinMode(routeButtons[i], INPUT_PULLUP);
    }
}

void loop() {
    unsigned long currentMillis = millis();

    // Handle physical button press
    for (int i = 0; i < channels; i++) {
        bounce[i].update();
        if (bounce[i].fell()) {
            toggle(i);
        }
    }
    for (int i = 0; i < routes; i++) {
        routeBounce[i].update();
        if (routeBounce[i].fell()) {
            toggle(i);
        }
    }

    // Handle serial instruction
    if (Serial.available() > 0) {
        int j = Serial.parseInt();
        toggle(j);
    }

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        outputState();
    }
}

void toggle(int which) {
    state[which] = !state[which];
    digitalWrite(led[which], !state[which]);
    digitalWrite(led[which] + 1, state[which]);
    sendToMegapoints();
    outputState();
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

    Wire.beginTransmission(servoControllerAddress);
    Wire.write(bitArrayToInt(bitArrayOne));
    Wire.write(bitArrayToInt(bitArrayTwo));
    Wire.endTransmission();
}

void outputState() {
    // Outputs a JSON-compatible array of the current state
    Serial.print('[');
    for (int i = 0; i < channels; i++) {
        Serial.print(state[i]);
        if (i != channels - 1) {
            Serial.print(',');
        }
    }
    Serial.println(']');
}

int bitArrayToInt(bool arr[]) {
    byte ret = 0;
    int tmp;
    for (int i = 0; i < 8; i++) {
        tmp = arr[i];
        ret |= tmp << (8 - i - 1);
    }

    return ret;
}
