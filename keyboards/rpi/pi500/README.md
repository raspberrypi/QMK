# Pi 500 Membrane Keyboard

![qmk_pi500_mem](imgur.com image replace me!)

This is the software for the Pi 500 Keyboard. It is a "matrix" keyboard with the firmware running on an RP2040 microprocessor
## Making firmware

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

Once QMK is setup, clone this repository into `qmk_firmware/keyboards/rpi` folder.

To build the membrane keyboard (all languages), run following command in qmk environment:

    qmk compile -kb rpi/pi500 -km default -j 0

## Bootloader

Entering the bootloader:

Bootsel and run can be triggered through the GPIO pins on the pi500.

    pinctrl KEYB_BOOTSEL op pn dh
    pinctrl KEYB_RUN op pn dl
    pinctrl KEYB_RUN dh
    pinctrl KEYB_BOOTSEL dl

