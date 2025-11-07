#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <FS.h>
// Cabeceras esenciales para el Reconocimiento Facial
#include "esp_camera.h"
#include "fb_gfx.h" 
#include "esp_face_detection.h"
#include "esp_face_recognition.h" 
// (Se asume que estas librerías están disponibles en el entorno de ESP32)

// ** CONFIGURACIÓN **
const char* ssid = "TU_RED_WIFI";
const char* password = "TU_PASSWORD";
const int RELAY_PIN = 23; // Pin para la cerradura

// ** VARIABLES DE RECONOCIMIENTO **
// Se debe entrenar una o varias caras y guardar su 'face_id' o 'enrolamiento'
// Ejemplo: Guardar datos de reconocimiento en un archivo de la SD.
// const char* FACE_DATA_PATH = "/faces.dat";

WebServer server(80);

// --- CONTROL DE CERRADURA ---
void openLock() {
    Serial.println("Acceso Concedido. Abriendo cerradura.");
    digitalWrite(RELAY_PIN, LOW); // Activa el relé (ajustar a HIGH o LOW según tu módulo)
    delay(5000); // 5 segundos abierta
    digitalWrite(RELAY_PIN, HIGH); // Cierra la cerradura
}

// --- RECONOCIMIENTO FACIAL (Función placeholder, requiere implementación detallada) ---
bool runFaceRecognition(const char* filename) {
    // 1. Leer imagen de SD a Buffer (frame buffer)
    File f = SD.open(filename);
    if (!f) return false;
    size_t f_size = f.size();
    uint8_t * buf = (uint8_t*) malloc(f_size);
    f.read(buf, f_size);
    f.close();

    // 2. Decodificar JPEG y crear un fb_gfx_t (necesario para el Face Recognition)
    // Esto es MUY complejo y requiere la estructura camera_fb_t y decodificadores.
    // En la práctica, es más fácil si el ESP-CAM envía el buffer ya decodificado (raw),
    // pero eso aumenta el tamaño. Asumiremos que puedes cargar y decodificar el JPEG.
    
    // 3. Ejecutar Detección y Reconocimiento
    // face_detection_init(config);
    // detect_faces(frame_buffer, &results);
    // face_recognize(frame_buffer, &face_box, &enrolment_data);
    
    // ** Simulación (DEBES REEMPLAZAR ESTO CON EL CÓDIGO REAL) **
    Serial.println("Simulando Reconocimiento...");
    bool recognized = random(0, 2) == 1; // 50% chance de éxito en simulación
    free(buf);
    return recognized; 
}


// --- FUNCIÓN CLAVE: Manejo de la carga de imagen por POST ---
void handleUpload() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.println("Iniciando recepción de foto...");
        // Abrir/crear archivo en SD
        File file = SD.open("/capture.jpg", FILE_WRITE); 
        file.close(); 

    } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Escribir los chunks de datos
        File file = SD.open("/capture.jpg", FILE_APPEND);
        file.write(upload.buf, upload.currentSize);
        file.close();

    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.printf("Foto recibida, %u bytes.\n", upload.totalSize);
        server.send(200, "text/plain", "OK");

        // ** PROCESAR LA IMAGEN RECIBIDA **
        bool recognized = runFaceRecognition("/capture.jpg");
        if (recognized) {
            openLock();
        } else {
            Serial.println("Acceso Denegado.");
        }

    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Serial.println("Carga abortada.");
        server.send(500, "text/plain", "Fallo.");
    }
}

void setup() {
    Serial.begin(115200);

    // 1. Configuración del relé
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Estado inicial: Cerradura cerrada

    // 2. Inicialización de la SD
    if (!SD.begin(5)) { // Asume CS pin 5
        Serial.println("Error SD.");
    } else {
        Serial.println("SD OK.");
    }

    // 3. Conexión WiFi
    WiFi.begin(ssid, password);
    // ... (Espera conexión y muestra IP) ...

    // 4. Configuración del Servidor (Ruta para recibir el POST)
    server.on("/upload", HTTP_POST, [](){}, handleUpload); 
    
    server.begin();
    Serial.println("Servidor iniciado.");
}

void loop() {
    server.handleClient();
}