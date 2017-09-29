#ifndef __BLEKEYBOARD_H__
#define __BLEKEYBOARD_H__

#include "ble/BLE.h"
#include "ble/GattAttribute.h"

#define BLE_UUID_DESCRIPTOR_REPORT_REFERENCE 0x2908

enum Modifier
{
    MODIFIER_NONE = 0,

    MODIFIER_LEFT_CTRL = 1,
    MODIFIER_LEFT_SHIFT = 2,
    MODIFIER_LEFT_ALT = 4,
    MODIFIER_LEFT_META = 8,

    MODIFIER_RIGHT_CTRL = 16,
    MODIFIER_RIGHT_SHIFT = 32,
    MODIFIER_RIGHT_ALT = 64,
    MODIFIER_RIGHT_META = 128,
};

enum KeyCode
{
    KEY_NONE = 0x00,

    KEY_A = 0x04,
    KEY_B = 0x05,
    KEY_C = 0x06,
    KEY_D = 0x07,
    KEY_E = 0x08,
    KEY_F = 0x09,
    KEY_G = 0x0a,
    KEY_H = 0x0b,
    KEY_I = 0x0c,
    KEY_J = 0x0d,
    KEY_K = 0x0e,
    KEY_L = 0x0f,
    KEY_M = 0x10,
    KEY_N = 0x11,
    KEY_O = 0x12,
    KEY_P = 0x13,
    KEY_Q = 0x14,
    KEY_R = 0x15,
    KEY_S = 0x16,
    KEY_T = 0x17,
    KEY_U = 0x18,
    KEY_V = 0x19,
    KEY_W = 0x1a,
    KEY_X = 0x1b,
    KEY_Y = 0x1c,
    KEY_Z = 0x1d,

    KEY_1 = 0x1e,
    KEY_2 = 0x1f,
    KEY_3 = 0x20,
    KEY_4 = 0x21,
    KEY_5 = 0x22,
    KEY_6 = 0x23,
    KEY_7 = 0x24,
    KEY_8 = 0x25,
    KEY_9 = 0x26,
    KEY_0 = 0x27,

    KEY_ENTER = 0x28,
    KEY_ESCAPE = 0x29,
    KEY_BACKSPACE = 0x2a,
    KEY_TAB = 0x2b,
    KEY_SPACE = 0x2c,

    KEY_MINUS = 0x2d,
    KEY_EQUAL = 0x2e,
    KEY_LEFT_BRACKET = 0x2f,
    KEY_RIGHT_BRACKET = 0x30,
    KEY_BACKSLASH = 0x31,
    KEY_SEMICOLON = 0x33,
    KEY_APOSTROPHE = 0x34,
    KEY_GRAVE_ACCENT = 0x35,
    KEY_COMMA = 0x36,
    KEY_PERIOD = 0x37,
    KEY_SLASH = 0x38,
    KEY_CAPS_LOCK = 0x39,

    KEY_F1 = 0x3a,
    KEY_F2 = 0x3b,
    KEY_F3 = 0x3c,
    KEY_F4 = 0x3d,
    KEY_F5 = 0x3e,
    KEY_F6 = 0x3f,
    KEY_F7 = 0x40,
    KEY_F8 = 0x41,
    KEY_F9 = 0x42,
    KEY_F10 = 0x43,
    KEY_F11 = 0x44,
    KEY_F12 = 0x45,

    KEY_PRINT_SCREEN = 0x46,
    KEY_SCROLL_LOCK = 0x47,
    KEY_PAUSE = 0x48,
    KEY_INSERT = 0x49,
    KEY_HOME = 0x4a,
    KEY_PAGE_UP = 0x4b,
    KEY_DELETE = 0x4c,
    KEY_END = 0x4d,
    KEY_PAGE_DOWN = 0x4e,

    KEY_RIGHT_ARROW = 0x4f,
    KEY_LEFT_ARROW = 0x50,
    KEY_DOWN_ARROW = 0x51,
    KEY_UP_ARROW = 0x52,
    KEY_NUM_LOCK = 0x53,

    KEY_KEYPAD_DIVIDE = 0x54,
    KEY_KEYPAD_MULTIPLY = 0x55,
    KEY_KEYPAD_SUBTRACT = 0x56,
    KEY_KEYPAD_ADD = 0x57,
    KEY_KEYPAD_ENTER = 0x58,
    KEY_KEYPAD_1 = 0x59,
    KEY_KEYPAD_2 = 0x5A,
    KEY_KEYPAD_3 = 0x5B,
    KEY_KEYPAD_4 = 0x5C,
    KEY_KEYPAD_5 = 0x5D,
    KEY_KEYPAD_6 = 0x5E,
    KEY_KEYPAD_7 = 0x5F,
    KEY_KEYPAD_8 = 0x60,
    KEY_KEYPAD_9 = 0x61,
    KEY_KEYPAD_0 = 0x62,
    KEY_KEYPAD_DECIMAL = 0x63,

    KEY_EUROPE_2 = 0x64,

    KEY_APPLICATION = 0x65,
    KEY_POWER = 0x66,

    KEY_KEYPAD_EQUAL = 0x67,

    KEY_F13 = 0x68,
    KEY_F14 = 0x69,
    KEY_F15 = 0x6A,

    KEY_CONTROL_LEFT = 0xE0,
    KEY_SHIFT_LEFT = 0xE1,
    KEY_ALT_LEFT = 0xE2,
    KEY_GUI_LEFT = 0xE3,
    KEY_CONTROL_RIGHT = 0xE4,
    KEY_SHIFT_RIGHT = 0xE5,
    KEY_ALT_RIGHT = 0xE6,
    KEY_GUI_RIGHT = 0xE7,
};

#define INPUT_REPORT 0x1
#define OUTPUT_REPORT 0x2
#define FEATURE_REPORT 0x3

#define BOOT_PROTOCOL 0x0
#define REPORT_PROTOCOL 0x1

typedef struct
{
    uint8_t ID;
    uint8_t type;
} report_reference_t;

/** 
 * A class to communicate a BLE Keyboard device
 */
class BluetoothKeyboardService
{
  public:
    /**
     * Constructor
     * @param dev BLE device
     */
    BluetoothKeyboardService(BLEDevice *device);

    /**
     * Send a BLE Keyboard key-down message
     * @param modifier modifier key combination
     * @param keyCode key code of US Keyboard
     */
    void sendKeyDownMessage(Modifier modifier, uint8_t keyCode);

    /**
     * Send a BLE Keyboard key-up message
     */
    void sendKeyUpMessage();

    /**
     * Send a text with BLE Keyboard
     * @param text ASCII string
     */
    void sendString(ManagedString text);

    /**
     * Send a BLE Keyboard key-up/down message
     * @param modifier modifier key combination
     * @param keyCode key code of US Keyboard
     */
    void sendKeyCode(Modifier modifier, uint8_t keyCode);

    /**
     * Get the Key code for specified character
     * @param character ASCII character code
     * @return key code of US Keyboard
     */
    uint8_t getKeyCode(uint8_t character);

    /**
     * Get the modifier key combination for specified character
     * @param character ASCII character code
     * @return modifier key combination
     */
    Modifier getModifier(uint8_t character);

  private:
    BLEDevice &ble;
    bool connected;

    Ticker reportTicker;
    bool reportTickerIsActive;

    uint8_t protocolMode;
    uint8_t controlPointCommand;
    uint8_t inputReportData[8];

    ManagedString keyBuffer;
    Modifier previousModifier;
    uint8_t previousKeyCode;
    bool sendKeyUp = false;

    void onConnection(const Gap::ConnectionCallbackParams_t *params);
    void onDisconnection(const Gap::DisconnectionCallbackParams_t *params);

    GattCharacteristic *protocolModeCharacteristic;
    GattCharacteristic *inputReportCharacteristic;
    GattCharacteristic *reportMapCharacteristic;
    GattCharacteristic *hidInformationCharacteristic;
    GattCharacteristic *hidControlPointCharacteristic;

    GattAttribute *inputReportReferenceDescriptor;
    GattAttribute *inputReportDescriptors[1];

    void startReportTicker();

    void stopReportTicker();

    void onDataSent(unsigned count);

    void sendCallback();

    void startAdvertise();

    void startService();
};

#endif /* __BLEKEYBOARD_H__ */
