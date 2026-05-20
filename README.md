# OMOTE - Open Universal Remote - Firmware

## OTA (Over-the-Air) Updates

OTA is enabled for the `esp32-s3-Rev5` environment. After flashing once via USB with the new partition table, subsequent firmware updates can be pushed wirelessly.

The device runs an HTTP server on port 3232. Your machine POSTs the firmware binary to it — all connections are host → device, so it works across VLANs as long as your machine can reach the IoT VLAN (no return connection required).

### Password

OTA updates and [telnet](#telnet) are protected by the same password, taken from the environment variable `OTA_PASSWORD`. It is compiled into the firmware and used by the upload script. For example on Windows:

```
setx OTA_PASSWORD "your password"
```

Then restart the terminal and the IDE, so that they see the variable.

**A firmware built without the variable has OTA and telnet disabled**, also when it is flashed by USB. The upload script refuses to send such a firmware.

To change the password, set the new one in `OTA_PASSWORD` and the old one in `OTA_PASSWORD_OLD` for the next upload.

The password is sent unencrypted, by OTA (HTTP basic authentication) as well as by telnet. After 3 wrong passwords, further attempts are refused for 30 seconds.

**To push an OTA update:** select the `esp32-s3-Rev5-ota` environment in PlatformIO and run **Upload**. The device must be awake and on WiFi. In VSCode this environment appears at the bottom of the Project Tasks panel — scroll down if you don't see it. You can also use the CLI:

```
pio run -e esp32-s3-Rev5-ota -t upload
```

Or with curl directly:

```
curl -u omote -F "firmware=@.pio/build/esp32-s3-Rev5/firmware.bin" http://OMOTE:3232/update
```

If `OMOTE` doesn't resolve, replace `upload_port` in `platformio.ini` (or the URL above) with the device's IP address, visible in the serial console at startup.

**To disable OTA** (e.g. if the firmware grows too large for the 6 MB slot):

1. In [platformio.ini](platformio.ini), under `[env:esp32-s3-Rev5]`:
   - Change `board_build.partitions` back to `noota_16MB_custom.csv`
   - In `build_unflags`: remove `-D ENABLE_OTA=0`
   - In `build_flags`: change `-D ENABLE_OTA=1` to `-D ENABLE_OTA=0`
2. Flash via USB — this restores the single 12.5 MB app slot.

No other source files need touching; all OTA code is `#if (ENABLE_OTA == 1)` gated in `hardware/ESP32/ota_hal_esp32.cpp`.

## Battery state on the keyboard backlight

Only for hardware rev 5 and higher.

- When the remote is picked up and wakes from sleep with a battery below 10%, the keyboard backlight blinks for 5 seconds. This needs "lift to wake" to be enabled.
- While charging with a battery below 95%, the keyboard backlight does a long blink (2 seconds on at 25% brightness, 2 seconds off) and the remote stays awake, with the screen on. Plugging in the cable does not wake the remote by itself.
- Both signals are configured with the same commands on the serial port (115200 baud), with `<s>` being `low` or `charge`: `blink <s>` (battery level, 0 = off), `blink <s> time`, `blink <s> speed` and `blink <s> light`. `blink reset` restores the defaults.

### Telnet

The same commands are available over the network with telnet (port 23). It is enabled as soon as a [password](#password) is set.

Connect with `telnet <hostname or IP address>`, or with PuTTY and the connection type "Telnet". Type `exit` to close the session.

Activities can be started and single commands can be sent, by telnet as well as on the serial port. Names are not case sensitive.

| Command | Effect |
|---|---|
| `scene` | List the activities, the active one is marked with `*` |
| `scene Regarder un film` | Start an activity. `scene Off` switches everything off |
| `scene TV force` | Run the start sequence again, even if the activity is already active |
| `cmd list` / `cmd list bose` | List all commands, or those containing a text |
| `cmd BOSE_VOL_UP` | Send a command, named like in the source code |
| `cmd HOME_MQTT_COMMAND toggle:switch.eclairage_salon` | Send a command with a payload (MQTT and BLE keyboard only) |

- Telnet is **not encrypted**: the password and the commands can be read on the network. Only use it on a network you trust.
- The remote can only be reached while it is awake and connected to WiFi. An open session keeps it awake, and is closed after 5 minutes without activity.
- There is one session at a time. After 3 wrong passwords, new connections are refused for 30 seconds.
- Received IR codes (`ir on`) and BLE messages are shown in the session. The other logs are only on the serial port.


---

GUIS - I've used Squareline Studio for some of this stuff, and cherry-picked the output code
It is a completely manual process, unlike some others I've worked on.

I have backing art on some screens, Squareline Studio generated the code, but the asset files I've used come from the online [image converter](https://lvgl.io/tools/imageconverter)

Even so I've had to modify the code to match the Apple logo asset.  This is probably due to version
mismatches. LVGL in this build is 8.4.0


# ---- original readme follows ---
![Ubuntu build](https://github.com/OMOTE-Community/OMOTE-Firmware/actions/workflows/build-platformio-ubuntu.yml/badge.svg)
![Windows Build](https://github.com/OMOTE-Community/OMOTE-Firmware/actions/workflows/build-platformio-windows.yml/badge.svg)
![MacOS Build](https://github.com/OMOTE-Community/OMOTE-Firmware/actions/workflows/build-platformio-macos.yml/badge.svg)
[![OMOTE Discord](https://discordapp.com/api/guilds/1138116475559882852/widget.png?style=shield)][link1]

## Overview

This is the ESP32 Arduino based firmware for the OMOTE - Open Universal Remote.

To run this firmware, you have two options
*  run it on the [OMOTE ESP32 Hardware](https://github.com/OMOTE-Community/OMOTE-Hardware/)
*  run it in the simulator on Linux, macOs or Windows

### The state of this project

The software can be adjusted to your needs. You can add your own amplifier, TV and media player. Smart home devices can be controlled with MQTT. The software is an example made up of:
* a TV and an amplifier controlled with infrared
* a Fire TV media player controlled with BLE (bluetooth keyboard)
* some smart home devices controlled with MQTT
* an IR receiver for decoding the IR codes from your remote

Please see the [wiki on how to understand and modify the firmware.](https://github.com/OMOTE-Community/OMOTE-Firmware/wiki/How-to-understand-and-modify-the-firmware)

You need to have PlatformIO running, and you need to know how to compile and flash your own firmware with PlatformIO. There is no prebuilt firmware.

The remote can be charged and programmed via its USB-C port. Open the PlatformIO project to compile and upload the code to the ESP32.

As a long term goal, maybe a prebuilt firmware will be published, where you can configure your OMOTE via a web interface.

### LVGL GUI simulator for Windows, Linux, and macOS

A simulator for running the LVGL UI on your local Windows, Linux, or macOS machine is available.

You can run the simulator in Visual Studio Code with PlatformIO. No need for any other compiler or development environment (no Visual Studio needed as often done in other LVGL simulators).
<div align="center">
  <img src="images/WindowsSimulator.gif" width="60%">
</div>

For details, please see the [wiki for the software simulator for fast creating and testing of LVGL GUIs.](https://github.com/OMOTE-Community/OMOTE-Firmware/wiki/Software-simulator-for-fast-creating-and-testing-of-LVGL-GUIs)

### To-dos for software

Long term goals (not yet scheduled)
- [ ] Add an interface for graphically editing the configuration
- [ ] Store the configuration in Flash (e.g. as a editable json file)

See the [open issues](https://github.com/OMOTE-Community/OMOTE-Firmware/issues) and [discussions](https://github.com/OMOTE-Community/OMOTE-Firmware/discussions) for a full list of proposed features (and known issues).

## Contributing

If you have a suggestion for an improvement, please fork the repo and create a pull request. You can also simply open an issue or - for more general feature requests - head over to the [discussions](https://github.com/OMOTE-Community/OMOTE-Firmware/discussions).

## License

Distributed under the GPL v3 License. See [LICENSE](https://github.com/OMOTE-Community/OMOTE-Firmware/blob/main/LICENSE) for more information.

## Contact

[![OMOTE Discord](https://discordapp.com/api/guilds/1138116475559882852/widget.png?style=banner2 "OMOTE Discord")][link1]

Join the OMOTE Discord: [https://discord.gg/5PnYFAsKsG](https://discord.gg/5PnYFAsKsG)

[link1]: https://discord.gg/5PnYFAsKsG
