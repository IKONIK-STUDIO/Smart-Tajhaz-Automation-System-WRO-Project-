// KÖNYVTÁRAK
#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// SZERVÓ
Servo szervo;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// VÍZ HŐMÉRŐ
OneWire oneWire(2); // a víz hőmérője a 2-es pinre van kötve
DallasTemperature sensors(&oneWire);

// ESZKÖZÖK
const int fenyBal = A0;
const int fenyJobb = A1;
const int napelemIn = A2;
const int szeleromuIn = A3;
const int napelemLed = 7;
const int szeleromuLed = 8;
const int szivattyuNap = 11;
const int kezmosoInfra = 4;
const int napkollektorMelegitoLed = 12;
const int napkollektorHutoLed = 13;

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
boolean vanFust = false;
boolean beriaszt = false;

// lcd
int lcdStep = 1;
int lcdRiasztoStep = 1;
int lcdIf2SideAStep = 1;
int lcdIf2SideBStep = 1;
int lcdIf4SideAStep = 1;
int lcdIf5SideAStep = 1;
int oldal = 1;
boolean kellKoviOldal = false;
boolean lcdIf1Side = false; // false = sideB
boolean lcdIf2Side = false; // false = sideB
boolean lcdIf3Side = false; // false = sideB
boolean lcdIf4Side = false; // false = sideB
boolean lcdIf5Side = false; // false = sideB
unsigned long lcdElozoMillik = 0;
unsigned long lcdJelenlegiMillik = 0;
const long lcdIntervallum = 4000;
unsigned long lcdRiasztoElozoMillik = 0;
unsigned long lcdRiasztoJelenlegiMillik = 0;
const long lcdRiasztoIntervallum = 2000;

// led
int ledStep = 1;
boolean ledIf1Side = false; // false = sideB
boolean ledIf2Side = false; // false = sideB
const int napelemOutMin = 614; // állítható
const int szeleromuOutMin = 614; // állítható

// víz hőmérő
int vizStep = 1;
int count = 0;
float vizHomerseklet = 0;
unsigned long vizElozoMillik = 0;
unsigned long vizJelenlegiMillik = 0;
const long vizIntervallum = 4000;

// szivattyú
int szivattyuStep = 1;
boolean szivattyuIf1Side = false; // false = sideB

// kézmosó infra
int infraStep = 1;
int infraIf1Side = false; // false = sideB
boolean kellMelegViz = false;

// melegvíz hőmérséklet kezelés
int melegvizStep = 1;
boolean melegvizIf1Side = false; // false = sideB
boolean melegvizIf2Side = false; // false = sideB

unsigned long mainElozoMillik = 0;
unsigned long mainJelenlegiMillik = 0;

// FÜGGVÉNYEK
void napelemMain(){
    if(napelemStep == 1){
        if(napelemFor1Count <= 10){
            szum += analogRead(fenyBal);
            napelemFor1Count++;
            napelemStep--;
        }else{
            napelemFor1Count = 1;
        }
    }else if(napelemStep == 2){bal = szum / 10;
    }else if(napelemStep == 3){szum = 0;
    }else if(napelemStep == 4){
        if(napelemFor2Count <= 10){
            szum += analogRead(fenyJobb);
            napelemFor2Count++;
            napelemStep--;
        }else{
            napelemFor2Count = 1;
        }
    }else if(napelemStep == 5){jobb = szum / 10;
    }else if(napelemStep == 6){szum = 0;
    }else if(napelemStep == 7){diff = bal - jobb;
    }else if(napelemStep == 8){if(abs(diff) > halottZona){napelemIf1Side = true;}else{napelemIf1Side = false; napelemStep++;}
    }else if(napelemStep == 9 && napelemIf1Side && napelemIf1SideAStep == 1){
        if(diff > 0){
            napelemIf2Side = true;
            napelemIf1SideAStep++;
            napelemStep--;
        }else{
            napelemIf2Side = false;
            napelemIf1SideAStep++;
            napelemStep--;
        }
    }else if(napelemStep == 9 && napelemIf1Side && napelemIf1SideAStep == 2 && napelemIf2Side){poz += 5; napelemIf1SideAStep++; napelemStep--;
    }else if(napelemStep == 9 && napelemIf1Side && napelemIf1SideAStep == 2 && !napelemIf2Side){poz -= 5; napelemIf1SideAStep++; napelemStep--;
    }else if(napelemStep == 9 && napelemIf1Side && napelemIf1SideAStep == 3){poz = constrain(poz, minPoz, maxPoz); napelemIf1SideAStep++; napelemStep--;
    }else if(napelemStep == 9 && napelemIf1Side && napelemIf1SideAStep == 4){szervo.write(poz); napelemIf1SideAStep = 1;}
    
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
            String s5 = Serial.readStringUntil('\n');
            String s6 = Serial.readStringUntil('\n');

            s1.trim();
            s2.trim();
            s3.trim();
            s4.trim();
            s5.trim();
            s6.trim();

            belsoHomerseklet = s1.toFloat();
            belsoParatartalom = s2.toFloat();
            kulsoHomerseklet = s3.toFloat();
            kulsoParatartalom = s4.toFloat();
            vanFust = (s5 == "1") ? true : false;
            beriaszt = (s6 == "1") ? true : false;
        }
    }
}

void lcdMain(){
    if(lcdStep == 1){lcd.clear();
    }else if(lcdStep == 2){if(vanFust){lcdIf1Side = true;}else{lcdIf1Side = false;}
    }else if(lcdStep == 3 && lcdIf1Side){kellKoviOldal = true;
    }else if(lcdStep == 3 && !lcdIf1Side){kellKoviOldal = false;
    }else if(lcdStep == 4){if(oldal == 1){lcdIf2Side = true;}else{lcdIf2Side = false;}


    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 1){lcd.setCursor(0, 0); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 2){lcd.print("B:");; lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 3){lcd.print(belsoHomerseklet, 1); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 4){lcd.print("C "); lcdStep--; lcdIf2SideAStep++;

    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 5){lcd.print("K:"); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 6){lcd.print(kulsoHomerseklet, 1); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 7){lcd.print("C"); lcdStep--; lcdIf2SideAStep++;

    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 8){lcd.setCursor(0, 1); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 9){lcd.print("BP:"); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 10){lcd.print(belsoParatartalom, 0); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 11){lcd.print("%  "); lcdStep--; lcdIf2SideAStep++;

    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 12){lcd.print("KP:"); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 13){lcd.print(kulsoParatartalom, 0); lcdStep--; lcdIf2SideAStep++;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 14){lcd.print("%"); lcdStep--; lcdIf2SideAStep++;

    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 15){
        if(kellKoviOldal){
            lcdIf3Side = true;
            lcdStep--;
            lcdIf2SideAStep++;
        }else{
            lcdIf3Side = false;
            lcdStep--;
            lcdIf2SideAStep++;
        }
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 16 && lcdIf3Side){oldal = 2; lcdIf2SideAStep = 1; lcdStep--;
    }else if(lcdStep == 5 && lcdIf2Side && lcdIf2SideAStep == 16 && !lcdIf3Side){lcdIf2SideAStep = 1; lcdStep--;
    
    
    }else if(lcdStep == 5 && !lcdIf2Side){
        if(vanFust){
            lcdIf4Side = true;
            lcdStep--;
            lcdIf2SideBStep++;
        }else{
            lcdIf4Side = false;
            lcdStep--;
            lcdIf2SideBStep++;
        }
    }else if(lcdStep == 5 && !lcdIf2Side && lcdIf4Side && lcdIf4SideAStep == 1){
        lcd.setCursor(0, 0);
        lcdStep--;
        lcdIf4SideAStep++;
    }else if(lcdStep == 5 && !lcdIf2Side && lcdIf4Side && lcdIf4SideAStep == 2){
        lcd.print("FUST VESZELY!");
        lcdStep--;
        lcdIf4SideAStep = 1;
    }

    lcdStep++;
}

void ledMain(){
    if(ledStep == 1){if(analogRead(napelemIn) > napelemOutMin){ledIf1Side = true;}else{ledIf1Side = false;}
    }else if(ledStep == 2 && ledIf1Side){digitalWrite(napelemLed, HIGH);
    }else if(ledStep == 2 && !ledIf1Side){digitalWrite(napelemLed, LOW);
    }else if(ledStep == 3){if(analogRead(szeleromuIn) > szeleromuOutMin){ledIf2Side = true;}else{ledIf2Side = false;}
    }else if(ledStep == 4 && ledIf2Side){digitalWrite(szeleromuLed, HIGH);
    }else if(ledStep == 4 && !ledIf2Side){digitalWrite(szeleromuLed, LOW);}

    ledStep++;
}

void vizHomeroMain(){
    if(vizStep == 1){sensors.requestTemperatures();
    }else if(vizStep == 2){vizHomerseklet = sensors.getTempCByIndex(0);
    }else if(vizStep == 3){count++;
    }else if(vizStep == 4){sensors.setUserDataByIndex(0, count);
    }else if(vizStep == 5){int x = sensors.getUserDataByIndex(0);}

    vizStep++;
}

void szivattyuMain(){
    if(szivattyuStep == 1){if(vizHomerseklet > 23 && vizHomerseklet < 28 && kellMelegViz){szivattyuIf1Side = true;}else{szivattyuIf1Side = false;}
    }else if(szivattyuStep == 2 && szivattyuIf1Side){digitalWrite(szivattyuNap, HIGH);
    }else if(szivattyuStep == 2 && !szivattyuIf1Side){digitalWrite(szivattyuNap, LOW);}

    szivattyuStep++;
}

void infraMain(){
    if(infraStep == 1){if(digitalRead(kezmosoInfra) == HIGH){infraIf1Side = true;}else{infraIf1Side = false;}
    }else if(infraStep == 2 && infraIf1Side){kellMelegViz = true;
    }else if(infraStep == 2 && !infraIf1Side){kellMelegViz = false;}

    infraStep++;
}

void melegvizHomeroMain(){
    if(melegvizStep == 1){if(vizHomerseklet < 23){melegvizIf1Side = true;}else{melegvizIf1Side = false;}
    }else if(melegvizStep == 2 && melegvizIf1Side){digitalWrite(napkollektorMelegitoLed, HIGH);
    }else if(melegvizStep == 2 && !melegvizIf1Side){digitalWrite(napkollektorMelegitoLed, LOW);
    }else if(melegvizStep == 3){if(vizHomerseklet > 28){melegvizIf2Side = true;}else{melegvizIf2Side = false;}
    }else if(melegvizStep == 4 && melegvizIf2Side){digitalWrite(napkollektorHutoLed, HIGH);
    }else if(melegvizStep == 4 && !melegvizIf2Side){digitalWrite(napkollektorHutoLed, LOW);}

    melegvizStep++;
}

void riasztasMain(){
    belsoHomerseklet = 0;
    belsoParatartalom = 0;
    kulsoHomerseklet = 0;
    kulsoParatartalom = 0;
    vizHomerseklet = 0; 

    vanFust = false;
    kellMelegViz = false;
    beriaszt = true;

    digitalWrite(napelemLed, LOW);
    digitalWrite(szeleromuLed, LOW);
    digitalWrite(napkollektorHutoLed, LOW);
    digitalWrite(napkollektorMelegitoLed, LOW);
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

    // víz hőmérő
    sensors.begin();

    // pinMode-ok
    pinMode(fenyBal, INPUT);
    pinMode(fenyJobb, INPUT);
    pinMode(napelemIn, INPUT);
    pinMode(szeleromuIn, INPUT);
    pinMode(napelemLed, OUTPUT);
    pinMode(szeleromuLed, OUTPUT);
    pinMode(szivattyuNap, OUTPUT);

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
    vanFust = false;
    beriaszt = false;

    lcdStep = 1; // lcd
    lcdRiasztoStep = 1;
    lcdIf2SideAStep = 1;
    lcdIf4SideAStep = 1;
    oldal = 1;
    kellKoviOldal = false;
    lcdIf1Side = false;
    lcdIf2Side = false;
    lcdIf3Side = false;
    lcdIf4Side = false;

    ledStep = 1; // led
    ledIf1Side = false;
    ledIf2Side = false;

    vizStep = 1; // víz hőmérő
    count = 0;
    vizHomerseklet = 0;

    szivattyuStep = 1; // szivattyú
    szivattyuIf1Side = false;

    infraStep = 1; // kézmosó infra
    infraIf1Side = false;

    melegvizStep = 1; // meleg víz hőmérő
    melegvizIf1Side = false;
    melegvizIf2Side = false;
}

void loop(){
    // mainJelenlegiMillik = millis();

    // 1. NAPELEM
    if(!beriaszt){
        if(napelemStep == 10){napelemStep = 1;}
        napelemMain();
    }

    // 2. SERIAL
    if(!beriaszt){
        serialMain();
    }

    // 3. LCD
    if(!beriaszt){
        if(lcdStep == 6){lcdStep = 1;}
        
        lcdJelenlegiMillik = millis();
        if((lcdJelenlegiMillik - lcdElozoMillik >= lcdIntervallum) && lcdStep == 1){
            lcdElozoMillik = lcdJelenlegiMillik;

            lcdMain();
        }else if(lcdStep > 1){
            lcdMain();
        }
    }else{
        lcdRiasztoJelenlegiMillik = millis();
        
        if(lcdRiasztoStep == 1 && (lcdRiasztoJelenlegiMillik - lcdRiasztoElozoMillik >= lcdIntervallum)){
            lcdRiasztoElozoMillik = lcdRiasztoJelenlegiMillik;
            lcdRiasztoStep++;

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("RIASZTAS");
        }else if(lcdRiasztoStep == 2 && (lcdRiasztoJelenlegiMillik - lcdRiasztoElozoMillik >= lcdIntervallum)){
            lcdRiasztoElozoMillik = lcdRiasztoJelenlegiMillik;
            lcdRiasztoStep = 1;

            lcd.clear();
        }
    }

    // 4. LED
    if(!beriaszt){
        if(ledStep == 5){ledStep = 1;}
        ledMain();
    }

    // 5. VÍZ HŐMÉRŐ
    if(!beriaszt){
        if(vizStep == 6){vizStep = 1;}

        vizJelenlegiMillik = millis();
        if((vizJelenlegiMillik - vizElozoMillik >= vizIntervallum) && vizStep == 1){
            vizElozoMillik = vizJelenlegiMillik;

            vizHomeroMain();
        }else if(vizStep > 1){
            vizHomeroMain();
        }
    }

    // 6. SZIVATTYÚ
    if(szivattyuStep == 3){szivattyuStep = 1;}
    szivattyuMain();

    // 7. KÉZMOSÓ INFRA
    if(!beriaszt){
        if(infraStep == 3){infraStep = 1;}
        infraMain();
    }

    // 8. MELEG VÍZ HŐMÉRŐ
    if(!beriaszt){
        if(melegvizStep == 5){melegvizStep = 1;}
        melegvizHomeroMain();
    }

    // 9. RIASZTÁS
    if(beriaszt){riasztasMain();}

    // Serial.println("Futasi ido: ");
    // Serial.print(mainJelenlegiMillik - mainElozoMillik);
    // mainElozoMillik = mainJelenlegiMillik;
}