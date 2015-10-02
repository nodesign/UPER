# Makefile - GNU Makefile for TUXCORE

# General flags
#
OBJ_DIR = ./obj

RM := rm -rf

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

TARGET = UPER-Release
CPU = cortex-m0

# C specific flags
CDEFS = -D__USE_CMSIS -DNDEBUG -D__USE_ROMDIVIDE -D__NEWLIB__ -U__REDLIB__
CFLAGS = -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=$(CPU) -mthumb -MMD -MP 

# ASM specific flags
SDEFS = -D__NEWLIB__ -DNDEBUG -D__USE_ROMDIVIDE 
SFLAGS = -c -x assembler-with-cpp -mcpu=$(CPU) -mthumb

# Linker flags
LDFLAGS = -nostdlib -Xlinker -Map="$(TARGET).map" -Xlinker --gc-sections -Xlinker --allow-multiple-definition -mcpu=cortex-m0 -mthumb

# Includes
INCLUDE_DIRS = -I"./UPER/inc" -I"./UPER/inc/Modules" -I"./UPER/inc/USB_h" -I"./UPER/inc/Driver" -I"./UPER/inc/System" \
	       -I"./MemoryManager/inc" -I"./SFP/inc/" -I"./SFP/inc/SFP" -I"./UserFunctions"

CSOURCE = $(wildcard *.c)
HEADERS = $(wildcard *.h)

# Objects
OBJS = \
$(OBJ_DIR)/cdc_desc.o \
$(OBJ_DIR)/cr_startup_lpc11u.o \
$(OBJ_DIR)/main.o \
$(OBJ_DIR)/time.o \
$(OBJ_DIR)/CDC.o \
$(OBJ_DIR)/system_LPC11Uxx.o \
$(OBJ_DIR)/DHTxx.o \
$(OBJ_DIR)/HC-SR04.o \
$(OBJ_DIR)/LPC_INTERRUPT.o \
$(OBJ_DIR)/LPC_PORT.o \
$(OBJ_DIR)/LPC_ADC.o \
$(OBJ_DIR)/LPC_GPIO.o \
$(OBJ_DIR)/LPC_I2C.o \
$(OBJ_DIR)/LPC_PWM.o \
$(OBJ_DIR)/LPC_SPI.o \
$(OBJ_DIR)/LPC_UART.o \
$(OBJ_DIR)/core_cm0.o \
$(OBJ_DIR)/aeabi_romdiv_patch.o \
$(OBJ_DIR)/SFPFunction.o \
$(OBJ_DIR)/SFPMisc.o \
$(OBJ_DIR)/SFPServer.o \
$(OBJ_DIR)/MemoryManager.o \
$(OBJ_DIR)/UserFunctions.o

.PHONY: all
# All Target
all: pre-build $(TARGET).axf post-build
$(OBJ_DIR)/cdc_desc.o: ./UPER/src/cdc_desc.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/cr_startup_lpc11u.o: ./UPER/src/cr_startup_lpc11u.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/main.o: ./UPER/src/main.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/time.o: ./UPER/src/time.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/CDC.o: ./UPER/src/CDC/CDC.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/system_LPC11Uxx.o: ./UPER/src/Driver/system_LPC11Uxx.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/DHTxx.o: ./UPER/src/Modules/Devices/DHTxx.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/HC-SR04.o: ./UPER/src/Modules/Devices/HC-SR04.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_INTERRUPT.o: ./UPER/src/Modules/LPC_INTERRUPT.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_PORT.o: ./UPER/src/Modules/LPC_PORT.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_ADC.o: ./UPER/src/Modules/LPC_ADC.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_GPIO.o: ./UPER/src/Modules/LPC_GPIO.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_I2C.o: ./UPER/src/Modules/LPC_I2C.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_PWM.o: ./UPER/src/Modules/LPC_PWM.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_SPI.o: ./UPER/src/Modules/LPC_SPI.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/LPC_UART.o: ./UPER/src/Modules/LPC_UART.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/core_cm0.o: ./UPER/src/System/core_cm0.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/aeabi_romdiv_patch.o: ./UPER/src/aeabi_romdiv_patch.s
	$(CC) $(INCLUDE_DIRS) $(SFLAGS) -o "$@" "$<"
$(OBJ_DIR)/SFPFunction.o: ./SFP/src/SFP/SFPFunction.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/SFPMisc.o: ./SFP/src/SFP/SFPMisc.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/SFPServer.o: ./SFP/src/SFP/SFPServer.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/MemoryManager.o: ./MemoryManager/src/MemoryManager/MemoryManager.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
$(OBJ_DIR)/UserFunctions.o: ./UserFunctions/UserFunctions.c
	$(CC) $(INCLUDE_DIRS) $(CDEFS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

# Tool invocations
$(TARGET).axf: $(OBJS) $(USER_OBJS)
	$(CC) $(LDFLAGS) -T "./UPER/$(TARGET).ld" -o "$(TARGET).axf" $(OBJS) $(USER_OBJS) $(LIBS) $(LIBS_PATH)

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) $(TARGET).axf $(TARGET).bin $(TARGET).lss $(TARGET).map $(OBJ_DIR)
	-@echo ' '

pre-build:
	-@mkdir -p $(OBJ_DIR)

post-build:
	-@$(OBJCOPY) -O binary $(TARGET).axf $(TARGET).bin
	-@echo ' '
	-@python ./utils/checksum.py $(TARGET).bin
	-@$(OBJDUMP) -S $(TARGET).axf > $(TARGET).lss;
	-@echo ' '
	-$(SIZE) $(TARGET).axf

.PHONY: all clean dependents
.SECONDARY: post-build
