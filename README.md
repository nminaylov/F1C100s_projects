# Bare metal libraries and projects for Allwinner F1C100s/F1C200s SOC

## Libraries and tools

- \_arm926_ - Various ARMv5-related headers
- \_drivers_ - Low-level peripheral drivers. Currently work-in-progress: 
    - f1c100s_clock - Clock controller (CCU)
    - f1c100s_intc - Interrupt controller
    - f1c100s_gpio - GPIO, external interrupts
    - f1c100s_timer - Timers, watchdog
    - f1c100s_pwm - PWM outputs
    - f1c100s_uart - 16550-compatible UART
    - f1c100s_de - Display Engine (TCON, DEBE, DEFE)
    - f1c100s_tve - Undocumented TV(CVBS) encoder

- \_lib_ - Miscellaneous non hardware-related libraries
- \_tools_/mksunxi - A tool to add checksum to eGON.BT0 header. Taken from xboot project

## Projects

All projects are targeted for Lichee Nano devboard with 800x480 LCD. 

- hello_led - Yet another LED blinking project
- simple_loader - Very simple second-stage loader for booting bare metal apps from SPI flash
- lcd_test - Example of display driver usage with simple graphics library
- LVGL_demo - Demonstration project of LVGL library
- tv_out_test - Test project for TVE driver

More projects will be added later.

## Required tools (Windows)
- [Eclipse Embedded CDT](https://projects.eclipse.org/projects/iot.embed-cdt)
- [xPack Package Manager](https://xpack.github.io/install/)
- [Windows Build Tools](https://xpack.github.io/windows-build-tools/install/) (Install with xPack)
- [arm-none-eabi-gcc](https://xpack.github.io/arm-none-eabi-gcc/install/) (Install with xPack)

For debugging:
- Any OpenOCD-compatible JTAG adapter (I'm using FT2232)
- MicroSD to JTAG adapter board ([pinout](https://linux-sunxi.org/MicroSD_Breakout)) ([DIY adapter PCB project for DipTrace](https://github.com/nminaylov/F1C100s_info/blob/master/JTAG/sd_jtag.dip))
- [OpenOCD for Windows](https://gnutoolchains.com/arm-eabi/openocd/)
- [OpenOCD configuration file](https://github.com/nminaylov/F1C100s_info/blob/master/JTAG/allwinner_f1c100s.cfg)
- Simple DRAM initialization program in SPI flash (for example, simple_loader project). Also, you can use U-Boot SPL with disabled SD card support (not tested yet)

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

## Debugging
//todo