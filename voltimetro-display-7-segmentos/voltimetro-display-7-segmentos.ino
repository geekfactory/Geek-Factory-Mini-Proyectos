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
  
  Resumen: Este programa implementa un voltimetro digital utilizando un display de 7 segmentos
  multiplexado de 4 digitos.

  Overview: This program implements a digital voltmeter using a 4 digit, 7 segments multiplexed
  display.
*/

/**
Librerias utilizadas por este proyecto
*/
#include <GFDisplay7S.h>
#include <TimerOne.h>

// arreglo de pines donde se conectan los segmentos
const uint8_t pinesSegmentos[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };
// arreglo de pines comunes de los displays
const uint8_t pinesComunes[4] = { 10, 11, 12, 13 };
// objeto para gestionar el display
GFDisplay7S display(pinesSegmentos, pinesComunes, 4, 3000);

// referencia interna (UNO)
const float voltajeRef = 1.1;
// resistencia superior del divisor (270k)
const float rTop = 270000.0;
// resistencia inferior (10k)
const float rBottom = 10000.0;

// pin de entrada analógico donde mediremos el voltaje
const int pinEntradaAnalogico = A0;

/**
  Función setup: se ejecuta una vez cuando encendemos el arduino
*/
void setup() {
  // usar la referencia interna de 1.1 V para el ADC
  analogReference(INTERNAL);

  // iniciar la librería de display de 7 segmentos
  display.begin();

  // preparar el timer para generar una interrupción cada 3 ms (3000 uS)
  Timer1.initialize(3000);
  // la función actualizarDisplay se ejecuta en cada interrupción
  Timer1.attachInterrupt(actualizarDisplayInterrupcion);
}

/**
   Función loop: se ejecuta continuamente mientras el arduino permanece encendido
*/
void loop() {
  static uint32_t ultimaActualizacion = 0;

  // actualizar el valor del voltaje cada 250 ms
  if (millis() - ultimaActualizacion >= 250) {
    // guardar referencia del tiempo donde hicimos la lectura
    ultimaActualizacion = millis();

    // leer el pin analógico
    int lecturaADC = analogRead(pinEntradaAnalogico);
    // calcular el voltaje en el pin ADC
    float voltajeMedido = (lecturaADC / 1023.0) * voltajeRef;
    // calcular el voltaje real de entrada usando el divisor resistivo
    float voltajeEntrada = voltajeMedido * (rTop + rBottom) / rBottom;

    // borrar el display
    display.clear();
    // mostrar el resultado con 2 decimales
    display.print(voltajeEntrada, 2);
  }
}

/**
  Función que ejecutaremos en la rutina de servicio de interrupción del timer
*/
void actualizarDisplayInterrupcion() {
  // actualizar el display en la ISR
  display.process();
}
