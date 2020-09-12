TARGET = CheckCurrentTest
EMBEDDED_REPO_ROOT = ../../../../

UNITY_DIR = $(EMBEDDED_REPO_ROOT)/UnityTestingTool/
UNITY_SRC_DIR = $(UNITY_DIR)/src
CMOCK_DIR = $(UNITY_DIR)/CMock/src

AUX_BMS_DIR = $(EMBEDDED_REPO_ROOT)/EpsilonAuxBmsV2/
AUX_BMS_TESTS_DIR = $(AUX_BMS_DIR)/Tests
HELPER_DIR = $(AUX_BMS_TESTS_DIR)
AUX_BMS_SRC_DIR = $(AUX_BMS_DIR)/Core/Src/
AUX_BMS_SRC_TASKS_DIR = $(AUX_BMS_DIR_DIR)/Tasks

AUX_BMS_INC_DIR = $(AUX_BMS_DIR)/Core/Inc/
AUX_BMS_INC_DIR_TASKS = $(AUX_BMS_INC_DIR)/Tasks
MIDDLEWARES_DIR = $(AUX_BMS_DIR)/Middlewares/
DRIVERS_PATH = $(AUX_BMS_DIR)/Drivers/
MOCK_DIR = $(AUX_BMS_TESTS_DIR)/mocks/

ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -rf
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -rf
	MKDIR = mkdir -p
	TARGET_EXTENSION=out
endif

.PHONY: clean
.PHONY: test
.PHONY: all

PATHS = -I . \
-I $(MOCK_DIR) \
-I $(AUX_BMS_TESTS_DIR) \
-I $(AUX_BMS_INC_DIR) \
-I $(AUX_BMS_INC_DIR_TASKS)/ \
-I $(AUX_BMS_INC_DIR)/Types/ \
-I $(AUX_BMS_INC_DIR)/Tasks/ContactorFunctions \
-I $(UNITY_SRC_DIR) \
-I $(CMOCK_DIR) \
-I $(MIDDLEWARES_DIR)/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
-I $(MIDDLEWARES_DIR)/Third_Party/FreeRTOS/Source/include \
-I $(MIDDLEWARES_DIR)/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-I $(DRIVERS_PATH)/STM32F4xx_HAL_Driver/Inc/ \
-I $(DRIVERS_PATH)/CMSIS/Device/ST/STM32F4xx/Include/ \
-I $(DRIVERS_PATH)/CMSIS/Include

PATHT = ./
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

COMPILE=gcc
CFLAGS=$(PATHS) -DSTM32F407xx -D__NO_RETURN=""

AUX_BMS_SOURCES = \
				$(AUX_BMS_SRC_DIR)/Tasks/ContactorFunctions/CheckCurrent.c \

TEST_SOURCES = \
			main.c \
			CheckCurrentTest.c \

HELPER_SOURCES = \

UNITY_SOURCES = $(UNITY_SRC_DIR)unity.c
CMOCK_SOURCES = $(CMOCK_DIR)cmock.c

MOCK_SOURCES = \
	$(MOCK_DIR)Mockstm32f4xx_hal_adc.c \
	# $(MOCK_DIR)Mockcmsis_os2.c \
	# $(MOCK_DIR)Mockstm32f4xx_hal_gpio.c \
	# $(MOCK_DIR)MockCheckContactorError.c \
	#$(MOCK_DIR)Mockstm32f4xx_hal_can.c  \


# Get object file name of each source
# Each object file will be compiled as a target
TEST_OBJ = $(patsubst %.c, $(PATHO)%.o, $(TEST_SOURCES))
AUX_BMS_OBJ = $(patsubst $(AUX_BMS_SRC_DIR)%.c, $(PATHO)%.o, $(AUX_BMS_SOURCES))
UNITY_OBJ = $(patsubst $(UNITY_SRC_DIR)%.c, $(PATHO)%.o, $(UNITY_SOURCES))
CMOCK_OBJ = $(patsubst $(CMOCK_DIR)%.c, $(PATHO)%.o, $(CMOCK_SOURCES))
MOCK_OBJ = $(patsubst $(MOCK_DIR)%.c, $(PATHO)%.o, $(MOCK_SOURCES))
HELPER_OBJ = $(patsubst $(HELPER_DIR)%.c, $(PATHO)%.o, $(HELPER_SOURCES))

default: mkdirs $(TARGET)

# Parse files within this folder
$(PATHO)%.o: %.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse the unity sources
$(PATHO)%.o: $(UNITY_SRC_DIR)/%.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse files within the AuxBMS source
$(PATHO)%.o: $(AUX_BMS_SRC_DIR)/%.c
	echo "building $@"
	$(MKDIR) $(dir $@)
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse files within the cmock source
$(PATHO)%.o: $(CMOCK_DIR)/%.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse files within the mock source
$(PATHO)%.o: $(MOCK_DIR)/%.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse files within the helpers source
$(PATHO)%.o: $(HELPER_DIR)/%.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

$(TARGET): $(TEST_OBJ) $(UNITY_OBJ) $(AUX_BMS_OBJ) $(MOCK_OBJ) $(CMOCK_OBJ) $(HELPER_OBJ)
	$(COMPILE) -o $(PATHB)$@ $^ $(CFLAGS)

# Make build directories
mkdirs:
	$(MKDIR) $(PATHB)
		echo "$(AUX_BMS_OBJ)"
	$(MKDIR) $(PATHO)

clean:
	$(CLEANUP) $(PATHB)

test:
	$(PATHB)/$(TARGET)

.PRECIOUS: $(PATHB)Test%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATHD)%.d
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHR)%.txt
.PRECIOUS: $(PATHB)$(TARGET)
