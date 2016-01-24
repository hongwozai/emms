TARGET  = emms
NETCARD = enp0s3
PASS    = "123"

# /ora/app/lm/product/12.1.0/client_1
ORACLE_INCLUDE = ${ORACLE_HOME}/rdbms/public
ORACLE_LIB     = ${ORACLE_HOME}/lib

# 当正式版本时CFLAGS改为-DNDEBUG -O3
CC     = gcc
CFLAGS = -std=gnu89
CFLAGS+= -I$(ORACLE_INCLUDE)
LDFLAGS= -L$(ORACLE_LIB)
LIBS   = -lz -lm -lpthread -lnids -lpcap -lclntsh
# no-omit-frame-pointer 是优化时禁止使用esp等堆栈寄存器作为普通寄存器的
OPT   += -std=gnu89 -g -Wall -O1 -rdynamic
OPT   += -fno-omit-frame-pointer -DHAVE_DB -UNDEBUG -UKILL_WITH_TPOOL

MKDIR  = mkdir -p
CP     = cp
RM     = rm -rf
AR     = ar r

BIN    = bin
TARGET:= $(BIN)/$(TARGET)
SRC    = src
INC    = inc
UTILS  = utils
CONFIG = config
TOOLS  = tools

SRC_DEPS    = $(SRC)/deps
SRC_OBJS    = $(SRC)/objs
UTILS_DEPS  = $(UTILS)/deps
UTILS_OBJS  = $(UTILS)/objs

SRC_FILES   = $(wildcard $(SRC)/*.c)
SRC_FILES  := $(notdir $(SRC_FILES))
UTILS_FILES = $(wildcard $(UTILS)/*.c)
UTILS_FILES:= $(notdir $(UTILS_FILES))

SRC_DEPS_FILES    = $(patsubst %.c,%.deps,$(SRC_FILES))
SRC_DEPS_FILES   := $(addprefix $(SRC_DEPS)/,$(SRC_DEPS_FILES))
UTILS_DEPS_FILES  = $(patsubst %.c,%.deps,$(UTILS_FILES))
UTILS_DEPS_FILES := $(addprefix $(UTILS_DEPS)/,$(UTILS_DEPS_FILES))

SRC_OBJS_FILES    = $(patsubst %.c,%.o,$(SRC_FILES))
SRC_OBJS_FILES   := $(addprefix $(SRC_OBJS)/,$(SRC_OBJS_FILES))
UTILS_OBJS_FILES  = $(patsubst %.c,%.o,$(UTILS_FILES))
UTILS_OBJS_FILES := $(addprefix $(UTILS_OBJS)/,$(UTILS_OBJS_FILES))

TEST = test
TEST_LIB  = libtest.a
TEST_LIB := $(TEST)/$(TEST_LIB)
TEST_BIN  = emms-test
TEST_BIN := $(TEST)/$(TEST_BIN)

.PHONY: all clean test autotest set run clean

# 在虚拟机运行之前需要先运行set目标
all: $(TARGET)
	$(CP) $(CONFIG)/emms.ini $(BIN)/emms.ini
	$(CP) $(CONFIG)/spam.data $(BIN)/spam.data

clean:
	$(RM) $(SRC_OBJS)/* $(SRC_DEPS)/* $(UTILS_DEPS)/* $(UTILS_OBJS)/*
	$(RM) $(TEST_BIN) $(TEST_LIB) $(BIN)/*

set:
	echo $(PASS) | sudo -S ethtool -K $(NETCARD) tso off
	echo $(PASS) | sudo -S ethtool -K $(NETCARD) gso off
	echo $(PASS) | sudo -S ethtool -K $(NETCARD) gro off

run: $(TARGET) $(BIN)/emms.ini
	$(CP) $(CONFIG)/emms.ini $(BIN)/emms.ini
	sudo ./$<

client:
	./$(TOOLS)/client.py

test: $(TEST_BIN)

autotest: $(TEST_BIN)
	echo $(PASS) | sudo valgrind ./$<

# 这个必须在假目标后面
ifneq ("$(MAKECMDGOALS)", "client")
ifneq ("$(MAKECMDGOALS)", "clean")
include $(SRC_DEPS_FILES)
include $(UTILS_DEPS_FILES)
endif
endif

# 以下两个是单元测试
$(TEST_BIN): $(wildcard $(TEST)/*.c) $(TEST_LIB)
	$(CC) -o $@ $^ $(OPT) -iquote . $(LIBS) $(LDFLAGS)

$(TEST_LIB): $(filter-out $(SRC_OBJS)/main.o,$(SRC_OBJS_FILES)) $(UTILS_OBJS_FILES)
	$(AR) $@ $^

# 以下是编译过程
$(TARGET): $(SRC_OBJS_FILES) $(filter-out $(UTILS_OBJS)/unitest.o,$(UTILS_OBJS_FILES))
	$(CC) -o $@ $^ $(OPT) $(LIBS) $(LDFLAGS)

$(SRC_OBJS)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $<  $(OPT) -iquote . $(CFLAGS)

$(UTILS_OBJS)/%.o: $(UTILS)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(OPT)

$(SRC_DEPS)/%.deps: $(SRC)/%.c
	@set -e ;\
	$(CC) -MM $< -iquote . > $@.tmp;\
	sed 's,\(.*\):,$(SRC_OBJS)/\1:,g' < $@.tmp > $@;\
	$(RM) $@.tmp;

$(UTILS_DEPS)/%.deps: $(UTILS)/%.c
	@set -e ;\
	$(CC) -MM $< > $@.tmp;\
	sed 's,\(.*\):,$(UTILS_OBJS)/\1:,g' < $@.tmp > $@;\
	$(RM) $@.tmp;
