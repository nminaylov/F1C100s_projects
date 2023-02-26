# Bare metal libraries and projects for Allwinner F1C100s/F1C200s SOC

## Libraries and tools

- f1c100s/arm926 - Various ARMv5-related headers
- f1c100s/drivers - Low-level peripheral drivers. Currently work-in-progress: 
    - f1c100s_clock - Clock controller (CCU)
    - f1c100s_intc - Interrupt controller
    - f1c100s_gpio - GPIO, external interrupts
    - f1c100s_timer - Timers, watchdog
    - f1c100s_pwm - PWM outputs
    - f1c100s_uart - 16550-compatible UART
    - f1c100s_de - Display Engine (TCON, DEBE, DEFE)
    - f1c100s_tve - Undocumented TV(CVBS) encoder
    - f1c100s_tvd - Undocumented TV(CVBS) decoder

- lib - Miscellaneous non hardware-related libraries
- tools/mksunxi - A tool to add checksum to eGON.BT0 header. Taken from xboot project

## Projects

All projects are targeted for Lichee Nano devboard with 800x480 LCD. 

- hello_led - Yet another LED blinking project
- simple_loader - Very simple second-stage loader for booting bare metal apps from SPI flash
- lcd_test - Example of display driver usage with simple graphics library
- lvgl_demo - Demonstration project of LVGL library
- tv_out_test - Test project for TVE driver
- tv_in_test - Test project for TVD driver

## Required tools
- Visual Studio Code with Cortex-Debug plugin
- arm-none-eabi-gcc

For debugging:
- Any OpenOCD-compatible JTAG adapter (I'm using FT2232)
- MicroSD to JTAG adapter board ([pinout](https://linux-sunxi.org/MicroSD_Breakout)) ([DIY adapter PCB project for DipTrace](https://github.com/nminaylov/F1C100s_info/blob/master/JTAG/sd_jtag.dip))
- OpenOCD

## Building
Run `make -C projects/<project_name>`

## Running
### With U-Boot
- Copy \<project_name\>.bin file to SD card
- Run it with the following commands:
```
fatload mmc 0:1 80000000 <project_name>.bin
go 80000000
```
### With simple_loader
- Compile simple_loader project
- Use mksunxi tool to fix eGON.BT0 header in simple_loader.bin
- Flash simple_loader.bin to SPI flash at offset 0, using U-Boot or FEL mode
- Compile your project with -DLOAD_HEADER option (enabled by default in all projects)
- Flash your code to SPI flash at offset 0x10000

### Via JTAG
This will work only if there is no bootable code in the SPI Flash chip.

Set your JTAG adapter config file path in the `OPENOCD_ADAPTER` option of makefile, run `make -C projects/<project_name> load`

## Debugging
You can debug your code using Cortex-Debug plugin

- Compile and write simple_loader to SPI flash, it will enable JTAG and perform DRAM initialization on start
- Set your JTAG adapter type in `.vscode/launch.json` config file
- Press F5 to start debug
