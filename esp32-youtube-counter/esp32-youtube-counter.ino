#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// Configuración de la pantalla OLEDdd
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

// Hardware: Pin del Pulsador
const int PIN_BOTON = 4;

// Control de antirrebote (Debounceee)
int estadoBotonAnterior = HIGH;
unsigned long ultimoTiempoRebote = 0;
const unsigned long DELAY_REBOTE = 50;

// Función para conectar a Wi-Fii
void conectarWiFi() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(F("Conectando a WiFi..."));
  display.println(WIFI_SSID);
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(F("\nConectado a Wi-Fi"));
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(F("WiFi Conectado!"));
  display.display();
  delay(1000);
}

// Función para obtener los suscriptores desde la API de YouTube
void obtenerSuscriptores() {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(F("Consultando YouTube..."));
  display.display();

  String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=" + YOUTUBE_CHANNEL_ID + "&key=" + YOUTUBE_API_KEY;
  
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    
    // Parseo de JSON
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      const char* subs = doc["items"][0]["statistics"]["subscriberCount"];
      
      // Mostrar en pantalla OLED
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println(F("YOUTUBE COUNTER"));
      display.setCursor(0, 20);
      display.setTextSize(2);
      display.print(subs);
      display.setTextSize(1);
      display.println(F(" subs"));
      display.display();

      Serial.print(F("Suscriptores: "));
      Serial.println(subs);
    } else {
      Serial.println(F("Error en parseo JSON"));
    }
  } else {
    Serial.print(F("Error HTTP: "));
    Serial.println(httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BOTON, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error al inicializar SSD1306"));
    for (;;);
  }

  conectarWiFi();
  obtenerSuscriptores(); 
}

void loop() {
  int lecturaActual = digitalRead(PIN_BOTON);

  if (lecturaActual != estadoBotonAnterior) {
    ultimoTiempoRebote = millis();
  }

  if ((millis() - ultimoTiempoRebote) > DELAY_REBOTE) {
    static int estadoBotonEstable = HIGH;
    if (lecturaActual != estadoBotonEstable) {
      estadoBotonEstable = lecturaActual;

      // Al presionar el botón
      if (estadoBotonEstable == LOW) {
        Serial.println(F("Boton presionado: Actualizando contador..."));
        obtenerSuscriptores();
      }
    }
  }

  estadoBotonAnterior = lecturaActual;
}