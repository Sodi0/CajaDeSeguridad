# SecurityBox - Aplicación Android para Control de Caja Fuerte Inteligente

SecurityBox es una aplicación Android que permite controlar una caja fuerte a través de comandos enviados mediante MQTT. La aplicación incluye un sistema de autenticación mediante un PIN de 4 dígitos, que garantiza la seguridad del acceso a la caja fuerte.

## Funcionalidades

- **Autenticación con PIN:** La aplicación solicita un PIN de 4 dígitos para acceder. Si el usuario es nuevo, se le pide que ingrese un PIN.
- **Control de la caja fuerte:** A través de MQTT, se envían comandos para abrir o cerrar la caja fuerte.
- **Estado en tiempo real:** La aplicación muestra el estado de la caja fuerte en tiempo real, indicando si está abierta o cerrada.

## Requisitos

- **Android 5.0 o superior.**
- **Permiso de acceso a internet** para conectarse al servidor MQTT.

## Estructura de la Aplicación

La aplicación está dividida en dos actividades principales:

1. **Login Activity**: La pantalla de inicio donde el usuario ingresa su PIN.
2. **Main Activity**: La pantalla principal donde se visualiza el estado de la caja fuerte y se pueden enviar los comandos de apertura o cierre.

## Dependencias

La aplicación utiliza las siguientes dependencias:

- **MQTT Client**: `org.eclipse.paho.client.mqttv3` para la comunicación con el servidor MQTT.
- **JSON**: Para el manejo de los mensajes recibidos.

## Configuración

### MQTT Server

El servidor MQTT utilizado en esta aplicación es **broker.emqx.io**, que es un servidor público y gratuito. Si deseas utilizar tu propio servidor, puedes cambiar la URL del broker en el siguiente fragmento de código:

```java
private static final String MQTT_SERVER = "tcp://broker.emqx.io:1883";
```

### Permisos
La aplicación requiere permisos de internet. Asegúrate de que el archivo AndroidManifest.xml contenga la siguiente línea de permisos:
```xml
<uses-permission android:name="android.permission.INTERNET" />
```

