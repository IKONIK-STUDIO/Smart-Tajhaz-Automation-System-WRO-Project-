// több lehetséges indito dallam a hangszórón

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
const int vizszintSzenzor = A2;
const int vilagitas = 12;
const int szivattyu = 7;
const int wcGomb = A3;
const int mozgaserzekelo = 13;
const int klima = 4;
const int kandallo = 2;
const int fusterzekelo = 6;
const int fenyerzekelo = A1;
const int hangszoro = 5;
const int paraelszivoRele = 11;
const int demoMod = A0;
const int laser1 = 8;
const int laser2 = 9;
const int laser3 = 10;

// VÁLTOZÓK
// hőmérők
int homerokStep = 1;
unsigned long homerokElozoMillik = 0;
unsigned long homerokJelenlegiMillik = 0;
const long homerokIntervallum = 2000;

// hőmérséklet kezelés
int kezelesStep = 1;
int rendszerMod = 0;
boolean kezelesIf1Side = false; // kandalló
boolean kezelesIf2Side = false; // klíma
boolean kezelesIf3Side = false; // ablak

// WC gomb
int wcStep = 1;
boolean wcSide = false; // false = sideB

// vízszint
int vizszintStep = 1;
boolean vizszintSide = false; // false = sideB

// füstérzékelő
int fusterzekeloStep = 1;
boolean fusterzekeloSide = false; // false = sideB

// mozgásérzékelő
int mozgaserzekeloStep = 1;
boolean mozgaserzekeloSide = false; // false = sideB

// fényérzékelő
int fenyerzekeloStep = 1;
boolean fenyerzekeloSide = false; // false = sideB

// szivattyú
int szivattyuStep = 1;
boolean szivattyuSide = false; // false = sideB

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

// világítás
int vilagitasStep = 1;
boolean vilagitasSide = false; // false = sideB

// páraelszívó
int paraelszivoStep = 1;
boolean paraelszivoSide = false; // false = sideB

// laser
int laserStep = 1;
boolean laserSide1 = false; // false = sideB
boolean laserSide2 = false; // false = sideB
boolean laserSide3 = false; // false = sideB

// serial
unsigned long serialElozoMillik = 0;
unsigned long serialJelenlegiMillik = 0;
const long serialIntervallum = 3000;

// hangszóró riasztás
int hangszoroStep = 1;
unsigned long hangszoroElozoMillik = 0;
unsigned long hangszoroJelenlegiMillik = 0;
const long hangszoroIntervallum = 1000;

// demo mód
int demoModStep = 1;
int demoModAktivStep = 1; // 3 a maximum
boolean demoModSide = false; // false = sideB
unsigned long demoModElozoMillik = 0;
unsigned long demoModJelenlegiMillik = 0;
const long demoModIntervallum = 4000;

unsigned long mainElozoMillik = 0;
unsigned long mainJelenlegiMillik = 0;

float belsoHomerseklet = 0;
float belsoParatartalom = 0;
float kulsoHomerseklet = 0;
float kulsoParatartalom = 0;

boolean elegViz = false;
boolean kevesFeny = false;
boolean mozgas = false;
boolean wcGombLenyomva = false;
boolean vanFust = false;
boolean demoModAktiv = false;
boolean beriaszt = false;

const uint16_t dallam1[] = {
  392, 440, 494, 494,
  440, 440, 494, 392, 440,

  494, 494, 440, 440, 494, 
  392, 294,

  392, 440, 494, 494,
  440, 440, 494, 392, 370, 330,

  440, 440, 494, 392, 370,
  330, 330
};

const int idohossz1[] = {
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

void homerokMain(){
  if(homerokStep == 1){
    float temp = belsoHomero.readTemperature();
    if(!isnan(temp)) belsoHomerseklet = temp;
  }else if(homerokStep == 2){
    float temp = belsoHomero.readHumidity();
    if(!isnan(temp)) belsoParatartalom = temp;
  }else if(homerokStep == 3){
    float temp = kulsoHomero.readTemperature();
    if(!isnan(temp)) kulsoHomerseklet = temp;
  }else if(homerokStep == 4){
    float temp = kulsoHomero.readHumidity();
    if(!isnan(temp)) kulsoParatartalom = temp;}

  homerokStep++;
}

void homersekletKezelesMain(){
  if(kezelesStep == 1){
    if(vanFust){
      kezelesIf1Side = false;
      kezelesIf2Side = false;
      kezelesIf3Side = false;
    }else if(belsoHomerseklet < 22){
      kezelesIf1Side = true;
      kezelesIf2Side = false;
      kezelesIf3Side = false;
    }else if(belsoHomerseklet >= 26){
      kezelesIf1Side = false;
      kezelesIf2Side = true;
      kezelesIf3Side = false;
    }else if(belsoHomerseklet > 24 && kulsoHomerseklet < belsoHomerseklet){
      kezelesIf1Side = false;
      kezelesIf2Side = false;
      kezelesIf3Side = true;
    }else{
      kezelesIf1Side = false;
      kezelesIf2Side = false;
      kezelesIf3Side = false;
    }
  }
  else if(kezelesStep == 2 && kezelesIf1Side){rendszerMod = 1;} // kandalló
  else if(kezelesStep == 2 && kezelesIf2Side){rendszerMod = 3;} // klíma
  else if(kezelesStep == 2 && kezelesIf3Side){rendszerMod = 2;} // ablak
  else if(kezelesStep == 2){rendszerMod = 0;}

  kezelesStep++;
}

void wcMain(){
  if(wcStep == 1){if(digitalRead(wcGomb) == HIGH){wcSide = true;}else{wcSide = false;}
  }else if(wcStep == 2 && wcSide == true){wcGombLenyomva = true;
  }else if(wcStep == 2 && wcSide == false){wcGombLenyomva = false;}

  wcStep++;
}

void vizszintMain(){
  if(vizszintStep == 1){if(analogRead(vizszintSzenzor) > 500){vizszintSide = true;}else{vizszintSide = false;}
  }else if(vizszintStep == 2 && vizszintSide == true){elegViz = true;
  }else if(vizszintStep == 2 && vizszintSide == false){elegViz = false;}

  vizszintStep++;
}

void fusterzekeloMain(){
  if(fusterzekeloStep == 1){if(digitalRead(fusterzekelo) == LOW){fusterzekeloSide = true;}else{fusterzekeloSide = false;}
  }else if(fusterzekeloStep == 2 && fusterzekeloSide == true){vanFust = true;
  }else if(fusterzekeloStep == 2 && fusterzekeloSide == false){vanFust = false;}

  fusterzekeloStep++;
}

void mozgaserzekeloMain(){
  if(mozgaserzekeloStep == 1){if(digitalRead(mozgaserzekelo)){mozgaserzekeloSide = true;}else{mozgaserzekeloSide = false;}
  }else if(mozgaserzekeloStep == 2 && mozgaserzekeloSide == true){mozgas = true;
  }else if(mozgaserzekeloStep == 2 && mozgaserzekeloSide == false){mozgas = false;}

  mozgaserzekeloStep++;
}

void fenyerzekeloMain(){
  if(fenyerzekeloStep == 1){if(analogRead(fenyerzekelo) > 500){fenyerzekeloSide = true;}else{fenyerzekeloSide = false;}
  }else if(fenyerzekeloStep == 2 && fenyerzekeloSide == true){kevesFeny = true;
  }else if(fenyerzekeloStep == 2 && fenyerzekeloSide == false){kevesFeny = false;}

  fenyerzekeloStep++;
}

void szivattyuMain(){
  if(szivattyuStep == 1){if(elegViz && wcGombLenyomva){szivattyuSide = true;}else{szivattyuSide = false;}
  }else if(szivattyuStep == 2 && szivattyuSide == true){digitalWrite(szivattyu, HIGH);
  }else if(szivattyuStep == 2 && szivattyuSide == false){digitalWrite(szivattyu, LOW);}

  szivattyuStep++;
}

void kandalloMain(){
  if(kandalloStep == 1){if(rendszerMod == 1){kandalloSide = true;}else{kandalloSide = false;}
  }else if(kandalloStep == 2 && kandalloSide == true){digitalWrite(kandallo, HIGH);
  }else if(kandalloStep == 2 && kandalloSide == false){digitalWrite(kandallo, LOW);}

  kandalloStep++;
}

void ablakMain(){
  if(ablakStep == 1){
    if(rendszerMod == 2 && !ablakNyitva){
      ablakSide = true;
    }else if(rendszerMod != 2 && ablakNyitva){
      ablakSide = false;
    }else{
      ablakStep--;
    }
  }else if(ablakStep == 2 && ablakSide){
    if(ablakFor1Count <= 12){
      poz += 7;
      ablakFor1Count++;
      ablakStep--;
    }else{
      ablakNyitva = true;
      ablakFor1Count = 1;
    }
    ablak.write(poz);
  }else if(ablakStep == 2 && !ablakSide){
    if(ablakFor2Count <= 12){
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

void klimaMain(){
  if(klimaStep == 1){if(rendszerMod == 3){klimaSide = true;}else{klimaSide = false;}
  }else if(klimaStep == 2 && klimaSide == true){digitalWrite(klima, HIGH);
  }else if(klimaStep == 2 && klimaSide == false){digitalWrite(klima, LOW);}

  klimaStep++;
}

void vilagitasMain(){
  if(vilagitasStep == 1){if(kevesFeny && mozgas){vilagitasSide = true;}else{vilagitasSide = false;}
  }else if(vilagitasStep == 2 && vilagitasSide == true){digitalWrite(vilagitas, HIGH);
  }else if(vilagitasStep == 2 && vilagitasSide == false){digitalWrite(vilagitas, LOW);}

  vilagitasStep++;
}

void paraelszivoMain(){
  if(paraelszivoStep == 1){if(belsoParatartalom > 30){paraelszivoSide = true;}else{paraelszivoSide = false;}
  }else if(paraelszivoStep == 2 && paraelszivoSide == true){digitalWrite(paraelszivoRele, HIGH);
  }else if(paraelszivoStep == 2 && paraelszivoSide == false){digitalWrite(paraelszivoRele, LOW);}

  paraelszivoStep++;
}

void laserMain(){
  if(laserStep == 1){if(digitalRead(laser1) == LOW){laserSide1 = true;}else{laserSide1 = false;}
  }else if(laserStep == 2 && !beriaszt && laserSide1){beriaszt = true;
  }else if(laserStep == 2 && !beriaszt && !laserSide1){beriaszt = false;
  }else if(laserStep == 3){if(digitalRead(laser2) == LOW){laserSide2 = true;}else{laserSide2 = false;}
  }else if(laserStep == 4 && !beriaszt && laserSide2){beriaszt = true;
  }else if(laserStep == 4 && !beriaszt && !laserSide2){beriaszt = false;
  }else if(laserStep == 5){if(digitalRead(laser3) == LOW){laserSide3 = true;}else{laserSide3 = false;}
  }else if(laserStep == 6 && !beriaszt && laserSide3){beriaszt = true;
  }else if(laserStep == 6 && !beriaszt && !laserSide3){beriaszt = false;}

  laserStep++;
}

void serialMain(){
  Serial.println("a");
  Serial.println(belsoHomerseklet);
  Serial.println(belsoParatartalom);
  Serial.println(kulsoHomerseklet);
  Serial.println(kulsoParatartalom);
  Serial.println(vanFust);
  Serial.println(beriaszt);
}

void demoModMain(){
  if(demoModStep == 1){if(digitalRead(demoMod) == HIGH){demoModSide = true;}else{demoModSide = false;}
  }else if(demoModStep == 2 && demoModSide){demoModAktiv = true;
  }else if(demoModStep == 2 && !demoModSide){demoModAktiv = false;}

  demoModStep++;
}

void riasztasMain(){
  rendszerMod = 1;

  belsoHomerseklet = 0;
  belsoParatartalom = 0;
  kulsoHomerseklet = 0;
  kulsoParatartalom = 0;

  elegViz = false;
  kevesFeny = false;
  mozgas = false;
  wcGombLenyomva = false;
  vanFust = false;
  demoModAktiv = false;
  beriaszt = true;
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
  pinMode(vilagitas, OUTPUT);
  pinMode(szivattyu, OUTPUT);
  pinMode(hangszoro, OUTPUT);

  pinMode(vizszintSzenzor, INPUT);
  pinMode(wcGomb, INPUT);
  pinMode(mozgaserzekelo, INPUT);
  pinMode(fusterzekelo, INPUT);
  pinMode(fenyerzekelo, INPUT);

  // szenzorok végig futtatása
  for (int i = 0; i < 4; i++) {homerokMain();}            // 1. hőmérők
  for (int i = 0; i < 2; i++) {homersekletKezelesMain();} // 2. hőmérséklet kezelés
  for (int i = 0; i < 2; i++) {wcMain();}                 // 3. WC gomb
  for (int i = 0; i < 2; i++) {vizszintMain();}           // 4. vízszint
  for (int i = 0; i < 2; i++) {fusterzekeloMain();}       // 5. füstérzékelő
  for (int i = 0; i < 2; i++) {mozgaserzekeloMain();}     // 6. mozgásérzékelő
  for (int i = 0; i < 2; i++) {fenyerzekeloMain();}       // 7. fényérzékelő
  serialMain();                                           // 8. SERIAL

  // változók
  homerokStep = 1; // hőmérők

  kezelesStep = 1; // hőmérséklet kezelés
  kezelesIf1Side = false;
  kezelesIf2Side = false;
  kezelesIf3Side = false;
  
  wcStep = 1; // WC gomb
  wcSide = false;

  vizszintStep = 1; // vízszint
  vizszintSide = false;

  fusterzekeloStep = 1; // füstérzékelő
  fusterzekeloSide = false;

  mozgaserzekeloStep = 1; // mozgásérzékelő
  mozgaserzekeloSide = false;

  fenyerzekeloStep = 1; // fényérzékelő
  fenyerzekeloSide = false;

  szivattyuStep = 1; // szivattyú
  szivattyuSide = false;

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

  vilagitasStep = 1; // világítás
  vilagitasSide = false;

  paraelszivoStep = 1; // páraelszívó
  paraelszivoSide = false;

  laserStep = 1; // laser
  laserSide1 = false;
  laserSide2 = false;
  laserSide3 = false;

  demoModStep = 1; // demo mód
  demoModAktivStep = 1;
  demoModSide = false;
  demoModAktiv = false;

  // bekapcsoló zene
  zeneLejatszas(
    dallam1,
    idohossz1,
    sizeof(dallam1) / sizeof(dallam1[0]),
    1000,
    1.30
  );
}

void loop(){
  mainJelenlegiMillik = millis();

  // 1. HŐMÉRŐK
  if(!beriaszt){
    if(homerokStep == 5){homerokStep = 1;}

    homerokJelenlegiMillik = millis();
    if((homerokJelenlegiMillik - homerokElozoMillik >= homerokIntervallum) && homerokStep == 1){
      homerokElozoMillik = homerokJelenlegiMillik;

      homerokMain();
    }else if(homerokStep > 1){
      homerokMain();
    }
  }

  // 2. HŐMÉRSÉKLET KEZELÉS
  if(!demoModAktiv && !beriaszt){
    if(kezelesStep == 4){kezelesStep = 1;}
    homersekletKezelesMain();
  }
  
  // 3. WC GOMB
  if(!beriaszt){
    if(wcStep == 3){wcStep = 1;}
    wcMain();
  }

  // 4. VÍZSZINT
  if(!beriaszt){
    if(vizszintStep == 3){vizszintStep = 1;}
    vizszintMain();
  }

  // 5. FÜSTÉRZÉKELŐ
  if(!beriaszt){
    if(fusterzekeloStep == 3){fusterzekeloStep = 1;}
    fusterzekeloMain();
  }

  // 6. MOZGÁSÉRZÉKELŐ
  if(!beriaszt){
    if(mozgaserzekeloStep == 3){mozgaserzekeloStep = 1;}
    mozgaserzekeloMain();
  }

  // 7. FÉNYÉRZÉKELŐ
  if(!beriaszt){
    if(fenyerzekeloStep == 3){fenyerzekeloStep = 1;}
    fenyerzekeloMain();
  }

  // 8. SZIVATTYÚ
  if(szivattyuStep == 3){szivattyuStep = 1;}
  szivattyuMain();

  // 9. KANDALLÓ
  if(kandalloStep == 3){kandalloStep = 1;}
  kandalloMain();

  // 10. ABLAK
  if(ablakStep == 3){ablakStep = 1;}
  ablakMain();

  // 11. KLÍMA
  if(klimaStep == 3){klimaStep = 1;}
  klimaMain();

  // 12. VILÁGÍTÁS
  if(vilagitasStep == 3){vilagitasStep = 1;}
  vilagitasMain();

  // 13. PÁRAELSZÍVÓ
  if(paraelszivoStep == 3){paraelszivoStep = 1;}
  paraelszivoMain();

  // 14. LASER
  if(!beriaszt){
    if(laserStep == 7){laserStep = 1;}
    laserMain();
  }

  // 15. SERIAL
  serialJelenlegiMillik = millis();
  if((serialJelenlegiMillik - serialElozoMillik >= serialIntervallum)){
    serialElozoMillik = serialJelenlegiMillik;

    serialMain();
  }

  // 16. HANGSZÓRÓ
  if(beriaszt){
    hangszoroJelenlegiMillik = millis();
    
    if(hangszoroStep == 1 && (hangszoroJelenlegiMillik - hangszoroElozoMillik >= hangszoroIntervallum)){
      hangszoroElozoMillik = hangszoroJelenlegiMillik;
      hangszoroStep++;

      noTone(hangszoro);
      tone(hangszoro, 700);
    }else if(hangszoroStep == 2 && (hangszoroJelenlegiMillik - hangszoroElozoMillik >= hangszoroIntervallum)){
      hangszoroElozoMillik = hangszoroJelenlegiMillik;
      hangszoroStep = 1;

      noTone(hangszoro);
      tone(hangszoro, 900);
    }
  }

  // 17. DEMO MÓD
  if(!beriaszt){
    if(!demoModAktiv){
      if(demoModStep == 3){demoModStep = 1;}
      demoModMain();
    }else{
      demoModJelenlegiMillik = millis();

      if(demoModAktivStep == 1){
        demoModElozoMillik = demoModJelenlegiMillik;
        rendszerMod = 1;
      }else if(demoModAktivStep == 2 && (demoModJelenlegiMillik - demoModElozoMillik >= demoModIntervallum)){
        demoModElozoMillik = demoModJelenlegiMillik;
        rendszerMod = 2;
      }else if(demoModAktivStep == 3 && (demoModJelenlegiMillik - demoModElozoMillik >= demoModIntervallum)){
        demoModElozoMillik = demoModJelenlegiMillik;
        rendszerMod = 3;
      }else if(demoModAktivStep == 4 && (demoModJelenlegiMillik - demoModElozoMillik >= demoModIntervallum)){
        demoModAktivStep = 0;
        demoModAktiv = false;
        rendszerMod = 0;
      }

      demoModAktivStep++;
    }
  }

  // 18. RIASZTÁS
  if(beriaszt){riasztasMain();}
  
  // csak tesztelésre
  //Serial.println("Futasi ido: ");
  //Serial.print(mainJelenlegiMillik - mainElozoMillik);
  
  mainElozoMillik = mainJelenlegiMillik;
}