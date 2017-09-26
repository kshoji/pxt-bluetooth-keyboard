#ifndef __BLEKEYBOARD_H__
#define __BLEKEYBOARD_H__

#include "ble/BLE.h"
#include "ble/GattAttribute.h"

#define BLE_UUID_DESCRIPTOR_REPORT_REFERENCE 0x2908

enum ModifierKey
{
    MODIFIER_KEY_NONE = 0,
    MODIFIER_KEY_CTRL = 1,
    MODIFIER_KEY_SHIFT = 2,
    MODIFIER_KEY_ALT = 4,
};

enum KeyCodes
{
    KEY_ENTER = 0x28,
    KEY_BS = 0x2a,
    KEY_DEL = 0x2a,
    KEY_TAB = 0x2b,

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
    KEY_CAPS_LOCK = 0x39,
    KEY_NUM_LOCK = 0x53,
    KEY_INSERT = 0x49,
    KEY_HOME = 0x4a,

    KEY_PAGE_UP = 0x4b,
    KEY_PAGE_DOWN = 0x4e,

    KEY_RIGHT_ARROW = 0x4f,
    KEY_LEFT_ARROW = 0x50,
    KEY_DOWN_ARROW = 0x51,
    KEY_UP_ARROW = 0x52,
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
    void sendKeyDownMessage(ModifierKey modifier, uint8_t keyCode);

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
    void sendKeyCode(ModifierKey modifier, uint8_t keyCode);

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
    ModifierKey getModifier(uint8_t character);

  private:
    BLEDevice &ble;
    bool connected;

    Ticker reportTicker;
    bool reportTickerIsActive;

    uint8_t protocolMode;
    uint8_t controlPointCommand;
    uint8_t inputReportData[8];

    ManagedString keyBuffer;
    ModifierKey previousModifier;
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
