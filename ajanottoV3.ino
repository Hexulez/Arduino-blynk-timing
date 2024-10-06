/* Koodissa on jonkin verran kohtia mitä olisi voinut funktioiden avulla kierrättää eikä olisi tarvinut olla niin paljoa samaa tekstiä, mutta se toimii ja on melko luettavan näköistä jotenka kelpaa minulle */
//blynkin projektin tiedot
#define BLYNK_TEMPLATE_ID "TMPL4_zFtp-aI"
#define BLYNK_TEMPLATE_NAME "SalaisiaAsioita"
#define BLYNK_AUTH_TOKEN "esXlcGXWQJBrXpgVQUbVAF7RUkxZglzV"

//kutsutaan kirjastot
#include <SPI.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

// Blynk-yhteyden tiedot
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Hacknet";
char pass[] = "KailaVaiEi";
//char ssid[] = "4G-Gateway-E6F8";
//char pass[] = "EE5NTMT4TGB";

BlynkTimer ajastin;

// LED- ja nappipinnit sijoitettu muuttujaan jotta helpompi erottaa toisistaan
const int redPin = 5;      // Punainen LED (pin 5)
const int yellowPin = 4;   // Keltainen LED (pin 4)
const int greenPin = 3;    // Vihreä LED (pin 3)
const int summeri = 2;     // Summeri (pin 2)
const int nappi = 1;       // Nappi (pin 1)

unsigned long startTime = 0;  // Muuttuja ajanoton aloitusajalle
unsigned long recordTime = 0; // Muuttuja ennätysajalle
bool timerRunning = false;    // Tila muuttuja ajanotolle
bool newRecord = false;       // Uuden ennätyksen tila

void setup() {
  // Määritellään LEDit ja summeri ulostuloiksi
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(summeri, OUTPUT);
  pinMode(nappi, INPUT_PULLDOWN);  // Nappi määritelty vetämään maahan

  // Alustetaan kaikki LEDit ja summeri pois päältä
  digitalWrite(redPin, 0);
  digitalWrite(yellowPin, 0);
  digitalWrite(greenPin, 0);
  digitalWrite(summeri, 0);

  // Alustetaan Blynk-yhteys
  Blynk.begin(auth, ssid, pass);

  // Asetetaan ajastin ajan reaaliaikaiseen näyttämiseen 100 ms välein
  ajastin.setInterval(100, displayTime);
  //tyhjennetään ennätyksen tiedot uudelleen käynnistyksestä
  Blynk.virtualWrite(V3, 0);
}

void loop() {
  Blynk.run();
  ajastin.run();

  // Tarkistetaan onko nappi painettu, aloittaen lähtölaskenta tai pysäyttäen ajanoton
  if (digitalRead(nappi) == 1 && !timerRunning) {
  StartingCountdown();  // Aloitetaan lähtölaskenta
  } else if (digitalRead(nappi) == 1 && timerRunning) {
    stopTimer();  // Pysäytetään ajanotto
  }
}

// Lähtölaskenta LEDien ja summerin avulla
void StartingCountdown() {
  // Punainen LED ja ääni
  digitalWrite(redPin, 1);
  CountdownSounds(1);
  delay(1000);  // 1 sekunnin viive
  digitalWrite(redPin, 0);

  // Keltainen LED ja ääni
  digitalWrite(yellowPin, 1);
  CountdownSounds(1);
  delay(1000);  // 1 sekunnin viive
  digitalWrite(yellowPin, 0);

  // Vihreä LED ja ääni
  digitalWrite(greenPin, 1);
  CountdownSounds(1);
  delay(1000);  // 1 sekunnin viive
  digitalWrite(greenPin, 0);

  // Satunnainen viive ennen starttia
  delay(random(1000, 2000));

  // Summeri soi merkiksi ja ajanotto alkaa
  //digitalWrite(summeri, 1);
  digitalWrite(redPin, 1);
  digitalWrite(yellowPin, 1);
  digitalWrite(greenPin, 1);
  CountdownSounds(0);
  
  delay(500);
  //digitalWrite(summeri, 0);
  digitalWrite(redPin, 0);
  digitalWrite(yellowPin, 0);
  digitalWrite(greenPin, 0);

  startTimer();  // Aloitetaan ajanotto
}

// Laskennan äänimerkki
void CountdownSounds(int kerta) {
  if (kerta == 1) {
    //tone kutsuu summerin jossa annetaan ensin taajuus ja sitten aika kuinka kauan soi
    tone(summeri, 500, 250);  // Lyhyt piippaus
  } else {
    tone(summeri, 1000, 500);  // Pidempi piippaus
  }
}

// Ajanoton aloitus
void startTimer() {
  startTime = millis();  // Tallennetaan aloitusaika
  timerRunning = true;   // Ajanotto käynnissä
}

// Ajanoton pysäytys
void stopTimer() {
  unsigned long elapsedTime = millis() - startTime;  // Lasketaan kulunut aika
  timerRunning = false;  // Ajanotto pysäytetty

  // Lasketaan aika sekunneissa ja näytetään se Blynkissä
  double timeInSeconds = elapsedTime / 1000.0;
  Blynk.virtualWrite(V2, timeInSeconds);

  // Tarkistetaan onko uusi ennätys
  if (recordTime == 0 || elapsedTime < recordTime) {
    recordTime = elapsedTime;  // record time on long minkä takia se ei toiminut oikein Blynkissä
    double feed = elapsedTime / 1000.0;   //purkkapatentti jota en uskalla enää poistaa koska ei ole enää laitetta millä testata 
    //newRecord = true;
    Blynk.virtualWrite(V3, feed); //kirjoitetaan purkkapatenttina tehty arvo parhaaksi ajaksi
    flashNewRecord();  // Näytetään uusi ennätys LEDien avulla
  }
  else{
    flashFinnish();
  }

  // Palautetaan laite lähtötilaan
  resetToIdle();
}

// Näytetään reaaliaikainen kulunut aika
void displayTime() {
  if (timerRunning) {
    unsigned long elapsedTime = millis() - startTime;
    double timeInSeconds = elapsedTime / 1000.0;
    Blynk.virtualWrite(V2, timeInSeconds);  // Näytetään reaaliaikainen aika Blynkissä
  }
}

// Vilkutetaan LEDejä uuden ennätyksen kunniaksi
void flashNewRecord() {
  for (int i = 0; i < 15; i++) {
    int ledColor = random(3,5);
    //turha if else lauseke joka vilkuttaa satunnaisesti valoja.
    if (ledColor == 3){
      digitalWrite(redPin, 1);
    }
    else if (ledColor == 4){
      digitalWrite(yellowPin, 1);
    }
    else if (ledColor == 5){
    digitalWrite(greenPin, 1);
    //digitalWrite(ledColor, 1);
    }
    delay(100);
    //alustaa ledit uuteen kierrokseen
    digitalWrite(redPin, 0);
    digitalWrite(yellowPin, 0);
    digitalWrite(greenPin, 0);
    delay(100);
  }
  

}

//Vilkutetaan ledejä jos ei saatu uutta ennätystä
void flashFinnish(){
  for (int i = 0; i < 5; i++) {
    digitalWrite(redPin, 1);
    digitalWrite(yellowPin, 1);
    digitalWrite(greenPin, 1);
    delay(100);
    digitalWrite(redPin, 0);
    digitalWrite(yellowPin, 0);
    digitalWrite(greenPin, 0);
    delay(100);
  }
}

// Palautetaan laite odottamaan seuraavaa juoksua
void resetToIdle() {
  digitalWrite(redPin, 0);
  digitalWrite(yellowPin, 0);
  digitalWrite(greenPin, 0);
  digitalWrite(summeri, 0);
  //newRecord = false;
}



