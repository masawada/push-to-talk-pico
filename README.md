# push-to-talk-pico

A firmware that turns Raspberry Pi Pico / Pico 2 into a USB HID Telephony Device, using the BOOTSEL button as a push-to-talk button for video conferencing tools like Google Meet.

## Behavior

- BOOTSEL button pressed: Unmute (speak)
- BOOTSEL button released: Mute
- On-board LED indicates mute state (ON = unmuted)

## Supported Boards

- Raspberry Pi Pico (RP2040)
- Raspberry Pi Pico 2 (RP2350)

## Prerequisites

- [CMake](https://cmake.org/) (>= 3.17)
- [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm) (`arm-none-eabi-gcc`)

### macOS (Homebrew)

```sh
brew install cmake
brew install --cask gcc-arm-embedded
```

### Ubuntu / Debian

```sh
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi
```

## Build

### 1. Clone the repository

Make sure to clone with `--recursive` to fetch submodules.

```sh
git clone --recursive https://github.com/masawada/push-to-talk-pico.git
cd push-to-talk-pico
```

If you have already cloned the repository, sync and update the submodules:

```sh
git submodule sync --recursive
git submodule update --init --recursive
```

### 2. Run the build

```sh
./build.sh
```

This builds firmware for both Pico and Pico 2.

```
build/pico/push_to_talk_pico.uf2    # for Raspberry Pi Pico
build/pico2/push_to_talk_pico.uf2   # for Raspberry Pi Pico 2
```

## Flashing

1. Hold the BOOTSEL button on the Pico while plugging it into your PC via USB
2. The Pico will mount as a mass storage device
3. Drag and drop the `.uf2` file for your board:
   - Pico: `build/pico/push_to_talk_pico.uf2`
   - Pico 2: `build/pico2/push_to_talk_pico.uf2`
4. The Pico will automatically reboot and be recognized as a USB HID device

## License

MIT
