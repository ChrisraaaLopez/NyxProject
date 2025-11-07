#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h> // Para manejar la página web

// ** CONFIGURACIÓN DE RED **
const char* ssid = "TU_RED_WIFI";
const char* password = "TU_PASSWORD";
const char* host_principal = "192.168.1.100"; // ¡IP DEL ESP32 PRINCIPAL!
const int port_principal = 80;

// ** CONFIGURACIÓN DE PINES (AI-THINKER) **
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define LED_GPIO_NUM 4 // Flash LED

WebServer server(80);

// HTML de la página web (MUY simplificado)
const char* INDEX_HTML = R"raw(
<html>
<head><title>ESP32-CAM Control</title></head>
<body>
<h1>Stream & Capture</h1>
<img id="stream" src="/stream" width="640" height="480"><br/>
<button onclick="capture()">Tomar Foto & Enviar</button>
<script>
    function capture() {
        fetch('/capture').then(response => {
            alert('Foto enviada al ESP32 Principal: ' + response.status);
        });
    }
</script>
</body>
</html>
)raw";

// FUNCIÓN CLAVE: Toma foto y la envía por HTTP POST
void handleCapture() {
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get(); // Captura de foto
    if (!fb) {
        server.send(500, "text/plain", "Fallo captura.");
        return;
    }

    HTTPClient http;
    String serverPath = "http://" + String(host_principal) + ":" + String(port_principal) + "/upload";
    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "image/jpeg");
    
    int httpResponseCode = http.POST(fb->buf, fb->len);
    esp_camera_fb_return(fb); // Limpia buffer

    if (httpResponseCode == 200) {
        server.send(200, "text/plain", "Foto enviada OK.");
    } else {
        server.send(500, "text/plain", "Error envio POST.");
    }
    http.end();
}

void startCameraServer() {
    // Manejo de la página principal y el botón de captura
    server.on("/", HTTP_GET, [](){
        server.send(200, "text/html", INDEX_HTML);
    });
    server.on("/capture", HTTP_GET, handleCapture);

    // Manejo del stream (código necesario para el streaming)
    // ... (Se requiere la función 'handle_jpg_stream' del ejemplo original)
    
    server.begin();
}

void setup() {
    // 1. Inicialización de Cámara (configuración recomendada)
    camera_config_t config;
    // ... (Configuración de la cámara: PCLK, VSYNC, etc. como en CameraWebServer)
    config.frame_size = FRAMESIZE_QVGA; 
    config.jpeg_quality = 10;
    // ... esp_camera_init(&config); ...

    // 2. Conexión WiFi
    WiFi.begin(ssid, password);
    // ... (Espera conexión) ...

    // 3. Inicia Servidor Web
    startCameraServer();
}

void loop() {
    server.handleClient();
}