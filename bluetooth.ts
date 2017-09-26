namespace bluetooth {
    /**
     * Starts the Keyboard service over Bluetooth and registers it as the Keyboard transport.
     */
    //% blockId="bluetooth_start_keyboard"
    //% block="bluetooth start keyboard service"
    //% parts="bluetooth"
    export function startKeyboardService() {
        function send(text: string) {
            bluetooth.keyboardSendText(text);
        }
        bluetooth.keyboardSendKeyDown(ModifierKey.MODIFIER_KEY_NONE, pins.createBuffer(0));
    }

    /**
     * Sends a Keyboard key-down message
     */
    //% shim=bluetooth::keyboardSendKeyDown
    //% advanced=true
    export function keyboardSendKeyDown(modifier: ModifierKey, data: Buffer) {
        return;
    }

    /**
     * Sends a Keyboard key-up message
     */
    //% shim=bluetooth::keyboardSendKeyUp
    //% advanced=true
    export function keyboardSendKeyUp() {
        return;
    }

    /**
     * Sends a Keyboard key-up/down message (with KeyCodes)
     */
    //% blockId="bluetooth_keyboard_send_one_key_code"
    //% block="keyboard|send one key with %modifier|and %keyCode"
    //% parts="bluetooth"
    //% shim=bluetooth::keyboardSendOneKeyCode
    //% advanced=true
    export function keyboardSendOneKeyCode(modifier: ModifierKey, keyCode: number) {
        return;
    }

    /**
     * Gets a Key code for specified character
     */
    //% blockId="bluetooth_keyboard_get_key_code"
    //% block="keyboard|get keyCode with %character"
    //% parts="bluetooth"
    //% shim=bluetooth::keyboardGetKeyCode
    //% advanced=true
    export function keyboardGetKeyCode(character: number): number {
        return;
    }

    /**
     * Sends a Keyboard message
     */
    //% blockId="bluetooth_keyboard_send_text"
    //% block="keyboard|send %text"
    //% parts="bluetooth"
    //% shim=bluetooth::keyboardSendText
    //% advanced=true
    export function keyboardSendText(text: string) {
        return;
    }
}
