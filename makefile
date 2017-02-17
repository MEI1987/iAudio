#****************************************#
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = iAudio
CFLAGS = -Wall -O2 -std=c99  -D_XOPEN_SOURCE -D_GNU_SOURCE

CC=/usr/local/arm/tool-chain/fsl-linaro-toolchain/bin/arm-linux-gcc
LIBS = -L . -lpthread -lugw -lugw_br -lm -Wl,-dynamic-linker=/system/lib/ld-linux.so.3

#LIBS = -L .  -lpthread -lugw -lugw_br -lm 
$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(LIBS) $(CFLAGS) -o $(TARGET)
	date >> md5.txt
	md5sum $(TARGET) >> md5.txt

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)






