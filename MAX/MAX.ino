
// By shiki
//------------------------------------------------------------------------
#include "Arduino.h"
const char testtext[] PROGMEM = "ABC";


// MAX7456
#define  MAX7456ENABLE            PORTD&=B10111111; 
#define  MAX7456DISABLE           PORTD|=B01000000; 
#define  MAX7456SETHARDWAREPORTS  DDRB|=B00101100;DDRB&=B11101111;DDRD|=B01000000;DDRD&=B11111011;
#define  MAX7456HWRESET           PORTB&=B11111011;delay(100);PORTB|=B00000100;
uint8_t  MAX7456_video_standard = 0;
uint16_t MAX7456_screen_size;
char     MAX7456_screen[480];          // Main screen ram for MAX7456
char     MAX7456_screen_buffer[20];    // Character buffer used to write to main screen



//------------------------------------------------------------------------
void setup()
{
  MAX7456SETHARDWAREPORTS // Set up hardware to support MAX IC
  MAX7456Setup();         // Initialise software for MAX IC
}



//------------------------------------------------------------------------
void loop() // Example
{
  MAX7456_writebuffer("123", (2*30)+10);
  MAX7456_writeconstant(testtext, (3*30)+10);
  MAX7456_DrawScreen();
  MAX7456CheckStatus();   // Verify MAX IC functionality / autodetect video signal type change
}



//------------------------------------------------------------------------
// Functions
uint8_t spi_transfer(uint8_t t_data)
{
  SPDR = t_data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
    ;
  return SPDR;                    // return the received byte
}
