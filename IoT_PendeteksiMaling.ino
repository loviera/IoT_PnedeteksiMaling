#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* botToken = "BOT_TOKEN"; // Ganti dengan token bot Telegram
const char* chatId = "CHAT_ID"; // Ganti dengan ID chat Telegram

#define PIR_SENSOR 27
#define LED_MERAH 26
#define BUZZER 25

volatile bool motionDetected = false; // Flag untuk mendeteksi gerakan

void IRAM_ATTR motionISR() {
    motionDetected = true; // Set flag saat ada gerakan
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password, 6);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println(" Connected!");

    pinMode(PIR_SENSOR, INPUT);
    pinMode(LED_MERAH, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(PIR_SENSOR), motionISR, RISING); // Aktifkan interrupt
}

void loop() {
    if (motionDetected) {
        motionDetected = false; // Reset flag

        // 🔴 Prioritas 1: Nyalakan alarm (LED & buzzer)
        digitalWrite(LED_MERAH, HIGH);
        digitalWrite(BUZZER, HIGH);
        Serial.println("⚠️ Gerakan Terdeteksi! Mengaktifkan Alarm...");
        delay(5000); // Alarm menyala selama 5 detik
        digitalWrite(LED_MERAH, LOW);
        digitalWrite(BUZZER, LOW);

        // 📲 Prioritas 2: Kirim notifikasi ke Telegram
        sendNotification();
    }
}

// 🔹 Fungsi untuk mengirim notifikasi Telegram
void sendNotification() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        String url = "https://api.telegram.org/bot";
        url += botToken;
        url += "/sendMessage?chat_id=";
        url += chatId;
        url += "&text=MALING!! GERAKAN TERDETEKSI!";

        http.begin(client, url);
        int httpCode = http.GET();
        String payload = http.getString();  // Ambil respon dari Telegram

        Serial.print("HTTP Code: ");
        Serial.println(httpCode);
        Serial.print("Response: ");
        Serial.println(payload);

        if (httpCode > 0) {
            Serial.println("✅ Notifikasi Terkirim!");
        } else {
            Serial.println("❌ Gagal mengirim notifikasi.");
        }
        http.end();
    } else {
        Serial.println("❌ WiFi tidak terhubung!");
    }
}
