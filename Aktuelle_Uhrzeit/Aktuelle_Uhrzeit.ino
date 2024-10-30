#include <M5Core2.h>        // M5Stack Core2-Bibliothek
#include <WiFi.h>           // WLAN-Bibliothek
#include <NTPClient.h>      // NTPClient-Bibliothek für Zeitabfrage
#include <WiFiUdp.h>        // UDP-Bibliothek, erforderlich für NTP

// WLAN-Zugangsdaten
const char* ssid = "XiaomiT11";         // WLAN-SSID hier einfügen
const char* password = "Andreas2110"; // WLAN-Passwort hier einfügen

// NTP-Client-Setup
WiFiUDP ntpUDP;                           // UDP-Objekt für die Kommunikation
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000); // Zeitzone und Updateintervall

// Funktion zum Berechnen von Datum aus dem Unix-Timestamp
void getFormattedDate(int& day, int& month, int& year) {
    unsigned long rawTime = timeClient.getEpochTime(); // Epoch-Time in Sekunden
    struct tm * ti;
    ti = localtime(&rawTime);   // Umwandeln in Kalenderzeit
    day = ti->tm_mday;
    month = ti->tm_mon + 1;     // Monate sind in C++ 0-basiert, daher +1
    year = ti->tm_year + 1900;  // Jahr seit 1900, daher +1900
}

void setup() {
    // Initialisierung
    M5.begin();
    M5.Lcd.fillScreen(BLACK);            // Hintergrundfarbe
    M5.Lcd.setTextColor(WHITE);          // Textfarbe
    M5.Lcd.setTextSize(3);               // Textgröße

    // WLAN-Verbindung herstellen
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.setCursor(20, 100);
        M5.Lcd.print("Verbinde...");
    }

    // Starten des NTP-Clients
    timeClient.begin();
    M5.Lcd.fillScreen(BLACK);            // Bildschirm nach Verbindung löschen
}

void loop() {
    // Zeit vom NTP-Server abrufen
    timeClient.update();

    // Datum berechnen
    int currentDay, currentMonth, currentYear;
    getFormattedDate(currentDay, currentMonth, currentYear);

    // Aktuelle Stunde, Minute und Sekunde abrufen
    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
    int currentSecond = timeClient.getSeconds();

    // Datum und Uhrzeit auf dem Display anzeigen
    M5.Lcd.setCursor(20, 80);   // Position für Datum
    M5.Lcd.fillScreen(BLACK);   // Bildschirm vor jeder Aktualisierung löschen
    M5.Lcd.printf("Date: %02d/%02d/%04d", currentDay, currentMonth, currentYear);
    
    M5.Lcd.setCursor(20, 140);  // Position für Uhrzeit
    M5.Lcd.printf("Time: %02d:%02d:%02d", currentHour, currentMinute, currentSecond);

    delay(1000); // Aktualisierung jede Sekunde
}