#include <M5Core2.h>        // M5Stack Core2-Bibliothek
#include <WiFi.h>           // WLAN-Bibliothek
#include <NTPClient.h>      // NTPClient-Bibliothek für Zeitabfrage
#include <WiFiUdp.h>        // UDP-Bibliothek, erforderlich für NTP

// WLAN-Zugangsdaten
const char* ssid = "WLAN NAME";         
const char* password = "WLAN PASSWORD"; 

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

    // Überprüfen, ob Tasten berührt werden
    if (M5.Touch.ispressed()) {
        Point touchPoint = M5.Touch.getPressPoint(); // Koordinaten des Touchpunkts abrufen
        handleTouch(touchPoint);
    }

    // Buttons anzeigen
    drawButtons();

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

// Funktion zum Deaktivieren des Alarms
void disableAlarm() {
    alarmTriggered = false; // Alarm zurücksetzen
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 50);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("Alarm Deactivated");
    delay(2000); // Zeige Nachricht für 2 Sekunden an
}

// Funktion zur Handhabung der Touch-Eingabe
void handleTouch(Point touchPoint) {
    // Touch-Bereich für die linke Taste (Alarmstunde erhöhen)
    if (touchPoint.x < 107 && touchPoint.y > 200) { // Linke untere Ecke
        alarmHour = (alarmHour + 1) % 24;  // Stunde erhöhen
        alarmSet = true; // Alarm wird gesetzt
    }
    // Touch-Bereich für die mittlere Taste (Alarmminute erhöhen)
    else if (touchPoint.x >= 107 && touchPoint.x < 213 && touchPoint.y > 200) { // Mittlere untere Ecke
        alarmMinute = (alarmMinute + 1) % 60; // Minute erhöhen
        alarmSet = true; // Alarm wird gesetzt
    }
    // Touch-Bereich für die rechte Taste (Alarm ausschalten)
    else if (touchPoint.x >= 213 && touchPoint.y > 200) { // Rechte untere Ecke
        if (alarmTriggered) {
            disableAlarm();
        }
    }

    // Aktualisiere die Anzeige für die neue Weckzeit
    updateDisplay();
}

// Funktion zur Aktualisierung der Anzeige
void updateDisplay() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 50);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Set Alarm:");

    M5.Lcd.setCursor(20, 100);
    M5.Lcd.printf("New Alarm: %02d:%02d", alarmHour, alarmMinute);
}

// Funktion zum Zeichnen der Buttons
void drawButtons() {
    M5.Lcd.fillRect(0, 200, 80, 40, BLUE); // Linker Button
    M5.Lcd.fillRect(107, 200, 107, 40, GREEN); // Mittlerer Button
    M5.Lcd.fillRect(213, 200, 107, 40, RED); // Rechter Button

    // Buttontexte
    M5.Lcd.setCursor(20, 210);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("Stunde +"); // Text für linken Button

    M5.Lcd.setCursor(130, 210);
    M5.Lcd.print("Minute +"); // Text für mittleren Button

    M5.Lcd.setCursor(230, 210);
    M5.Lcd.print("Alarm Off"); // Text für rechten Button
}