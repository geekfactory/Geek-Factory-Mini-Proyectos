/**
  Geek Factory - "Dale vuelo a tus proyectos"
  www.geekfactory.mx

  PROYECTOS CON ARDUINO: ESTACIÓN METEOROLÓGICA CON ARDUINO - PROGRAMA EN MODO TEXTO
  ARDUINO PROJECTS: ARDUINO WHEATHER STATION - TEXT BASED PROGRAM

  CONSULTA EL ARCHIVO LICENCIA.TXT PARA CONOCER MÁS SOBRE LOS DERECHOS DE DISTRIBUCIÓN DE ESTE
  MATERIAL O LA SIGUIENTE URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  CHECK OUT THE LICENCE.TXT FILE TO LEARN MORE ABOUT THE DISTRIBUTION RIGHTS OF THIS WORK OR
  CHECK THE FOLLOWING URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  PUBLICADO BAJO LICENCIA ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL (CC BY-NC-SA 4.0).
  PUBLISHED UNDER THE ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL (CC BY-NC-SA 4.0)
  LICENCE.
  
  Resumen: Este programa implementa una estación meteorológica con Arduino usando los sensores
  DHT11 y BMP280. La lectura de estos sensores se muentra en una pantalla OLED de 128X64 pixeles.

  Overview: This program implements a basic weather station using Arduino and the DHT11 and BMP280
  sensors. The readings are displayed in a 128x64 pixles OLED display.
*/

#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <U8x8lib.h>

const int pinSensorHumedad = 3;

/**
 * Objeto para el sensor de presión barométrica y sensor de temperatura
 */
Adafruit_BMP280 barometro;

/**
 * Objeto para sensor de humedad y temperatura DHT11
 * El primer parámetro indica el pin donde se conecta el sensor
 * El segundo parámetro es el tipo de sensor (DHT11, DHT22, etc.)
 */
DHT higrometro(pinSensorHumedad, DHT11);

/**
 * Objeto para realizar la interfaz con la pantalla OLED de 128x16
 * este constructor se utiliza para los módulos genéricos conectados
 * por I2C.
 */
U8X8_SSD1306_128X64_NONAME_HW_I2C oled(U8X8_PIN_NONE);

/**
 * Variables de tipo float para almacenar la lectura de los sensores
 */
float presion = 0, temperatura = 0, humedad = 0;

void setup() {
  // iniciar puerto serie
  Serial.begin(115200);

  // mostrar mensaje inicial
  Serial.println(F("----------------------------------------------------"));
  Serial.println(F("     ESTACIÓN METEOROLÓGICA CON ARDUINO - TEXTO     "));
  Serial.println(F("            https://www.geekfactory.mx              "));
  Serial.println(F("----------------------------------------------------"));

  // preparar el sensor DHT11 para su uso
  higrometro.begin();

  // preparar el sensor BMP280 para su uso
  if (!barometro.begin()) {
    Serial.println(F("Error: No se detecta sensor BMP280"));
    while (1)
      ;
  }

  // inicializar la pantalla OLED
  oled.begin();
  oled.setFlipMode(1);
  oled.setFont(u8x8_font_chroma48medium8_r);
}

void loop() {
  // esperar dos segundos para actualizar las mediciones
  delay(2000);

  // leer el sensor de temperatura (BMP280)
  temperatura = barometro.readTemperature();
  // leer la presión atmosferica (BMP280)
  presion = barometro.readPressure();
  // leer la humedad relativa (DHT11)
  humedad = higrometro.readHumidity();

  // escribir las mediciones a la pantalla OLED

  // mover el cursor al inicio de la pantalla
  oled.setCursor(0, 0);
  // escribir la leyenda "Temperatura"
  oled.print("Temperatura");
  // mover el cursor a la segunda línea de la pantalla
  oled.setCursor(0, 1);
  // escribir el valor de la variable
  oled.print(temperatura);

  // repetimos el procedimiento con la variable de humedad...
  oled.setCursor(0, 3);
  oled.print("Humedad");
  oled.setCursor(0, 4);
  oled.print(humedad);

  // ...finalmente con la presión atmosférica
  oled.setCursor(0, 6);
  oled.print("Presion");
  oled.setCursor(0, 7);
  oled.print(presion);

  // imprimir al puerto serie
  Serial.print("Temperatura:");
  Serial.println(temperatura);
  Serial.print("Presión:");
  Serial.println(presion);
  Serial.print("Humedad:");
  Serial.println(humedad);
}
