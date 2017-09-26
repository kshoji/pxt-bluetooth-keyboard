# bluetooth-keyboard

This PXT package allows the micro:bit to act as a Keyboard peripheral.

## Usage

Place a ``||bluetooth start keyboard service||`` block in your program to enable Bluetooth LE Keyboard.
With this block, the `micro:bit` starts advertise BLE packets as a Keyboard peripheral.

```blocks
bluetooth.startKeyboardService();
```

**TBD**
To Send `Hello World!` to the paired host, place block like this:

```blocks
bluetooth.keyboardSendText("Hello World!");
```

## Supported Platforms

Currently, tested with `micro:bit` and `Android` host only.
Mac OS X can connect with `micro:bit`, but it can't receive Keyboard message.

## Supported targets

* for PXT/microbit

(The metadata above is needed for package search.)

## License

MIT

```package
bluetooth
bluetooth-keyboard=github:kshoji/pxt-bluetooth-keyboard
```
