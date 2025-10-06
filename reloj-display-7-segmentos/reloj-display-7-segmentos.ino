/**
  -->     PROYECTOS CON ARDUINO: TERMOMETRO CON ARDUINO Y LM35      <--
  --> ARDUINO PROJECTS: DIGITAL THERMOMETER WITH ARDUINO AND LM35DZ <--
  -->           MATERIAL DE APOYO - SUPPORT MATERIAL                <--

  Potenciado por:
  Geek Factory - "Dale vuelo a tus proyectos"
  www.geekfactory.mx

  CONSULTA EL ARCHIVO LICENCIA.TXT PARA CONOCER MÁS SOBRE LOS DERECHOS DE DISTRIBUCIÓN DE ESTE
  MATERIAL O LA SIGUIENTE URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.
  CHECK OUT THE LICENCE.TXT FILE TO LEARN MORE ABOUT THE DISTRIBUTION RIGHTS OF THIS WORK OR
  CHECK THE FOLLOWING URL: HTTPS://WWW.GEEKFACTORY.MX/LICENCIA-CONTENIDO.

  PUBLICADO BAJO LICENCIA ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL
  (CC BY-NC-SA 4.0).
  PUBLISHED UNDER THE ATTRIBUTION-NONCOMMERCIAL-SHAREALIKE 4.0 INTERNATIONAL
  (CC BY-NC-SA 4.0) LICENCE.

  Resumen: Este código implementa un reloj con displays de 7 segmentos multiplexados. Para la base
  de tiempo se utiliza un módulo RTC DS3231. La fecha y hora pueden ajustarse con solo dos botones.

  Overview: This code implements a clock using multiplexed 7 segment displays. For the time base we
  use a DS3231 RTC module. Time and date can be adjusted using only two buttons.
*/

/**
  Required libraries for this project
*/
#include "GFDisplay7S.h"
#include "GFButton.h"
#include "GFRTC.h"
#include "TimerOne.h"

enum clock_modes {
  E_CLOCK_MODE_RUN = 0,
  E_CLOCK_MODE_ADJ,
};

enum clock_adjust {
  E_CLOCK_ADJUST_HOUR = 0,
  E_CLOCK_ADJUST_MINUTE,
  E_CLOCK_ADJUST_SECOND,
  E_CLOCK_ADJUST_DAY,
  E_CLOCK_ADJUST_MONTH,
  E_CLOCK_ADJUST_YEAR,
};


/**
  Buttons to set time and date
*/
GFButton setButton(3);
GFButton upButton(4);

/**
  Multiplexed 7 segment display object configured for 6 digits
*/
GFDisplay7S display((const uint8_t[]){ 5, 6, 7, 8, 9, 10, 11, 0xFF }, (const uint8_t[]){ 12, 13, A0, A1, A2, A3 }, 6);

/**
  This enumerated variable defines if the clock is running or being adjusted
*/
enum clock_modes mode = E_CLOCK_MODE_RUN;

/**
  This variable defines what component of the time and date is being adjusted
*/
uint8_t adjvar = E_CLOCK_ADJUST_HOUR;

/**
  Structure used to perform operations on time and date
*/
struct timelib_tm datetime;

/**
  Flag set when the RTC interrupt occurs
*/
volatile bool rtcflag = false;


/**
  Setup function: runs once on power on or reset
*/
void setup() {
  // initialize multiplexed 7 segments displays
  display.begin();

  // setup timer to refresh the displays at arround 55 Hz
  Timer1.initialize(500);
  Timer1.attachInterrupt(interrupt_display_refresh);

  // prepare the RTC library, display E1 error if cannot communicate with RTC chip
  GFRTC.begin(true);
  if (!GFRTC.isPresent()) {
    display.print("E1");
    for (;;)
      ;
  }
  // enable sqrwave output with 1 Hz frequency from RTC module
  GFRTC.setIntSqwMode(E_SQRWAVE_1_HZ);
  // attach interrupt, this calls isrhandler function on every falling edge
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), interrupt_rtc, FALLING);
}

/**
  Loop function: runs continuously as long as the board receives power
*/
void loop() {
  bool datetimedisp = false;

  switch (mode) {
    case E_CLOCK_MODE_RUN:
      // if we hold the set button for more than 5 seconds we enter clock adjustment mode
      if (setButton.wasLongPress()) {
        // clear press event
        setButton.wasPressed();
        // enter adjust mode  adjusting the hours variable
        adjvar = 0;
        clock_adj_blink(adjvar);
        // switch to ADJ mode
        mode = E_CLOCK_MODE_ADJ;
        break;
      }
      // check if we should display time or date
      datetimedisp = upButton.isPressed();
      // check if we received interrupt signal to update the time or date on the displays
      if (rtcflag) {
        // clear flag and display time
        rtcflag = false;
        // read date and time from RTC chip
        if (GFRTC.read(datetime)) {
          // check upButton status and display time or date accordingly
          (datetimedisp) ? clock_show_date() : clock_show_time();
        }
      }
      break;

    case E_CLOCK_MODE_ADJ:
      // adjust button long press exits clock adjustment mode
      if (setButton.wasLongPress()) {
        // save date to RTC chip
        GFRTC.write(datetime);
        // stop any blinking
        display.noBlink();
        // switch to RUN mode
        mode = E_CLOCK_MODE_RUN;
        break;
      }
      // short press changes the variable to adjust
      if (setButton.wasPressed()) {
        if (++adjvar >= 6)
          adjvar = 0;
        // show time or date according to the selected variable
        (adjvar < 3) ? clock_show_time() : clock_show_date();
        // blink digits of the variable we're adjusting
        clock_adj_blink(adjvar);
      }
      // up button changes the selected variable of datetime struct
      if (upButton.wasPressed()) {
        clock_increase_var(adjvar);
        (adjvar < 3) ? clock_show_time() : clock_show_date();
      }
      break;
  }
}

/**
  Blinks the appropiate digits when we're in adjustment mode
*/
void clock_adj_blink(uint8_t var) {
  // disable blinking
  display.noBlink();
  // blink the appropiate digits
  switch (var) {
    case E_CLOCK_ADJUST_HOUR:
    case E_CLOCK_ADJUST_DAY:
      display.blinkDigit(0);
      display.blinkDigit(1);
      break;
    case E_CLOCK_ADJUST_MINUTE:
    case E_CLOCK_ADJUST_MONTH:
      display.blinkDigit(2);
      display.blinkDigit(3);
      break;
    case E_CLOCK_ADJUST_SECOND:
    case E_CLOCK_ADJUST_YEAR:
      display.blinkDigit(4);
      display.blinkDigit(5);
      break;
  }
}

/**
  Increases the selected time or date variable and checks the limits for hour, minute, seconds,etc.
*/
void clock_increase_var(uint8_t var) {
  switch (var) {
    case E_CLOCK_ADJUST_HOUR:
      datetime.tm_hour = (datetime.tm_hour + 1 < 24) ? datetime.tm_hour + 1 : 0;
      break;
    case E_CLOCK_ADJUST_MINUTE:
      datetime.tm_min = (datetime.tm_min + 1 < 60) ? datetime.tm_min + 1 : 0;
      break;
    case E_CLOCK_ADJUST_SECOND:
      datetime.tm_sec = (datetime.tm_sec + 1 < 60) ? datetime.tm_sec + 1 : 0;
      break;
    case E_CLOCK_ADJUST_DAY:
      datetime.tm_mday = (datetime.tm_mday + 1 <= 31) ? datetime.tm_mday + 1 : 1;
      break;
    case E_CLOCK_ADJUST_MONTH:
      datetime.tm_mon = (datetime.tm_mon + 1 <= 12) ? datetime.tm_mon + 1 : 1;
      break;
    case E_CLOCK_ADJUST_YEAR:
      datetime.tm_year = (datetime.tm_year + 1 < 80) ? datetime.tm_year + 1 : 30;
      break;
  }
}

/**
  Show the time on the displays
*/
void clock_show_time() {
  display.home();
  clock_print_2_digits(datetime.tm_hour);
  clock_print_2_digits(datetime.tm_min);
  clock_print_2_digits(datetime.tm_sec);
}

/**
  Show the date on the displays
*/
void clock_show_date() {
  display.home();
  clock_print_2_digits(datetime.tm_mday);
  clock_print_2_digits(datetime.tm_mon);
  clock_print_2_digits(timelib_tm2y2k(datetime.tm_year));
}

/**
   Helper function to print always two digits
*/
void clock_print_2_digits(uint8_t number) {
  if (number < 10)
    display.write('0');
  display.print(number);
}

/**
  Timer interrupt takes care of updating the multiplexed 7 segment displays
*/
void interrupt_display_refresh() {
  display.process();
}

/**
  RTC interrupt makes the microcontroller to update the time and date shown on the displays
*/
void interrupt_rtc() {
  rtcflag = true;
}
