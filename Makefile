CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = ChiBebinam
SOURCE = main.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) users.csv films.csv

.PHONY: clean run 