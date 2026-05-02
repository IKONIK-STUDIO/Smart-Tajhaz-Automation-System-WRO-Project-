// KÖNYVTÁRAK
#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// SZERVÓ
Servo szervo;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ESZKÖZÖK
const int fenyBal = A0;
const int fenyJobb = A1;

// VÁLTOZÓK
// napelem
int napelemStep = 0;
int napelemIf1SideAStep = 0;
int napelemFor1Count = 0;
int napelemFor2Count = 0;
boolean napelemIf1Side = false; // false = sideB
boolean napelemIf2Side = false; // false = sideB
int poz = 90;
int bal = 0;
int jobb = 0;
int diff = 0;
const int minPoz = 0;
const int maxPoz = 180;
const int halottZona = 30;
long szum = 0;

// serial
float belsoHomerseklet = 0;
float belsoParatartalom = 0;
float kulsoHomerseklet = 0;
float kulsoParatartalom = 0;

// lcd
int lcdStep = 1;
unsigned long lcdElozoMillik = 0;
unsigned long lcdJelenlegiMillik = 0;
const long lcdIntervallum = 4000;

unsigned long mainElozoMillik = 0;
unsigned long mainJelenlegiMillik = 0;

// FÜGGVÉNYEK
void napelemMain(const int napelemStepFv, const int napelemIf1SideAStepFv, const int napelemFor1CountFv, const int napelemFor2CountFv, const boolean napelemIf1SideFv, const boolean napelemIf2SideFv){
    if(napelemStepFv == 1){
        if(napelemFor1CountFv <= 10){
            szum += analogRead(fenyBal);
            napelemFor1Count++;
            napelemStep--;
        }else{
            napelemFor1Count = 1;
        }
    }else if(napelemStepFv == 2){bal = szum / 10;
    }else if(napelemStepFv == 3){szum = 0;
    }else if(napelemStepFv == 4){
        if(napelemFor2CountFv <= 10){
            szum += analogRead(fenyJobb);
            napelemFor2Count++;
            napelemStep--;
        }else{
            napelemFor2Count = 1;
        }
    }else if(napelemStepFv == 5){jobb = szum / 10;
    }else if(napelemStepFv == 6){szum = 0;
    }else if(napelemStepFv == 7){diff = bal - jobb;
    }else if(napelemStepFv == 8){if(abs(diff) > halottZona){napelemIf1Side = true;}else{napelemIf1Side = false; napelemStep++;}
    }else if(napelemStepFv == 9 && napelemIf1SideFv && napelemIf1SideAStepFv == 1){
        if(diff > 0){
            napelemIf2Side = true;
            napelemIf1SideAStep++;
            napelemStep--;
        }else{
            napelemIf2Side = false;
            napelemIf1SideAStep++;
            napelemStep--;
        }
    }else if(napelemStepFv == 9 && napelemIf1SideFv && napelemIf1SideAStepFv == 2 && napelemIf2SideFv){poz += 5; napelemIf1SideAStep++; napelemStep--;
    }else if(napelemStepFv == 9 && napelemIf1SideFv && napelemIf1SideAStepFv == 2 && !napelemIf2SideFv){poz -= 5; napelemIf1SideAStep++; napelemStep--;
    }else if(napelemStepFv == 9 && napelemIf1SideFv && napelemIf1SideAStepFv == 3){poz = constrain(poz, minPoz, maxPoz); napelemIf1SideAStep++; napelemStep--;
    }else if(napelemStepFv == 9 && napelemIf1SideFv && napelemIf1SideAStepFv == 4){szervo.write(poz); napelemIf1SideAStep = 1;}
    
    napelemStep++;
}

void serialMain(){
    if(Serial.available() > 0){
        String teszt = Serial.readStringUntil('\n');
        teszt.trim();

        if(teszt == "a"){
            String s1 = Serial.readStringUntil('\n');
            String s2 = Serial.readStringUntil('\n');
            String s3 = Serial.readStringUntil('\n');
            String s4 = Serial.readStringUntil('\n');

            s1.trim();
            s2.trim();
            s3.trim();
            s4.trim();

            belsoHomerseklet = s1.toFloat();
            belsoParatartalom = s2.toFloat();
            kulsoHomerseklet = s3.toFloat();
            kulsoParatartalom = s4.toFloat();
        }
    }
}

void lcdMain(const int lcdStepFv){
    if(lcdStepFv == 1){lcd.clear();
    }else if(lcdStepFv == 2){lcd.setCursor(0, 0);
    }else if(lcdStepFv == 3){lcd.print("B:");
    }else if(lcdStepFv == 4){lcd.print(belsoHomerseklet, 1);
    }else if(lcdStepFv == 5){lcd.print("C ");

    }else if(lcdStepFv == 6){lcd.print("K:");
    }else if(lcdStepFv == 7){lcd.print(kulsoHomerseklet, 1);
    }else if(lcdStepFv == 8){lcd.print("C");
    
    }else if(lcdStepFv == 9){lcd.setCursor(0, 1);
    }else if(lcdStepFv == 10){lcd.print("BP:");
    }else if(lcdStepFv == 11){lcd.print(belsoParatartalom, 0);
    }else if(lcdStepFv == 12){lcd.print("%  ");
    
    }else if(lcdStepFv == 13){lcd.print("KP:");
    }else if(lcdStepFv == 14){lcd.print(kulsoParatartalom, 0);
    }else if(lcdStepFv == 15){lcd.print("%");}

    lcdStep++;
}

void setup(){
    // serial indítása
    Serial.begin(9600);

    // szervó
    szervo.attach(9);
    szervo.write(poz);

    // lcd
    lcd.init();
    lcd.backlight();

    // pinMode-ok
    pinMode(fenyBal, INPUT);
    pinMode(fenyJobb, INPUT);

    // változók
    napelemStep = 1; // napelem
    napelemIf1SideAStep = 1;
    napelemFor1Count = 1;
    napelemFor2Count = 1;
    poz = 90;
    bal = 0;
    jobb = 0;
    diff = 0;
    szum = 0;
    napelemIf1Side = false;
    napelemIf2Side = false;

    belsoHomerseklet = 0; // serial
    belsoParatartalom = 0;
    kulsoHomerseklet = 0;
    kulsoParatartalom = 0;

    lcdStep = 1; // lcd
}

void loop(){
    // mainJelenlegiMillik = millis();

    // 1. NAPELEM
    if(napelemStep == 10){napelemStep = 1;}
    napelemMain(napelemStep, napelemIf1SideAStep, napelemFor1Count, napelemFor2Count, napelemIf1Side, napelemIf2Side);

    // 2. SERIAL
    serialMain();

    // 3. LCD
    if(lcdStep == 16){lcdStep = 1;}
    
    lcdJelenlegiMillik = millis();
    if((lcdJelenlegiMillik - lcdElozoMillik >= lcdIntervallum) && lcdStep == 1){
        lcdElozoMillik = lcdJelenlegiMillik;

        lcdMain(lcdStep);
    }else if(lcdStep > 1){
        lcdMain(lcdStep);
    }

    delay(10);
    // Serial.println("Futasi ido: ");
    // Serial.print(mainJelenlegiMillik - mainElozoMillik);
    // mainElozoMillik = mainJelenlegiMillik;
}