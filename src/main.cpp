#include <Arduino.h>

#include "hal/ESP8266I2C.hpp"
#include "drivers/display/SSD1306.hpp"
#include "utils/graphics/graphics.hpp"
#include "services/HelloWorldService.hpp"


ESP8266I2C i2c(Wire, 0x3D); // I2C address for SSD1306
drivers::display::SSD1306<64, 128, 8> display(i2c); // 64 rows, 128 columns, 8 pages
Graphics<64, 128, 8> gfx(display);
HelloWorldService<64, 128, 8> helloService(gfx);

uint16_t frameBuffer[128 * 8] = {0};

void setup(){

    i2c.init();
    display.init();
    helloService.sayHello();
    display.update(frameBuffer); // Update the display with the current buffer content
    delay(1000);

}

void loop(){

  display.clear();
  helloService.sayHello();
  display.update(frameBuffer); // Update the display with the current buffer content
  delay(1000);


  // display.clear();

  //   // 1. Draw a box around the very edges of the screen
  //   for(int x = 0; x < 128; x++) { 
  //       display.drawPixel(x, 0, 1);  // Top edge
  //       display.drawPixel(x, 63, 1); // Bottom edge
  //   }
  //   for(int y = 0; y < 64; y++) { 
  //       display.drawPixel(0, y, 1);  // Left edge
  //       display.drawPixel(127, y, 1); // Right edge
  //   }

  //   // 2. Draw a diagonal line
  //   for(int i = 0; i < 64; i++) { 
  //       display.drawPixel(i*2, i, 1); 
  //   }

  //   display.update(frameBuffer); // Update the display with the current buffer content
  //   delay(5000);

}
