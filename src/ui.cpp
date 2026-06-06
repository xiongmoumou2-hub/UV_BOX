#include "ui.h"

// ==================== 全局对象 ====================
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// ==================== 外部变量声明 ====================
extern bool isWeatherInfogetted;
extern int CPU_PERCENT;
extern int MEM_PERCENT;
extern String PROCESSOR_TEMP;
extern String PROCESSOR_PW;
extern String PROCESSOR_NAME;
extern String MEM_AVAILABLE;
extern String COMPUTER_NAME;
extern String host;

// ==================== 硬件配置（宏定义） ====================
#define NUM_LEDS    1       // WS2812 灯珠数
#define DATA_PIN    32      // WS2812 数据引脚
CRGB leds[NUM_LEDS];

#define FREQ_BLK        2000    // 背光 PWM 频率
#define CHANNEL_BLK     0       // 背光 PWM 通道
#define RESOLUTION_BLK  8       // 背光 PWM 分辨率
#define BLK_PIN         17      // 背光引脚
int16_t brightness = 10;        // 当前背光亮度

// ==================== 传感器全局数据 ====================
AHT20Data aht20Data;            // AHT20 温湿度
BMP280Data bmp280Data;          // BMP280 气压海拔
uint8_t uvLevel;                // 紫外线强度
float batteryVoltage;           // 电池电压

// ==================== UI 图标数组 ====================
const uint16_t *main_icon[]     = { icon_sensor,    icon_network,    icon_led,      icon_setting };
const uint16_t *sensor_icon[]   = { icon_uv,        icon_temp_humi,  icon_pressure, icon_computer };
const uint16_t *network_icon[]  = { icon_time,      icon_bilibili,   icon_forecast, icon_aiq };
const uint16_t *setting_icon[]  = { icon_brightness,icon_wifi,       icon_bluetooth,icon_about };

// ==================== 功能数量常量 ====================
uint8_t MENU_FUN_NUM     = 4;
uint8_t NETWORK_FUN_NUM  = 4;
uint8_t SENSOR_FUN_NUM   = 4;
uint8_t LIGHT_FUN_NUM    = 4;
uint8_t SETTING_FUN_NUM  = 1;

// ==================== 页面导航变量 ====================
int8_t currentPage;     // 当前页面 0:首页 1:传感器 2:网络 3:灯光 4:设置 /5:UV 6:AHT20 7:BMP280 /8:CLOCK 9:BILIBILI 10:FORECAST 11:AIQ /12:BRIGHTNESS 13:WIFI 14:BLUETOOTH
int8_t currentIndex;    // 0:默认页 1:1级菜单 2:2级菜单 3:3级菜单
int8_t menuIndex;       // 主菜单索引 0:sensor 1:network 2:led 3:setting
int8_t sensorIndex;     // 传感器页索引 0:UV 1:AHT20 2:BMP280 3:COMPUTER
int8_t networkIndex;    // 网络页索引 0:clock 1:bilibili 2:forecast 3:airq
int8_t settingIndex;    // 设置页索引 0:brightness 1:wifi 2:bluetooth 3:about
int8_t ledIndex;        // 灯光页索引 0:led
int8_t lightIndex;      // 灯光颜色索引
uint8_t bar = 0;
bool pushFlag   = true;     // 页面刷新标志
bool enterFlag  = false;    // 确认标志
bool brightness_page = false;

// ==================== 页面名称 ====================
String menuNames[4]     = { "SENSOR", "NETWORK", "LIGHT", "SETTING" };
String sensorNames[4]   = { "UV SD", "AHT20", "BMP280", "COMPUTER" };
String networkNames[4]  = { "CLOCK", "BILIBILI", "FORECAST", "AIR Q" };
String settingNames[4]  = { "BRIGHTNESS", "WIFI", "BLUETOOTH", "ABOUT" };
String ledNames[1]      = { "LIGHT" };

// ==================== 灯光颜色 ====================
uint32_t lightColor[7] = {
    0xb000, 0xf340, 0xf760, 0x27c0, 0x069e, 0xa9bf, 0xf9bd
};
CRGB lightColorRGB[7] = {
    CRGB(181, 0, 0), CRGB(247, 105, 0), CRGB(247, 239, 0),
    CRGB(33, 251, 0), CRGB(0, 210, 247), CRGB(173, 53, 255),
    CRGB(255, 53, 239)
};

// ==================== 硬件初始化 ====================

/**
 * 初始化屏幕
 */
void screen_init(void)
{
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
}

/**
 * 初始化 LED 灯
 */
void light_init(void)
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(200);
}

/**
 * 关闭灯光
 */
void light_close(void)
{
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
}

/**
 * 设置背光亮度
 * @param brightness 亮度 0-250
 */
void set_back_brightness(uint8_t brightness)
{
    ledcSetup(CHANNEL_BLK, FREQ_BLK, RESOLUTION_BLK);
    ledcAttachPin(BLK_PIN, CHANNEL_BLK);
    if (brightness > 250) brightness = 250;
    ledcWrite(CHANNEL_BLK, brightness);
}

// ==================== 工具函数 ====================

/**
 * 将整数转换为字符串（个位数补零）
 */
String num2str(int digits)
{
    String s = "";
    if (digits < 10) s = s + "0";
    s = s + digits;
    return s;
}

/**
 * 绘制进度条
 * @param n  百分比 0-100
 * @param x  左上角 X
 * @param y  左上角 Y
 */
void drawProgressbar(int n, int x, int y)
{
    tft.fillRect(x, y, 100, 14, 0x630C);
    tft.fillRect(x, y, n, 14, 0xF79E);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(0xB596);
    tft.drawString(String(n) + "%", x + 50, y);
}

// ==================== 页面指示器 ====================

/**
 * 自动绘制页面指示器（底部圆点）
 */
void AUTO_INDICATOR(uint8_t itemCount, uint8_t activeIndex)
{
    int screenWidth = 128;
    int itemWidth = 10;
    int itemGap = 10;
    int totalWidth = (itemCount * itemWidth) + ((itemCount - 1) * itemGap);
    int startX = (screenWidth - totalWidth) / 2;

    for (int i = 0; i < itemCount; i++)
    {
        uint16_t rectColor = (i == activeIndex) ? TFT_CYAN : TFT_WHITE;
        int currentX = startX + i * (itemWidth + itemGap);
        spr.fillRoundRect(currentX, 140, itemWidth, 5, 1, rectColor);
    }
}

// ==================== 启动画面 ====================

/**
 * 开机初始化动画
 */
void INIT_UI(void)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.loadFont(BoxedRegular18);

    tft.pushImage(23, 0, 80, 80, icon_logo);
    tft.drawString("ESP32 UV BOX", 2, 90);

    // 进度条
    tft.drawRoundRect(20, 120, 84, 10, 2, TFT_WHITE);
    for (uint8_t i = 0; i < 80; i++)
    {
        tft.drawFastVLine(22 + i, 122, 6, 0xb3df);
        vTaskDelay(random(20, 35));
    }
    vTaskDelay(1000);
    tft.fillScreen(TFT_BLACK);
}

// ==================== 天气时钟默认界面 ====================

/**
 * 默认界面（时钟+天气），每分钟刷新
 */
void default_UI(void)
{
    if (!isWeatherInfogetted) return;

    static unsigned long lastRefreshTime = 0;
    static bool once = (CLOCK_UI(), true);

    if (millis() - lastRefreshTime >= 60000)
    {
        CLOCK_UI();
        lastRefreshTime = millis();
    }
}

// ==================== 首页导航 (BOX_UI) ====================

/**
 * 主页面路由：根据 currentIndex 和 currentPage 分发到对应子页面
 */
void BOX_UI(void)
{
    if (currentIndex == 0)
    {
        default_UI();
    }
    else if (currentIndex == 1)
    {
        if (pushFlag)
        {
            tft.fillScreen(TFT_BLACK);
            currentPage = 0;
            spr.createSprite(128, 160);
            spr.fillSprite(TFT_BLACK);
            spr.setTextDatum(TC_DATUM);
            spr.setTextSize(2);
            spr.setTextColor(TFT_CYAN);
            spr.drawString(menuNames[menuIndex], 64, 20);
            spr.pushImage(23, 50, 80, 80, main_icon[menuIndex]);
            AUTO_INDICATOR(MENU_FUN_NUM, menuIndex);
            spr.pushSprite(0, 0);
            spr.deleteSprite();
            pushFlag = false;
        }
    }
    else if (currentIndex == 2)
    {
        if (pushFlag)
        {
            switch (menuIndex)
            {
            case 0: currentPage = 1; SENSOR_UI(); break;
            case 1: currentPage = 2; NETWORK_UI(); break;
            case 2: currentPage = 3; LIGHT_UI(); break;
            case 3: currentPage = 4; SETTING_UI(); break;
            }
            pushFlag = false;
        }
    }
    else if (currentIndex == 3)
    {
        switch (currentPage)
        {
        case 1:
            if (sensorIndex == 0)       UV_UI();
            else if (sensorIndex == 1)  AHT20_UI();
            else if (sensorIndex == 2)  BMP280_UI();
            else if (sensorIndex == 3)  COMPUTER_MONITOR_UI();
            break;
        case 2:
            if (networkIndex == 0)      CLOCK_UI();
            else if (networkIndex == 1) BILIBILI_UI();
            else if (networkIndex == 2) FORECAST_UI();
            else if (networkIndex == 3) AIQ_UI();
            break;
        case 4:
            if (settingIndex == 0)      { brightness_page = true; BRIGHTNESS_UI(); }
            else if (settingIndex == 1) WIFI_UI();
            else if (settingIndex == 2) BLUETOOTH_UI();
            else if (settingIndex == 3) ABOUT_UI();
            break;
        }
    }
}

// ==================== 二级菜单页 ====================

/**
 * 传感器菜单页
 */
void SENSOR_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(sensorNames[sensorIndex], 64, 20);
    spr.pushImage(23, 50, 80, 80, sensor_icon[sensorIndex]);
    AUTO_INDICATOR(SENSOR_FUN_NUM, sensorIndex);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * 网络菜单页
 */
void NETWORK_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(networkNames[networkIndex], 64, 20);
    spr.pushImage(23, 50, 80, 80, network_icon[networkIndex]);
    AUTO_INDICATOR(NETWORK_FUN_NUM, networkIndex);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * 灯光页
 */
void LIGHT_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.pushImage(23, 30, 80, 80, icon_led);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
    tft.fillCircle(63, 130, 8, lightColor[lightIndex]);
    leds[0] = lightColorRGB[lightIndex];
    FastLED.show();
}

/**
 * 设置菜单页
 */
void SETTING_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(settingNames[settingIndex], 64, 20);
    spr.pushImage(23, 50, 80, 80, setting_icon[settingIndex]);
    AUTO_INDICATOR(SETTING_FUN_NUM, settingIndex);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

// ==================== 传感器子页面 ====================

/**
 * AHT20 温湿度详情页
 */
void AHT20_UI(void)
{
    if (pushFlag)
    {
        Serial.println("AHT20");
        tft.fillScreen(TFT_BLACK);

        tft.loadFont(CCH20);
        tft.pushImage(0, 25, 36, 36, icon_temp);
        tft.setTextColor(TFT_CYAN);
        tft.drawString("温度", 36, 30);

        tft.pushImage(0, 85, 36, 36, icon_humi);
        tft.drawString("湿度", 36, 90);
        tft.unloadFont();
        pushFlag = false;
    }
    // 温度数据
    spr.loadFont(BoxedRegular18);
    spr.createSprite(50, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(CC_DATUM);
    spr.drawFloat(aht20Data.temperature, 1, 25, 9);
    spr.pushSprite(78, 30);
    spr.deleteSprite();

    // 温度条
    spr.createSprite(120, 6);
    spr.drawRoundRect(0, 0, 90, 6, 2, TFT_WHITE);
    spr.fillRoundRect(0, 0, (int)(aht20Data.temperature * 0.9), 6, 2, 0xfb60);
    spr.pushSprite(36, 55);
    spr.deleteSprite();

    // 湿度数据
    spr.createSprite(50, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(CC_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(aht20Data.humidity, 1, 25, 9);
    spr.pushSprite(78, 90);
    spr.deleteSprite();
    spr.unloadFont();

    // 湿度条
    spr.createSprite(120, 6);
    spr.drawRoundRect(0, 0, 90, 6, 2, TFT_WHITE);
    spr.fillRoundRect(0, 0, (int)(aht20Data.humidity * 0.9), 6, 2, 0x3626);
    spr.pushSprite(36, 115);
    spr.deleteSprite();
}

/**
 * BMP280 气压海拔详情页
 */
void BMP280_UI(void)
{
    if (pushFlag)
    {
        Serial.println("BMP280");
        tft.fillScreen(TFT_BLACK);

        tft.loadFont(CCH20);
        tft.pushImage(0, 20, 36, 36, icon_temp);
        tft.setTextColor(TFT_CYAN);
        tft.drawString("温度", 45, 20);

        tft.pushImage(0, 70, 36, 36, icon_alti);
        tft.drawString("海拔", 45, 70);

        tft.pushImage(0, 120, 36, 36, icon_press);
        tft.drawString("气压", 45, 120);
        tft.unloadFont();
        pushFlag = false;
    }

    // 温度数据
    spr.createSprite(80, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(bmp280Data.temperature, 1, 0, 9);
    spr.unloadFont();
    spr.pushSprite(45, 42);
    spr.deleteSprite();

    // 海拔数据
    spr.createSprite(80, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(bmp280Data.altitude, 1, 0, 9);
    spr.unloadFont();
    spr.pushSprite(45, 92);
    spr.deleteSprite();

    // 压力数据
    spr.createSprite(80, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(bmp280Data.pressure / 100, 1, 0, 9);
    spr.unloadFont();
    spr.pushSprite(45, 142);
    spr.deleteSprite();
}

/**
 * UV 紫外线强度页
 */
void UV_UI(void)
{
    if (pushFlag)
    {
        Serial.println("UV");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(2);
        tft.drawString("UV SD", 35, 25);
        tft.drawString("Level: ", 10, 100);
        tft.drawString("BAT:", 5, 5);

        tft.fillCircle(15, 130, 8, 0x3626);
        tft.fillCircle(39, 130, 8, 0xffe0);
        tft.fillCircle(63, 130, 8, 0xfca0);
        tft.fillCircle(87, 130, 8, 0xf800);
        tft.fillCircle(111, 130, 8, 0xc4bf);
        pushFlag = false;
    }

    // UV 数值
    spr.createSprite(40, 18);
    spr.setTextColor(TFT_CYAN);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawString(num2str(uvLevel), 0, 9);
    spr.unloadFont();
    spr.pushSprite(85, 100);
    spr.deleteSprite();

    // 电池电压
    spr.createSprite(60, 18);
    spr.setTextColor(TFT_GREEN);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawString(String(batteryVoltage, 2) + "V", 0, 9);
    spr.unloadFont();
    spr.pushSprite(55, 5);
    spr.deleteSprite();

    // UV 等级颜色指示块
    spr.createSprite(50, 30);
    switch (uvLevel)
    {
    case 1:  case 2:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0x3626); break;
    case 3:  case 4:  case 5:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xffe0); break;
    case 6:  case 7:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xfb60); break;
    case 8:  case 9:  case 10:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xf800); break;
    case 11: case 12:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xc4bf); break;
    default:
        spr.fillRoundRect(0, 0, 50, 30, 5, TFT_CYAN); break;
    }
    spr.pushSprite(38, 55);
    spr.deleteSprite();

    // UV 等级指示三角
    spr.createSprite(128, 6);
    switch (uvLevel)
    {
    case 1:  case 2:
        spr.drawTriangle(15, 0, 10, 5, 20, 5, TFT_WHITE); break;
    case 3:  case 4:  case 5:
        spr.drawTriangle(39, 0, 34, 5, 44, 5, TFT_WHITE); break;
    case 6:  case 7:
        spr.drawTriangle(63, 0, 58, 5, 68, 5, TFT_WHITE); break;
    case 8:  case 9:  case 10:
        spr.drawTriangle(87, 0, 82, 5, 92, 5, TFT_WHITE); break;
    case 11: case 12:
        spr.drawTriangle(111, 0, 106, 5, 116, 5, TFT_WHITE); break;
    default: break;
    }
    spr.pushSprite(0, 145);
    spr.deleteSprite();
}

// ==================== 网络子页面 ====================

/**
 * 时钟 + 当前天气页
 */
void CLOCK_UI(void)
{
    if (pushFlag || currentIndex == 0)
    {
        Serial.println("CLOCK");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(0, 85, 30, 30, icon_temp1);
        tft.pushImage(0, 125, 30, 30, icon_humi1);
        pushFlag = false;
    }
    spr.createSprite(128, 40);
    spr.fillSprite(TFT_BLACK);
    spr.fillRoundRect(10, 0, 45, 32, 2, 0xc9a0);
    spr.fillRoundRect(75, 0, 45, 32, 2, 0x0494);

    spr.loadFont(BoxedRegular35);
    spr.setTextDatum(CC_DATUM);
    spr.setTextColor(0xef7d);
    spr.drawString(num2str(currentHour) + " : " + num2str(currentMinute), 64, 20);
    spr.unloadFont();
    spr.pushSprite(0, 10);
    spr.deleteSprite();

    spr.loadFont(AlimamaShuHeiTi16);
    spr.createSprite(36, 18);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xfcc3);
    spr.setTextDatum(CC_DATUM);
    spr.drawString(wk[currentWeek], 18, 9);
    spr.pushSprite(7, 60);
    spr.deleteSprite();

    spr.createSprite(80, 18);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(CC_DATUM);
    spr.drawString(num2str(currentMonth) + "月" + num2str(currentDay) + "日", 40, 9);
    spr.pushSprite(45, 60);
    spr.deleteSprite();

    spr.createSprite(45, 30);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(num2str(nowWeather.temp), 0, 0);
    spr.drawRoundRect(0, 20, 45, 6, 1, 0xf7ff);
    spr.fillRoundRect(0, 20, (int)(nowWeather.temp * 0.45), 6, 1, 0xfb60);
    spr.pushSprite(30, 85);
    spr.deleteSprite();

    spr.createSprite(45, 30);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(num2str(nowWeather.humidity), 0, 0);
    spr.drawRoundRect(0, 20, 45, 6, 1, 0xf7ff);
    spr.fillRoundRect(0, 20, (int)(nowWeather.humidity * 0.45), 6, 1, 0x3626);
    spr.pushSprite(30, 125);
    spr.deleteSprite();

    spr.createSprite(50, 72);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(TC_DATUM);
    spr.drawString(nowWeather.text, 25, 0);
    set_weather_icon(nowWeather.icon);
    spr.pushSprite(75, 85);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * B站粉丝数据页
 */
void BILIBILI_UI(void)
{
    if (pushFlag)
    {
        Serial.println("BILIBILI");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(0, 0, 128, 60, icon_bili);
        pushFlag = false;
    }
    spr.createSprite(128, 95);
    spr.loadFont(BoxedRegular18);
    spr.fillRoundRect(3, 6, 5, 36, 2, 0x063d);
    spr.fillRoundRect(3, 53, 5, 36, 2, 0x063d);
    spr.setTextDatum(TL_DATUM);
    spr.setTextColor(TFT_WHITE);
    spr.drawString("Following: ", 11, 6);
    spr.drawString(num2str(followingCount), 11, 27);
    spr.drawString("Follower: ", 11, 53);
    spr.drawString(num2str(fansCount), 11, 74);
    spr.pushSprite(0, 60);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * 天气预报页（未来6天）
 */
void FORECAST_UI(void)
{
    if (pushFlag)
    {
        Serial.println("FORECAST");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    spr.createSprite(128, 160);
    spr.loadFont(AlimamaShuHeiTi25);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);

    spr.fillRoundRect(0, 2, 128, 25, 5, 0x529d);
    spr.fillRoundRect(0, 28, 128, 25, 5, 0x751f);
    spr.fillRoundRect(0, 54, 128, 25, 5, 0x36ae);
    spr.fillRoundRect(0, 80, 128, 25, 5, 0xfb50);
    spr.fillRoundRect(0, 106, 128, 25, 5, 0xfbea);
    spr.fillRoundRect(0, 132, 128, 25, 5, 0xfd20);

    spr.setTextColor(TFT_WHITE);
    spr.drawString(wk[(currentWeek + 1) % 7], 5, 3);
    spr.drawString(wk[(currentWeek + 2) % 7], 5, 29);
    spr.drawString(wk[(currentWeek + 3) % 7], 5, 55);
    spr.drawString(wk[(currentWeek + 4) % 7], 5, 81);
    spr.drawString(wk[(currentWeek + 5) % 7], 5, 107);
    spr.drawString(wk[(currentWeek + 6) % 7], 5, 133);

    spr.drawString(futureWeather.day1wea, 63, 3);
    spr.drawString(futureWeather.day2wea, 63, 29);
    spr.drawString(futureWeather.day3wea, 63, 55);
    spr.drawString(futureWeather.day4wea, 63, 81);
    spr.drawString(futureWeather.day5wea, 63, 107);
    spr.drawString(futureWeather.day6wea, 63, 133);

    spr.pushSprite(0, 0);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * 空气质量页
 */
void AIQ_UI(void)
{
    if (pushFlag)
    {
        Serial.println("AIQ");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    spr.createSprite(128, 160);
    spr.loadFont(AlimamaShuHeiTi25);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);

    spr.fillRoundRect(0, 2, 128, 25, 5, 0xfd20);
    spr.fillRoundRect(0, 28, 128, 25, 5, 0xfbea);
    spr.fillRoundRect(0, 54, 128, 25, 5, 0xfb50);
    spr.fillRoundRect(0, 80, 128, 25, 5, 0x36ae);
    spr.fillRoundRect(0, 106, 128, 25, 5, 0x751f);
    spr.fillRoundRect(0, 132, 128, 25, 5, 0x529d);

    spr.setTextColor(TFT_WHITE);
    spr.drawString("PM10", 5, 3);
    spr.drawString("PM2.5", 5, 29);
    spr.drawString("NO2", 5, 55);
    spr.drawString("SO2", 5, 81);
    spr.drawString("CO", 5, 107);
    spr.drawString("O3", 5, 133);

    spr.drawString(nowWeather.pm10, 82, 3);
    spr.drawString(nowWeather.pm2p5, 82, 29);
    spr.drawString(nowWeather.no2, 82, 55);
    spr.drawString(nowWeather.so2, 82, 81);
    spr.drawString(nowWeather.co, 82, 107);
    spr.drawString(nowWeather.o3, 82, 133);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
    spr.unloadFont();
}

// ==================== 设置子页面 ====================

/**
 * 亮度调节页
 */
void BRIGHTNESS_UI(void)
{
    if (pushFlag)
    {
        Serial.println("BRIGHTNESS");
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    bar = (brightness - 10) / 24;
    set_back_brightness(brightness);
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TC_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawString("BRIGHTNESS", 64, 5);
    spr.unloadFont();
    spr.drawRect(10, 30, 108, 40, TFT_WHITE);
    spr.drawRect(10, 75, 108, 82, TFT_WHITE);
    spr.loadFont(DigitalNumbers30);
    spr.drawString(num2str(brightness), 64, 35);
    spr.unloadFont();
    for (int i = 0; i < 10; i++)
    {
        if (i < bar)
            spr.fillRect(15, 145 - i * 7, 98, 5, 0xfa00);
        else
            spr.fillRect(15, 145 - i * 7, 98, 5, 0xa140);
    }
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * WiFi 页
 */
void WIFI_UI(void)
{
    if (pushFlag)
    {
        Serial.println("WIFI");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(23, 39, 80, 80, icon_wifi);
        pushFlag = false;
    }
}

/**
 * 蓝牙页
 */
void BLUETOOTH_UI(void)
{
    if (pushFlag)
    {
        Serial.println("BLUETOOTH");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(23, 39, 80, 80, icon_bluetooth);
        pushFlag = false;
    }
}

/**
 * 关于页
 */
void ABOUT_UI(void)
{
    if (pushFlag)
    {
        Serial.println("ABOUT");
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    spr.createSprite(128, 150);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.pushImage(31, 2, 64, 64, icon_head);
    spr.drawString("ESP32 UV BOX", 0, 70);
    spr.drawString("author: levi5", 0, 90);
    spr.drawString("version: .v1", 0, 110);
    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

// ==================== 天气图标 ====================

/**
 * 根据天气代码设置天气图标
 */
void set_weather_icon(int icon)
{
    switch (icon)
    {
    case 100: case 150:
        spr.pushImage(0, 20, 50, 50, icon_sunny); break;
    case 101: case 102: case 103: case 151: case 152: case 153:
        spr.pushImage(0, 20, 50, 50, icon_cloudy); break;
    case 104:
        spr.pushImage(0, 20, 50, 50, icon_overcast); break;
    case 305: case 309: case 313: case 314: case 399:
        spr.pushImage(0, 20, 50, 50, icon_light_rain); break;
    case 300: case 301:
        spr.pushImage(0, 20, 50, 50, icon_shower_rain); break;
    case 302: case 303: case 304:
        spr.pushImage(0, 20, 50, 50, icon_thundershower); break;
    case 306: case 307: case 308: case 310: case 311: case 312:
    case 315: case 316: case 317: case 318: case 350: case 351:
        spr.pushImage(0, 20, 50, 50, icon_heavy_rain); break;
    case 400: case 401: case 402: case 403: case 404: case 405:
    case 406: case 407: case 408: case 409: case 410: case 456:
    case 457: case 499:
        spr.pushImage(0, 20, 50, 50, icon_light_snow); break;
    case 500: case 501: case 509: case 510: case 514: case 515:
        spr.pushImage(0, 20, 50, 50, icon_foggy); break;
    case 502: case 511: case 512: case 513:
        spr.pushImage(0, 20, 50, 50, icon_haze); break;
    case 503: case 504:
        spr.pushImage(0, 20, 50, 50, icon_sand); break;
    case 507: case 508:
        spr.pushImage(0, 20, 50, 50, icon_sandstorm); break;
    case 900:
        spr.pushImage(0, 20, 50, 50, icon_sunny); break;       // hot
    case 901:
        spr.pushImage(0, 20, 50, 50, icon_light_snow); break;  // cold
    case 999: default:
        spr.pushImage(0, 20, 50, 50, icon_null); break;
    }
}

// ==================== 电脑性能监控 ====================

/**
 * 电脑性能监控页（CPU / RAM / 系统信息）
 */
void COMPUTER_MONITOR_UI()
{
    // 静态内容（仅首次绘制）
    if (pushFlag)
    {
        Serial.println("COMPUTER_MONITOR - 128x160 Mode");
        tft.fillScreen(TFT_BLACK);

        tft.setTextSize(2);
        tft.setTextColor(0x630C);
        tft.drawString("Monitor", 35, 5);

        tft.drawFastHLine(0, 25, 128, 0x2104);
        tft.drawFastHLine(0, 85, 128, 0x2104);

        tft.setTextSize(1);
        tft.setTextColor(0x630C);
        tft.drawString("CPU:", 5, 32);
        tft.drawString("Temp:", 5, 52);
        tft.drawString("PWR:", 5, 72);
        tft.drawString("RAM:", 5, 95);
        tft.drawString("Free:", 5, 115);
        tft.drawString("PC:", 5, 135);

        pushFlag = false;
    }

    // 动态内容（Sprite 局部刷新防闪烁）
    Computer_Monitor_Get();

    // CPU 进度条
    spr.createSprite(75, 10);
    spr.fillRect(0, 0, 75, 10, 0x3186);
    spr.fillRect(0, 0, (CPU_PERCENT * 75 / 100), 10, 0xF79E);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(MC_DATUM);
    spr.drawString(String(CPU_PERCENT) + "%", 37, 5);
    spr.pushSprite(50, 37);
    spr.deleteSprite();

    // CPU 温度
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(PROCESSOR_TEMP, 0, 0);
    spr.pushSprite(50, 57);
    spr.deleteSprite();

    // CPU 功耗
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(PROCESSOR_PW, 0, 0);
    spr.pushSprite(50, 77);
    spr.deleteSprite();

    // RAM 进度条
    spr.createSprite(75, 10);
    spr.fillRect(0, 0, 75, 10, 0x3186);
    spr.fillRect(0, 0, (MEM_PERCENT * 75 / 100), 10, 0xF79E);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(MC_DATUM);
    spr.drawString(String(MEM_PERCENT) + "%", 37, 5);
    spr.pushSprite(50, 100);
    spr.deleteSprite();

    // RAM 剩余容量
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(MEM_AVAILABLE, 0, 0);
    spr.pushSprite(50, 120);
    spr.deleteSprite();

    // 处理器名称
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(PROCESSOR_NAME.substring(0, 12), 0, 0);
    spr.pushSprite(50, 140);
    spr.deleteSprite();
}
