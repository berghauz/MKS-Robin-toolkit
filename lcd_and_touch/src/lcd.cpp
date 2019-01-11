#include <Arduino.h>

#include "lcd.h"
#include "font.h"

extern uint16_t reg00, lcdId;
extern uint16_t color, bgColor;
extern char text[];

void lcdInit() {
/*
  delay(100);
  pinMode(PF6, OUTPUT); // initialize LCD reset pin
  digitalWrite(PF6, LOW);
  delay(200);
  digitalWrite(PF6, HIGH);
  delay(200);
*/

  switch(lcdId) {
    case 0x1505: break; // R61505U
    case 0x8552: st7789v_Init(); break; // ST7789V
    case 0x8989: break; // SSD1289
    case 0x9325: break; // ILI9325
    case 0x9328: ili9328_Init(); break; // ILI9328
    case 0x9341: break; // ILI9341
    case 0x0404: break; // No LCD Controller detected
    default: break; // Unknown LCD Controller
  }
}

void lcdSetWindow(uint16_t Xmin, uint16_t Ymin, uint16_t XMax, uint16_t Ymax) {
  switch(lcdId) {
//    case 0x1505: break; // R61505U
    case 0x8552: st7789v_SetWindow(Xmin, Ymin, XMax, Ymax); break; // ST7789V
//    case 0x8989: break; // SSD1289
//    case 0x9325: break; // ILI9325
    case 0x9328: ili9328_SetWindow(Xmin, Ymin, XMax, Ymax); break; // ILI9328
//    case 0x9341: break; // ILI9341
    case 0x0404: break; // No LCD Controller detected
    default: // Unknown LCD Controller
      if (reg00 != 0)
        ili9328_SetWindow(Xmin, Ymin, XMax, Ymax); // try "0x20 0x21 0x22" commands
      else
        st7789v_SetWindow(Xmin, Ymin, XMax, Ymax); // try "0x2A 0x2B 0x2C" commands
      break;
  }
}

void lcdWriteRam(void) {
  switch(lcdId) {
//    case 0x1505: break; // R61505U
    case 0x8552: st7789v_WriteRam(); break; // ST7789V
//    case 0x8989: break; // SSD1289
//    case 0x9325: break; // ILI9325
    case 0x9328: ili9328_WriteRam(); break; // ILI9328
//    case 0x9341: break; // ILI9341
    case 0x0404: break; // No LCD Controller detected
    default: // Unknown LCD Controller
      if (reg00 == 0)
        ili9328_WriteRam(); // try "0x20 0x21 0x22" commands
      else
        st7789v_WriteRam(); // try "0x2A 0x2B 0x2C"  commands
      break;
      break;
  }
}

void lcdPrint(uint16_t x, uint16_t y, char *text) {
  uint8_t character;

  uint16_t i, j, k, l;
  for (l = 0; (*(uint8_t*)(text + l) != 0) && ((x + l * 8 + 8) < 320); l ++) {
    character = (*(uint8_t*)(text + l) < 32 || *(uint8_t*)(text + l) > 127) ? 0 : *(text + l) - 32;
    lcdSetWindow(x + l * 8, y, x + l * 8 + 7, y + 15);

    for (i = 0; i < 2; i++)
      for (j = 0; j < 8; j++)
        for (k = 0; k < 8; k++)
          LCD_IO_WriteData(font[character][i + 2 * k] & (128 >> j) ? color  : bgColor);
  }
}

void lcdFill(uint16_t color, uint32_t count) {
  while (count > 0) {
    dma_setup_transfer(FSMC_DMA_DEV, FSMC_DMA_CHANNEL, &color, DMA_SIZE_16BITS, &LCD->RAM, DMA_SIZE_16BITS, DMA_MEM_2_MEM);
    dma_set_num_transfers(FSMC_DMA_DEV, FSMC_DMA_CHANNEL, count > 65535 ? 65535 : count);
    dma_clear_isr_bits(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);
    dma_enable(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);

    while ((dma_get_isr_bits(FSMC_DMA_DEV, FSMC_DMA_CHANNEL) & 0x0A) == 0) {};
    dma_disable(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);

    count = count > 65535 ? count - 65535 : 0;
  }
}

void lcdClear(uint16_t color) {
  lcdSetWindow(0, 0, 319, 239);
  lcdFill(color, 76800);
}
