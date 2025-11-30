TARGET = freertos_sim
OBJ_DIR = lib

CC = gcc

VPATH = src:FreeRTOS/source:FreeRTOS/portable/ThirdParty/GCC/Posix

CFLAGS = -g -Wall \
         -I./src
#         -I./FreeRTOS/include \
#         -I./FreeRTOS/portable/ThirdParty/GCC/Posix

APP_SOURCES = main.c \
              scheduler.c \
              tasks.c \

#RTOS_SOURCES = $(notdir $(wildcard FreeRTOS/source/*.c)) \
#               $(notdir $(wildcard FreeRTOS/portable/ThirdParty/GCC/Posix/*.c))

C_SOURCES = $(APP_SOURCES)
# $(RTOS_SOURCES)

OBJS = $(addprefix $(OBJ_DIR)/, $(C_SOURCES:.c=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -lpthread

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
