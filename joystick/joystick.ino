#include <RF24.h>

#define PIN_X_THUMB A0
#define PIN_Y_THUMB A1

#define PIN_RF24CE 9
#define PIN_RF24CS 10
#define PIN_LED_LINKLOSS 13

#define CFG_X_OFFSET -6
#define CFG_Y_OFFSET -0
#define CFG_X_DEADZONE 3
#define CFG_Y_DEADZONE 3


/**
  Definición de la estructura del paquete para la transmisión por radio
*/
struct joystick_packet {
  /** Analogo eje X */
  int16_t x_thumb;
  /** Analogo eje Y */
  int16_t y_thumb;
  /** Estado de botón A */
  uint8_t a_btn;
  /** Estado de botón B */
  uint8_t b_btn;
  /** Estado de botón C */
  uint8_t c_btn;
  /** Estado de botón D */
  uint8_t d_btn;
};

/**
Declaración de una estructura llamada "packet"
*/
struct joystick_packet packet;

/**
 * Objeto de la librería para la interfaz con el NRF24L01+
 */
RF24 radio(PIN_RF24CE, PIN_RF24CS);


void setup() {

  Serial.begin(115200);

  pinMode(PIN_LED_LINKLOSS, OUTPUT);

  // preparar el radio NRF24L01+ para su uso
  if (radio.begin()) {
    radio.setAutoAck(1);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setRetries(15, 15);
    radio.openWritingPipe((byte*)"RMOTE");
    radio.stopListening();
    radio.printDetails();
    Serial.println(F("RF24 init ok"));
  } else {
    Serial.println(F("RF24 init failure"));
    while (true)
      ;
  }
}

void loop() {
  joystick_task();
}

void joystick_task() {
  static uint32_t last = 0;
  static uint32_t lastAck = 0;
  uint32_t now = millis();

  if (now - last >= 50) {
    // actualizar tiempo de la ultima transmisión
    last = now;

    // leer joystick
    readJoystick(&packet);

    // leer botones (pendiente)
    readButtons(&packet);

    // escribir el paquete al radio
    if (radio.write(&packet, sizeof(struct joystick_packet))) {
      lastAck = now;
      digitalWrite(PIN_LED_LINKLOSS, LOW);
    }

    if (now - lastAck >= 500) {
      digitalWrite(PIN_LED_LINKLOSS, HIGH);
    }
  }
}

/**
  @brief Lee y acondiciona las señales de los botones
*/
void readButtons(struct joystick_packet* pkt) {
}

/**
  @brief Lee y acondiciona las señales del joystick

  Esta función lee los pines analógicos y procesa las señales provenientes del joystick.
  Procesa los límites y zona muerta del controlador analógico y coloca los valores en el
  paquete de datos que se transmitirá por el radio.

  @param pkt Apuntador a una estructura joystick_packet donde se almacenerá el valor de
  los ejes del joystick después de su procesamiento.
*/
void readJoystick(struct joystick_packet* pkt) {
  pkt->x_thumb = constrain(analogRead(PIN_X_THUMB) - 511 + (CFG_X_OFFSET), -512, 512);
  pkt->y_thumb = constrain(analogRead(PIN_Y_THUMB) - 511 + (CFG_Y_OFFSET), -512, 512);
  if (pkt->x_thumb >= -CFG_X_DEADZONE && pkt->x_thumb <= CFG_X_DEADZONE) pkt->x_thumb = 0;
  if (pkt->y_thumb >= -CFG_Y_DEADZONE && pkt->y_thumb <= CFG_Y_DEADZONE) pkt->y_thumb = 0;
  // Serial.print(pkt->x_thumb), Serial.print('\t'),Serial.println(pkt->y_thumb);
}