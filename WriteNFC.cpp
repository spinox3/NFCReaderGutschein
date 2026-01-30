#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>

/////////////////////////
// PN532 I2C PINS
/////////////////////////
#define SDA_PIN 21
#define SCL_PIN 22
#define PN532_IRQ   4
#define PN532_RESET 5

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

/////////////////////////
// Beispiel JSON (kommt später vom Web-Modul)
/////////////////////////
String inputJson = R"({
  "name":"Sebi Rosenberger",
  "klasse":"4BHET",
  "grund":"Besondere Leistung",
  "tage_total":2,
  "verbrauch":[]
})";

/////////////////////////
// JSON VALIDIERUNG
/////////////////////////
bool validateJson(const String &jsonStr) {
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, jsonStr);

  if (err) {
    Serial.println("Ungültiges JSON");
    return false;
  }

  if (!doc["name"] || !doc["klasse"] || !doc["grund"] || !doc["tage_total"]) {
    Serial.println("Pflichtfelder fehlen");
    return false;
  }

  return true;
}

/////////////////////////
// TAG BESCHREIBEN
/////////////////////////
bool writeToTag(const String &data) {
  uint8_t page = 4;        // Nutzdaten ab Seite 4
  uint8_t buffer[4];

  int len = data.length();
  int index = 0;

  while (index < len) {
    memset(buffer, 0, 4);

    for (int i = 0; i < 4 && index < len; i++) {
      buffer[i] = data[index++];
    }

    if (!nfc.ntag2xx_WritePage(page, buffer)) {
      Serial.println("Schreibfehler");
      return false;
    }

    page++;

    if (page > 129) { // NTAG215 Limit
      Serial.println("Speicher voll");
      return false;
    }
  }

  return true;
}

/////////////////////////
// WRITE WORKFLOW
/////////////////////////
void writeWorkflow(String jsonData) {

  // JSON prüfen
  if (!validateJson(jsonData)) return;

  Serial.println("➡️ Warte auf NFC Tag...");

  uint8_t uid[7];
  uint8_t uidLength;

  // Auf Tag warten
  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.println("Kein NFC-Tag erkannt");
    return;
  }

  Serial.println("Tag erkannt");

  // Kompakt speichern
  StaticJsonDocument<512> doc;
  deserializeJson(doc, jsonData);

  char buffer[300];
  serializeJson(doc, buffer);

  // Schreiben
  if (writeToTag(String(buffer))) {
    Serial.println("Tag erfolgreich beschrieben");
  } else {
    Serial.println("Schreiben fehlgeschlagen");
  }
}

/////////////////////////
// SETUP
/////////////////////////
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Starte NFC Writer...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("PN532 nicht gefunden");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("PN532 bereit");

  // Schreibvorgang starten
  writeWorkflow(inputJson);
}

/////////////////////////
// LOOP
/////////////////////////
void loop() {
}
