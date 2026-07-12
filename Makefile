CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = distributeur
SOURCES = main.cpp Automat.cpp Slot.cpp SmallSlot.cpp WideSlot.cpp Motor.cpp CoinSlot.cpp Keypad.cpp DropCheck.cpp

SERVER_TARGET = serveur_distributeur
SERVER_SOURCES = server.cpp Automat.cpp Slot.cpp SmallSlot.cpp WideSlot.cpp Motor.cpp CoinSlot.cpp Keypad.cpp DropCheck.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

server: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_SOURCES) httplib.h
	$(CXX) $(CXXFLAGS) -pthread -o $(SERVER_TARGET) $(SERVER_SOURCES)

clean:
	rm -f $(TARGET) $(SERVER_TARGET)

.PHONY: all server clean