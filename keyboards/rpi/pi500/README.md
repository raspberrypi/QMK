# Pi 500 Membrane Keyboard

This is the software for the Pi 500 Keyboard. It's a [membrane keyboard](https://en.wikipedia.org/wiki/Membrane_keyboard) with the firmware running on an RP2040 microprocessor

## Making firmware

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

The Pi 500 keyboard code is in the `qmk_firmware/keyboards/rpi/pi500` folder.

To build the keyboard, run following command in the qmk environment:

    qmk compile -kb rpi/pi500 -km default -j 0

## Programming the keyboard

The firmware on the RP2040 is programmed like any other RP2040 device with Bootsel which can be triggered through the GPIO pins on the pi500.

    pinctrl KEYB_BOOTSEL op pn dh
    pinctrl KEYB_RUN op pn dl
    pinctrl KEYB_RUN dh
    pinctrl KEYB_BOOTSEL dl

Then drag the `qmk_firmware/rpi_pi500_default.uf2` file onto the RP2040 drive.
