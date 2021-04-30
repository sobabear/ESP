
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

//This example shows how to read, store and update database using get, set, push and update functions.

//This example is for ESP32 with LAN8720 Ethernet board.

/**
 * There are may sources for LAN8720 and ESP32 interconnection on the internet which may
 * work for your LAN8720 board.
 * 
 * Some methods worked unless no IP is available.
 * 
 * This modification and interconnection provided in this example are mostly worked as
 * the 50 MHz clock was created internally in ESP32 which GPIO 17 is set to be output of this clock
 * and feeds to the LAN8720 chip XTAL input.
 * 
 * The on-board LAN8720 50 MHz XTAL chip will be disabled by connect its enable pin or pin 1 to GND.
 * 
 * Pleae see the images in the folder "modified_LAN8720_board_images" for how to modify the LAN8720 board.
 * 
 * The LAN8720 Ethernet modified board and ESP32 board wiring connection.
 * 
 * ESP32                        LAN8720                       
 * 
 * GPIO17 - PHY_POWER   : NC - Osc. Enable - 4k7 Pulldown
 * GPIO22 - EMAC_TXD1   : TX1
 * GPIO19 - EMAC_TXD0   : TX0
 * GPIO21 - EMAC_TX_EN  : TX_EN
 * GPIO26 - EMAC_RXD1   : RX1
 * GPIO25 - EMAC_RXD0   : RX0
 * GPIO27 - EMAC_RX_DV  : CRS
 * GPIO00 - EMAC_TX_CLK : nINT/REFCLK (50MHz) - 4k7 Pullup
 * GPIO23 - SMI_MDC     : MDC
 * GPIO18 - SMI_MDIO    : MDIO
 * GND                  : GND
 * 3V3                  : VCC
 * 
*/

#include <FirebaseESP32.h>

#define FIREBASE_HOST "https://ygfirebasetest-default-rtdb.firebaseio.com/"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "QKxCxRK3tbuKgXFwzbIddadtjx61L3BaqCgKuDok"

#ifdef ETH_CLK_MODE
#undef ETH_CLK_MODE
#endif
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_OUT //RMII clock output from GPIO17

// Pin# of the enable signal for the external crystal oscillator (-1 to disable)
#define ETH_POWER_PIN -1

// Type of the Ethernet PHY (LAN8720 or TLK110)
#define ETH_TYPE ETH_PHY_LAN8720

// I²C-address of Ethernet PHY (0 or 1 for LAN8720, 31 for TLK110)
#define ETH_ADDR 1

// Pin# of the I²C clock signal for the Ethernet PHY
#define ETH_MDC_PIN 23

// Pin# of the I²C IO signal for the Ethernet PHY
#define ETH_MDIO_PIN 18

static bool eth_connected = false;


//Define FirebaseESP32 data object
FirebaseData fbdo;

FirebaseJson json;

unsigned long prevMillis = 0;

void printResult(FirebaseData &data);

String mRef = "/automate_factory/test/pi1/scale";
bool flag = true;

int count = 0;
void WiFiEvent(WiFiEvent_t event)
{
    //Do not run any function here to prevent stack overflow or nested interrupt
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Serial.println("ETH Started");
        //set eth hostname here
        ETH.setHostname("esp32-ethernet");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        eth_connected = true;
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
}


void customFirebase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Firebase.setReadTimeout(fbdo, 1000 * 60);
  Firebase.setwriteSizeLimit(fbdo, "tiny");
    json.set("/value", count);
      Firebase.updateNode(fbdo, mRef, json);
  delay(8);


}


void setup()
{

    Serial.begin(115200);
    Serial.println();

    WiFi.onEvent(WiFiEvent);

    ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);

    customFirebase();


}

void loop()
{

  if (eth_connected) {

    Serial.println("hello");
    delay(1000);
  } else {
    Serial.println("no enttter");
    delay(1000);
  }
  count++;
  Serial.print("a count is : ");
  Serial.println(count);
//    if (eth_connected && (millis() - prevMillis > 60000 || prevMillis == 0))
//    {
//        prevMillis = millis();
//        testFirebase();
//    }
}
