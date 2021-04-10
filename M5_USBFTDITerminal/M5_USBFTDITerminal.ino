/*************************************************************
  USB FTDI CDC terminal for M5Stack USB Host Shield
  Based on USBFTDILoopback for USB Host Shield ver2.0
  https://github.com/felis/USB_Host_Shield_2.0
  2021/04/10
  Customized for M5Stack by @tomorrow56
 *************************************************************/
#include <M5Stack.h>
#include "M5StackUpdater.h"

// https://github.com/totsucom/M5Stack_ScrollTextWindow
#include "ScrollTextWindow.h"
#define TOP_FIXED_HEIGHT       8  //Top fixed area
#define BOTTOM_FIXED_HEIGHT    8  //Bottom fixed area
#define TEXT_HEIGHT 8 // Height of textSize(1) (240 dots/15 lines)
#define TEXT_WIDTH  6 // WIDTH of textSize(1) (320 dots/26 chracters)

ScrollTextWindow *stw;              //ScrollText class

#include <cdcftdi.h>
#include <usbhub.h>

#include "pgmstrings.h"

int baud = 115200;

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

class FTDIAsync : public FTDIAsyncOper{
  public:
    uint8_t OnInit(FTDI *pftdi);
};

uint8_t FTDIAsync::OnInit(FTDI *pftdi){
  uint8_t rcode = 0;

  rcode = pftdi->SetBaudRate(baud);

  if (rcode){
    ErrorMessage<uint8_t>(PSTR("SetBaudRate"), rcode);
    return rcode;
  }
  rcode = pftdi->SetFlowControl(FTDI_SIO_DISABLE_FLOW_CTRL);

  if (rcode){
    ErrorMessage<uint8_t>(PSTR("SetFlowControl"), rcode);
  }

  return rcode;
}

USB              Usb;
//USBHub         Hub(&Usb);
FTDIAsync        FtdiAsync;
FTDI             Ftdi(&Usb, &FtdiAsync);

void setup(){
  // M5Stack::begin(LCDEnable, SDEnable, SerialEnable, I2CEnable);
  M5.begin(true, true, true, true);
  M5.Power.begin();
  //Wire.begin();
  //Serial.begin( 115200 );

  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  M5.Lcd.setBrightness(64);
  M5.Lcd.setTextSize(1);
  M5.Lcd.fillScreen(BLACK);

  //Fixed area text alignment
/*
//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
#define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10 // Centre character baseline
#define R_BASELINE 11 // Right character baseline
*/
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setTextColor(WHITE, BLUE);

  Serial.println(F("USB CDC FTDI Terminal"));

  //Top area
  M5.Lcd.fillRect(0, 0, SCREEN_WIDTH, TOP_FIXED_HEIGHT, BLUE);
  M5.Lcd.drawString("USB CDC FTDI Terminal test", SCREEN_WIDTH/2, TOP_FIXED_HEIGHT/2);

  Serial.println("Start");
 
  String USB_STATUS;
  if (Usb.Init() == -1) {
    M5.Lcd.setTextColor(RED, BLUE);
    Serial.println("OSC did not start.");
    USB_STATUS = "OSC did not start.";
  }else{
    M5.Lcd.setTextColor(GREEN, BLUE);
    USB_STATUS = "USB CDC Baud Rate:" + String(baud) + "bps";
  }

  //Bottom area
  M5.Lcd.fillRect(0, SCREEN_HEIGHT-BOTTOM_FIXED_HEIGHT, SCREEN_WIDTH, BOTTOM_FIXED_HEIGHT, BLUE);
  M5.Lcd.drawString(USB_STATUS, SCREEN_WIDTH/2, SCREEN_HEIGHT - BOTTOM_FIXED_HEIGHT/2);

  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.setTextColor(WHITE, BLACK);

  //Init ScrollText class
  stw = new ScrollTextWindow(TOP_FIXED_HEIGHT, BOTTOM_FIXED_HEIGHT, BLACK, TEXT_WIDTH, TEXT_HEIGHT);
  stw->cls();
}

void loop(){
  Usb.Task();

  if( Usb.getUsbTaskState() == USB_STATE_RUNNING ){
    uint8_t  rcode;
    uint8_t  buf[64];

    for (uint8_t i=0; i<64; i++){
      buf[i] = 0;
     }

    uint16_t rcvd = 64;

    rcode = Ftdi.RcvData(&rcvd, buf);

    if (rcode && rcode != hrNAK){
      ErrorMessage<uint8_t>(PSTR("Ret"), rcode);
    }

    if( rcvd > 2 ) { //more than 2 bytes received
      for(uint16_t i = 2; i < rcvd; i++ ){
        Serial.print((char)buf[i]);
        uint8_t  data = buf[i];

        if (data > 31 && data < 128) {
          stw->print((char)data);
        }

        if (data == '\t') {
          stw->print("  ");
        }

        if (data == '\r') {
          stw->print("\r\n");
        }
      }
    }
  }
}
