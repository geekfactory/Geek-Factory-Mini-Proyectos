/**
  Geek Factory - "Dale vuelo a tus proyectos"
  www.geekfactory.mx

  PROYECTOS CON ARDUINO: TERMOMETRO CON ARDUINO Y LM35DZ
  ARDUINO PROJECTS: DIGITAL THERMOMETER WITH ARDUINO AND LM35DZ

  CONSULTA EL ARCHIVO LICENCIA.TXT PARA CONOCER MÁS SOBRE LOS DERECHOS DE DISTRIBUCIÓN DE ESTE
  MATERIAL O LA SIGUIENTE URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  CHECK OUT THE LICENCE.TXT FILE TO LEARN MORE ABOUT THE DISTRIBUTION RIGHTS OF THIS WORK OR
  CHECK THE FOLLOWING URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  PUBLICADO BAJO LICENCIA ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL (CC BY-NC-SA 4.0).
  PUBLISHED UNDER THE ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL (CC BY-NC-SA 4.0)
  LICENCE.
  
  Resumen: Este programa implementa un termómetro básico con el sensor LM35DZ, el resultado de la
  medición de temperatura se muestra en un display LCD de 16 caracteres y 2 lineas.

  Overview: This program implements a basic thermometer using the LM35DZ sensor, the measurement
  is shown in the 16x2 character LCD display.
*/

/**
Librerias utilizadas por este proyecto
*/
#include <LiquidCrystal.h>

// variable para guardar el numero de pin donde conectaremos el sensor LM35DZ
const int pinsensor = A0;

// objeto para manejar la pantalla LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/**
  Función setup: se ejecuta una vez cuando encendemos el arduino
*/
void setup() {
  // utilizamos la referencia interna de voltaje del convertidor A/D
  // esta función configura la referencia del ADC ya sea interna o externa
  // para este programa utilizaremos la referencia interna de 1.1 volts
  analogReference(INTERNAL);

  // indicar a la librería que tenemos conectada una pantalla de 16x2
  lcd.begin(16, 2);
  // mover el cursor a la primera posición de la pantalla (0,0)
  // y al mismo tiempo borramos su contenido
  lcd.clear();
  // imprimir mensaje inicial en la pantalla
  lcd.print(" GEEKFACTORY.MX ");
  lcd.setCursor(0, 1);
  lcd.print("TERMOMETRO  LM35");

  // esperar cinco segundos antes de iniciar el programa principal
  delay(5000);
}

/**
   Función loop: se ejecuta continuamente mientras el arduino permanece encendido
*/
void loop() {
  // variable local para el valor del ADC que va en un rango de 0 a 1023
  int valadc = analogRead(pinsensor);

  // calculamos la temperatura con la fórmula siguiente
  float temperatura = (110.0 * valadc) / 1024.0;

  // borrar toda la pantalla
  lcd.clear();
  // imprimir a la pantalla una leyenda
  lcd.print("  TEMPERATURA   ");
  // mover el cursor a la segunda línea
  lcd.setCursor(5, 1);
  // imprimir el valor de la medición de temperatura
  lcd.print(temperatura, 1);
  // secuencia de escape para imprimir el simbolo de grados que forma
  // parte de los caracteres especiales de la pantalla LCD
  lcd.print(" \337C");

  // esperamos 5 segundos antes de realizar una nueva medición
  // podemos personalizar este retardo a nuestro gusto
  delay(5000);
}