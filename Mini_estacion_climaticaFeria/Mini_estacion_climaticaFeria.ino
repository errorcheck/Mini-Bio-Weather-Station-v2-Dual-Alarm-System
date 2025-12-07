#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define R 9
#define G 10
#define B 11
#define BUZZER 6
#define BOTON 7

#define UMBRAL_FRIO 17
#define UMBRAL_CALOR 26   //  NUEVO: alarma por calor desde 26°C

DHT dht(DHTPIN, DHTTYPE);
bool alarmaActiva = true;

unsigned long lastSensorRead = 0;
unsigned long lastBuzz = 0;
unsigned long debounce = 0;

// Variables globales
float t = NAN, h = NAN;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTON, INPUT_PULLUP);

  Serial.println("== Estacion climatica (millis) ==");
  Serial.println("Temperatura\tHumedad");  
}

void loop() {
  unsigned long now = millis();

  // -------- BOTÓN (antirrebote) --------
  if (digitalRead(BOTON) == LOW && now - debounce > 300) {
    alarmaActiva = !alarmaActiva;
    debounce = now;
    Serial.print("Alarma ");
    Serial.println(alarmaActiva ? "ACTIVADA" : "DESACTIVADA");
  }

  // -------- LECTURA DEL SENSOR CADA 7 SEGUNDOS --------
  if (now - lastSensorRead >= 7000) {
    lastSensorRead = now;

    h = dht.readHumidity();
    t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Error al leer DHT11");
    } else {
      Serial.print(t);
      Serial.print("\t");
      Serial.println(h);

      Serial.print("Temp: ");
      Serial.print(t);
      Serial.print(" °C | Humedad: ");
      Serial.print(h);
      Serial.println(" %");
      Serial.println("----------------------");
    }
  }

  // -------- CONTROL RGB + BUZZER --------
  analogWrite(R,0);
  analogWrite(G,0);
  analogWrite(B,0);
  noTone(BUZZER);

  if (!isnan(t)) {

    //  FRÍO
    if (t < UMBRAL_FRIO) {                 
      analogWrite(B,255);

      if (alarmaActiva && now - lastBuzz > 1000) {
        lastBuzz = now;
        tone(BUZZER, 1000, 200);
        Serial.println("Frío detectado");
      }
    }

    //  TEMPLADO
    else if (t < UMBRAL_CALOR) {            
      analogWrite(G,255);
    }

    //  CALOR
    else {                        
      analogWrite(R,255);

      if (alarmaActiva && now - lastBuzz > 1000) {
        lastBuzz = now;
        tone(BUZZER, 1500, 200);   // tono distinto para calor
        Serial.println("Calor extremo detectado");
      }
    }
  }
}
