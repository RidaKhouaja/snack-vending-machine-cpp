CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = distributeur
SOURCES = main.cpp Automat.cpp Slot.cpp SmallSlot.cpp WideSlot.cpp Motor.cpp CoinSlot.cpp Keypad.cpp DropCheck.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: all clean
