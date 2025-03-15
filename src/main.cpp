#include <Arduino.h>
#include <TaskScheduler.h>
#include <PinChangeInterrupt.h>

const int RED_LED = 11;
const int YELLOW_LED = 10;
const int GREEN_LED = 9;
const int SWITCH_PIN1 = 2;
const int SWITCH_PIN2 = 3;
const int SWITCH_PIN3 = 4;
const int POTENTIOMETER_PIN = A0;

volatile bool emergencyMode = false;
volatile bool cautionMode = false;
volatile bool blinkMode = false;
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 300;
bool ledState = false;

int redTime = 2000;
int yellowTime = 500;
int greenTime = 2000;

Scheduler runner;
void task1();
void task2();
void task3();
void task4();
void task5();
void handleBlink();

Task t1(redTime, TASK_FOREVER, &task1, &runner, false);
Task t2(yellowTime, TASK_FOREVER, &task2, &runner, false);
Task t3(greenTime, TASK_FOREVER, &task3, &runner, false);
Task t4(1000, TASK_FOREVER, &task4, &runner, false);
Task t5(yellowTime, TASK_FOREVER, &task5, &runner, false);

void emergencyISR() {
    emergencyMode = !digitalRead(SWITCH_PIN1);
    runner.disableAll();
    Serial.println(emergencyMode ? "MODE: Emergency" : "MODE: Default");
    if (!emergencyMode) runner.enableAll();
}

void cautionISR() {
    cautionMode = !digitalRead(SWITCH_PIN2);
    runner.disableAll();
    Serial.println(cautionMode ? "MODE: Caution" : "MODE: Default");
    if (!cautionMode) runner.enableAll();
}

void blinkISR() {
    blinkMode = !digitalRead(SWITCH_PIN3);
    runner.disableAll();
    Serial.println(blinkMode ? "MODE: Blink" : "MODE: Default");
    if (!blinkMode) runner.enableAll();
}

void handleBlink() {
    if (millis() - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = millis();
        ledState = !ledState;
        digitalWrite(RED_LED, ledState);
        digitalWrite(YELLOW_LED, ledState);
        digitalWrite(GREEN_LED, ledState);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(SWITCH_PIN1, INPUT_PULLUP);
    pinMode(SWITCH_PIN2, INPUT_PULLUP);
    pinMode(SWITCH_PIN3, INPUT_PULLUP);
    pinMode(POTENTIOMETER_PIN, INPUT);
    
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN1), emergencyISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN2), cautionISR, CHANGE);
    attachPCINT(digitalPinToPCINT(SWITCH_PIN3), blinkISR, CHANGE);
    
    t1.enableDelayed(1);
    t2.enableDelayed(2000);
    t3.enableDelayed(2500);
    t4.enableDelayed(4500);
    t5.enableDelayed(5500);
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        int values[3];
        int index = 0;
        char *ptr = strtok((char *)input.c_str(), ",");
        while (ptr != NULL && index < 3) {
            values[index++] = atoi(ptr);
            ptr = strtok(NULL, ",");
        }
        if (index == 3) {
            redTime = values[0];
            yellowTime = values[1];
            greenTime = values[2];
            t1.setInterval(redTime);
            t2.setInterval(yellowTime);
            t3.setInterval(greenTime);
            t5.setInterval(yellowTime);
        }
    }

    int potValue = analogRead(POTENTIOMETER_PIN);
    int brightness = map(potValue, 0, 1023, 0, 255);
    Serial.print("Brightness: ");
    Serial.println(brightness);

    if (emergencyMode) {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        Serial.println("RED");
    } else if (cautionMode) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        Serial.println("YELLOW");
    } else if (blinkMode) {
        handleBlink();
        Serial.println("BLINK");
    } else {
        runner.execute();
    }
}

void task1() {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    delay(redTime);
    digitalWrite(RED_LED, LOW);
}

void task2() {
    digitalWrite(YELLOW_LED, HIGH);
    delay(yellowTime);
    digitalWrite(YELLOW_LED, LOW);
}

void task3() {
    digitalWrite(GREEN_LED, HIGH);
    delay(greenTime);
    digitalWrite(GREEN_LED, LOW);
}

void task4() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(GREEN_LED, LOW);
        delay(250);
        digitalWrite(GREEN_LED, HIGH);
        delay(250);
    }
    digitalWrite(GREEN_LED, LOW);
}

void task5() {
    digitalWrite(YELLOW_LED, HIGH);
    delay(yellowTime);
    digitalWrite(YELLOW_LED, LOW);
}
