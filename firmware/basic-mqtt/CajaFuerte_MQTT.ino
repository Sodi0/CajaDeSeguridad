#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <ArduinoJson.h>

#include "config.h"

// Pin del Servo y Ángulos
#define SERVO_PIN D7
#define BUZZER_PIN D4  
const int ANGULO_CERRADO = 0;
const int ANGULO_ABIERTO = 180;

// Variables de estado
bool estaAbierto = false;

// Objetos
WiFiClient espClient;
PubSubClient client(espClient);
Servo servoMotor;

// Función para conectar a la red WiFi
void setupWifi() {
    Serial.print("Conectando a WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado. IP: ");
    Serial.println(WiFi.localIP());
}

// Función de reconexión a MQTT
void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Conectando a MQTT...");
        if (client.connect(MQTT_CLIENT_NAME)) {
            Serial.println("Conectado a MQTT");
            client.subscribe(TOPIC_ABIERTO);
            client.subscribe(TOPIC_CERRADO);
            publicarEstado(estaAbierto ? "ABIERTO" : "CERRADO");
        } else {
            Serial.print("Error, rc=");
            Serial.print(client.state());
            Serial.println(". Intentando reconexión en 5 segundos");
            delay(5000);
        }
    }
}

// Publica el estado actual de la caja fuerte en formato JSON
void publicarEstado(const char* estado) {
    StaticJsonDocument<100> doc;
    doc["estado"] = estado;
    String estadoJson;
    serializeJson(doc, estadoJson);
    client.publish(TOPIC_ESTADO, estadoJson.c_str());
}

// Genera un sonido con el buzzer para indicar apertura
void sonarBuzzerAbierto() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }
}

// Genera un sonido con el buzzer para indicar cierre
void sonarBuzzerCerrado() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
}

// Abre la caja fuerte, mueve el servo y actualiza el estado
void abrirCaja() {
    if (!estaAbierto) {
        servoMotor.write(ANGULO_ABIERTO);
        estaAbierto = true;
        Serial.println("Abriendo caja fuerte...");
        sonarBuzzerAbierto();
        publicarEstado("ABIERTO");
    }
}

// Cierra la caja fuerte, mueve el servo y actualiza el estado
void cerrarCaja() {
    if (estaAbierto) {
        servoMotor.write(ANGULO_CERRADO);
        estaAbierto = false;
        Serial.println("Cerrando caja fuerte...");
        sonarBuzzerCerrado();
        publicarEstado("CERRADO");
    }
}

// Callback de mensajes MQTT que procesa los comandos de apertura y cierre
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

    // Procesa el comando de apertura o cierre basado en el mensaje recibido
    if (strcmp(topic, TOPIC_ABIERTO) == 0 && doc["msg"] == "ABIERTO") {
        abrirCaja();
    } else if (strcmp(topic, TOPIC_CERRADO) == 0 && doc["msg"] == "CERRADO") {
        cerrarCaja();
    }
}

// Configuración inicial del sistema
void setup() {
    Serial.begin(115200);
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(ANGULO_CERRADO);
    estaAbierto = false;

    setupWifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

// Bucle principal que mantiene la conexión MQTT y procesa mensajes
void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
}