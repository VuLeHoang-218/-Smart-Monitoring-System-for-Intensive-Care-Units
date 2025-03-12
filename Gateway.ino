#include <esp_now.h>
#include <WiFi.h>
#include <NimBLEDevice.h>

// Replace with your network credentials (STATION)
const char* ssid = "Vu Huy T3";
const char* password = "20032009";

// BLE UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

NimBLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// Structure to receive data
typedef struct struct_message {
  int id;         // Board ID
  float data;     // Temperature or Pulse Value
  int readingId;  // Reading ID
} struct_message;

struct_message incomingReadings;

// Callback function to handle received ESP-NOW data
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  // Get the sender's MAC address
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.printf("Packet received from: %s\n", macStr);

  // Copy received data into the structure
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  // Debug print
  Serial.printf("Board ID: %u, Data: %4.2f, Reading ID: %d\n",
                incomingReadings.id, incomingReadings.data, incomingReadings.readingId);

  // Send data to BLE
  if (deviceConnected) {
    String bleMessage = "ID: " + String(incomingReadings.id) + ", Data: " + String(incomingReadings.data) + ", ReadingID: " + String(incomingReadings.readingId);
    pCharacteristic->setValue(bleMessage.c_str());
    pCharacteristic->notify();
  }
}

class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    deviceConnected = true;
    Serial.println("BLE Device Connected");
  }

  void onDisconnect(NimBLEServer* pServer) {
    deviceConnected = false;
    Serial.println("BLE Device Disconnected");
  }
};

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.printf("Connected to Wi-Fi. IP: %s\n", WiFi.localIP().toString().c_str());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW Initialized");

  // Initialize NimBLE
  NimBLEDevice::init("ESP32_BLE");
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID,
                     NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
                   );
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("NimBLE Initialized and Advertising");
}

void loop() {
  // Main loop does nothing, BLE notifications are sent in the ESP-NOW callback
}
