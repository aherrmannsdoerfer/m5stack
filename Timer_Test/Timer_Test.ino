#include <M5Core2.h>        // M5Stack Core2-Bibliothek
#include <WiFi.h>           // WLAN-Bibliothek
#include <NTPClient.h>      // NTPClient-Bibliothek für Zeitabfrage
#include <WiFiUdp.h>        // UDP-Bibliothek, erforderlich für NTP

// WLAN-Zugangsdaten
const char* ssid = "WLAN_NAME";         
const char* password = "WLAN_PASSWORT"; 

// NTP-Client-Setup
WiFiUDP ntpUDP;                              
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000); // Zeitzone und Updateintervall

// Timer-Variablen
int timerHours = 0;     // Stunden
int timerMinutes = 0;   // Minuten
unsigned long timerMillis = 0; // Millisekunden für den Timer
bool timerActive = false; // Timer aktiv

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
    // Aktualisiere den NTP-Client
    timeClient.update();

    // Timer-Logik
    if (timerActive) {
        unsigned long currentMillis = millis();
        unsigned long elapsedTime = currentMillis - timerMillis;

        // Berechne verbleibende Zeit in Sekunden
        unsigned long totalRemainingTime = (timerHours * 3600 + timerMinutes * 60) - (elapsedTime / 1000); // in Sekunden
        if (totalRemainingTime <= 0) {
            timerActive = false; // Timer deaktivieren
            vibrate();           // Vibration aktivieren
        } else {
            // Update der verbleibenden Zeit
            displayRemainingTime(totalRemainingTime);
        }
    }

    // Anzeigen der Timerzeit
    if (!timerActive) {
        M5.Lcd.fillRect(20, 50, 240, 40, BLACK);
        M5.Lcd.setCursor(20, 50);
        M5.Lcd.printf("Timer: %02d:%02d", timerHours, timerMinutes);
    }

    // Überprüfen, ob Tasten berührt werden
    if (M5.Touch.ispressed()) {
        Point touchPoint = M5.Touch.getPressPoint(); // Koordinaten des Touchpunkts abrufen
        handleTouch(touchPoint);
    }

    // Zeichne die Buttons
    drawButtons();

    delay(100); // Aktualisierung
}

// Funktion zur Vibrationsauslösung
void vibrate() {
    M5.Axp.SetLDO2(true); // Vibration aktivieren
    delay(10000);         // 10 Sekunden vibrieren
    M5.Axp.SetLDO2(false); // Vibration deaktivieren
}

// Funktion zur Handhabung der Touch-Eingabe
void handleTouch(Point touchPoint) {
    // Touch-Bereich für die linke Taste (Timerstunden erhöhen)
    if (touchPoint.x < 107 && touchPoint.y > 200) { // Linke untere Ecke
        timerHours = (timerHours + 1) % 24;  // Stunde erhöhen
    }
    // Touch-Bereich für die mittlere Taste (Timer Minuten erhöhen)
    else if (touchPoint.x >= 107 && touchPoint.x < 213 && touchPoint.y > 200) { // Mittlere untere Ecke
        timerMinutes = (timerMinutes + 1) % 60; // Minute erhöhen
    }
    // Touch-Bereich für die rechte Taste (Timer starten)
    else if (touchPoint.x >= 213 && touchPoint.y > 200) { // Rechte untere Ecke
        if (!timerActive) { // Timer starten, wenn er nicht aktiv ist
            timerMillis = millis(); // Aktuelle Zeit speichern
            timerActive = true; // Timer aktivieren
            displayStartFeedback(); // Feedback anzeigen
        }
    }
}

// Funktion zur Anzeige des Start-Feedbacks
void displayStartFeedback() {
    M5.Lcd.fillRect(20, 120, 240, 40, BLACK); // Hintergrund löschen
    M5.Lcd.setCursor(20, 120);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.printf("Timer gestartet! %02d:%02d", timerHours, timerMinutes);
}

// Funktion zur Anzeige der verbleibenden Zeit
void displayRemainingTime(unsigned long remainingTime) {
    // Anzeige der verbleibenden Zeit in Sekunden
    M5.Lcd.fillRect(20, 50, 240, 40, BLACK); // Hintergrund löschen
    M5.Lcd.setCursor(20, 50);
    M5.Lcd.printf("Verbleibend: %lu Sek", remainingTime);
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
    M5.Lcd.print("Start"); // Text für rechten Button
}