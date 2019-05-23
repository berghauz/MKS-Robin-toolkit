#pragma once
// select one of
//#define MKS_ROBIN
#define MKS_ROBIN_MINI

#ifdef MKS_ROBIN
  #define LCD_BACKLIGHT_PIN   PG11
  #define LCD_RESET_PIN       PF6
  #define LED_PIN	      PB2
#endif

#ifdef MKS_ROBIN_MINI
  #define LCD_BACKLIGHT_PIN   PD13
  #define LCD_RESET_PIN       PC6
#endif
