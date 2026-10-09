#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_VERTE 2
#define LED_ROUGE A0
#define BUZZER A1

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_ROUGE, LOW);
  noTone(BUZZER);
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("SmartBus Ready");
  lcd.setCursor(0, 1);
  lcd.print("Approchez carte");

  SPI.begin();
  rfid.PCD_Init();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // 1. Lecture de l'UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  // Affichage attente
  lcd.clear();
  lcd.print("Verification...");

  // 2. Envoi de l'UID a la Raspberry Pi via USB
  Serial.println(uid);

  // 3. Attente de la reponse de la Raspberry Pi (timeout de 2 sec)
  unsigned long startTime = millis();
  bool reponseRecue = false;

  while (millis() - startTime < 2000) {
    if (Serial.available() > 0) {
      String reponse = Serial.readStringUntil('\n');
      reponse.trim();

      // TRAITEMENT CAS ACCES AUTORISE (OK:Nom)
      if (reponse.startsWith("OK:")) {
        String nom = reponse.substring(3);
        
        digitalWrite(LED_VERTE, HIGH);
        tone(BUZZER, 1500, 150);
        
        lcd.clear();
        lcd.print("Bon voyage !");
        lcd.setCursor(0, 1);
        lcd.print(nom);
        
        delay(3000);
        digitalWrite(LED_VERTE, LOW);
      } 
      // TRAITEMENT CAS ACCES REFUSE (NOK:Raison)
      else if (reponse.startsWith("NOK:")) {
        String raison = reponse.substring(4);
        
        digitalWrite(LED_ROUGE, HIGH);
        tone(BUZZER, 400, 600);
        
        lcd.clear();
        lcd.print("Acces Refuse !");
        lcd.setCursor(0, 1);
        lcd.print(raison);
        
        delay(3000);
        digitalWrite(LED_ROUGE, LOW);
      }
      
      reponseRecue = true;
      break;
    }
  }

  // Si la Raspberry Pi n'a pas repondu
  if (!reponseRecue) {
    lcd.clear();
    lcd.print("Erreur Serveur");
    delay(2000);
  }

  // Reset
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  
  lcd.clear();
  lcd.print("SmartBus Ready");
  lcd.setCursor(0, 1);
  lcd.print("Approchez carte");
}