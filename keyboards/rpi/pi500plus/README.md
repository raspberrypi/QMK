# Pi 500+ Mechanical Keyboard

This is the software for the Pi 500+ Keyboard. It's a mechanical keyboard with the firmware running on an RP2040 microprocessor. 

There are three versions of the Pi 500+ keyboard for different keyboard country layouts:
- 84 Key ANSI for US layout
- 85 Key ISO for European layouts (UK/FR/ES/DE/IT/PT/NOR) 
- 88 Key JIS for Japanese layout

## Making firmware

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

The Pi 500+ keyboard code is in the `qmk_firmware/keyboards/rpi/pi500plus` folder.

To build the ANSI keyboard firmware, run the following command in the qmk environment:

    qmk compile -kb rpi/pi500plus/ansi -km default -j 0

To build the ISO keyboard firmware, run the following command in the qmk environment:

    qmk compile -kb rpi/pi500plus/iso -km default -j 0

To build the JIS keyboard firmware, run the following command in the qmk environment:

    qmk compile -kb rpi/pi500plus/jis -km default -j 0

After build, the firmware .uf2 file can be found in the `qmk_firwmare/.build` folder.

## Programming the keyboard

The firmware on the RP2040 is programmed like any other RP2040 device with Bootsel which can be triggered through the GPIO pins on the Pi 500+.

    pinctrl KEYB_BOOTSEL op pn dh
    pinctrl KEYB_RUN op pn dl
    pinctrl KEYB_RUN dh
    pinctrl KEYB_BOOTSEL dl

Then drag the `qmk_firmware/rpi_pi500plus_{layout}_default.uf2` file onto the RP2040 drive.