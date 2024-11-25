package com.example.securitybox;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class Login extends AppCompatActivity {

    private EditText txtPass;
    private Button btnIngresar, btnLimpiar;
    private StringBuilder inputBuilder;
    private SharedPreferences preferences;
    private static final String PREF_NAME = "sharedPreference";
    private static final String PIN_KEY = "PIN";
    private static final int MAX_PIN_LENGTH = 4;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        preferences = getSharedPreferences(PREF_NAME, MODE_PRIVATE);
        txtPass = findViewById(R.id.txt_pass);
        btnIngresar = findViewById(R.id.btn_ingresar);
        btnLimpiar = findViewById(R.id.btn_limpiar);

        inputBuilder = new StringBuilder();
        setupNumberButtons();
        setupActionButtons();
        checkFirstRun();
    }

    /**
     *Metodo para darle la funcionalidad a los botones numericos,
     * Se define un arreglo con los id de los botones,
     * Luego en un listener asigna los dígitos presionados para agregar al inputBuilder,
     * Tambien verifica que no se exceda la cantidad maxima para el pin (MAX_PIN_LENGTH = 4).
     * Finalmente actualiza la pantalla con el PIN ingresado.
     */
    private void setupNumberButtons() {
        int[] buttonIds = {
                R.id.btn_0, R.id.btn_1, R.id.btn_2, R.id.btn_3, R.id.btn_4,
                R.id.btn_5, R.id.btn_6, R.id.btn_7, R.id.btn_8, R.id.btn_9
        };

        View.OnClickListener numberClickListener = v -> {
            if (inputBuilder.length() < MAX_PIN_LENGTH) {
                Button clickedButton = (Button) v;
                String digit = clickedButton.getText().toString();
                inputBuilder.append(digit);
                updatePinDisplay();
            }
        };

        for (int id : buttonIds) {
            findViewById(id).setOnClickListener(numberClickListener);
        }
    }

    /**
     *Configuracion de los btn principales
     */
    private void setupActionButtons() {
        btnIngresar.setOnClickListener(v -> validatePin());

        btnLimpiar.setOnClickListener(v -> {
            inputBuilder.setLength(0);
            updatePinDisplay();
        });
    }

    /**
     *Actualiza el txt_pass
     */
    private void updatePinDisplay() {
        txtPass.setText(String.join("", new String(new char[inputBuilder.length()]).replace("\0", "*")));
    }

    /**
     *Valida el pin del usuario,
     * Si el usuario ingreso anteriormente lo recupera del SharedPreference.
     * Si nunca se ha ingresado un pin, muestra un mensaje para que el usuario ingrese un pin de inicio, y tras el primer ingreso lo guarda.
     *
     * Si el PIN ingresado coincide con el almacenado, prosigue a la siguiente actividad,
     * Si no mustra mensaje de errror y limpia el campo.
     */
    private void validatePin() {
        String storedPin = preferences.getString(PIN_KEY, "");
        String enteredPin = inputBuilder.toString();

        if (enteredPin.isEmpty()) {
            showToast("Debe ingresar su pin");
            return;
        }

        if (storedPin.isEmpty()) {
            savePin(enteredPin);
            showToast("PIN establecido correctamente");
            proceedToMainActivity();
        } else if (enteredPin.equals(storedPin)) {
            proceedToMainActivity();
        } else {
            showToast("PIN incorrecto");
            inputBuilder.setLength(0);
            updatePinDisplay();
        }
    }

    /**
     * Alamcena el pin en el sharedPreference
     * @param pin
     */
    private void savePin(String pin) {
        preferences.edit()
                .putString(PIN_KEY, pin)
                .apply();
    }

    /**
     *Verifica si es la primera vez que se ocupa la aplicacion,
     * Si no hay ningun PIN almacenado, solicita al usuario que lo ingrese.
     */
    private void checkFirstRun() {
        String storedPin = preferences.getString(PIN_KEY, "");
        if (storedPin.isEmpty()) {
            showToast("Por favor, establezca un PIN de 4 dígitos");
        }
    }

    /**
     * Funcion para pasar a la siguiente actividad
     */
    private void proceedToMainActivity() {
        Intent intent = new Intent(Login.this, MainActivity.class);
        startActivity(intent);
        finish();
    }

    /**
     * Funcion reutilizable para mostrar un mensaje
     * @param message: Mensaje a mostrar
     */
    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

}