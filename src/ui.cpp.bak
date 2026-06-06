#include "ui.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// 功能数量
uint8_t MENU_FUN_NUM = 4;
uint8_t NETWORK_FUN_NUM = 4;
uint8_t SENSOR_FUN_NUM = 4;
uint8_t LIGHT_FUN_NUM = 4;
uint8_t SETTING_FUN_NUM = 1;

// SENSOR数据
AHT20Data aht20Data;   // AHT20数据
BMP280Data bmp280Data; // BMP280数据
uint8_t uvLevel;       // 紫外线强度
float batteryVoltage;  // 电池电压

// WS2812参数
#define NUM_LEDS 1   // 灯珠数
#define DATA_PIN 32  // 数据引脚
CRGB leds[NUM_LEDS]; // 灯珠数组

// 背光参数
#define FREQ_BLK 2000    // 频率
#define CHANNEL_BLK 0    // 通道
#define RESOLUTION_BLK 8 // 分辨率
#define BLK_PIN 17       // LED背光引脚
int16_t brightness = 10;

// 首页图标
const uint16_t *main_icon[] = {
    icon_sensor,
    icon_network,
    icon_led,
    icon_setting};

// 传感器页图标
const uint16_t *sensor_icon[] = {
    icon_uv,
    icon_temp_humi,
    icon_pressure,
    icon_computer};

// 网络页图标
const uint16_t *network_icon[] = {
    icon_time,
    icon_bilibili,
    icon_forecast,
    icon_aiq};

// 设置页图标
const uint16_t *setting_icon[] = {
    icon_brightness,
    icon_wifi,
    icon_bluetooth,
    icon_about};
bool brightness_page = false;

int8_t currentPage;  // 当前页面 0: 首页 1: 传感器 2: 网络 3: 灯光 4: 设置 /5: UV 6: AHT20 7: BMP280 /8:    CLOCK 9: BILIBILI 10: FORECAST 11: AIQ /12: BRIGHHTNESS 13: WIFI 14: BLUETOOTH
int8_t currentIndex; // 0:默认页 1: 1级 2: 2级 3: 3级
int8_t menuIndex;    // 主菜单页索引 0：sensor 1：network 2：led 3：setting
int8_t sensorIndex;  // 传感器页索引 0：UV 1：AHT20 2：BMP280 *新增 3:电脑性能监控
int8_t networkIndex; // 网络页索引 0: clock 1: bilibili 2:forecast 3: airq
int8_t settingIndex; // 设置页索引 0: set
int8_t ledIndex;     // 灯光页索引 0: led
int8_t lightIndex;   // 灯光颜色索引
uint8_t bar = 0;
bool pushFlag = true;                                                                                                                                         // 刷新标志
bool enterFlag = false;                                                                                                                                       // 确认标志
String menuNames[4] = {"SENSOR", "NETWORK", "LIGHT", "SETTING"};                                                                                              // 主菜单名称
String sensorNames[4] = {"UV SD", "AHT20", "BMP280", "COMPUTER"};                                                                                             // 传感器页名称
String networkNames[4] = {"CLOCK", "BILIBILI", "FORECAST", "AIR Q"};                                                                                          // 网络页名称
String settingNames[4] = {"BRIGHTNESS", "WIFI", "BLUETOOTH", "ABOUT"};                                                                                        // 设置页名称
String ledNames[1] = {"LIGHT"};                                                                                                                               // 灯光页名称
uint32_t lightColor[7] = {0xb000, 0xf340, 0xf760, 0x27c0, 0x069e, 0xa9bf, 0xf9bd};                                                                            // 灯光颜色
CRGB lightColorRGB[7] = {CRGB(181, 0, 0), CRGB(247, 105, 0), CRGB(247, 239, 0), CRGB(33, 251, 0), CRGB(0, 210, 247), CRGB(173, 53, 255), CRGB(255, 53, 239)}; // 灯光颜色

extern bool isWeatherInfogetted;
void default_UI(void)
{
    if (!isWeatherInfogetted)
    {
        return;
    }
    static unsigned long lastRefreshTime = 0;

    if (isWeatherInfogetted)
    {
        static bool once = (CLOCK_UI(), true);
        if (millis() - lastRefreshTime >= 60000)
        {
            CLOCK_UI(); // 调用时钟绘制
            lastRefreshTime = millis();
        }
    }
}

/**
 * @name light_init
 * @brief 初始化并设置LED灯
 * 该函数负责初始化LED灯 strip，并设置其亮度。它使用FastLED库来控制LED灯。
 * @author levi5
 * @param 无
 * @return 无
 */
void light_init(void)
{
    // 初始化LED灯，指定LED类型为NEOPIXEL，数据线引脚为DATA_PIN，假设LED的顺序为GRB
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    // 设置LED灯的亮度为200（介于0和255之间）
    FastLED.setBrightness(200);
}

/**
 * @name light_close
 * @brief 关闭灯光
 * @author levi5
 */
void light_close(void)
{
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
}

/**
 * @name set_back_brightness
 * @brief 设置背光亮度
 * @author levi5
 * @param brightness 亮度 0-100
 */
void set_back_brightness(uint8_t brightness)
{
    ledcSetup(CHANNEL_BLK, FREQ_BLK, RESOLUTION_BLK);
    ledcAttachPin(BLK_PIN, CHANNEL_BLK);
    brightness = brightness >= 0 ? (brightness <= 250 ? brightness : 250) : 0;
    ledcWrite(CHANNEL_BLK, brightness);
}

/**
 * @name num2str
 * @brief 将整数转换为字符串
 * @author levi5
 * @param digits 整数数字
 * @return 返回包含数字的字符串。如果数字小于10，则在数字前添加一个0。
 */
String num2str(int digits)
{
    String s = ""; // 初始化为空字符串
    // 如果数字小于10，就在字符串前添加一个0
    if (digits < 10)
        s = s + "0";
    // 将整数转换为字符串并追加到s中
    s = s + digits;
    return s; // 返回最终构建的字符串
}

/**
 * @name screen_init
 * @brief 初始化屏幕
 * @author levi5
 */
void screen_init(void)
{
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
}

/**
 * @name INIT_UI
 * @brief 初始化UI
 */
void INIT_UI(void)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.loadFont(BoxedRegular18); // 10

    tft.pushImage(23, 0, 80, 80, icon_logo);
    tft.drawString("ESP32 UV BOX", 2, 90);

    // 画进度条外框 (宽84, 高10)
    tft.drawRoundRect(20, 120, 84, 10, 2, TFT_WHITE);

    // 内部填充进度条
    // 起点 X 偏移 2 个像素 (22), 避免覆盖左侧圆角
    // 内部高度为 6 (120 + 2 到 120 + 8)
    for (uint8_t i = 0; i < 80; i++)
    {
        // 每次只画一根垂直线，视觉极度顺滑，且不破坏边框
        tft.drawFastVLine(22 + i, 122, 6, 0xb3df);
        vTaskDelay(random(20, 35));
    }

    // 进度条满后，停留 1000ms 让用户看清
    vTaskDelay(1000);
    tft.fillScreen(TFT_BLACK);
}
/**
 * @name BOX_UI
 * @brief 首页UI
 * @author levi5
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
            currentPage = 0; // 首页
            spr.createSprite(128, 160);
            spr.fillSprite(TFT_BLACK);
            spr.setTextDatum(TC_DATUM);
            spr.setTextSize(2);
            spr.setTextColor(TFT_CYAN);
            spr.drawString(menuNames[menuIndex], 64, 20);        // 显示菜单名称
            spr.pushImage(23, 50, 80, 80, main_icon[menuIndex]); // 显示图标
            AUTO_INDICATOR(MENU_FUN_NUM, menuIndex);             // 显示指示器
            spr.pushSprite(0, 0);
            spr.deleteSprite();
            pushFlag = false;
        }
    }
    else if (currentIndex == 2) // 确定 三级页面图标
    {
        if (pushFlag)
        {
            switch (menuIndex)
            {
            case 0:
                currentPage = 1; // 传感器页
                SENSOR_UI();
                break;
            case 1:
                currentPage = 2; // 网络页
                NETWORK_UI();
                break;
            case 2:
                currentPage = 3; // 灯光页
                LIGHT_UI();
                break;
            case 3:
                currentPage = 4; // 设置页
                SETTING_UI();
                break;
            default:
                break;
            }
            pushFlag = false;
        }
    }
    else if (currentIndex == 3)
    {
        switch (currentPage)
        {
        case 1:
            if (sensorIndex == 0)
            {
                UV_UI();
            }
            else if (sensorIndex == 1)
            {
                AHT20_UI();
            }
            else if (sensorIndex == 2)
            {
                BMP280_UI();
            }
            else if (sensorIndex == 3)
            {
                COMPUTER_MONITOR_UI();
            }
            break;
        case 2:
            if (networkIndex == 0)
            {
                CLOCK_UI();
            }
            else if (networkIndex == 1)
            {
                BILIBILI_UI();
            }
            else if (networkIndex == 2)
            {
                FORECAST_UI();
            }
            else if (networkIndex == 3)
            {
                AIQ_UI();
            }
            break;
        case 4:
            if (settingIndex == 0)
            {
                brightness_page = true;
                BRIGHTNESS_UI();
            }
            else if (settingIndex == 1)
            {
                WIFI_UI();
            }
            else if (settingIndex == 2)
            {
                BLUETOOTH_UI();
            }
            else if (settingIndex == 3)
            {
                ABOUT_UI();
            }
            break;
        }
    }
}

/**
 * @name SENSOR_UI
 * @brief 传感器UI
 * @author levi5
 */
void SENSOR_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(sensorNames[sensorIndex], 64, 20);        // 显示传感器名称
    spr.pushImage(23, 50, 80, 80, sensor_icon[sensorIndex]); // 显示传感器图标
    AUTO_INDICATOR(SENSOR_FUN_NUM, sensorIndex);             // 显示指示器
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * @name NETWORK_UI
 * @brief 网络UI
 * @author levi5
 */
void NETWORK_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(networkNames[networkIndex], 64, 20);        // 网络功能名称
    spr.pushImage(23, 50, 80, 80, network_icon[networkIndex]); // 网络功能图标
    AUTO_INDICATOR(NETWORK_FUN_NUM, networkIndex);             // 显示指示器
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * @name LIGHT_UI
 * @brief 灯光UI
 * @author levi5
 */
void LIGHT_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.pushImage(23, 30, 80, 80, icon_led);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
    tft.fillCircle(63, 130, 8, lightColor[lightIndex]); // 灯光
    leds[0] = lightColorRGB[lightIndex];                // 灯光颜色
    FastLED.show();
}

/**
 * @name SETTING_UI
 * @brief 设置UI
 * @author levi5
 */
void SETTING_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(settingNames[settingIndex], 64, 20);        // 显示传感器名称
    spr.pushImage(23, 50, 80, 80, setting_icon[settingIndex]); // 显示传感器图标
    AUTO_INDICATOR(SETTING_FUN_NUM, settingIndex);             // 显示指示器
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * @name AHT20_UI
 * @brief AHT20 UI
 * @author levi5
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
 * @name BMP280_UI
 * @brief BMP280 UI
 * @author levi5
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

    // 压强数据
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
 * @name UV_UI
 * @brief UV UI
 * @author levi5
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

        // --- 👇 新增：电池静态标签 ---
        tft.drawString("BAT:", 5, 5); // 在左上角 (5, 5) 的位置写上 BAT:

        tft.fillCircle(15, 130, 8, 0x3626);
        tft.fillCircle(39, 130, 8, 0xffe0);
        tft.fillCircle(63, 130, 8, 0xfca0);
        tft.fillCircle(87, 130, 8, 0xf800);
        tft.fillCircle(111, 130, 8, 0xc4bf);

        pushFlag = false;
    }
    spr.createSprite(40, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.setTextColor(TFT_CYAN);
    spr.loadFont(BoxedRegular18);
    spr.drawString(num2str(uvLevel), 0, 9);
    spr.unloadFont();
    spr.pushSprite(85, 100);
    spr.deleteSprite();

    spr.createSprite(60, 18);     // 创建一个宽60，高18的画布 (足够容纳类似 4.20V 的文字)
    spr.setTextColor(TFT_GREEN);  // 设为绿色，和 UV 数据区分开，也可以用 TFT_WHITE
    spr.setTextDatum(ML_DATUM);   // 设置文本对齐方式（左中对齐）
    spr.loadFont(BoxedRegular18); // 加载你原本使用的字体
    // 将 float 转换为字符串，保留2位小数，并拼接上 "V"
    String batStr = String(batteryVoltage, 2) + "V";
    spr.drawString(batStr, 0, 9); // 在画布内画出电压值
    spr.unloadFont();             // 卸载字体
    spr.pushSprite(55, 5);        // 将画布推送到屏幕上 (X:55 紧挨着刚才画的 BAT: 标签, Y:5 保持水平对齐)
    spr.deleteSprite();           // 释放内存

    spr.createSprite(50, 30);
    // 等级颜色指示
    switch (uvLevel)
    {
    case 1:
    case 2:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0x3626);
        break;
    case 3:
    case 4:
    case 5:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xffe0);
        break;
    case 6:
    case 7:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xfb60);
        break;
    case 8:
    case 9:
    case 10:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xf800);
        break;
    case 11:
    case 12:
        spr.fillRoundRect(0, 0, 50, 30, 5, 0xc4bf);
        break;
    default:
        spr.fillRoundRect(0, 0, 50, 30, 5, TFT_CYAN);
        break;
    }
    spr.pushSprite(38, 55);
    spr.deleteSprite();

    // 当前等级指示
    spr.createSprite(128, 6);
    switch (uvLevel)
    {
    case 1:
    case 2:
        spr.drawTriangle(15, 0, 10, 5, 20, 5, TFT_WHITE);
        break;
    case 3:
    case 4:
    case 5:
        spr.drawTriangle(39, 0, 34, 5, 44, 5, TFT_WHITE);
        break;
    case 6:
    case 7:
        spr.drawTriangle(63, 0, 58, 5, 68, 5, TFT_WHITE);
        break;
    case 8:
    case 9:
    case 10:
        spr.drawTriangle(87, 0, 82, 5, 92, 5, TFT_WHITE);
        break;
    case 11:
    case 12:
        spr.drawTriangle(111, 0, 106, 5, 116, 5, TFT_WHITE);
        break;
    default:
        break;
    }
    spr.pushSprite(0, 145);
    spr.deleteSprite();
}

/**
 * @name CLOCK_UI
 * @brief CLOCK_UI
 * @author levi5
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
    // spr.drawFastHLine(10, 16, 45,0xef7d);
    // spr.drawFastHLine(75, 16, 45,0xef7d);

    spr.loadFont(BoxedRegular35);
    spr.setTextDatum(CC_DATUM);
    spr.setTextColor(0xef7d);
    spr.drawString(num2str(currentHour) + " : " + num2str(currentMinute), 64, 20);

    // spr.setTextDatum(ML_DATUM);
    // spr.drawString(num2str(currentHour), 7, 18);//num2str(currentHour) + " : " + num2str(currentMinute)
    // spr.setTextDatum(CC_DATUM);
    // spr.drawString(" : ", 63, 18);//num2str(currentHour) + " : " + num2str(currentMinute)
    // spr.setTextDatum(MR_DATUM);
    // spr.drawString(num2str(currentMinute), 120, 18);//num2str(currentHour) + " : " + num2str(currentMinute)

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
 * @name AIQ_UI
 * @brief AIQ_UI
 * @author levi5
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

/**
 * @name FORECAST_UI
 * @brief FORECAST_UI
 * @author levi5
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
 * @name BILIBILI_UI
 * @brief BILIBILI_UI
 * @author levi5
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
 * @name BRIGHTNESS_UI
 * @brief BRIGHTNESS_UI
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
        {
            spr.fillRect(15, 145 - i * 7, 98, 5, 0xfa00);
        }
        else
        {
            spr.fillRect(15, 145 - i * 7, 98, 5, 0xa140);
        }
    } // 82 89 96 103 110 117 124 131 138 145
    // 10 34 58 82 106 130 154 178 202 226 250
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

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
    spr.loadFont(BoxedRegular18); // 10
    spr.pushImage(31, 2, 64, 64, icon_head);
    spr.drawString("ESP32 UV BOX", 0, 70);
    spr.drawString("author: levi5", 0, 90);
    spr.drawString("version: .v1", 0, 110);
    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}
/**
 * @name set_weather_icon
 * @brief 设置天气图标
 * @param icon 天气图标
 * @author levi5
 */
void set_weather_icon(int icon)
{
    switch (icon)
    {
    case 100:
    case 150:
        spr.pushImage(0, 20, 50, 50, icon_sunny);
        break;
    case 101:
    case 102:
    case 103:
    case 151:
    case 152:
    case 153:
        spr.pushImage(0, 20, 50, 50, icon_cloudy);
        break;
    case 104:
        spr.pushImage(0, 20, 50, 50, icon_overcast);
        break;
    case 305:
    case 309:
    case 313:
    case 314:
    case 399:
        spr.pushImage(0, 20, 50, 50, icon_light_rain);
        break;
    case 300:
    case 301:
        spr.pushImage(0, 20, 50, 50, icon_shower_rain);
        break;
    case 302:
    case 303:
    case 304:
        spr.pushImage(0, 20, 50, 50, icon_thundershower);
        break;
    case 306:
    case 307:
    case 308:
    case 310:
    case 311:
    case 312:
    case 315:
    case 316:
    case 317:
    case 318:
    case 350:
    case 351:
        spr.pushImage(0, 20, 50, 50, icon_heavy_rain);
        break;
    case 400:
    case 401:
    case 402:
    case 403:
    case 404:
    case 405:
    case 406:
    case 407:
    case 408:
    case 409:
    case 410:
    case 456:
    case 457:
    case 499:
        spr.pushImage(0, 20, 50, 50, icon_light_snow);
        break;
    case 500:
    case 501:
    case 509:
    case 510:
    case 514:
    case 515:
        spr.pushImage(0, 20, 50, 50, icon_foggy);
        break;
    case 502:
    case 511:
    case 512:
    case 513:
        spr.pushImage(0, 20, 50, 50, icon_haze);
        break;
    case 503:
    case 504:
        spr.pushImage(0, 20, 50, 50, icon_sand);
        break;
    case 507:
    case 508:
        spr.pushImage(0, 20, 50, 50, icon_sandstorm);
        break;
    case 900:
        spr.pushImage(0, 20, 50, 50, icon_sunny); // hot
        break;
    case 901:
        spr.pushImage(0, 20, 50, 50, icon_light_snow); // cold
        break;
    case 999:
    default:
        spr.pushImage(0, 20, 50, 50, icon_null);
        break;
    }
}

void drawProgressbar(int n, int x, int y)
{
    tft.fillRect(x, y, 100, 14, 0x630C); // 白色
    tft.fillRect(x, y, n, 14, 0xF79E);   // 灰色
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(0xB596); // 介于白色灰色
    tft.drawString(String(n) + "%", x + 50, y);
}

extern int CPU_PERCENT;
extern int MEM_PERCENT;
extern String PROCESSOR_TEMP;
extern String PROCESSOR_PW;
extern String PROCESSOR_NAME;
extern String MEM_AVAILABLE;
extern String COMPUTER_NAME;
extern String host;

void COMPUTER_MONITOR_UI()
{
    // ----------------- 静态内容（只画一次） -----------------
    if (pushFlag)
    {
        Serial.println("COMPUTER_MONITOR - 128x160 Mode");
        tft.fillScreen(TFT_BLACK);

        // 顶部标题
        tft.setTextSize(2);
        tft.setTextColor(0x630C);
        tft.drawString("Monitor", 35, 5); // 标题居中一点

        // 1. 顶部标题分割线 (Y=25)
        tft.drawFastHLine(0, 25, 128, 0x2104); // 暗灰色线，宽度128

        // 2. 在屏幕正中间 (Y=85) 画一条分割线，区分 CPU 和 RAM 区块
        tft.drawFastHLine(0, 85, 128, 0x2104);

        // 3. 设置静态字体 (1 号字，宽6高8)
        tft.setTextSize(1);
        tft.setTextColor(0x630C);

        // CPU 区块静态标签 (Y依次为 32, 52, 72)
        tft.drawString("CPU:", 5, 32);
        tft.drawString("Temp:", 5, 52);
        tft.drawString("PWR:", 5, 72);

        // RAM 和 系统区块静态标签 (Y依次为 95, 115, 135)
        tft.drawString("RAM:", 5, 95);
        tft.drawString("Free:", 5, 115);
        tft.drawString("PC:", 5, 135);

        pushFlag = false;
    }

    // ----------------- 动态内容（Sprite 局部刷新防闪烁） -----------------
    Computer_Monitor_Get(); // 获取最新的监控数据，更新全局变量

    // 1. CPU 百分比与进度条
    // 画布宽度改为 75，X 起点为 48 (48 + 75 = 123，右侧留白 5 像素)
    spr.createSprite(75, 10);
    spr.fillRect(0, 0, 75, 10, 0x3186);                       // 暗色底槽
    spr.fillRect(0, 0, (CPU_PERCENT * 75 / 100), 10, 0xF79E); // 按75像素比例计算
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(MC_DATUM);                       // 文字居中对齐
    spr.drawString(String(CPU_PERCENT) + "%", 37, 5); // 居中坐标为 37
    spr.pushSprite(50, 37);                           // X=48往右移，Y=33往下微调1像素对齐文字中心
    spr.deleteSprite();

    // 2. CPU 温度
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(PROCESSOR_TEMP, 0, 0);
    spr.pushSprite(50, 57); // Y=53往下微调1像素
    spr.deleteSprite();

    // 3. CPU 功耗
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(PROCESSOR_PW, 0, 0);
    spr.pushSprite(50, 77); // Y=73往下微调1像素
    spr.deleteSprite();

    // 4. RAM 百分比与进度条
    spr.createSprite(75, 10);
    spr.fillRect(0, 0, 75, 10, 0x3186);
    spr.fillRect(0, 0, (MEM_PERCENT * 75 / 100), 10, 0xF79E); // 按75像素比例计算
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(MC_DATUM);
    spr.drawString(String(MEM_PERCENT) + "%", 37, 5);
    spr.pushSprite(50, 100); // Y=96往下微调1像素
    spr.deleteSprite();

    // 5. RAM 剩余容量
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(MEM_AVAILABLE, 0, 0);
    spr.pushSprite(50, 120); // Y=116往下微调1像素
    spr.deleteSprite();

    // 6. 电脑名称 / 处理器名称
    spr.createSprite(75, 10);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    // 画布宽度75，Size 1 字体每个字母宽6，最多显示12个字符防超界截断
    spr.drawString(PROCESSOR_NAME.substring(0, 12), 0, 0);
    spr.pushSprite(50, 140); // Y=136往下微调1像素
    spr.deleteSprite();
}

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