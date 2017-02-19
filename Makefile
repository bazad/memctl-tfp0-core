# Makefile

ARCH     ?= arm64
SDK      ?= iphoneos
MEMCTL_INC_DIR ?= .

CLANG   := $(shell xcrun --sdk $(SDK) --find clang)
AR      := $(shell xcrun --sdk $(SDK) --find ar)
ifeq ($(CLANG),)
$(error Could not find clang for SDK $(SDK))
endif
SYSROOT := $(shell xcrun --sdk $(SDK) --show-sdk-path)
CC      := $(CLANG) -isysroot $(SYSROOT) -arch $(ARCH)

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = .

ERRFLAGS = -Wall -Wpedantic -Wno-gnu -Werror
CFLAGS   = -O3 -I$(SRC_DIR) -I"$(MEMCTL_INC_DIR)" $(ERRFLAGS)
ARFLAGS  = r

CORE_LIB ?= $(LIB_DIR)/libmemctl_core.a

TFP0_CORE_SRCS = core.c

TFP0_CORE_HDRS =

TFP0_CORE_OBJS := $(TFP0_CORE_SRCS:%.c=$(OBJ_DIR)/%.o)

vpath % $(SRC_DIR)

all: $(CORE_LIB)

$(OBJ_DIR)/%.o: %.c $(TFP0_CORE_HDRS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(CORE_LIB): $(TFP0_CORE_OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf -- $(OBJ_DIR) $(CORE_LIB)
