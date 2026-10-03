# Comunicación MQTT

## Configuración observada

| Elemento | Valor |
| --- | --- |
| Broker | `broker.emqx.io` |
| Puerto | `1883` |
| Transporte | MQTT sobre TCP, sin TLS configurado en el código |

## Tópicos

| Tópico | Publicadores | Suscriptores | Propósito |
| --- | --- | --- | --- |
| `ESP/Abierto` | Android, Node-RED | Firmware | Solicitar apertura |
| `ESP/Cerrado` | Android, Node-RED | Firmware | Solicitar cierre |
| `ESP/Estado` | Firmware | Android, Node-RED | Publicar el estado actual |

## Formatos comprobados

Los comandos se publican como JSON:

```json
{"msg":"ABIERTO"}
```

```json
{"msg":"CERRADO"}
```

El firmware publica el estado como JSON:

```json
{"estado":"ABIERTO"}
```

```json
{"estado":"CERRADO"}
```

Los clientes usan los mismos tópicos y valores definidos en el código. Si se cambia el broker o los tópicos, debe actualizarse la aplicación, el firmware y el flujo Node-RED de forma coordinada.

## Elección del broker

Se utilizó el broker público `broker.emqx.io` en lugar de desplegar un broker propio (por ejemplo, Mosquitto). Esto evita instalar y configurar infraestructura adicional y permite que Android, Node-RED y el ESP8266 se comuniquen desde cualquier red sin abrir puertos.

Como contrapartida, el broker es público y no usa autenticación ni TLS, por lo que cualquier cliente que conozca los tópicos podría publicar o suscribirse. Es una configuración adecuada para un proyecto académico, pero en un entorno real se debería usar un broker propio con TLS y autenticación por usuario.

Durante el desarrollo se utilizó [MQTTX](https://mqttx.app/) como cliente de escritorio para publicar comandos.