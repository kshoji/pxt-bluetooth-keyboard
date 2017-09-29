#include "pxt.h"
#include "BluetoothKeyboardService.h"
using namespace pxt;
/**
 * A set of functions to send keyboard commands over Bluetooth
 */
namespace bluetooth
{
BluetoothKeyboardService *pKeyboardInstance = nullptr;
BluetoothKeyboardService *getKeyboard()
{
    if (pKeyboardInstance == nullptr)
    {
        pKeyboardInstance = new BluetoothKeyboardService(uBit.ble);
    }
    return pKeyboardInstance;
}

//%
void keyboardSendOneKeyCode(Modifier modifier, uint8_t keyCode)
{
    BluetoothKeyboardService *pKeyboard = getKeyboard();
    pKeyboard->sendKeyCode(modifier, keyCode);
}

//%
uint8_t keyboardGetKeyCode(uint8_t character)
{
    BluetoothKeyboardService *pKeyboard = getKeyboard();
    return pKeyboard->getKeyCode(character);
}

//%
void keyboardSendText(StringData *data)
{
    BluetoothKeyboardService *pKeyboard = getKeyboard();
    ManagedString buf(data);
    if (buf.length() > 0)
    {
        pKeyboard->sendString(buf);
    }
}
}
