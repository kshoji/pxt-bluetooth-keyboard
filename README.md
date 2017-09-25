# bluetooth-keyboard

This PXT package allows the micro:bit to act as a Keyboard peripheral.

## Make connection to micro:bit as a Keyboard device

Once a connection succeeded, the host recognize the `micro:bit` as a Keyboard.

1. At first, the host should be paired with `micro:bit` . At this time, the `micro:bit` doesn't act as Keyboard, it's just paired with each other.
1. Next, connect once to `micro:bit` and disconnect from it. The host checks BLE services/characteristics, and understands that the `micro:bit` has Keyboard function.
1. And Then, connect with `micro:bit` again.

## Usage

Place a ``||bluetooth start keyboard service||`` block in your program to enable Bluetooth LE Keyboard.

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
Mac OS X can't recognize `micro:bit` as a Keyboard. (Maybe because of: this implementation lacks BLE Advertising.)

## Supported targets

* for PXT/microbit

(The metadata above is needed for package search.)

## License

MIT

```package
bluetooth
bluetooth-keyboard=github:kshoji/pxt-bluetooth-keyboard
```
