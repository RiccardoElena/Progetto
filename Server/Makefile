CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LIBS = -lcurl -ljson-c -lpthread
TARGET = robot_dialog_server
SOURCES = main.c network.c protocol.c gemini_ai.c thread_pool.c utils.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

debug: CFLAGS += -DDEBUG -fsanitize=address
debug: $(TARGET)

release: CFLAGS = -Wall -O2 -DNDEBUG
release: $(TARGET)

clean:
	rm -f $(TARGET)

install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential libcurl4-openssl-dev libjson-c-dev

run: $(TARGET)
	./$(TARGET) -k $(GEMINI_API_KEY) -p 8080

.PHONY: clean debug release install-deps run
