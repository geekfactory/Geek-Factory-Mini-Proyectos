#include <GFMotor.h>
#include <RF24.h>
/**
  Definition of pins used in this project
*/
#define PIN_L_MOTOR_1 4
#define PIN_L_MOTOR_2 5
#define PIN_R_MOTOR_1 3
#define PIN_R_MOTOR_2 2
#define PIN_L_MOTOR_PWM 6
#define PIN_R_MOTOR_PWM 9

/**
  Objects for motor control
*/
GFMotor leftMotor(PIN_L_MOTOR_1, PIN_L_MOTOR_2, PIN_L_MOTOR_PWM);
GFMotor rightMotor(PIN_R_MOTOR_1, PIN_R_MOTOR_2, PIN_R_MOTOR_PWM);

/**
  NRF24L01 + interface object
*/
RF24 radio(7, 8);

/**
  Definicion de la estructura de cada paquete enviado por la interfaz inalambrica
*/
struct radio_packet {
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
  /** Estado de botón start */
  uint8_t start_btn;
  /** Estado de botón select */
  uint8_t select_btn;
};

struct radio_packet pkt;

const byte identificacion[6] = "00001";

/**
  Compute the speed of a pair of DC motors to control the direction of the car based
  on the joystick input.
  
  @param x The x value of the joystick input.
  @param y The y value of the hoystick input.
  @param sl Pointer to a variable used to place the result of the speed computation
  for left motor.
  @param sr Pointer to a variable used to place the result of the speed computation
  for right motor.
*/
void rcrx_compute_speed(int16_t x, int16_t y, int16_t* sl, int16_t* sr) {
  // initialize local variables
  int16_t ax = abs(x);
  int16_t ay = abs(y);
  int16_t mini = min(ax, ay);
  int16_t maxi = max(ax, ay);

  // speed is based on the biggest input value from the joystick
  int16_t speedr = map(maxi, 0, 512, 0, 100);
  // both motors turn at the same speed unless they're affected by joystick inputs
  int16_t speedl = speedr;
  // percentage of speed of the slowest motor referenced to the fastest motor
  int16_t ratio;

  // we use the relationship between x and y values to compute the duty cycle of the PWM signal
  if (x == 0) {
    ratio = 100;
  } else if (y == 0) {
    ratio = 0;
  } else if (ax == ay) {
    ratio = 50;
  } else if (ax > ay) {
    ratio = (mini * 50) / maxi;
  } else if (ax < ay) {
    ratio = 100 - ((mini * 50) / maxi);
  }

  // compute the percentage of the speed for the slowest motor related to the fastest motor
  // affect the output variable depending on the left or right direction of the joystick
  if (x < 0)
    speedl = ratio * speedr / 100;
  else if (x > 0)
    speedr = ratio * speedl / 100;

  // apply signs according to the forward / backward joystick position
  if (y >= 0) {
    *sl = speedl;
    *sr = speedr;
  } else if (y < 0) {
    *sl = -speedl;
    *sr = -speedr;
  }
}

/**
*/
void setup() {

  // inicializar motores
  rightMotor.begin();
  leftMotor.begin();

  // Preparar el radio NRF24L01+ para su uso
  radio.begin();
  radio.openReadingPipe(0, identificacion);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

/**
  Main program loop
*/
void loop() {
  int16_t speedr = 0;
  int16_t speedl = 0;
  static uint32_t lastPacket = 0;

  uint32_t now = millis();

  if (radio.available()) {
    // keep track of the time of the last received packet
    lastPacket = now;
    // read packet to the structure
    radio.read(&pkt, sizeof(struct radio_packet));
    // compute motor speed and generate PWM signals
    rcrx_compute_speed(pkt.x_thumb, pkt.y_thumb, &speedl, &speedr);
    leftMotor.setSpeed(speedl);
    rightMotor.setSpeed(speedr);
  } else {
    // turn off motors if no packet is received within 200 milliseconds
    if (now - lastPacket >= 200) {
      leftMotor.setSpeed(0);
      rightMotor.setSpeed(0);
    }
  }
