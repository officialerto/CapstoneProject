// HTTP istekleri yapmak ve WiFi bağlantısını yönetmek için kullanılır.
#include <ESP8266HTTPClient.h> 
#include <ESP8266WiFi.h> 

// Asenkron web sunucusu ve TCP bağlantıları için kullanılır.
#include <ESPAsyncTCP.h> 
#include <ESPAsyncWebServer.h> 

#include <WebSocketsServer.h> // WebSocket sunucusunu yönetir.

#include <Wire.h> // Seri haberleşme protokol kütüph. I2C

#include <Adafruit_AMG88xx.h> // AMG8833 8x8 termal kamera sensörünü yönetmek için kullanılır.

#include <MPU6050_tockn.h> // MPU6050 ivme ve jiroskop sensörünü yönetmek için kullanılır.

const char* ssid = "***";
const char* password = "***";

const char* serverName = "http://192.168.0.11:5000/data";  // Verilerin gönderileceği sunucu adresi.

// Asenkron web ve WebSocket sunucuları oluşturulur.
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Sensör kütüphanelerini kullanmak için nesne türetiyoruz
Adafruit_AMG88xx amg;
MPU6050 mpu6050(Wire);

// Referans değerleri
float ref_ax = 0;
float ref_ay = 0;
float ref_az = 0;

// Bu HTML şablonu, tarayıcıda sensör verilerini gösterecek web sayfasını oluşturur. 
// WebSocket üzerinden alınan veriler dinamik olarak bu sayfada güncellenir.
char html_template[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 Sensor Data</title>
    <style>
        body {
            max-width: 600px;
            margin: auto;
            font-family: Arial, Helvetica, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            padding: 20px;
        }
        h1 {
            color: #333;
        }
        #sensorData {
            font-size: 18px;
            margin: 10px 0;
            background-color: #fff;
            padding: 10px;
            border-radius: 5px;
            box-shadow: 0 0 5px rgba(0, 0, 0, 0.1);
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        th, td {
            padding: 10px;
            border: 1px solid #ddd;
            text-align: left;
        }
        th {
            background-color: #f2f2f2;
        }
        .temperature-grid {
            display: grid;
            grid-template-columns: repeat(8, 1fr);
            gap: 2px;
            margin-top: 20px;
        }
        .temperature-cell {
            padding: 10px;
            border: 1px solid #ddd;
            background-color: #fff;
            color: #fff;
        }
        .alert {
            color: #fff;
            background-color: #ff4c4c;
            padding: 10px;
            border-radius: 5px;
            margin-top: 20px;
            box-shadow: 0 0 10px rgba(255, 76, 76, 0.5);
            font-size: 20px;
            font-weight: bold;
        }
    </style>
    <script>
        function getColorForTemperature(temp, minTemp, maxTemp) {
            var ratio = 2 * (temp - minTemp) / (maxTemp - minTemp);
            var b = Math.max(0, 255 * (1 - ratio));
            var r = Math.max(0, 255 * (ratio - 1));
            var g = 255 - b - r;
            return "rgb(" + Math.round(r) + "," + Math.round(g) + "," + Math.round(b) + ")";
        }

        var socket = new WebSocket("ws://" + location.host + ":81");
        socket.onopen = function(e) {
            console.log("[socket] socket.onopen ");
        };
        socket.onerror = function(e) {
            console.log("[socket] socket.onerror ");
        };
        socket.onmessage = function(e) {
            console.log("[socket] " + e.data);
            var data = JSON.parse(e.data);

            document.getElementById("accX").innerHTML = data.acceleration.x.toFixed(2);
            document.getElementById("accY").innerHTML = data.acceleration.y.toFixed(2);
            document.getElementById("accZ").innerHTML = data.acceleration.z.toFixed(2);

            var temperatureGrid = document.getElementById("temperatureGrid");
            temperatureGrid.innerHTML = "";

            var minTemp = Math.min(...data.temperature);
            var maxTemp = Math.max(...data.temperature);

            for (var i = 0; i < data.temperature.length; i++) {
                var cell = document.createElement("div");
                cell.className = "temperature-cell";
                cell.innerHTML = data.temperature[i].toFixed(2) + " °C";
                cell.style.backgroundColor = getColorForTemperature(data.temperature[i], minTemp, maxTemp);
                temperatureGrid.appendChild(cell);
            }

            if (data.alert) {
                document.getElementById("alert").innerHTML = data.alert;
            } else {
                document.getElementById("alert").innerHTML = "";
            }
        };
    </script>
</head>
<body>
    <div>
        <h1>Sensor Data</h1>
    </div>
    <div id="sensorData">
        <h2>Acceleration</h2>
        <table>
            <tr>
                <th>X</th>
                <td id="accX"></td>
            </tr>
            <tr>
                <th>Y</th>
                <td id="accY"></td>
            </tr>
            <tr>
                <th>Z</th>
                <td id="accZ"></td>
            </tr>
        </table>
        <h2>Temperature</h2>
        <div id="temperatureGrid" class="temperature-grid">
            <!-- 8x8 grid cells will be inserted here by JavaScript -->
        </div>
        <p id="alert" class="alert"></p>
    </div>
</body>
</html>
)=====";

// Bu fonksiyon, WebSocket bağlantı olaylarını işler.
// Bağlantı kurulduğunda, bağlantı kesildiğinde veya metin/binary veriler alındığında ne yapılacağını belirler.
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "0");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      break;
  }
}

// Bu fonksiyon, sensörleri başlatır, kalibrasyon yapar, WiFi'ya bağlanır ve web sunucusunu başlatır.
void setup(void) {
  Serial.begin(115200);
  delay(1000);
  
  Wire.begin();
  mpu6050.begin();
  Serial.println("MPU6050 sensor initialized.");

//OFFSET
  mpu6050.calcGyroOffsets(true);
  ref_ax = mpu6050.getAccX();
  ref_ay = mpu6050.getAccY();
  ref_az = mpu6050.getAccZ();
  Serial.println("MPU6050 sensor calibrated.");

  bool status = amg.begin();
  if (!status) {
    Serial.println("AMG8833 sensor not found. Please check connections!");
    while (1);
  }
  Serial.println("AMG8833 sensor initialized.");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html_template);
  });
  server.begin();
}


void loop(void) {
  webSocket.loop();

  // Sensör verilerini güncelle
  mpu6050.update();

  float ax = mpu6050.getAccX() - ref_ax;
  float ay = mpu6050.getAccY()- ref_ay;
  float az = mpu6050.getAccZ()- ref_az;

  float totalAcc = sqrt(ax * ax + ay * ay + az * az); // Pisagor Teoremi ile Toplam İvme Hesaplama
  // Bu formül, üç boyutlu uzayda bir noktadan başka bir noktaya olan düz çizgi mesafesini hesaplamak için Pisagor teoreminden türetilmiştir.
  // İvme vektörünün büyüklüğünü bulmak için, her bir bileşenin karesini alıp toplayarak ve ardından karekökünü alarak net ivmeyi hesaplamış olursunuz.

  // AMG8833 sensöründen 8x8 sıcaklık verilerini okur.
  float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
  amg.readPixels(pixels);

  // JSON formatında sensör verilerini hazırlar. (data)
  String data = "{\"acceleration\": {\"x\": " + String(ax) + ", \"y\": " + String(ay) + ", \"z\": " + String(az) + "}, \"temperature\": [";
  for (int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    data += String(pixels[i]);
    if (i < AMG88xx_PIXEL_ARRAY_SIZE - 1) {
      data += ",";
    }
  }
  data += "]";

  // Eğer ivme değeri eşik değerini aşıyorsa uyarı
  if (totalAcc > 1.35) {
    data += ", \"alert\": \"Bir şey düştü!\"";
  } else {
    data += ", \"alert\": \"Her şey yolunda\""; 
  }

  data += "}";

  // Sensör verilerini tüm bağlı WebSocket istemcilerine gönderir.
  webSocket.broadcastTXT(data);

   // Sensör verilerini belirli bir sunucuya POST isteğiyle gönderir.
   if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(data);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }

  delay(1000);
}
