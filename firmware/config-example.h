/*
 * INSTRUCCIONES:
 * 1. Haz una copia de este archivo y renómbralo a "config.h".
 * 2. Reemplaza los valores de abajo con tus credenciales reales.
 * 3. El archivo "config.h" está en el .gitignore, por lo que tus claves estarán protegidas.
 */

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Config
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT Config
const char* MQTT_SERVER = "broker.emqx.io";
const int   MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME = "CajaFuerte_CambiarEsteID"; // Usar un ID único

// MQTT Topics
const char* TOPIC_ABIERTO = "ESP/Abierto";
const char* TOPIC_CERRADO = "ESP/Cerrado";
const char* TOPIC_ESTADO  = "ESP/Estado";

#endif