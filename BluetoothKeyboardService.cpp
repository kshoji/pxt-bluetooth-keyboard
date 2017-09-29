/** 
 * A class to communicate a BLE Keyboard device
 */
#include "MicroBit.h"
#include "pxt.h"
#include "ble/GapAdvertisingData.h"
#include "ble/GattService.h"
#include "ble/GattCharacteristic.h"
#include "BluetoothKeyboardService.h"
#include "USBHID_Types.h"

namespace
{

static const uint8_t RESPONSE_HID_INFORMATION[] = {0x11, 0x01, 0x00, 0x03};

static const uint16_t uuid16_list[] = {GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE,
                                       GattService::UUID_DEVICE_INFORMATION_SERVICE};

/**
 * Characteristic Data(Report Map)
 */
static const uint8_t REPORT_MAP[] = {
    USAGE_PAGE(1), 0x01, // Generic Desktop Ctrls
    USAGE(1), 0x06,      // Keyboard
    COLLECTION(1), 0x01, // Application
    USAGE_PAGE(1), 0x07, //   Kbrd/Keypad
    USAGE_MINIMUM(1), 0xE0,
    USAGE_MAXIMUM(1), 0xE7,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_SIZE(1), 0x01, //   1 byte (Modifier)
    REPORT_COUNT(1), 0x08,
    INPUT(1), 0x02,        //   Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position
    REPORT_COUNT(1), 0x01, //   1 byte (Reserved)
    REPORT_SIZE(1), 0x08,
    INPUT(1), 0x01,        //   Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
    REPORT_COUNT(1), 0x05, //   5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01,
    USAGE_PAGE(1), 0x08,    //   LEDs
    USAGE_MINIMUM(1), 0x01, //   Num Lock
    USAGE_MAXIMUM(1), 0x05, //   Kana
    OUTPUT(1), 0x02,        //   Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile
    REPORT_COUNT(1), 0x01,  //   3 bits (Padding)
    REPORT_SIZE(1), 0x03,
    OUTPUT(1), 0x01,       //   Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile
    REPORT_COUNT(1), 0x06, //   6 bytes (Keys)
    REPORT_SIZE(1), 0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x65, //   101 keys
    USAGE_PAGE(1), 0x07,      //   Kbrd/Keypad
    USAGE_MINIMUM(1), 0x00,
    USAGE_MAXIMUM(1), 0x65,
    INPUT(1), 0x00, //   Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
    END_COLLECTION(0),
};

static const uint8_t emptyInputReportData[] = {0, 0, 0, 0, 0, 0, 0, 0};
}

/**
 * Constructor
 * @param dev BLE device
 */
BluetoothKeyboardService::BluetoothKeyboardService(BLEDevice *dev) : ble(*dev)
{
    startService();
    startAdvertise();
}

void BluetoothKeyboardService::startService()
{
    memset(inputReportData, 0, sizeof(inputReportData));
    connected = false;
    protocolMode = REPORT_PROTOCOL;
    reportTickerIsActive = false;

    protocolModeCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_PROTOCOL_MODE_CHAR,
                                                        &protocolMode, 1, 1,
                                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

    inputReportCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_REPORT_CHAR,
                                                       inputReportData, sizeof(inputReportData), sizeof(inputReportData),
                                                       GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                                           GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
                                                           GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    reportMapCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_REPORT_MAP_CHAR,
                                                     const_cast<uint8_t *>(REPORT_MAP), sizeof(REPORT_MAP), sizeof(REPORT_MAP),
                                                     GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);

    hidInformationCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_HID_INFORMATION_CHAR,
                                                          const_cast<uint8_t *>(RESPONSE_HID_INFORMATION), sizeof(RESPONSE_HID_INFORMATION), sizeof(RESPONSE_HID_INFORMATION),
                                                          GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);

    hidControlPointCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_HID_CONTROL_POINT_CHAR,
                                                           &controlPointCommand, 1, 1,
                                                           GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

    SecurityManager::SecurityMode_t securityMode = SecurityManager::SECURITY_MODE_ENCRYPTION_NO_MITM;
    protocolModeCharacteristic->requireSecurity(securityMode);
    inputReportCharacteristic->requireSecurity(securityMode);
    reportMapCharacteristic->requireSecurity(securityMode);

    GattCharacteristic *keyboardCharacteristics[]{
        hidInformationCharacteristic,
        reportMapCharacteristic,
        protocolModeCharacteristic,
        hidControlPointCharacteristic,
        inputReportCharacteristic};

    ble.gap().onConnection(this, &BluetoothKeyboardService::onConnection);
    ble.gap().onDisconnection(this, &BluetoothKeyboardService::onDisconnection);

    GattService keyboardService(GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE, keyboardCharacteristics, sizeof(keyboardCharacteristics) / sizeof(GattCharacteristic *));

    ble.gattServer().addService(keyboardService);

    ble.gattServer().onDataSent(this, &BluetoothKeyboardService::onDataSent);
}

void BluetoothKeyboardService::startAdvertise()
{
    ble.gap().stopAdvertising();
    ble.gap().clearAdvertisingPayload();

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED |
                                           GapAdvertisingData::LE_GENERAL_DISCOVERABLE);

    ManagedString BLEName("BBC micro:bit");
    ManagedString namePrefix(" [");
    ManagedString namePostfix("]");
    BLEName = BLEName + namePrefix + uBit.getName() + namePostfix;

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME,
                                           (uint8_t *)BLEName.toCharArray(), BLEName.length());

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS,
                                           (uint8_t *)uuid16_list, sizeof(uuid16_list));

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::KEYBOARD);

    uint16_t minInterval = Gap::MSEC_TO_GAP_DURATION_UNITS(25);
    if (minInterval < 6)
    {
        minInterval = 6;
    }
    uint16_t maxInterval = minInterval * 2;
    Gap::ConnectionParams_t params = {minInterval, maxInterval, 0, 3200}; // timeout in 32 seconds
    ble.gap().setPreferredConnectionParams(&params);

    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(50);
    ble.gap().setAdvertisingPolicyMode(Gap::ADV_POLICY_IGNORE_WHITELIST);
    ble.gap().startAdvertising();
}

void BluetoothKeyboardService::startReportTicker()
{
    if (reportTickerIsActive)
    {
        return;
    }
    reportTicker.attach_us(this, &BluetoothKeyboardService::sendCallback, 24000);
    reportTickerIsActive = true;
}

void BluetoothKeyboardService::stopReportTicker()
{
    if (!reportTickerIsActive)
    {
        return;
    }
    reportTicker.detach();
    reportTickerIsActive = false;
}

void BluetoothKeyboardService::onDataSent(unsigned count)
{
    startReportTicker();
}

void BluetoothKeyboardService::onConnection(const Gap::ConnectionCallbackParams_t *params)
{
    ble.gap().stopAdvertising();
    connected = true;
    keyBuffer = ManagedString::EmptyString;
    previousModifier = MODIFIER_NONE;
    previousKeyCode = 0;
}

void BluetoothKeyboardService::onDisconnection(const Gap::DisconnectionCallbackParams_t *params)
{
    connected = false;
    stopReportTicker();
    startAdvertise();
}

/**
 * Get the modifier key combination for specified character
 * @param character ASCII character code
 * @return modifier key combination
 */
Modifier BluetoothKeyboardService::getModifier(uint8_t character)
{
    switch (character)
    {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '!':
    case '@':
    case '#':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
    case '(':
    case ')':
    case '_':
    case '+':
    case '{':
    case '}':
    case '|':
    case ':':
    case '"':
    case '~':
    case '<':
    case '>':
    case '?':
        return MODIFIER_LEFT_SHIFT;
    default:
        return MODIFIER_NONE;
    }
}

/**
 * Get the Key code for specified character
 * @param character ASCII character code
 * @return key code of US Keyboard
 */
uint8_t BluetoothKeyboardService::getKeyCode(uint8_t character)
{
    switch (character)
    {
    case 'A':
    case 'a':
        return 0x04;
    case 'B':
    case 'b':
        return 0x05;
    case 'C':
    case 'c':
        return 0x06;
    case 'D':
    case 'd':
        return 0x07;
    case 'E':
    case 'e':
        return 0x08;
    case 'F':
    case 'f':
        return 0x09;
    case 'G':
    case 'g':
        return 0x0a;
    case 'H':
    case 'h':
        return 0x0b;
    case 'I':
    case 'i':
        return 0x0c;
    case 'J':
    case 'j':
        return 0x0d;
    case 'K':
    case 'k':
        return 0x0e;
    case 'L':
    case 'l':
        return 0x0f;
    case 'M':
    case 'm':
        return 0x10;
    case 'N':
    case 'n':
        return 0x11;
    case 'O':
    case 'o':
        return 0x12;
    case 'P':
    case 'p':
        return 0x13;
    case 'Q':
    case 'q':
        return 0x14;
    case 'R':
    case 'r':
        return 0x15;
    case 'S':
    case 's':
        return 0x16;
    case 'T':
    case 't':
        return 0x17;
    case 'U':
    case 'u':
        return 0x18;
    case 'V':
    case 'v':
        return 0x19;
    case 'W':
    case 'w':
        return 0x1a;
    case 'X':
    case 'x':
        return 0x1b;
    case 'Y':
    case 'y':
        return 0x1c;
    case 'Z':
    case 'z':
        return 0x1d;
    case '!':
    case '1':
        return 0x1e;
    case '@':
    case '2':
        return 0x1f;
    case '#':
    case '3':
        return 0x20;
    case '$':
    case '4':
        return 0x21;
    case '%':
    case '5':
        return 0x22;
    case '^':
    case '6':
        return 0x23;
    case '&':
    case '7':
        return 0x24;
    case '*':
    case '8':
        return 0x25;
    case '(':
    case '9':
        return 0x26;
    case ')':
    case '0':
        return 0x27;
    case '\n': // LF
        return 0x28;
    case '\b': // BS
        return 0x2a;
    case '\t': // TAB
        return 0x2b;
    case ' ':
        return 0x2c;
    case '_':
    case '-':
        return 0x2d;
    case '+':
    case '=':
        return 0x2e;
    case '{':
    case '[':
        return 0x2f;
    case '}':
    case ']':
        return 0x30;
    case '|':
    case '\\':
        return 0x31;
    case ':':
    case ';':
        return 0x33;
    case '"':
    case '\'':
        return 0x34;
    case '~':
    case '`':
        return 0x35;
    case '<':
    case ',':
        return 0x36;
    case '>':
    case '.':
        return 0x37;
    case '?':
    case '/':
        return 0x38;
    default:
        return 0;
    }
}

/**
 * Send a BLE Keyboard key-down message
 * @param modifier modifier key combination
 * @param keyCode key code of US Keyboard
 */
void BluetoothKeyboardService::sendKeyDownMessage(Modifier modifier, uint8_t keyCode)
{
    if (connected)
    {
        inputReportData[0] = modifier;
        inputReportData[1] = 0;
        inputReportData[2] = keyCode;
        inputReportData[3] = 0;
        inputReportData[4] = 0;
        inputReportData[5] = 0;
        inputReportData[6] = 0;
        inputReportData[7] = 0;

        ble.gattServer().write(inputReportCharacteristic->getValueHandle(), inputReportData, 8);
    }
}

/**
 * Send a BLE Keyboard key-up message
 */
void BluetoothKeyboardService::sendKeyUpMessage()
{
    if (connected)
    {
        ble.gattServer().write(inputReportCharacteristic->getValueHandle(), emptyInputReportData, 8);
    }
}

void BluetoothKeyboardService::sendCallback()
{
    if (connected)
    {
        int16_t length = keyBuffer.length();

        if (length > 0)
        {
            uint8_t charAt = keyBuffer.charAt(0);
            Modifier modifier = getModifier(charAt);
            uint8_t keyCode = getKeyCode(charAt);
            if (previousModifier == modifier && previousKeyCode == keyCode)
            {
                sendKeyUpMessage();
                previousModifier = MODIFIER_NONE;
                previousKeyCode = 0;
                return;
            }

            sendKeyDownMessage(modifier, keyCode);
            previousModifier = modifier;
            previousKeyCode = keyCode;
            // uBit.display.printChar((char)charAt);

            keyBuffer = keyBuffer.substring(1, length - 1);
            if (keyBuffer.length() == 0)
            {
                // last key sent, then send key-up message
                sendKeyUp = true;
            }
        }
        else if (sendKeyUp)
        {
            sendKeyUpMessage();
            sendKeyUp = false;
            stopReportTicker();
        }
    }
}

/**
 * Send a text with BLE Keyboard
 * @param text ASCII string
 */
void BluetoothKeyboardService::sendString(ManagedString text)
{
    if (connected)
    {
        keyBuffer = keyBuffer + text;
        // send first report
        sendCallback();
    }
}

/**
 * Send a BLE Keyboard key-up/down message
 */
void BluetoothKeyboardService::sendKeyCode(Modifier modifier, uint8_t keyCode)
{
    if (connected)
    {
        keyBuffer = ManagedString::EmptyString;

        sendKeyDownMessage(modifier, keyCode);
        sendKeyUp = true;
    }
}
