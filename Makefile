TAR=./bin/main
SRC=$(wildcard ./src/*.c)
OBJ=$(SRC:%.c=%.o)
CC=arm-linux-gcc
#CC=gcc
override CCONFIG+=-I./inc -L./lib -lfont -lMainfunction -lhead4animation -lpthread -lm

HOST=root@192.168.61.237#开发板的ip地址
SAVE_PATH=/mmount6G

$(TAR):$(SRC)					#如果需要生成.o文件，则需要将$(SRC) 改成$(OBJ)
	$(CC) $(^) $(CCONFIG) -o $(@) 
	scp $(TAR) $(HOST):$(SAVE_PATH)

%.o:%.c
	$(CC) $(^) -o $(@) $(CCONFIG) -c

scp:$(TAR)
	$(@) $(TAR) $(HOST):$(SAVE_PATH)

clean:
	$(RM) $(TAR) $(OBJ)

.PHONY:clean
