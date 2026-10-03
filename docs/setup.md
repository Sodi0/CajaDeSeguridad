# Configuración

## Requisitos

- Android SDK para compilar el proyecto Android.
- IDE de Arduino o una herramienta equivalente para cargar el firmware.
- Placa ESP8266 y el hardware conectado según el circuito del proyecto.
- Librerías Arduino usadas por los sketches: `ESP8266WiFi`, `PubSubClient`, `Servo`, `ArduinoJson`, `Wire` y, para `lcd-mqtt`, `LiquidCrystal_I2C`.
- Instancia de Node-RED con los nodos MQTT y Dashboard disponibles si se usará el flujo.

## Android

1. Abrir el proyecto desde `android/`.
2. Configurar el Android SDK en el entorno local.
3. Ejecutar las tareas Gradle desde ese directorio.

No se debe versionar `android/local.properties`, porque contiene una ruta local del SDK. La compilación debe realizarse en un entorno que tenga instalado el SDK requerido.

## Firmware

1. Copiar `firmware/config-example.h` como `config.h` en el directorio del sketch que se vaya a cargar.
2. Sustituir `YOUR_WIFI_SSID` y `YOUR_WIFI_PASSWORD` por valores locales.
3. Revisar el broker, puerto y tópicos antes de cargar el sketch.
4. Instalar las librerías requeridas y cargar el sketch en el ESP8266.

El archivo local `config.h` está excluido por `.gitignore` y no debe versionarse. No se incluyen credenciales reales en el repositorio.

## MQTT y Node-RED

El código referencia `broker.emqx.io:1883` y los tópicos `ESP/Abierto`, `ESP/Cerrado` y `ESP/Estado`. Importa [../node-red/flows.json](../node-red/flows.json) en Node-RED y verifica que la configuración del broker coincida con el firmware y Android.

Los detalles de los mensajes están en [mqtt.md](mqtt.md). El broker público no debe considerarse una configuración de producción; ver [Elección del broker](mqtt.md#elección-del-broker).

## Archivos que no deben versionarse

- `config.h`
- `android/local.properties`
- `.idea/`
- `.gradle/`
- Directorios `build/`
- APKs, AABs y artefactos generados
