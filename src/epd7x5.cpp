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
#define DATA_START_TRANSMISSION_2                   0x13
#define IMAGE_PROCESS                               0x13
#define LUT_FOR_VCOM                                0x20

#define LUT_WHITE_TO_WHITE                          0x21
#define LUT_BLACK_TO_WHITE                          0x22
#define LUT_WHITE_TO_BLACK                          0x23
#define LUT_BLACK_TO_BLACK                          0x24

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
#define PARTIAL_WINDOW                              0x90
#define PARTIAL_IN                                  0x91
#define PARTIAL_OUT                                 0x92

const unsigned char lut_vcom0[] =
{
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x00, 0x17, 0x17, 0x00, 0x00, 0x02,
0x00, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_vcom0_quick[] =
{
0x00, 0x0E, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_ww[] ={
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_ww_quick[] ={
0xA0, 0x0E, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


const unsigned char lut_bw[] ={
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bw_quick[] ={
0xA0, 0x0E, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bb[] ={
0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bb_quick[] ={
0x50, 0x0E, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_wb[] ={
0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_wb_quick[] ={
0x50, 0x0E, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void SendCommand(unsigned char command) {
    EpdIf::DigitalWrite(DC_PIN, LOW);
    //EpdIf::DigitalWrite(CS_PIN, LOW);
    EpdIf::SpiTransfer(command);
    //EpdIf::DigitalWrite(CS_PIN, HIGH);
}
void SendData(unsigned char data) {
    EpdIf::DigitalWrite(DC_PIN, HIGH);
    //EpdIf::DigitalWrite(CS_PIN, LOW);
    EpdIf::SpiTransfer(data);
    //EpdIf::DigitalWrite(CS_PIN, HIGH);
}


void WaitUntilIdle(void) {
    while(EpdIf::DigitalRead(BUSY_PIN) == 0) {      //0: busy, 1: idle
        EpdIf::DelayMs(1);
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

    //EpdIf::DelayMs(100);
    WaitUntilIdle();

    args.GetReturnValue().Set(1);
}

void displayFrameBuffer( unsigned char* frame_buffer) {
    //SendCommand(DATA_START_TRANSMISSION_1);
    SendCommand(IMAGE_PROCESS);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        SendData(frame_buffer[i]);
    }

    //SendCommand(DISPLAY_REFRESH);
    //EpdIf::DelayMs(100);
    //WaitUntilIdle();
}


void display(const FunctionCallbackInfo<Value>& args) {
    //Isolate* isolate = args.GetIsolate();

    v8::Local<v8::Uint8Array> view = args[0].As<v8::Uint8Array>();
    void *data = view->Buffer()->GetContents().Data();

    unsigned char* imagedata = static_cast<unsigned char*>(data);
    displayFrameBuffer(imagedata);
}

void refresh(const FunctionCallbackInfo<Value>& args) {
    SendCommand(DISPLAY_REFRESH);
}

void waitUntilIdle(const FunctionCallbackInfo<Value>& args) {
    WaitUntilIdle();
}

void setLutQuick(const FunctionCallbackInfo<Value>& args) {
    unsigned int count;
    SendCommand(LUT_FOR_VCOM);                            //vcom
    for(count = 0; count < 44; count++) {
        SendData(lut_vcom0_quick[count]);
    }

    SendCommand(LUT_WHITE_TO_WHITE);                      //ww --
    for(count = 0; count < 42; count++) {
        SendData(lut_ww_quick[count]);
    }

    SendCommand(LUT_BLACK_TO_WHITE);                      //bw r
    for(count = 0; count < 42; count++) {
        SendData(lut_bw_quick[count]);
    }

    SendCommand(LUT_WHITE_TO_BLACK);                      //wb w
    for(count = 0; count < 42; count++) {
        SendData(lut_wb_quick[count]);
    }

    SendCommand(LUT_BLACK_TO_BLACK);                      //bb b
    for(count = 0; count < 42; count++) {
        SendData(lut_bb_quick[count]);
    }
}

void setPartialWindow(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    v8::Local<v8::Uint8Array> view = args[0].As<v8::Uint8Array>();
    void *data = view->Buffer()->GetContents().Data();

    unsigned char* imagedata = static_cast<unsigned char*>(data);

    int32_t x = args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int32_t y = args[2]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int32_t width = args[3]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int32_t height = args[4]->Int32Value(isolate->GetCurrentContext()).ToChecked();

    //std::cout << "width: " << std::to_string(width) << std::endl;
    //std::cout << "height: " << std::to_string(height) << std::endl;

    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + width  - 1) >> 8);
    SendData(((x & 0xf8) + width  - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + height - 1) >> 8);
    SendData((y + height - 1) & 0xff);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default)
    // DelayMs(2);
    //EpdIf::DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    for(int i = 0; i < width * height / 8; i++) {
        SendData(imagedata[i]);
    }

    //for(int i = 0; i < width  / 8 * height; i++) {
    //    SendData(0x00);
    //}

    //EpdIf::DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: transmit partial data to the SRAM.  The final parameter chooses between dtm=1 and dtm=2
 */
void EPDSetPartialWindow(const unsigned char* buffer_black, int x, int y, int w, int l, int dtm) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default)
    // DelayMs(2);
    SendCommand((dtm == 1) ? DATA_START_TRANSMISSION_1 : DATA_START_TRANSMISSION_2);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(buffer_black[i]);
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(0x00);
        }
    }
    // DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

void InitAll(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "display", display);
  NODE_SET_METHOD(exports, "clear", clear);
  NODE_SET_METHOD(exports, "refresh", refresh);
  NODE_SET_METHOD(exports, "waitUntilIdle", waitUntilIdle);
  NODE_SET_METHOD(exports, "setLutQuick", setLutQuick);
  NODE_SET_METHOD(exports, "setPartialWindow", setPartialWindow);
}

NODE_MODULE(epd7x5, InitAll)
