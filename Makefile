
export RELEASE_DIR ?= $(PWD)

export ARCH ?= arm
export CHIP ?= xilin

export CROSS_COMPILE ?= arm-linux-gnueabihf-

export CC := $(CROSS_COMPILE)gcc
export CXX := $(CROSS_COMPILE)g++
export AR := $(CROSS_COMPILE)ar

# 设置模块名称.
APP_NAME := i2crw_16b

LIBS := 
COMM_OBJ := 

CFLAGS := 

# 预定义宏
#CFLAGS += -DCHIP=$(CHIP) -DDEBUG

# 编译参数
# -ffunction-sections -fdata-sections选项让所有函数都做为单独section
# 配合下面的-Wl,--gc-sections可以将不用的函数优化掉
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wl,-gc-sections

CFLAGS += -Wall -g

CFLAGS += -Os -pipe -mcpu=cortex-a9

# 下面这个-s选项让GCC 以strip方式链接，可以缩小生成的文件体积
LDFLAGS := -s
LDFLAGS += -lpthread -lm -ldl

# 设置 library 目录,可以在后面添加多个.
LIBS += 

# 设置 include 目录,可以在后面添加多个.
INC += -I.

# 设置 source 目录.
DIR_SOURCE += .

# 添加C文件
SEARCH_SRC := $(foreach PATH, $(DIR_SOURCE), $(shell find $(PATH) -type f -name '*.c' -print))
SEARCH_OBJ := $(SEARCH_SRC:%.c=%.o)
OBJ += $(SEARCH_OBJ)

# 增加 source 目录.每次只能增加一个目录
#DIR_SOURCE += .
# 设置编译和连接的C源码文件.
#SRC := $(shell find $(DIR_SOURCE) -type f -name '*.c' -print)
#OBJ += $(SRC:%.c=%.o)

all: $(APP_NAME)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(APP_NAME): $(OBJ) $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) -o $@ $^

clean:
	@rm -f $(OBJ)
	@rm -f $(APP_NAME)
