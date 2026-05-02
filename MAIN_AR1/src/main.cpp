// KÖNYVTÁRAK
#include <Arduino.h>
#include <DHT.h>
#include <Servo.h>

// DEFINÍCIÓK
#define homeroBelsoPin A4
#define homeroBelsoTipus DHT11
#define homeroKulsoPin A5
#define homeroKulsoTipus DHT11

// DHT
DHT belsoHomero(homeroBelsoPin, homeroBelsoTipus);
DHT kulsoHomero(homeroKulsoPin, homeroKulsoTipus);

// SZERVÓ
Servo ablak;

// ESZKÖZÖK
const int klima = 4;
const int kandallo = 2;
const int hangszoro = 5;

// VÁLTOZÓK
// hőmérők
int homerokStep = 1;
unsigned long homerokElozoMillik = 0;
unsigned long homerokJelenlegiMillik = 0;
const long homerokIntervallum = 2000;

// hőmérséklet kezelés
int kezelesStep = 1;
int rendszerMod = 0;
boolean kezelesIf1Side = false; // false = sideB
boolean kezelesIf2Side = false; // false = sideB
boolean kezelesIf3Side = false; // false = sideB

// kandalló
int kandalloStep = 1;
boolean kandalloSide = false; // false = sideB

// ablak
int ablakStep = 1;
int ablakFor1Count = 1;
int ablakFor2Count = 1;
int poz = 0;
boolean ablakNyitva = false; // false = becsukva
boolean ablakSide = false; // false = sideB

// klíma
int klimaStep = 1;
boolean klimaSide = false; // false = sideB

// serial
unsigned long serialElozoMillik = 0;
unsigned long serialJelenlegiMillik = 0;
const long serialIntervallum = 3000;

unsigned long mainElozoMillik = 0;
unsigned long mainJelenlegiMillik = 0;

float belsoHomerseklet = 0;
float belsoParatartalom = 0;
float kulsoHomerseklet = 0;
float kulsoParatartalom = 0;

const uint16_t dallam[] = {
  392, 440, 494, 494,
  440, 440, 494, 392, 440,

  494, 494, 440, 440, 494, 
  392, 294,

  392, 440, 494, 494,
  440, 440, 494, 392, 370, 330,

  440, 440, 494, 392, 370,
  330, 330
};

const int idohossz[] = {
  4,4,4,4,
  4,8,8,4,4,

  8,2,4,8,8,
  2,2,

  4,4,8,2,
  4,8,8,8,8,4,

  8,4,8,4,4,
  2,2
};

// FÜGGVÉNYEK
void zeneLejatszas(const uint16_t dallam[], const int idohossz[], int size, int tempoAlap, float huzas) {
  for (int i = 0; i < size; i++) {
    int hangHossz = tempoAlap / idohossz[i];
    tone(hangszoro, dallam[i], hangHossz);
    delay((int)(hangHossz * huzas));
    noTone(hangszoro);
  }
}

void homerokMain(const int homerokStepFv){
  if(homerokStepFv == 1){
    float temp = belsoHomero.readTemperature();
    if(!isnan(temp)) belsoHomerseklet = temp;
  }else if(homerokStepFv == 2){
    float temp = belsoHomero.readHumidity();
    if(!isnan(temp)) belsoParatartalom = temp;
  }else if(homerokStepFv == 3){
    float temp = kulsoHomero.readTemperature();
    if(!isnan(temp)) kulsoHomerseklet = temp;
  }else if(homerokStepFv == 4){
    float temp = kulsoHomero.readHumidity();
    if(!isnan(temp)) kulsoParatartalom = temp;}

  homerokStep++;
}

void homersekletKezelesMain(const int kezelesStepFv, const boolean kezelesIf1SideFv, const boolean kezelesIf2SideFv, const boolean kezelesIf3SideFv){
  if(kezelesStepFv == 1){
    if(belsoHomerseklet < 22){
      kezelesIf1Side = true; // kandalló
      kezelesIf2Side = false;
      kezelesIf3Side = false;
    }
    else if(belsoHomerseklet >= 26){
      kezelesIf1Side = false; // klíma
      kezelesIf2Side = true;
      kezelesIf3Side = false;
    }
    else if(belsoHomerseklet > 24 && kulsoHomerseklet < belsoHomerseklet){
      kezelesIf1Side = false; // ablak
      kezelesIf2Side = false;
      kezelesIf3Side = true;
    }
    else if(belsoHomerseklet > 24 && kulsoHomerseklet >= belsoHomerseklet){
      kezelesIf1Side = false; // klíma
      kezelesIf2Side = true;
      kezelesIf3Side = false;
    }
    else{
      kezelesIf1Side = false;
      kezelesIf2Side = false;
      kezelesIf3Side = false;
    }
  }else if(kezelesStepFv == 2 && kezelesIf1SideFv){rendszerMod = 1; // kandalló
  }else if(kezelesStepFv == 2 && kezelesIf2SideFv){rendszerMod = 3; // klíma
  }else if(kezelesStepFv == 2 && kezelesIf3SideFv){rendszerMod = 2; // ablak
  }else if(kezelesStepFv == 2){rendszerMod = 0;}

  kezelesStep++;
}

void kandalloMain(const int kandalloStepFv, const boolean kandalloSideFv){
  if(kandalloStepFv == 1){if(rendszerMod == 1){kandalloSide = true;}else{kandalloSide = false;}
  }else if(kandalloStepFv == 2 && kandalloSideFv == true){digitalWrite(kandallo, HIGH);
  }else if(kandalloStepFv == 2 && kandalloSideFv == false){digitalWrite(kandallo, LOW);}

  kandalloStep++;
}

void ablakMain(const int ablakStepFv, const boolean ablakSideFv, const int ablakFor1CountFv, const int ablakFor2CountFv){
  if(ablakStepFv == 1){
    if(rendszerMod == 2 && !ablakNyitva){
      ablakSide = true;
    }else if(rendszerMod != 2 && ablakNyitva){
      ablakSide = false;
    }else{
      ablakStep--;
    }
  }else if(ablakStepFv == 2 && ablakSideFv){
    if(ablakFor1CountFv <= 12){
      poz += 7;
      ablakFor1Count++;
      ablakStep--;
    }else{
      ablakNyitva = true;
      ablakFor1Count = 1;
    }
    ablak.write(poz);
  }else if(ablakStepFv == 2 && !ablakSideFv){
    if(ablakFor2CountFv <= 12){
      poz -= 7;
      ablakFor2Count++;
      ablakStep--;
    }else{
      ablakNyitva = false;
      ablakFor2Count = 1;
    }
    ablak.write(poz);
  }

  ablakStep++;
}

void klimaMain(const int klimaStepFv, const boolean klimaSideFv){
  if(klimaStepFv == 1){if(rendszerMod == 3){klimaSide = true;}else{klimaSide = false;}
  }else if(klimaStepFv == 2 && klimaSideFv == true){digitalWrite(klima, HIGH);
  }else if(klimaStepFv == 2 && klimaSideFv == false){digitalWrite(klima, LOW);}

  klimaStep++;
}

void serialMain(){
  Serial.println("a");
  Serial.println(belsoHomerseklet);
  Serial.println(belsoParatartalom);
  Serial.println(kulsoHomerseklet);
  Serial.println(kulsoParatartalom);
}

void setup(){
  // serial indítása
  Serial.begin(9600);

  // dht
  belsoHomero.begin();
  kulsoHomero.begin();

  // szervó
  ablak.attach(3);
  ablak.write(0);

  // pinMode-ok
  pinMode(klima, OUTPUT);
  pinMode(kandallo, OUTPUT);
  pinMode(hangszoro, OUTPUT);

  // szenzorok végig futtatása
  for (int i = 0; i < 4; i++) {homerokMain(homerokStep);} // 1. hőmérők
  for (int i = 0; i < 2; i++) {homersekletKezelesMain(kezelesStep, kezelesIf1Side, kezelesIf2Side, kezelesIf3Side);} // 2. hőmérséklet kezelés
  serialMain(); // 3. SERIAL

  // változók
  homerokStep = 1; // hőmérők

  kezelesStep = 1; // hőmérséklet kezelés
  kezelesIf1Side = false;
  kezelesIf2Side = false;
  kezelesIf3Side = false;

  kandalloStep = 1; // kandalló
  kandalloSide = false;

  ablakStep = 1; // ablak
  ablakFor1Count = 1;
  ablakFor2Count = 1;
  poz = 0;
  ablakNyitva = false;
  ablakSide = false;

  klimaStep = 1; // klíma
  klimaSide = false;

  // bekapcsoló zene
  zeneLejatszas(
    dallam,
    idohossz,
    sizeof(dallam) / sizeof(dallam[0]),
    1000,
    1.30
  );
}

void loop(){
  mainJelenlegiMillik = millis();

  // 1. HŐMÉRŐK
  if(homerokStep == 5){homerokStep = 1;}

  homerokJelenlegiMillik = millis();
  if((homerokJelenlegiMillik - homerokElozoMillik >= homerokIntervallum) && homerokStep == 1){
    homerokElozoMillik = homerokJelenlegiMillik;

    homerokMain(homerokStep);
  }else if(homerokStep > 1){
    homerokMain(homerokStep);
  }

  // 2. HŐMÉRSÉKLET KEZELÉS
  if(kezelesStep == 4){kezelesStep = 1;}
  homersekletKezelesMain(kezelesStep, kezelesIf1Side, kezelesIf2Side, kezelesIf3Side);

  // 3. KANDALLÓ
  if(kandalloStep == 3){kandalloStep = 1;}
  kandalloMain(kandalloStep, kandalloSide);

  // 4. ABLAK
  if(ablakStep == 3){ablakStep = 1;}
  ablakMain(ablakStep, ablakSide, ablakFor1Count, ablakFor2Count);

  // 5. KLÍMA
  if(klimaStep == 3){
    klimaStep = 1;
  }
  klimaMain(klimaStep, klimaSide);

  // 6. SERIAL
  serialJelenlegiMillik = millis();
  if((serialJelenlegiMillik - serialElozoMillik >= serialIntervallum)){
    serialElozoMillik = serialJelenlegiMillik;

    serialMain();
  }

  // csak tesztelésre
  //Serial.println("Futasi ido: ");
  //Serial.print(mainJelenlegiMillik - mainElozoMillik);
  
  mainElozoMillik = mainJelenlegiMillik;
}