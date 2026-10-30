#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// Configuración de la pantalla OLED
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Inicializar pantalla OLED en dirección 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error al inicializar SSD1306"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(F("Sistema Iniciado"));
  display.display();
  
  Serial.println(F("Sistema iniciado correctamente."));
}

void loop() {
  // Bucle principal vacío por el momento
}