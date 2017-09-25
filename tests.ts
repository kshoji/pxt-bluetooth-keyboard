bluetooth.onBluetoothConnected(() => {
    basic.showString("C")
})

bluetooth.onBluetoothDisconnected(() => {
    basic.showString("D")
})

bluetooth.startKeyboardService();
basic.showString("S")

input.onButtonPressed(Button.A, () => {
    basic.clearScreen()
    led.toggle(0, 0)

    bluetooth.keyboardSendText("Hello World!")
})

input.onButtonPressed(Button.B, () => {
    basic.clearScreen()
    led.toggle(1, 1)

    // ctrl-alt-del
    bluetooth.keyboardSendOneKeyCode(ModifierKey.MODIFIER_KEY_CTRL | ModifierKey.MODIFIER_KEY_ALT, KeyCodes.KEY_DEL)
})
