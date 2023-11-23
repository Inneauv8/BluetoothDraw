#include "BluetoothDraw.h"

namespace BluetoothDraw {
    void initialize(Stream &_stream) {
        stream = &_stream;
    }

    ReadingState update() {
        if (state == FAILED || state == DONE) {
            state = WAITING;
        }

        while(stream->available()) {
            processCharacter(stream->read());
        }

        return state;
    }

    namespace {
        Stream *stream;
        File *file;
        
        int bufferIndex = 0;
        char buffer[BLUETOOTH_DRAW_BUFFER_SIZE] = "\0";
        ReadingState state = WAITING;

        void processCharacter(char c) {
            if (c == 0x01) {
                state = READING_FILE_NAME;
                buffer[0] = '\0';
                bufferIndex = 0;
                stream->println("Reading file name");
                return;
            }

            if (c == 0x04) {
                state = DONE;
                file->close();
                return;
            }

            switch (state)
            {
            case READING_FILE_NAME:
                if (c == '\n') {
                    stream->println(buffer);
                    SD.remove(buffer);
                    file = &SD.open(buffer, FILE_WRITE);
                    state = READING_FILE;
                } else if (bufferIndex < BLUETOOTH_DRAW_BUFFER_SIZE - 1) {
                    buffer[bufferIndex] = c;
                    buffer[++bufferIndex] = '\0';
                } else {
                    state = FAILED;
                }
                break;

            case READING_FILE:
                file->write(c);
                break;
            }
        }
    }
}