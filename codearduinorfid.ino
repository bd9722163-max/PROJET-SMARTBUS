#include <SPI.h>
#include <MFRC522.h>

// -------------------------
// Définition des broches
// -------------------------

#define SS_PIN 10      // SDA du RC522
#define RST_PIN 9      // RST du RC522

#define LED 2          // LED blanche

// Création de l'objet RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
  // Initialisation du moniteur série
  Serial.begin(9600);

  // Initialisation du bus SPI
  SPI.begin();

  // Initialisation du lecteur RFID
  mfrc522.PCD_Init();

  // Configuration de la LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("--------------------------------");
  Serial.println("Lecteur RFID initialise");
  Serial.println("Approchez une carte...");
  Serial.println("--------------------------------");
}

void loop()
{
  // Vérifie si une nouvelle carte est présente
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Vérifie si la carte peut être lue
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  Serial.print("UID : ");

  // Affichage de l'identifiant
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    if (mfrc522.uid.uidByte[i] < 0x10)
      Serial.print("0");

    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();

  // Allumer la LED
  digitalWrite(LED, HIGH);

  delay(10000);

  digitalWrite(LED, LOW);

  // Fin de communication
  mfrc522.PICC_HaltA();

  mfrc522.PCD_StopCrypto1();
}
