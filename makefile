#****************************************#
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = iAudio
CFLAGS = -Wall -O2 -std=c99  -D_XOPEN_SOURCE -D_GNU_SOURCE

CC=/usr/local/arm/tool-chain/fsl-linaro-toolchain/bin/arm-linux-gcc
#CC= arm-linux-androideabi-gcc
#CC=/opt/tisdk-am335x-evm6/linux-devkit/sysroots/i686-arago-linux/usr/bin/arm-linux-gnueabihf-gcc
LIBS = -L . -lpthread -lugw -lugw_br -lm -Wl,-dynamic-linker=/system/lib/ld-linux.so.3

#LIBS = -L .  -lpthread -lugw -lugw_br -lm 
$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(LIBS) $(CFLAGS) -o $(TARGET)
	date >> md5.txt
	echo "$(MACRO)" >> md5.txt
	md5sum $(TARGET) >> md5.txt

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) $(MACRO) -c $< -o $@

clean:
	rm -rf $(OBJS) $(CPP_OBJS) $(TARGET)






