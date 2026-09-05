#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

// ===============================
// BROCHES DU RC522
// ===============================
#define SS_PIN 10
#define RST_PIN 9

// ===============================
// LEDS ET BUZZER
// ===============================
#define LED_VERTE 2
#define LED_ROUGE A0
#define BUZZER A1

// ===============================
// LCD 16x2 (RS, E, D4, D5, D6, D7)
// ===============================
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

// ===============================
// CREATION DU LECTEUR RFID
// ===============================
MFRC522 rfid(SS_PIN, RST_PIN);

// ===============================
// IDENTIFIANTS DES CARTES
// ===============================
String UID_AUTORISE_1 = "179E3B25"; // UID carte VALIDE (
String UID_REFUSE_2   = "4A97521B"; // UID CARTE REFUSEE

void setup()
{
  Serial.begin(9600);

  // Configuration des broches
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Tout éteindre au démarrage
  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_ROUGE, LOW);
  noTone(BUZZER);

  // Initialisation LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Systeme RFID");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation");

  delay(2000);

  // Initialisation RFID et bus SPI
  SPI.begin();
  rfid.PCD_Init();

  // Écran d'attente
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lecteur RFID");
  lcd.setCursor(0, 1);
  lcd.print("Approchez carte");

  Serial.println("Systeme pret. Approchez une carte...");
}

void loop()
{
  // Vérifie la présence d'une carte
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  {
    return;
  }

  // Lecture et conversion de l'UID 
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
    {
      uid += "0";
    }
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  // Affichage dans le moniteur série
  Serial.print("UID detecte : ");
  Serial.println(uid);

  // Affichage temporaire de l'UID sur le LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID recu:");
  lcd.setCursor(0, 1);
  lcd.print(uid);
  delay(1200);

  // ===============================
  // CAS 1 : CARTE AUTORISEE
  // ===============================
  if (uid == UID_AUTORISE_1)
  {
    Serial.println("-> Acces Autorise");

    // Voyants
    digitalWrite(LED_ROUGE, LOW);
    digitalWrite(LED_VERTE, HIGH);

    // Bip de validation (Aigu et court : 1500 Hz pendant 150ms)
    tone(BUZZER, 1500, 150);

    // Écran LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Carte Valide !");
    lcd.setCursor(0, 1);
    lcd.print("Acces Autorise");

    delay(3000);

    // Réinitialisation des voyants
    digitalWrite(LED_VERTE, LOW);
  }
  // ===============================
  // CAS 2 : CARTE MAUVAISE / REFUSEE
  // ===============================
  else
  {
    Serial.println("-> Acces Refuse");

    // Voyants
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ROUGE, HIGH);

    // Bip d'erreur (Grave et plus long : 400 Hz pendant 600ms)
    tone(BUZZER, 400, 600);

    // Écran LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Carte Refusee !");
    lcd.setCursor(0, 1);
    lcd.print("Acces Interdit");

    delay(3000);

    // Réinitialisation des voyants
    digitalWrite(LED_ROUGE, LOW);
  }

  // Arrêt de la communication avec la carte
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Retour au message d'accueil
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Approchez une");
  lcd.setCursor(0, 1);
  lcd.print("carte RFID...");
}
