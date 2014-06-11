#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ASSETS_BMP_MAP "assets/map.png"

#define SETTINGS_HIGHLIGHTOPACITY 0.5
#define SETTINGS_OPACITY 1
#define SETTINGS_BORDERWIDTH 3
#define SETTINGS_FONTFAMILY "Helvetica"
#define SETTINGS_FONTSIZE 38
#define SETTINGS_FONTWEIGHTBOLD true
#define SETTINGS_FONTCOLOR "White"
#define SETTINGS_RECTPADDING 10
#define SETTINGS_ALPHA 128

enum class SensorState
{
    Undefined,
    Disconnected,
    TemperatureNormal,
    TemperatureLower,
    TemperatureHigher
};

#endif // CONSTANTS_H
