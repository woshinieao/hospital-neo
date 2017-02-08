CC =/home/samba/githubcode/h3-neo/buildroot/output/external-toolchain/bin/arm-linux-gnueabi-gcc 

TARGET	=  $(patsubst Matrix-%.c,matrix-%, $(wildcard *.c))
SRC 	=  $(wildcard *.c)
SRC 	+= $(wildcard *.h)

$(TARGET):$(SRC)
	$(CC) -o $@ -L ../../lib/ -I ../../lib/includes/ $^ -lfahw -lm -Wall

.PHONY: clean 
clean:
	rm -f $(TARGET)
