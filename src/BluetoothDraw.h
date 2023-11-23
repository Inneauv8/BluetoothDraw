#ifndef BLUETOOTH_DRAW_H
#define BLUETOOTH_DRAW_H

#include <Arduino.h>
#include <SD.h>

#define BLUETOOTH_DRAW_BUFFER_SIZE 50

namespace BluetoothDraw {
    enum ReadingState {
        READING_FILE_NAME,
        READING_FILE,
        WAITING,
        DONE,
        FAILED
    };

    void initialize(Stream &_stream);
    ReadingState update();

    namespace {
        extern Stream *stream;
        extern File *file;

        extern int bufferIndex;
        extern char buffer[BLUETOOTH_DRAW_BUFFER_SIZE];
        extern ReadingState state;

        void processCharacter(char c);
    }
}

#endif // BLUETOOTH_DRAW_H