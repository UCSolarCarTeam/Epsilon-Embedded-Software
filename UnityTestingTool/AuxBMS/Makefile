# TARGET = EpsilonAuxBMSTests
AUX_BMS_DIR = ../../EpsilonAuxBMS/
SRC_DIR = $(AUX_BMS_DIR)Src/
INC_DIR = $(AUX_BMS_DIR)Inc/
MIDDLEWARES_DIR = $(AUX_BMS_DIR)Middlewares/
DRIVERS_PATH = $(AUX_BMS_DIR)Drivers/
UNITY_DIR = ../src/

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
-I $(INC_DIR) \
-I $(UNITY_DIR)/src \
-I $(MIDDLEWARES_DIR)/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
-I $(DRIVERS_PATH)/CMSIS/Include \
-I $(MIDDLEWARES_DIR)/Third_Party/FreeRTOS/Source/include \
-I $(MIDDLEWARES_DIR)/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F 

PATHT = ./
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

SRCT = $(wildcard $(PATHT)*.c)

COMPILE=gcc
CFLAGS=-I$(UNITY_DIR) $(PATHS)

AUX_BMS_SOURCES = $(SRC_DIR)Trip.c
TEST_SOURCES = main.c TripTest.c
UNITY_SOURCES = $(UNITY_DIR)unity.c  

# Get object file name of each source
# Each object file will be compiled as a target
AUX_BMS_OBJ = $(patsubst $(SRC_DIR)%.c, $(PATHO)%.o, $(AUX_BMS_SOURCES))
TEST_OBJ = $(patsubst %.c, $(PATHO)%.o, $(TEST_SOURCES))
UNITY_OBJ = $(patsubst $(UNITY_DIR)%.c, $(PATHO)%.o, $(UNITY_SOURCES))

default: mkdirs EpsilonAuxBMSTests

# Parse files within this folder
$(PATHO)%.o: %.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse the unity sources
$(PATHO)%.o: $(UNITY_DIR)/%.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

# Parse files within the AuxBMS source
$(PATHO)%.o: $(SRC_DIR)/%.c
	$(COMPILE) -c -o $@ $< $(CFLAGS)

EpsilonAuxBMSTests: $(TEST_OBJ) $(UNITY_OBJ) $(AUX_BMS_OBJ)
	$(COMPILE) -o $(PATHB)$@ $^ $(CFLAGS) 

# Make build directories
mkdirs:
	$(MKDIR) $(PATHB)
	$(MKDIR) $(PATHO)

clean:
	$(CLEANUP) $(PATHB)

.PRECIOUS: $(PATHB)Test%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATHD)%.d
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHR)%.txt
.PRECIOUS: $(PATHB)EpsilonAuxBMSTests
