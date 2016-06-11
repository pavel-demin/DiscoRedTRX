TARGET = DiscoRedTRX

OBJECTS = \
  stm32f7xx_hal.o \
  stm32f7xx_hal_cortex.o \
  stm32f7xx_hal_dma.o \
  stm32f7xx_hal_dma2d.o \
  stm32f7xx_hal_gpio.o \
  stm32f7xx_hal_i2c.o \
  stm32f7xx_hal_i2c_ex.o \
  stm32f7xx_hal_ltdc.o \
  stm32f7xx_hal_pwr.o \
  stm32f7xx_hal_pwr_ex.o \
  stm32f7xx_hal_qspi.o \
  stm32f7xx_hal_rcc.o \
  stm32f7xx_hal_rcc_ex.o \
  stm32f7xx_hal_sdram.o \
  stm32f7xx_hal_tim.o \
  stm32f7xx_hal_tim_ex.o \
  stm32f7xx_hal_uart.o \
  stm32f7xx_ll_fmc.o \
  ub_sgui_button.o \
  ub_sgui_checkbox.o \
  ub_sgui_dropdown.o \
  ub_sgui_floatedit.o \
  ub_sgui_gauge.o \
  ub_sgui_graph.o \
  ub_sgui_intedit.o \
  ub_sgui_label.o \
  ub_sgui_led.o \
  ub_sgui_listbox.o \
  ub_sgui_panel.o \
  ub_sgui_picture.o \
  ub_sgui_radiobtn.o \
  ub_sgui_screen.o \
  ub_sgui_selectbtn.o \
  ub_sgui_slider.o \
  ub_sgui_stringlist.o \
  ub_sgui_text.o \
  ub_sgui_window.o \
  ub_font_arial_7x10.o \
  ub_font_arial_8x13.o \
  ub_font_arial_10x15.o \
  ub_font_arial_11x18.o \
  ub_font_arial_13x19.o \
  ub_font_arial_16x25.o \
  stm32_ub_font.o \
  stm32_ub_graphic2d.o \
  stm32_ub_i2c3.o \
  stm32_ub_lcd_480x272.o \
  stm32_ub_qflash.o \
  stm32_ub_sdram.o \
  stm32_ub_sgui.o \
  stm32_ub_system.o \
  stm32_ub_tim2.o \
  stm32_ub_touch_480x272.o \
  stm32_ub_uart.o \
  main.o \
  stm32f7xx_it.o \
  system_stm32f7xx.o \
  startup_stm32f746xx.o \
  syscalls.o

SOURCES = \
  Drivers/CMSIS/Device/ST/STM32F7xx/Source \
  Drivers/STM32F7xx_HAL_Driver/Src \
  Middlewares/UB/F746/Src \
  Middlewares/UB/SGUI/Src \
  Middlewares/UB/Fonts/Src \
  Src

vpath %.c $(SOURCES)

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = \
  -Wall -O0 -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard \
  -fdata-sections -ffunction-sections -ffast-math \
  -DUSE_HAL_DRIVER -DUSE_USB_FS -DUSE_IOEXPANDER \
  -DSTM32F746xx -DUSE_STM32746G_DISCO \
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include \
  -IDrivers/CMSIS/Include \
  -IDrivers/STM32F7xx_HAL_Driver/Inc \
  -IMiddlewares/UB/F746/Inc \
  -IMiddlewares/UB/SGUI/Inc \
  -IInc

LDFLAGS = \
  -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard \
  -fdata-sections -ffunction-sections -ffast-math \
  -specs=nano.specs -Wl,-T,STM32F746NGHx_FLASH.ld -Wl,--gc-sections \
  -lc -lm -lnosys

all: $(TARGET).bin

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary -S $< $@

clean:
	rm -rf $(OBJECTS) $(TARGET).elf $(TARGET).bin
