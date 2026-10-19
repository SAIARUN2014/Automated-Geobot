/*
  Simple ESP32-CAM (AI-Thinker) sketch
  - Stream:  http://<ip>/stream
  - UI:      http://<ip>/
  - Check:   http://<ip>/check  (returns "TREE" or "NO_TREE")

  This is a simple tree-check demo using color detection.
*/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

// AI-Thinker camera pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

camera_config_t camera_config;

bool initCamera(camera_pix_format_t pix_fmt, framesize_t framesize) {
  esp_camera_deinit();

  camera_config.ledc_channel = LEDC_CHANNEL_0;
  camera_config.ledc_timer = LEDC_TIMER_0;
  camera_config.pin_d0 = Y2_GPIO_NUM;
  camera_config.pin_d1 = Y3_GPIO_NUM;
  camera_config.pin_d2 = Y4_GPIO_NUM;
  camera_config.pin_d3 = Y5_GPIO_NUM;
  camera_config.pin_d4 = Y6_GPIO_NUM;
  camera_config.pin_d5 = Y7_GPIO_NUM;
  camera_config.pin_d6 = Y8_GPIO_NUM;
  camera_config.pin_d7 = Y9_GPIO_NUM;
  camera_config.pin_xclk = XCLK_GPIO_NUM;
  camera_config.pin_pclk = PCLK_GPIO_NUM;
  camera_config.pin_vsync = VSYNC_GPIO_NUM;
  camera_config.pin_href = HREF_GPIO_NUM;
  camera_config.pin_sscb_sda = SIOD_GPIO_NUM;
  camera_config.pin_sscb_scl = SIOC_GPIO_NUM;
  camera_config.pin_pwdn = PWDN_GPIO_NUM;
  camera_config.pin_reset = RESET_GPIO_NUM;
  camera_config.xclk_freq_hz = 20000000;
  camera_config.pixel_format = pix_fmt;
  camera_config.frame_size = framesize;
  camera_config.jpeg_quality = 12;
  camera_config.fb_count = 1;

  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }
  return true;
}

const char* index_html = R"rawliteral(
<!doctype html>
<html>
  <head>
    <title>ESP32-CAM Tree Check</title>
  </head>
  <body>
    <h3>ESP32-CAM Stream</h3>
    <img src="/stream" style="max-width:100%;">
    <p>
      <button onclick="checkTree()">Check for tree</button>
      <span id="result"></span>
    </p>

    <script>
      function checkTree() {
        document.getElementById('result').innerText = 'Checking...';
        fetch('/check')
          .then(r => r.text())
          .then(t => document.getElementById('result').innerText = t)
          .catch(() => document.getElementById('result').innerText = 'Error');
      }
    </script>
  </body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleStream() {
  WiFiClient client = server.client();
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  client.print(response);

  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      break;
    }

    client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");
    esp_camera_fb_return(fb);

    delay(100);
  }
}

bool analyzeFrameForTree(camera_fb_t* fb) {
  if (!fb) return false;

  size_t len = fb->len;
  if (len < 4) return false;

  size_t pixels = len / 2;
  const uint8_t* buf = fb->buf;
  size_t greenCount = 0;

  for (size_t i = 0; i + 1 < len; i += 2) {
    uint16_t pix = buf[i] | (buf[i + 1] << 8);
    uint8_t r5 = (pix >> 11) & 0x1F;
    uint8_t g6 = (pix >> 5) & 0x3F;
    uint8_t b5 = pix & 0x1F;

    uint8_t r8 = (r5 * 255) / 31;
    uint8_t g8 = (g6 * 255) / 63;
    uint8_t b8 = (b5 * 255) / 31;

    if (g8 > 80 && g8 > r8 + 20 && g8 > b8 + 20) {
      greenCount++;
    }
  }

  float greenRatio = (float)greenCount / (float)pixels;
  Serial.printf("Green ratio: %.3f\n", greenRatio);

  return greenRatio > 0.12;
}

void handleCheck() {
  if (!initCamera(PIXFORMAT_RGB565, FRAMESIZE_QQVGA)) {
    server.send(500, "text/plain", "Camera init failed");
    return;
  }

  camera_fb_t* fb = esp_camera_fb_get();
  bool treeFound = false;

  if (fb) {
    treeFound = analyzeFrameForTree(fb);
    esp_camera_fb_return(fb);
  }

  if (!initCamera(PIXFORMAT_JPEG, FRAMESIZE_QVGA)) {
    Serial.println("Failed to restore JPEG mode");
  }

  if (treeFound) {
    server.send(200, "text/plain", "TREE");
  } else {
    server.send(200, "text/plain", "NO_TREE");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  if (!initCamera(PIXFORMAT_JPEG, FRAMESIZE_QVGA)) {
    Serial.println("Camera init failed");
    while (true) { delay(1000); }
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/check", HTTP_GET, handleCheck);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2);
}
