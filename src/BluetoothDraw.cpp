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
            if (c == 0x01) { // Start of heading
                state = READING_FILE_NAME;
                emptyBuffer();
                stream->println("Reading file name");
                return;
            }

            if (c == 0x04) { // End of transmission
                state = DONE;
                file->close();
                return;
            }

            if (c == 0x05 && state == READING_FILE) { // Enquiry
                if (strlen(buffer) == PACKET_SIZE) {
                    stream->write(0x06); // Acknowledge
                    file->print(buffer);
                } else {
                    stream->write(0x15); // Negative Acknowledge
                }
                emptyBuffer();
                return;
            }

            if (c == 0x06 && state == READING_FILE) {
                file->print(buffer);
                emptyBuffer();
            }

            switch (state)
            {
            case READING_FILE_NAME:
                if (c == '\n') {
                    stream->println(buffer);
                    SD.remove(buffer);
                    file = &SD.open(buffer, FILE_WRITE);
                    state = READING_FILE;
                    emptyBuffer();
                } else if (!fillBuffer(c)) {
                    state = FAILED;
                }
                break;

            case READING_FILE:
                if (!fillBuffer(c)) {
                    state = FAILED;
                }
                break;
            }
        }

        void emptyBuffer() {
            buffer[0] = '\0';
            bufferIndex = 0;
        }

        bool fillBuffer(char c) {
            if (bufferIndex < BLUETOOTH_DRAW_BUFFER_SIZE - 1) {
                buffer[bufferIndex] = c;
                buffer[++bufferIndex] = '\0';

                return true;
            }

            return false;
        }
    }
}