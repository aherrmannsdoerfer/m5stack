#include <M5Core2.h>        // M5Stack Core2-Bibliothek
#include <WiFi.h>           // WLAN-Bibliothek
#include <NTPClient.h>      // NTPClient-Bibliothek für Zeitabfrage
#include <WiFiUdp.h>        // UDP-Bibliothek, erforderlich für NTP

// WLAN-Zugangsdaten
const char* ssid = "Xiaomi11T";         
const char* password = "Andreas2110"; 

// NTP-Client-Setup
WiFiUDP ntpUDP;                              
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000); // Zeitzone und Updateintervall

// Variablen für Wecker
int alarmHour = 8;    // Standard-Weckstunde
int alarmMinute = 30; // Standard-Weckminute
bool alarmSet = false;
bool alarmTriggered = false;

void setup() {
    M5.begin();
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);

    // WLAN-Verbindung herstellen
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.setCursor(20, 100);
        M5.Lcd.print("Verbinde...");
    }

    // Starten des NTP-Clients
    timeClient.begin();
    M5.Lcd.fillScreen(BLACK);
}

void loop() {
    // Zeit vom NTP-Server abrufen
    timeClient.update();
    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
    int currentSecond = timeClient.getSeconds();

    // Uhrzeit und Weckzeit anzeigen
    M5.Lcd.setCursor(20, 50);
    M5.Lcd.fillRect(20, 50, 200, 40, BLACK);
    M5.Lcd.printf("Time: %02d:%02d:%02d", currentHour, currentMinute, currentSecond);

    M5.Lcd.setCursor(20, 100);
    M5.Lcd.fillRect(20, 100, 200, 40, BLACK);
    M5.Lcd.printf("Alarm: %02d:%02d", alarmHour, alarmMinute);
    
    // Prüfen, ob der Wecker aktiviert ist und die Zeit erreicht wurde
    if (alarmSet && !alarmTriggered && currentHour == alarmHour && currentMinute == alarmMinute) {
        triggerAlarm();
    }

    // Touchscreen-Eingabe zur separaten Einstellung der Stunden und Minuten
    if (M5.Touch.ispressed()) {
        setAlarm();
    }

    delay(1000); // Aktualisierung jede Sekunde
}

// Funktion zum Auslösen des Alarms
void triggerAlarm() {
    alarmTriggered = true;
    M5.Lcd.fillScreen(RED);
    M5.Lcd.setCursor(40, 100);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println("ALARM!");
}

// Funktion zum Setzen der Weckzeit durch Touch-Eingabe
void setAlarm() {
    Point touchPoint = M5.Touch.getPressPoint();  // Koordinaten des Touchpunkts abrufen

    // Bereich zum Erhöhen der Stunde (linker oberer Bildschirmbereich)
    if (touchPoint.x < 160 && touchPoint.y < 120) {
        alarmHour = (alarmHour + 1) % 24;  // Stunde erhöhen
        alarmTriggered = false; // Alarm zurücksetzen
    }
    // Bereich zum Erhöhen der Minute (linker unterer Bildschirmbereich)
    else if (touchPoint.x < 160 && touchPoint.y >= 120) {
        alarmMinute = (alarmMinute + 1) % 60; // Minute erhöhen
        alarmTriggered = false; // Alarm zurücksetzen
    }

    // Aktualisiere die Anzeige für die neue Weckzeit
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 50);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Set Alarm:");

    M5.Lcd.setCursor(20, 100);
    M5.Lcd.printf("New Alarm: %02d:%02d", alarmHour, alarmMinute);

    alarmSet = true;
    delay(500); // kleine Verzögerung zur Vermeidung doppelter Berührungen
}