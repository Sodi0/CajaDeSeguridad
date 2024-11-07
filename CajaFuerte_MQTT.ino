#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <ArduinoJson.h>

// Configuración WiFi
const char* ssid = "Nelly";
const char* password = "nelly200";

// Configuración MQTT
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_client_name = "CajaFuerte";

// Topics MQTT
const char* topic_abierto = "ESP/Abierto";
const char* topic_cerrado = "ESP/Cerrado";
const char* topic_estado = "ESP/Estado";

// Pin del Servo y Ángulos
#define SERVO_PIN D7
const int ANGULO_CERRADO = 0;
const int ANGULO_ABIERTO = 180;

// Variables de estado
bool estaAbierto = false;

// Objetos
WiFiClient espClient;
PubSubClient client(espClient);
Servo servoMotor;

// Función para configurar WiFi
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

// Función de reconexión a MQTT
void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Conectando a MQTT...");
        if (client.connect(mqtt_client_name)) {
            Serial.println("Conectado a MQTT");
            client.subscribe(topic_abierto);
            client.subscribe(topic_cerrado);
            publicarEstado(estaAbierto ? "ABIERTO" : "CERRADO");
        } else {
            Serial.print("Error, rc=");
            Serial.print(client.state());
            Serial.println(". Intentando reconexión en 5 segundos");
            delay(5000);
        }
    }
}

// Publicar estado actual de la caja
void publicarEstado(const char* estado) {
    StaticJsonDocument<100> doc;
    doc["estado"] = estado;
    String estadoJson;
    serializeJson(doc, estadoJson);
    client.publish(topic_estado, estadoJson.c_str());
}

// Abrir y cerrar caja fuerte
void abrirCaja() {
    if (!estaAbierto) {
        servoMotor.write(ANGULO_ABIERTO);
        estaAbierto = true;
        Serial.println("Abriendo caja fuerte...");
        publicarEstado("ABIERTO");
    }
}

void cerrarCaja() {
    if (estaAbierto) {
        servoMotor.write(ANGULO_CERRADO);
        estaAbierto = false;
        Serial.println("Cerrando caja fuerte...");
        publicarEstado("CERRADO");
    }
}

// Callback de mensajes MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    String mensaje = "";
    for (int i = 0; i < length; i++) {
        mensaje += (char)payload[i];
    }

    Serial.print("Mensaje recibido [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(mensaje);

    // Verificar si el mensaje es JSON antes de deserializar
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, mensaje);
    if (error) {
        Serial.print("Error al deserializar JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Procesar comandos de apertura o cierre
    if (strcmp(topic, topic_abierto) == 0 && doc["msg"] == "ABIERTO") {
        abrirCaja();
    } else if (strcmp(topic, topic_cerrado) == 0 && doc["msg"] == "CERRADO") {
        cerrarCaja();
    }
}

void setup() {
    Serial.begin(115200);
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(ANGULO_CERRADO);
    estaAbierto = false;

    setupWifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
}
