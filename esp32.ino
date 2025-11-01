// Combined SEND + RECEIVE example for MCP2515 (mcp_can library)

#include <SPI.h>
#include <mcp_can.h>

// -------------------- CONFIG --------------------
// MCP2515 wiring
//   CS  = 7
//   INT = 10
//   (If you’re on ESP32/ESP32-C3, see the SPI.begin(...) block below)

static const uint8_t PIN_CS  = 7;
static const uint8_t PIN_INT = 10;

// Choose MCP2515 crystal and bitrate:
#define MCP_CLK     MCP_8MHZ      // MCP_16MHZ or MCP_8MHZ (match your module!)
#define CAN_BAUD    CAN_250KBPS    // e.g. CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS

// Periodic TX payload (edit as needed)
uint8_t txData[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
const unsigned long TX_ID = 0x100; // standard 11-bit

// ------------------------------------------------

MCP_CAN CAN0(PIN_CS);

unsigned long t_last = 0;

void setup() {
  Serial.begin(115200);
  delay(50);

  // --- If on ESP32/ESP32-C3, set explicit SPI pins (adjust to your board) ---
  // Comment this block if you're on AVR (UNO/MEGA) or already using default SPI pins.
#if defined(ARDUINO_ARCH_ESP32)
  // Example pins for ESP32-C3 SuperMini:
  // SCK=4, MISO=5, MOSI=6, CS=PIN_CS
  SPI.begin(4, 5, 6, PIN_CS);
#else
  SPI.begin(); // default SPI pins on AVR, etc.
#endif

  // Init MCP2515
  if (CAN0.begin(MCP_ANY, CAN_BAUD, MCP_CLK) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully");
  } else {
    Serial.println("Error Initializing MCP2515");
    while (1) delay(100);
  }

  // Normal mode = ACK + TX enabled
  CAN0.setMode(MCP_NORMAL);

  pinMode(PIN_INT, INPUT); // /INT from MCP2515 (active LOW)

  Serial.println("SEND+RECV ready.");
}

void loop() {
  // ---------- Periodic TX (every 1000 ms) ----------
  unsigned long now = millis();
  if (now - t_last >= 1000) {
    t_last = now;

    byte sndStat = CAN0.sendMsgBuf(TX_ID, 0 /*std*/, 8, txData);
    if (sndStat == CAN_OK) {
      Serial.println("TX ok: id=0x100 len=8");
    } else {
      Serial.println("TX FAIL");
    }

    // (optional) modify payload so you see it changing
    txData[0]++;
  }

  // ---------- RX path ----------
  // Use the INT pin if wired; if not, you can poll with CAN0.checkReceive()
  if (digitalRead(PIN_INT) == LOW) {
    unsigned long rxId;
    uint8_t len = 0;
    uint8_t rxBuf[8];

    if (CAN0.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK) {
      printFrame(rxId, len, rxBuf);
    }
  }

  // If you prefer polling instead of INT pin, uncomment:
  /*
  if (CAN0.checkReceive() == CAN_MSGAVAIL) {
    unsigned long rxId;
    uint8_t len = 0;
    uint8_t rxBuf[8];
    if (CAN0.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK) {
      printFrame(rxId, len, rxBuf);
    }
  }
  */
}

// Pretty-print a CAN frame
void printFrame(unsigned long rxId, uint8_t len, const uint8_t *buf) {
  char line[128];

  const bool isExt = (rxId & 0x80000000UL);
  const bool isRTR = (rxId & 0x40000000UL);

  if (isExt) {
    snprintf(line, sizeof(line), "RX id=0x%08lX len=%u ext=1 rtr=%u data:",
             (rxId & 0x1FFFFFFFUL), (unsigned)len, isRTR ? 1u : 0u);
  } else {
    snprintf(line, sizeof(line), "RX id=0x%03lX len=%u ext=0 rtr=%u data:",
             rxId, (unsigned)len, isRTR ? 1u : 0u);
  }
  Serial.print(line);

  if (!isRTR) {
    for (uint8_t i = 0; i < len; i++) {
      snprintf(line, sizeof(line), " %02X", buf[i]);
      Serial.print(line);
    }
  }
  Serial.println();
}
