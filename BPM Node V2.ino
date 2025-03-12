#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// Set your Board ID
#define BOARD_ID 2

// Define analog input pin for pulse sensor
#define PULSE_PIN 34  // GPIO34 (Analog Input)

// Wi-Fi SSID of the network for synchronization
const char *WIFI_SSID = "Vu Huy T3";

// MAC Address of the receiver (replace with actual MAC address)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure to send data
typedef struct struct_message {
    int id;        // Board ID
    float data;    // BPM data
    int readingId; // Unique reading ID
} struct_message;

struct_message myData;
unsigned int readingId = 0;

unsigned long previousMillis = 0;
const long interval = 1000; // Interval for sending data (1 second)

// Variables for pulse detection
int BPM = 0; // Beats Per Minute
int Signal;  // Raw signal from the pulse sensor
int IBI = 600; // Time between beats (ms)
boolean Pulse = false;  // Heartbeat detected flag
boolean QS = false;     // Valid beat flag

// Pulse detection variables
unsigned long sampleCounter = 0;
unsigned long lastBeatTime = 0;
int P = 512;  // Peak of the pulse wave
int T = 512;  // Trough of the pulse wave
int thresh = 525;
int amp = 100;

// ESP-NOW peer information
esp_now_peer_info_t peerInfo;

// Function Prototypes
void detectPulse();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void setupWiFiAndESPNow();
int32_t getWiFiChannel(const char *ssid);

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Configure pulse sensor pin
    pinMode(PULSE_PIN, INPUT);

    // Setup Wi-Fi and ESP-NOW
    setupWiFiAndESPNow();

    Serial.println("ESP32 Pulse Sensor with ESP-NOW ready.");
}

void loop() {
    // Detect pulse and calculate BPM
    detectPulse();

    // Send BPM data if a valid beat is detected
    if (QS) {
        QS = false; // Reset flag

        // Prepare data to send
        myData.id = BOARD_ID;
        myData.data = BPM;
        myData.readingId = readingId++;

        // Send data via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
        if (result == ESP_OK) {
            Serial.printf("BPM: %d - Data sent successfully\n", BPM);
        } else {
            Serial.println("Error sending the data");
        }
    }
}

void setupWiFiAndESPNow() {
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Get the Wi-Fi channel of the specified SSID
    int32_t channel = getWiFiChannel(WIFI_SSID);
    if (channel == 0) {
        Serial.println("Failed to find Wi-Fi channel. Check SSID.");
        return;
    }
    Serial.printf("Wi-Fi channel for SSID '%s': %d\n", WIFI_SSID, channel);

    // Set Wi-Fi channel
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register send callback
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = channel;  // Use the synchronized channel
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

// Get Wi-Fi channel for the given SSID
int32_t getWiFiChannel(const char *ssid) {
    Serial.println("Scanning for Wi-Fi networks...");
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == ssid) {
            return WiFi.channel(i);
        }
    }
    return 0; // Return 0 if SSID not found
}

// Pulse detection function
void detectPulse() {
    Signal = analogRead(PULSE_PIN);  // Read raw signal
    sampleCounter += 2;             // Increment time counter
    int N = sampleCounter - lastBeatTime; // Time since last beat

    // Detect the trough of the pulse wave
    if (Signal < thresh && N > (IBI / 5) * 3) {
        if (Signal < T) T = Signal; // Keep track of lowest point
    }

    // Detect the peak of the pulse wave
    if (Signal > thresh && Signal > P) {
        P = Signal; // Keep track of highest point
    }

    // Detect a heartbeat
    if (N > 250) { // Wait 250ms between beats
        if ((Signal > thresh) && !Pulse && (N > (IBI / 5) * 3)) {
            Pulse = true;
            IBI = sampleCounter - lastBeatTime; // Time between beats
            lastBeatTime = sampleCounter;

            // Calculate BPM
            BPM = 60000 / IBI;
            QS = true; // Valid beat detected
        }
    }

    // Reset Pulse flag when signal drops below threshold
    if (Signal < thresh && Pulse) {
        Pulse = false;
        amp = P - T;
        thresh = amp / 2 + T;
        P = thresh;
        T = thresh;
    }

    // Reset if no beat detected for 2.5 seconds
    if (N > 2500) {
        thresh = 512;
        P = 512;
        T = 512;
        lastBeatTime = sampleCounter;
        BPM = 0;
    }
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
