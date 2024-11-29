#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi
const char* ssid = "Nelly";
const char* password = "nelly200";

// MQTT Configuration
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_client_name = "CajaFuerte";

// MQTT Topics
const char* topic_abierto = "ESP/Abierto";
const char* topic_cerrado = "ESP/Cerrado";
const char* topic_estado = "ESP/Estado";

// Pines ocupados
#define SERVO_PIN D7
#define BUZZER_PIN D5
#define LCD_SDA D1
#define LCD_SCL D2

byte LockClosed[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b11111,
  0b11011,
  0b11011,
  0b11111,
  0b00000
};

byte LockOpen[8] = {
  0b01110,
  0b10001,
  0b10000,
  0b11111,
  0b11011,
  0b11011,
  0b11111,
  0b00000
};

byte Alien[8] = {
0b11111,
0b10101,
0b11111,
0b11111,
0b01110,
0b01010,
0b11011,
0b00000
};
// Angulos del Servo
const int ANGULO_CERRADO = 180;
const int ANGULO_ABIERTO = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
bool estaAbierto = false;

WiFiClient espClient;
PubSubClient client(espClient);
Servo servoMotor;

void setupWifi() {
    Serial.print("Conectando a WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado. IP: ");
    Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Conectando a MQTT...");
        if (client.connect(mqtt_client_name)) {
            Serial.println("Conectado a MQTT");
            client.subscribe(topic_abierto);
            client.subscribe(topic_cerrado);
            publicarEstado(estaAbierto ? "ABIERTO" : "CERRADO");
            updateLCDEstado();
        } else {
            Serial.print("Error, rc=");
            Serial.print(client.state());
            Serial.println(". Intentando reconexión en 5 segundos");
            delay(5000);
        }
    }
}

// Publicar estado en formato JSON
void publicarEstado(const char* estado) {
    StaticJsonDocument<100> doc;
    doc["estado"] = estado;
    String estadoJson;
    serializeJson(doc, estadoJson);
    client.publish(topic_estado, estadoJson.c_str());
}

// 
void updateLCDEstado() {
    lcd.clear();
    lcd.createChar(0, LockClosed);
    lcd.createChar(1, LockOpen);
    lcd.createChar(2, Alien);
    lcd.setCursor(0, 0);
    lcd.print("Estado caja fuerte: ");
    lcd.setCursor(0, 1);
    if (estaAbierto) {
        lcd.write(1);
        lcd.print("   ABIERTO  ");
    } else {
        lcd.write(0);
        lcd.print("   CERRADA   ");
    }
}

void sonarBuzzerAbierto() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }
}

void sonarBuzzerCerrado() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
}

void abrirCaja() {
    if (!estaAbierto) {
        servoMotor.write(ANGULO_ABIERTO);
        estaAbierto = true;
        Serial.println("Abriendo caja fuerte...");
        sonarBuzzerAbierto();
        publicarEstado("ABIERTO");
        updateLCDEstado();
    }
}

void cerrarCaja() {
    if (estaAbierto) {
        servoMotor.write(ANGULO_CERRADO);
        estaAbierto = false;
        Serial.println("Cerrando caja fuerte...");
        sonarBuzzerCerrado();
        publicarEstado("CERRADO");
        updateLCDEstado();
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    String mensaje = "";
    for (int i = 0; i < length; i++) {
        mensaje += (char)payload[i];
    }
    Serial.print("Mensaje recibido [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(mensaje);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, mensaje);
    if (error) {
        Serial.print("Error al deserializar JSON: ");
        Serial.println(error.c_str());
        return;
    }

    if (strcmp(topic, topic_abierto) == 0 && doc["msg"] == "ABIERTO") {
        abrirCaja();
    } else if (strcmp(topic, topic_cerrado) == 0 && doc["msg"] == "CERRADO") {
        cerrarCaja();
    }
}

void setup() {
    Serial.begin(115200);

    Wire.begin(LCD_SDA, LCD_SCL);
    
    lcd.init();
    lcd.backlight();
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("Bienvenido ");
    lcd.write(2);
    lcd.setCursor(0, 1);
    lcd.print("Iniciando...");
    delay(2000);

    servoMotor.attach(SERVO_PIN);
    servoMotor.write(ANGULO_CERRADO);
    estaAbierto = false;

    setupWifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    updateLCDEstado();
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
}