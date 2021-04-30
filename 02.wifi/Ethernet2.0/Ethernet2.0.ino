
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
 * GPIO17 - EMAC_CLK_OUT_180    nINT/REFCLK - LAN8720 XTAL1/CLKIN     4k7 Pulldown
 * GPIO22 - EMAC_TXD1           TX1
 * GPIO19 - EMAC_TXD0           TX0
 * GPIO21 - EMAC_TX_EN          TX_EN
 * GPIO26 - EMAC_RXD1           RX1
 * GPIO25 - EMAC_RXD0           RX0
 * GPIO27 - EMAC_RX_DV          CRS
 * GPIO23 - MDC                 MDC
 * GPIO18 - MDIO                MDIO
 * GND                          GND
 * 3V3                          VCC
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

void testFirebase()
{
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    //Set database read timeout to 1 minute (max 15 minutes)
    Firebase.setReadTimeout(fbdo, 1000 * 60);
    //tiny, small, medium, large and unlimited.
    //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
    Firebase.setwriteSizeLimit(fbdo, "tiny");

    //optional, set the decimal places for float and double data to be stored in database
    Firebase.setFloatDigits(2);
    Firebase.setDoubleDigits(6);

    /**
     * This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
     * irewall that allows only GET and POST requests.
       
       Firebase.enableClassicRequest(fbdo, true);
    */

    String path = "/Test";

    Serial.println("------------------------------------");
    Serial.println("Set double test...");

    for (uint8_t i = 0; i < 10; i++)
    {
        //Also can use Firebase.set instead of Firebase.setDouble
        if (Firebase.setDouble(fbdo, path + "/Double/Data" + (i + 1), ((i + 1) * 10) + 0.123456789))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            Serial.println("ETag: " + fbdo.ETag());
            Serial.print("VALUE: ");

            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }
    }

    Serial.println("------------------------------------");
    Serial.println("Get double test...");

    for (uint8_t i = 0; i < 10; i++)
    {
        //Also can use Firebase.get instead of Firebase.setInt
        if (Firebase.getInt(fbdo, path + "/Double/Data" + (i + 1)))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            Serial.println("ETag: " + fbdo.ETag());
            Serial.print("VALUE: ");

            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }
    }

    Serial.println("------------------------------------");
    Serial.println("Push integer test...");

    for (uint8_t i = 0; i < 5; i++)
    {
        //Also can use Firebase.push instead of Firebase.pushInt
        if (Firebase.pushInt(fbdo, path + "/Push/Int", (i + 1)))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.print("PUSH NAME: ");
            Serial.println(fbdo.pushName());
            Serial.println("ETag: " + fbdo.ETag());
            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }
    }

    Serial.println("------------------------------------");
    Serial.println("Push JSON test...");

    for (uint8_t i = 5; i < 10; i++)
    {

        json.clear().add("Data" + String(i + 1), i + 1);

        //Also can use Firebase.push instead of Firebase.pushJSON
        //Json string is not support in v 2.6.0 and later, only FirebaseJson object is supported.
        if (Firebase.pushJSON(fbdo, path + "/Push/Int", json))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.print("PUSH NAME: ");
            Serial.println(fbdo.pushName());
            Serial.println("ETag: " + fbdo.ETag());
            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }
    }

    Serial.println("------------------------------------");
    Serial.println("Update test...");

    for (uint8_t i = 0; i < 5; i++)
    {

        json.set("Data" + String(i + 1), i + 5.5);

        if (Firebase.updateNode(fbdo, path + "/float", json))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            //No ETag available
            Serial.print("VALUE: ");

            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }
    }
}
void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void setup()
{

    Serial.begin(115200);
    Serial.println();

    WiFi.onEvent(WiFiEvent);

    ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);


}

void loop()
{
    if (eth_connected)
    {
      testClient("google.com", 80);
//        testFirebase();
    }

    delay(10000);
}
