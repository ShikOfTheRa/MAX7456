#define MAX7456ADD_VM0          0x00
#define MAX7456ADD_STAT         0xA0
#define MAX7456ADD_DMAH         0x05
#define MAX7456ADD_DMAL         0x06
#define MAX7456ADD_DMM          0x04
#define MAX7456ADD_DMDI         0x07
#define MAX7456_RESET_PAL       0x4C
#define MAX7456_RESET_NTSC      0x0C



//------------------------------------------------------------------------
void MAX7456Setup(void) // Configure MAX hardware
{
  uint8_t t_reset  = MAX7456_RESET_NTSC;
  uint8_t t_srdata = 0;
  uint8_t t_spi    = 0;

  MAX7456DISABLE
  MAX7456HWRESET
  SPCR = (1 << SPE) | (1 << MSTR);
  SPSR = (1 << SPI2X);
  t_spi = SPSR;
  t_spi = SPDR;
  delay(10);
  MAX7456ENABLE
  delay(1000 / 10);
  spi_transfer(MAX7456ADD_STAT);
  t_srdata = spi_transfer(0xFF);
  t_srdata &= B00000011;
  MAX7456_video_standard = t_srdata;
  if (t_srdata == B00000001) {     // PAL
    t_reset = MAX7456_RESET_PAL;
    MAX7456_screen_size = 480;
  }
  else { // B00000010 for NTSC
    MAX7456_screen_size = 390;
  }
  MAX7456_Send(MAX7456ADD_VM0, t_reset);
  MAX7456DISABLE
}


void MAX7456_writebuffer(char *t_string, int t_address) // Copy from temporary buffer to full screen buffer at designated position
{
  char *t_screenp = &MAX7456_screen[t_address];
  while (*t_string) {
    *t_screenp++ = *t_string++;
  }
}


void MAX7456_writeconstant(const char *t_string, int t_address) // Copy program memory to full screen buffer at designated position
{
  char t_char;
  char *t_screenp = &MAX7456_screen[t_address];
  while ((t_char = (char)pgm_read_byte(t_string++)) != 0)
    * t_screenp++ = t_char;
}


void MAX7456_DrawScreen() // Display contents of full screen buffer and initialise for new data.
{
  uint16_t t_address;
  MAX7456ENABLE;
  MAX7456_Send(MAX7456ADD_DMAH, 0);
  MAX7456_Send(MAX7456ADD_DMAL, 0);
  MAX7456_Send(MAX7456ADD_DMM,  1);
  for (t_address = 0; t_address < MAX7456_screen_size; ++t_address) {
    MAX7456_Send(MAX7456ADD_DMDI, MAX7456_screen[t_address]);
    {
      MAX7456_screen[t_address] = ' ';
    }
  }
  MAX7456_Send(MAX7456ADD_DMDI, 0xFF);
  MAX7456_Send(MAX7456ADD_DMM, 0);
  MAX7456DISABLE
}


void MAX7456_Send(uint8_t add, uint8_t data)
{
  spi_transfer(add);
  spi_transfer(data);
}


void MAX7456CheckStatus(void) {
  uint8_t t_srdata;
  MAX7456ENABLE
  spi_transfer(MAX7456ADD_STAT);
  t_srdata = spi_transfer(0xFF);
  t_srdata &= B00000011;
  if (MAX7456_video_standard != t_srdata) {
    MAX7456Setup();
    return;
  }
  spi_transfer(0x80);
  t_srdata = spi_transfer(0xFF);

  if ((B00001000 & t_srdata) == 0)
    MAX7456Setup();
}


