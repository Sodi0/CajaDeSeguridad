package com.example.securitybox;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {

    private TextView txtEstado;
    private Button btnAbrir, btnCerrar;
    private ImageView btnBack;
    private MqttAsyncClient mqttClient;
    private static final String MQTT_SERVER = "tcp://broker.emqx.io:1883";
    private static final String CLIENT_ID = "AndroidClient";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        txtEstado = findViewById(R.id.textView_estado);
        btnAbrir = findViewById(R.id.button_abrir);
        btnCerrar = findViewById(R.id.button_cerrar);

        connectToMqtt();

        btnAbrir.setOnClickListener(v -> sendCommand("ESP/Abierto", "{\n" +
                "  \"msg\": \"ABIERTO\"\n" +
                "}"));
        btnCerrar.setOnClickListener(v -> sendCommand("ESP/Cerrado", "{\n" +
                "  \"msg\": \"CERRADO\"\n" +
                "}"));

        LocalBroadcastManager.getInstance(this).registerReceiver(mqttReceiver,
                new IntentFilter("mqtt_data"));

        btnBack = findViewById(R.id.btn_back);
        btnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, Login.class);
                startActivity(intent);
            }
        });
    }

    private void connectToMqtt() {
        try {
            mqttClient = new MqttAsyncClient(MQTT_SERVER, CLIENT_ID, null);
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            mqttClient.connect(options, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.d("MQTT", "Conexión exitosa al broker");
                    subscribeToTopics();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.e("MQTT", "Error al conectar: " + exception.getMessage());
                }
            });

            mqttClient.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    Log.d("MQTT", "Conexión perdida: " + cause.getMessage());
                }

                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    Log.d("MQTT", "Mensaje recibido en el tópico: " + topic + ", Mensaje: " + message.toString());
                    Intent intent = new Intent("mqtt_data");
                    intent.putExtra("estado", message.toString());
                    LocalBroadcastManager.getInstance(MainActivity.this).sendBroadcast(intent);
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    // Manejar la entrega completa
                }
            });
        } catch (MqttException e) {
            Log.e("MQTT", "Error al crear cliente MQTT: " + e.getMessage());
        }
    }

    private void subscribeToTopics() {
        try {
            mqttClient.subscribe("ESP/Estado", 0);
        } catch (MqttException e) {
            Log.e("MQTT", "Error al suscribirse: " + e.getMessage());
        }
    }

    private void sendCommand(String topic, String message) {
        try {
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            mqttClient.publish(topic, mqttMessage);
        } catch (MqttException e) {
            Log.e("MQTT", "Error al enviar mensaje: " + e.getMessage());
        }
    }


    private final BroadcastReceiver mqttReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String estado = intent.getStringExtra("estado");
            if (estado != null) {
                try {
                    JSONObject jsonObject = new JSONObject(estado);
                    String estadoValor = jsonObject.getString("estado");
                    txtEstado.setText("Estado: " + estadoValor);
                } catch (JSONException e) {
                    Log.e("JSON", "Error al parsear JSON: " + e.getMessage());
                    txtEstado.setText("Estado: " + estado);
                }
            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mqttReceiver);
        try {
            if (mqttClient != null) {
                mqttClient.disconnect();
            }
        } catch (MqttException e) {
            Log.e("MQTT", "Error al desconectar: " + e.getMessage());
        }
    }


}
