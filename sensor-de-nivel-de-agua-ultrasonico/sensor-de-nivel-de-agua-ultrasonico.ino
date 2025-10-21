/**
  Geek Factory - "Dale vuelo a tus proyectos"
  www.geekfactory.mx

  PROYECTOS CON ARDUINO: SENSOR DE NIVEL DE AGUA ULTRASÓNICO
  ARDUINO PROJECTS: ULTRASONIC WATER LEVEL SENSOR

  CONSULTA EL ARCHIVO LICENCIA.TXT PARA CONOCER MÁS SOBRE LOS DERECHOS DE DISTRIBUCIÓN DE ESTE
  MATERIAL O LA SIGUIENTE URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  CHECK OUT THE LICENCE.TXT FILE TO LEARN MORE ABOUT THE DISTRIBUTION RIGHTS OF THIS WORK OR
  CHECK THE FOLLOWING URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  PUBLICADO BAJO LICENCIA ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL (CC BY-NC-SA 4.0).
  PUBLISHED UNDER THE ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL (CC BY-NC-SA 4.0)
  LICENCE.
  
  Resumen: Este programa permite conocer el nivel de líquido contenido en un recipiente mediante
  el uso de un sensor ultrasónico colocado en la parte superior y que mide la distancia con la
  superficie del líquido. Los datos se envían al monitor serial de Arduino y se muestran en una
  pantalla LCD 16X2.

  Overview: This program determines the level of a liquid in a container by using an ultrasonic
  sensor on the top of the tank that measures the distance to the liquid surface. Data is sent to
  the serial port and also displayed on a 16x2 screen.
*/
#include <LiquidCrystal.h>
#include <GFUltrasonic.h>
#include <GFLed.h>

// configurar aqui el pin trigger del sensor
#define CONFIG_LEVEL_SENSOR_TRIG_PIN 2
// configurar aqui el pin echo del sensor
#define CONFIG_LEVEL_SENSOR_ECHO_PIN 3
// configurar la distancia maxima a detectar
#define CONFIG_LEVEL_SENSOR_MAX_DISTANCE 200l
// configurar la capacidad máxima del recipiente
#define CONFIG_LEVEL_SENSOR_MAX_LITERS 5000l
// configurar la distancia cuando el recipiente está lleno
#define CONFIG_LEVEL_SENSOR_FULL 30
// configurar la distancia cuando el recipiente está vacío
#define CONFIG_LEVEL_SENSOR_EMPTY 178
// configurar el nivel de porcentaje donde empezará a funcionar el led
#define CONFIG_LEVEL_SENSOR_ALERT_LEVEL 20

// Objeto para manejar la pantalla LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// Objeto para la interfaz con el sensor ultrasónico
GFUltrasonic sonar(CONFIG_LEVEL_SENSOR_TRIG_PIN, CONFIG_LEVEL_SENSOR_ECHO_PIN, CONFIG_LEVEL_SENSOR_MAX_DISTANCE);
// LED indicador de estado
GFLEDRGB alarmLED(10, 11, 12);

uint32_t lastm = 0;

// funcion de setup de arduino
void setup() {
  // iniciar el puerto serie
  Serial.begin(115200);

  // mostrar mensaje inicial
  Serial.println(F("----------------------------------------------------"));
  Serial.println(F("   PROYECTO DE SENSOR DE NIVEL DE AGUA CON ARDUINO  "));
  Serial.println(F("            https://www.geekfactory.mx              "));
  Serial.println(F("----------------------------------------------------"));

  // iniciar el sensor ultrasónico
  sonar.begin();

  // iniciar pantalla LCD
  lcd.begin(16, 2);

  // iniciar indicador LED
  alarmLED.begin();
  alarmLED.setColor(GFLEDColor::GREEN);
}
// ciclo principal del programa
void loop() {
  // esperar entre mediciones
  if (millis() - lastm >= 5000) {
    lastm = millis();
    // obtener medicion de tiempo de viaje del sonido y guardar en variable us
    int32_t cm = sonar.read();
    // convertir distancia en porcentaje
    int32_t percent = map(cm, CONFIG_LEVEL_SENSOR_EMPTY, CONFIG_LEVEL_SENSOR_FULL, 0, 100);
    // convertir porcentaje a litros
    int32_t liters = percent * CONFIG_LEVEL_SENSOR_MAX_LITERS / 100;

    // imprimir al puerto serial
    Serial.print(F("Distancia / porcentaje / litros: "));
    Serial.print(cm);
    Serial.print(' ');
    Serial.print(percent);
    Serial.print(' ');
    Serial.print(liters);
    Serial.print(' ');
    Serial.println();

    // imprimir a la pantalla LCD
    lcd.clear();
    lcd.print(F("Nivel: "));
    lcd.print(percent);
    lcd.print('%');
    lcd.setCursor(0, 1);
    lcd.print(F("Volumen: "));
    lcd.print(liters);
    lcd.print('L');

    // led de alerta, enciende en rojo cuando el nivel esta por debajo del umbral configurado
    if (percent > CONFIG_LEVEL_SENSOR_ALERT_LEVEL) {
      alarmLED.setColor(GFLEDColor::GREEN);
      alarmLED.setFlashCount(1);
    } else {
      alarmLED.setColor(GFLEDColor::RED);
      alarmLED.setFlashCount(3);
    }
  }
  // parpadeo de led
  alarmLED.process();
}