#include <node.h>
#include <iostream>
#include "epdif.h"

using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

// Display resolution
#define EPD_WIDTH       800
#define EPD_HEIGHT      480

// EPD 7IN5 V2 commands
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define IMAGE_PROCESS                               0x13
#define LUT_FOR_VCOM                                0x20
#define LUT_BLUE                                    0x21
#define LUT_WHITE                                   0x22
#define LUT_GRAY_1                                  0x23
#define LUT_GRAY_2                                  0x24
#define LUT_RED_0                                   0x25
#define LUT_RED_1                                   0x26
#define LUT_RED_2                                   0x27
#define LUT_RED_3                                   0x28
#define LUT_XON                                     0x29
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_CALIBRATION                     0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define TCON_RESOLUTION                             0x61
#define SPI_FLASH_CONTROL                           0x65
#define REVISION                                    0x70
#define GET_STATUS                                  0x71
#define AUTO_MEASUREMENT_VCOM                       0x80
#define READ_VCOM_VALUE                             0x81
#define VCM_DC_SETTING                              0x82

void SendCommand(unsigned char command) {
    EpdIf::DigitalWrite(DC_PIN, LOW);
    EpdIf::DigitalWrite(CS_PIN, LOW);
    EpdIf::SpiTransfer(command);
    EpdIf::DigitalWrite(CS_PIN, HIGH);
}
void SendData(unsigned char data) {
    EpdIf::DigitalWrite(DC_PIN, HIGH);
    EpdIf::DigitalWrite(CS_PIN, LOW);
    EpdIf::SpiTransfer(data);
    EpdIf::DigitalWrite(CS_PIN, HIGH);
}


void WaitUntilIdle(void) {
    while(EpdIf::DigitalRead(BUSY_PIN) == 0) {      //0: busy, 1: idle
        EpdIf::DelayMs(100);
    }
}

void Reset(void) {
    EpdIf::DigitalWrite(RESET_PIN, HIGH);
    EpdIf::DelayMs(200);
    EpdIf::DigitalWrite(RESET_PIN, LOW);
    EpdIf::DelayMs(2);
    EpdIf::DigitalWrite(RESET_PIN, HIGH);
    EpdIf::DelayMs(200);
}


void init(const FunctionCallbackInfo<Value>& args) {
    // std::cout << "init\n";

    Isolate* isolate = args.GetIsolate();
    int returnvalue = 0;

    if (EpdIf::IfInit() != 0) {
        returnvalue = -1;
    } else {
        Reset();

        SendCommand(POWER_SETTING);
        SendData(0x07);
        SendData(0x07); // VGH=20V,VGL=-20V
        SendData(0x3f); // VDH=15V
        SendData(0x3f); // DL=-15V

        SendCommand(POWER_ON);
        EpdIf::DelayMs(100);
        WaitUntilIdle();

        SendCommand(PANEL_SETTING);
        SendData(0x1F); // KW-3f   KWR-2F  BWROTP 0f       BWOTP 1f

        SendCommand(TCON_RESOLUTION);
        SendData(0x03); // source 800
        SendData(0x20);
        SendData(0x01); // gate 480
        SendData(0xE0);

        SendCommand(0x15);
        SendData(0x00);

        SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
        SendData(0x10);
        SendData(0x07);

        SendCommand(TCON_SETTING);
        SendData(0x22);
    }

    Local<Number> num = Number::New(isolate, returnvalue);
    args.GetReturnValue().Set(num);
}

void clear(const FunctionCallbackInfo<Value>& args) {
    SendCommand(DATA_START_TRANSMISSION_1);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        SendData(0x00);
    }

    SendCommand(IMAGE_PROCESS);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        SendData(0x00);
    }

    SendCommand(DISPLAY_REFRESH);

    EpdIf::DelayMs(100);
    WaitUntilIdle();

    args.GetReturnValue().Set(1);
}

void displayFrameBuffer( unsigned char* frame_buffer) {
    //SendCommand(DATA_START_TRANSMISSION_1);
    SendCommand(IMAGE_PROCESS);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        SendData(frame_buffer[i]);
    }

    SendCommand(DISPLAY_REFRESH);
    EpdIf::DelayMs(100);
    WaitUntilIdle();
}


void display(const FunctionCallbackInfo<Value>& args) {
    //Isolate* isolate = args.GetIsolate();

    v8::Local<v8::Uint8Array> view = args[0].As<v8::Uint8Array>();
    void *data = view->Buffer()->GetContents().Data();

    unsigned char* imagedata = static_cast<unsigned char*>(data);
    displayFrameBuffer(imagedata);
}





void InitAll(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "display", display);
  NODE_SET_METHOD(exports, "clear", clear);
}

NODE_MODULE(epd7x5, InitAll)
