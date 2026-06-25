#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX_CYD : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
public:
  LGFX_CYD() {
    auto cfg = _bus_instance.config();
    cfg.spi_host = SPI2_HOST;
    cfg.spi_mode = 0;
    cfg.freq_write = 40000000;
    cfg.pin_sclk = 14;
    cfg.pin_mosi = 13;
    cfg.pin_miso = 12;
    cfg.pin_dc   = 2;
    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);

    auto p_cfg = _panel_instance.config();
    p_cfg.pin_cs           = 15;
    p_cfg.pin_rst          = -1;
    p_cfg.memory_width     = 240;
    p_cfg.memory_height    = 320;
    p_cfg.panel_width      = 240;
    p_cfg.panel_height     = 320;
    _panel_instance.config(p_cfg);
    setPanel(&_panel_instance);
  }
};

LGFX_CYD tft;
bool specsReceived = false;
unsigned long lastPacketTime = 0;
bool linkIsDown = false;
int currentDiskLabelMode = 0; 

void setup() {
  Serial.begin(115200);
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("AWAITING STREAM...", 10, 10);
}

void loop() {
  if (specsReceived && (millis() - lastPacketTime > 2000)) {
    if (!linkIsDown) {
      tft.fillRect(175, 42, 140, 26, TFT_BLACK);
      tft.setTextSize(1); tft.setTextColor(TFT_RED);
      tft.drawString("SYS LINK: SEVERED", 175, 45);
      tft.drawString("CHECK HOST ENGINE", 175, 56);
      linkIsDown = true;
    }
  }

  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    
    if (data.startsWith("INFO:")) {
      tft.fillScreen(TFT_BLACK);
      
      int p1 = data.indexOf('|');
      int p2 = data.indexOf('|', p1 + 1);
      int p3 = data.indexOf('|', p2 + 1);
      int p4 = data.indexOf('|', p3 + 1);
      
      String cpuName  = data.substring(5, p1);
      String gpu1Name = data.substring(p1 + 1, p2);
      String gpu2Name = data.substring(p2 + 1, p3);
      String ipAddr   = data.substring(p3 + 1, p4);
      String macAddr  = data.substring(p4 + 1);

      tft.setTextSize(2); tft.setTextColor(TFT_WHITE);
      tft.drawString("SYS CONTROL", 10, 6);
      tft.drawFastHLine(10, 24, 300, TFT_DARKGREY);
      
      tft.setTextSize(1); tft.setTextColor(TFT_GREEN);
      tft.drawString("IP: " + ipAddr, 175, 11);
      tft.setTextColor(TFT_LIGHTGRAY);
      tft.drawString("MC: " + macAddr, 175, 19);

      tft.drawString("CPU: " + cpuName, 10, 31);
      tft.drawString("G1:  " + gpu1Name, 10, 44);
      tft.drawString("G2:  " + gpu2Name, 10, 57);
      tft.drawString("NPU: ARCHITECTURE", 10, 70);
      tft.drawFastHLine(10, 83, 300, TFT_DARKGREY);
      
      tft.drawString("CPU WORKLOAD", 10, 91);
      tft.drawString("RAM CONFIG", 10, 137);
      currentDiskLabelMode = 0; 
      
      tft.drawFastVLine(205, 88, 145, TFT_DARKGREY);
      tft.setTextColor(TFT_ORANGE); tft.drawString("GPU CORE LOAD", 215, 91);
      tft.setTextColor(TFT_MAGENTA); tft.drawString("NET SPEED", 215, 142);
      tft.setTextColor(TFT_GREENYELLOW); tft.drawString("BATTERY MAIN", 215, 188);
      
      specsReceived = true;
      lastPacketTime = millis();
    } 
    else if (specsReceived) {
      int c1 = data.indexOf(','); int c2 = data.indexOf(',', c1 + 1);
      int c3 = data.indexOf(',', c2 + 1); int c4 = data.indexOf(',', c3 + 1);
      int c5 = data.indexOf(',', c4 + 1); int c6 = data.indexOf(',', c5 + 1);
      int c7 = data.indexOf(',', c6 + 1);
      
      if (c1 != -1 && c2 != -1 && c3 != -1 && c4 != -1 && c5 != -1 && c6 != -1 && c7 != -1) {
        int cpuVal    = data.substring(0, c1).toInt();
        int ramVal    = data.substring(c1 + 1, c2).toInt();
        int diskVal   = data.substring(c2 + 1, c3).toInt();
        int gpuVal    = data.substring(c3 + 1, c4).toInt();
        int tempVal   = data.substring(c4 + 1, c5).toInt();
        int netVal    = data.substring(c5 + 1, c6).toInt();
        int batVal    = data.substring(c6 + 1, c7).toInt();
        
        int c8 = data.indexOf(',', c7 + 1);
        String timeStr, diskModeStr;
        if(c8 != -1) {
           timeStr = data.substring(c7 + 1, c8);
           diskModeStr = data.substring(c8 + 1);
        } else {
           timeStr = data.substring(c7 + 1);
           diskModeStr = "1";
        }
        int diskMode  = diskModeStr.toInt();

        lastPacketTime = millis();

        tft.setTextSize(1);
        if (linkIsDown) {
          tft.fillRect(175, 42, 140, 26, TFT_BLACK);
          linkIsDown = false;
        }
        tft.fillRect(230, 56, 85, 10, TFT_BLACK); 
        tft.setTextColor(TFT_GREENYELLOW); tft.drawString("SYS LINK: UP TO DATE", 175, 45);
        tft.setTextColor(TFT_CYAN); tft.drawString("LAST UP:  " + timeStr, 175, 56);

        if (diskMode != currentDiskLabelMode) {
          tft.fillRect(10, 183, 130, 12, TFT_BLACK); 
          tft.setTextColor(TFT_LIGHTGRAY);
          if (diskMode == 2) {
            tft.drawString("D: SSD SPACE", 10, 183);
          } else {
            tft.drawString("C: DRIVE SPACE", 10, 183);
          }
          currentDiskLabelMode = diskMode;
        }

        tft.setTextSize(2);
        
        tft.fillRect(140, 91, 60, 16, TFT_BLACK);
        tft.setCursor(140, 91); tft.setTextColor(TFT_GREEN); tft.print(String(cpuVal) + "%");
        tft.drawRect(10, 110, 185, 14, TFT_WHITE); tft.fillRect(11, 111, 183, 12, TFT_BLACK);
        tft.fillRect(11, 111, (183 * cpuVal) / 100, 12, TFT_GREEN);

        tft.fillRect(140, 137, 60, 16, TFT_BLACK);
        tft.setCursor(140, 137); tft.setTextColor(TFT_CYAN); tft.print(String(ramVal) + "%");
        tft.drawRect(10, 156, 185, 14, TFT_WHITE); tft.fillRect(11, 157, 183, 12, TFT_BLACK);
        tft.fillRect(11, 157, (183 * ramVal) / 100, 12, TFT_CYAN);

        tft.fillRect(140, 183, 60, 16, TFT_BLACK);
        tft.setCursor(140, 183); tft.setTextColor(TFT_YELLOW); tft.print(String(diskVal) + "%");
        tft.drawRect(10, 202, 185, 14, TFT_WHITE); tft.fillRect(11, 203, 183, 12, TFT_BLACK);
        tft.fillRect(11, 203, (183 * diskVal) / 100, 12, TFT_YELLOW);

        tft.fillRect(215, 107, 100, 30, TFT_BLACK);
        tft.setTextColor(TFT_ORANGE); tft.drawString(String(gpuVal) + "%  " + String(tempVal) + "C", 215, 107);

        tft.fillRect(215, 158, 100, 20, TFT_BLACK);
        tft.setTextColor(TFT_MAGENTA); tft.drawString(String(netVal) + " Mbps", 215, 158);

        tft.fillRect(215, 204, 100, 20, TFT_BLACK);
        tft.setTextColor(TFT_GREENYELLOW); tft.drawString(String(batVal) + " %", 215, 204);
      }
    }
  }
}
