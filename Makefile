CC = cc
SRCS = $(wildcard *.c)

TARGET = demo

all:$(TARGET)
	@echo "build $(TARGET)"

$(TARGET):$(SRCS)
	$(CC)  -o  $@ $(SRCS)

clean:
	@echo "clean $(TARGET)"
	@rm -f $(TARGET)
