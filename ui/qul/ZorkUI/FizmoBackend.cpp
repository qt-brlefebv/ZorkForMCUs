/*
 * FizmoBackend.cpp
 *
 * Implementation of the Qt for MCUs backend for fizmo.
 */

#include "FizmoBackend.h"

#include <cstring>
#include <cstdlib>

#if defined(DESKTOP_STUB)
// Desktop stub implementations for testing UI without fizmo
static const char* s_demoText =
    "ZORK I: The Great Underground Empire\n"
    "Copyright (c) 1981, 1982, 1983 Infocom, Inc.\n"
    "All rights reserved.\n\n"
    "West of House\n"
    "You are standing in an open field west of a white house, "
    "with a boarded front door.\n"
    "There is a small mailbox here.\n\n";

static bool s_demoOutputSent = false;
static bool s_waitingInput = false;

extern "C" {
size_t fizmo_output_available(void) {
    if (!s_demoOutputSent) return strlen(s_demoText);
    return 0;
}

size_t fizmo_output_read(uint32_t *buffer, size_t max_chars) {
    if (s_demoOutputSent) return 0;
    s_demoOutputSent = true;
    size_t len = strlen(s_demoText);
    if (len > max_chars) len = max_chars;
    for (size_t i = 0; i < len; i++) {
        buffer[i] = static_cast<uint32_t>(s_demoText[i]);
    }
    s_waitingInput = true;
    return len;
}

bool fizmo_waiting_for_input(void) { return s_waitingInput; }
bool fizmo_waiting_for_char(void) { return false; }
bool fizmo_has_exited(void) { return false; }

bool fizmo_get_status_line(char *room, size_t room_size,
                           char *score_or_time, size_t score_size) {
    strncpy(room, "West of House", room_size - 1);
    room[room_size - 1] = '\0';
    strncpy(score_or_time, "Score: 0  Moves: 0", score_size - 1);
    score_or_time[score_size - 1] = '\0';
    return true;
}

void fizmo_submit_line(const char *line) {
    (void)line;
    s_waitingInput = true; // Stay in input mode for demo
}

void fizmo_submit_char(uint32_t ch) { (void)ch; }

void fizmo_bridge_init(const char *) {}
void fizmo_start_interpreter(void) {}
void fizmo_bridge_shutdown(void) {}
}

#define FIZMO_INPUT_BUFFER_SIZE 256

#elif defined(USE_FIZMO_BRIDGE)
// Desktop build with real fizmo interpreter via std::thread bridge
extern "C" {
#include "fizmo_bridge.h"
}

#define FIZMO_INPUT_BUFFER_SIZE 256

#else
// FreeRTOS build with real fizmo interpreter
extern "C" {
#include "fizmo_rtos_bridge.h"
}
#endif

// Global event queue instance
static FizmoEventQueue s_eventQueue;

// Poll interval in milliseconds
static const int POLL_INTERVAL_MS = 50;

// Temporary buffer for reading output
static const int READ_BUFFER_SIZE = 256;
static uint32_t s_readBuffer[READ_BUFFER_SIZE];

// Story file path - can be overridden via ZORK_STORY_PATH environment variable
#ifndef ZORK_STORY_PATH
#define ZORK_STORY_PATH "zork1.z3"
#endif

FizmoBackend::FizmoBackend()
    : outputVersion(0)
    , statusVersion(0)
    , commandVersion(0)
    , waitingForInput(false)
    , waitingForChar(false)
    , gameExited(false)
    , m_outputLength(0)
    , m_currentOutputStart(0)
    , m_commandLength(0)
{
    m_outputBuffer[0] = '\0';
    m_statusRoom[0] = '\0';
    m_statusScore[0] = '\0';
    m_commandBuffer[0] = '\0';

#if defined(USE_FIZMO_BRIDGE)
    // Initialize and start the fizmo interpreter
    const char *storyPath = ZORK_STORY_PATH;

    // Check environment variable override
    const char *envPath = getenv("ZORK_STORY_PATH");
    if (envPath != nullptr && envPath[0] != '\0') {
        storyPath = envPath;
    }

    if (fizmo_bridge_init(storyPath) == 0) {
        fizmo_start_interpreter();
    }
#endif

    // Set up polling timer
    m_pollTimer.setInterval(POLL_INTERVAL_MS);
    m_pollTimer.setSingleShot(false);
    m_pollTimer.onTimeout([this]() {
        pollFizmoOutput();
    });
    m_pollTimer.start();
}

const char* FizmoBackend::getOutputText() const
{
    return m_outputBuffer;
}

const char* FizmoBackend::getStatusRoom() const
{
    return m_statusRoom;
}

const char* FizmoBackend::getStatusScore() const
{
    return m_statusScore;
}

void FizmoBackend::submitLine(const Qul::Private::String &text)
{
    // Mark where the new output will start (after existing content)
    m_currentOutputStart = m_outputLength;

    // Get raw string data - QML TextInput provides UTF-8 or Latin1 format
    const char *utf8 = text.maybeUtf8();
    if (utf8 != nullptr) {
        // String is in UTF-8 format
        // Copy to null-terminated buffer since maybeUtf8() isn't guaranteed null-terminated
        int len = text.rawLength();
        if (len < FIZMO_INPUT_BUFFER_SIZE - 1) {
            char buffer[FIZMO_INPUT_BUFFER_SIZE];
            if (len > 0) {
                memcpy(buffer, utf8, len);
            }
            buffer[len] = '\0';

            // Echo the command to output
#if defined(USE_FIZMO_BRIDGE) || defined(DESKTOP_STUB)
            // Desktop build: fizmo already output ">", just add leading space
            char echoBuffer[FIZMO_INPUT_BUFFER_SIZE + 3];
            echoBuffer[0] = ' ';
            if (len > 0) {
                memcpy(echoBuffer + 1, buffer, len);
            }
            echoBuffer[len + 1] = '\n';
            echoBuffer[len + 2] = '\0';

            appendOutput(echoBuffer);
#else
            // Hardware (FreeRTOS): Add "> " prefix
            char echoBuffer[FIZMO_INPUT_BUFFER_SIZE + 4];
            int echoLen = 0;
            echoBuffer[echoLen++] = '>';
            echoBuffer[echoLen++] = ' ';
            if (len > 0) {
                memcpy(echoBuffer + echoLen, buffer, len);
                echoLen += len;
            }
            echoBuffer[echoLen++] = '\n';
            echoBuffer[echoLen] = '\0';

            appendOutput(echoBuffer);
#endif

            fizmo_submit_line(buffer);
        }
        return;
    }

    const char *latin1 = text.maybeLatin1();
    if (latin1 != nullptr) {
        // String is in Latin-1 format (ASCII compatible for typical input)
        int len = text.rawLength();
        if (len > 0 && len < FIZMO_INPUT_BUFFER_SIZE - 1) {
            char buffer[FIZMO_INPUT_BUFFER_SIZE];
            memcpy(buffer, latin1, len);
            buffer[len] = '\0';
            
            // Echo the command to output
            char echoBuffer[FIZMO_INPUT_BUFFER_SIZE + 3];
            echoBuffer[0] = ' ';
            memcpy(echoBuffer + 1, buffer, len);
            echoBuffer[len + 1] = '\n';
            echoBuffer[len + 2] = '\0';

            appendOutput(echoBuffer);            
            fizmo_submit_line(buffer);
        }
        return;
    }

    // For other formats (concatenation, formatted numbers, etc.), submit empty
    // This shouldn't happen for normal text input
    fizmo_submit_line("");
}

void FizmoBackend::submitChar(int ch)
{
    fizmo_submit_char(static_cast<uint32_t>(ch));
}

Qul::Private::String FizmoBackend::removeLastChar(const Qul::Private::String &text)
{
    int len = text.rawLength();
    if (len <= 0) {
        return Qul::Private::String();
    }

    // Get the raw data
    const char *utf8 = text.maybeUtf8();
    if (utf8 != nullptr) {
        // For UTF-8, we need to handle multi-byte characters
        // Walk backward to find the start of the last character
        int newLen = len - 1;
        while (newLen > 0 && (utf8[newLen] & 0xC0) == 0x80) {
            // This is a continuation byte, keep going back
            newLen--;
        }
        return Qul::Private::String(utf8, newLen);
    }

    const char *latin1 = text.maybeLatin1();
    if (latin1 != nullptr) {
        // Latin-1 is single-byte, simple case
        return Qul::Private::String(latin1, len - 1);
    }

    // Fallback - return empty string
    return Qul::Private::String();
}

void FizmoBackend::clearOutput()
{
    m_outputLength = 0;
    m_outputBuffer[0] = '\0';
    m_currentOutputStart = 0;
    outputVersion.setValue(outputVersion.value() + 1);
}

/*
 * Helper: Count the number of lines in a text buffer
 */
static int count_lines(const char* text, int length)
{
    int lines = 0;
    for (int i = 0; i < length; i++) {
        if (text[i] == '\n') {
            lines++;
        }
    }
    // Count the last line even if it doesn't end with \n
    if (length > 0 && text[length - 1] != '\n') {
        lines++;
    }
    return lines;
}

/*
 * Helper: Smart trim to maintain scrollback buffer
 * Keeps at least MIN_SCROLLBACK_LINES or the entire current output, whichever is larger
 */
static void trim_output_buffer(char* buffer, int* length, int* current_output_start)
{
#if defined(DISPLAY_RT1050)
    const int MIN_SCROLLBACK_LINES = 10;
#else
    const int MIN_SCROLLBACK_LINES = 20;  // Desktop/RT1170 can afford more history
#endif
    
    if (*length == 0) return;
    
    // Count lines in current output (from start marker to end)
    int current_output_length = *length - *current_output_start;
    int current_output_lines = count_lines(buffer + *current_output_start, current_output_length);
    
    // Determine how many total lines to keep
    int keep_lines = (current_output_lines > MIN_SCROLLBACK_LINES) 
                     ? current_output_lines 
                     : MIN_SCROLLBACK_LINES;
    
    // Count total lines
    int total_lines = count_lines(buffer, *length);
    
    // If we're within limits, nothing to do
    if (total_lines <= keep_lines) {
        return;
    }
    
    // Find the trim point: keep the last 'keep_lines' lines
    int lines_to_trim = total_lines - keep_lines;
    int trim_pos = 0;
    int lines_found = 0;
    
    for (int i = 0; i < *length && lines_found < lines_to_trim; i++) {
        if (buffer[i] == '\n') {
            lines_found++;
            if (lines_found == lines_to_trim) {
                trim_pos = i + 1;  // Trim up to and including this newline
                break;
            }
        }
    }
    
    // Shift the buffer contents
    if (trim_pos > 0 && trim_pos < *length) {
        int new_length = *length - trim_pos;
        memmove(buffer, buffer + trim_pos, new_length);
        buffer[new_length] = '\0';
        *length = new_length;
        
        // Adjust the current output start marker
        // Since we always keep the entire current output, recalculate from end
        int new_current_start = (*length >= current_output_length) 
                                ? (*length - current_output_length) 
                                : 0;
        *current_output_start = new_current_start;
    }
}

void FizmoBackend::postEvent(const FizmoEvent &event)
{
    s_eventQueue.postEvent(event);
}

void FizmoBackend::appendOutput(const char *text)
{
    if (text == nullptr || text[0] == '\0') {
        return;
    }

    int textLen = static_cast<int>(strlen(text));
    int available = MAX_OUTPUT_LENGTH - m_outputLength - 1;

    if (textLen > available) {
        // Buffer full - discard old content to make room
        // Try to discard on a newline boundary for cleaner display
        int discardTarget = MAX_OUTPUT_LENGTH / 2;  // Discard ~half the buffer
        int discardAt = discardTarget;
        
        // Look for a newline near the discard point
        for (int i = discardTarget; i < m_outputLength && i < discardTarget + 200; i++) {
            if (m_outputBuffer[i] == '\n') {
                discardAt = i + 1;  // Discard up to and including the newline
                break;
            }
        }
        
        int keepLen = m_outputLength - discardAt;
        if (keepLen > 0) {
            memmove(m_outputBuffer, m_outputBuffer + discardAt, keepLen);
            m_outputLength = keepLen;
        } else {
            m_outputLength = 0;
        }
        m_outputBuffer[m_outputLength] = '\0';
        available = MAX_OUTPUT_LENGTH - m_outputLength - 1;
    }

    // Append new text
    int toCopy = (textLen < available) ? textLen : available;
    memcpy(m_outputBuffer + m_outputLength, text, toCopy);
    m_outputLength += toCopy;
    m_outputBuffer[m_outputLength] = '\0';

    // Apply smart trimming to maintain scrollback buffer
    trim_output_buffer(m_outputBuffer, &m_outputLength, &m_currentOutputStart);

    // Increment version to trigger QML rebinding
    outputVersion.setValue(outputVersion.value() + 1);
}

void FizmoBackend::pollFizmoOutput()
{
    // Check for output from fizmo
    size_t available = fizmo_output_available();
    while (available > 0) {
        size_t toRead = (available < READ_BUFFER_SIZE) ? available : READ_BUFFER_SIZE;
        size_t read = fizmo_output_read(s_readBuffer, toRead);

        if (read > 0) {
            // Convert UTF-32 to UTF-8
            char utf8Buffer[READ_BUFFER_SIZE * 4 + 1];
            int utf8Len = 0;

            for (size_t i = 0; i < read && utf8Len < (int)sizeof(utf8Buffer) - 4; i++) {
                uint32_t ch = s_readBuffer[i];

                if (ch < 0x80) {
                    utf8Buffer[utf8Len++] = static_cast<char>(ch);
                } else if (ch < 0x800) {
                    utf8Buffer[utf8Len++] = static_cast<char>(0xC0 | (ch >> 6));
                    utf8Buffer[utf8Len++] = static_cast<char>(0x80 | (ch & 0x3F));
                } else if (ch < 0x10000) {
                    utf8Buffer[utf8Len++] = static_cast<char>(0xE0 | (ch >> 12));
                    utf8Buffer[utf8Len++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
                    utf8Buffer[utf8Len++] = static_cast<char>(0x80 | (ch & 0x3F));
                } else {
                    utf8Buffer[utf8Len++] = static_cast<char>(0xF0 | (ch >> 18));
                    utf8Buffer[utf8Len++] = static_cast<char>(0x80 | ((ch >> 12) & 0x3F));
                    utf8Buffer[utf8Len++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
                    utf8Buffer[utf8Len++] = static_cast<char>(0x80 | (ch & 0x3F));
                }
            }
            utf8Buffer[utf8Len] = '\0';

            // Append to output
            appendOutput(utf8Buffer);
        }

        available = fizmo_output_available();
    }

    // Update input waiting state
    bool waiting = fizmo_waiting_for_input();
    if (waiting != waitingForInput.value()) {
        waitingForInput.setValue(waiting);
    }

    bool waitingCh = fizmo_waiting_for_char();
    if (waitingCh != waitingForChar.value()) {
        waitingForChar.setValue(waitingCh);
    }

    // Update status line
    char room[64];
    char score[32];
    if (fizmo_get_status_line(room, sizeof(room), score, sizeof(score))) {
        bool changed = false;
        if (strcmp(room, m_statusRoom) != 0) {
            strncpy(m_statusRoom, room, sizeof(m_statusRoom) - 1);
            m_statusRoom[sizeof(m_statusRoom) - 1] = '\0';
            changed = true;
        }
        if (strcmp(score, m_statusScore) != 0) {
            strncpy(m_statusScore, score, sizeof(m_statusScore) - 1);
            m_statusScore[sizeof(m_statusScore) - 1] = '\0';
            changed = true;
        }
        if (changed) {
            statusVersion.setValue(statusVersion.value() + 1);
        }
    }

    // Check if game has exited
    if (fizmo_has_exited() && !gameExited.value()) {
        gameExited.setValue(true);
    }
}

/*
 * Event queue handler - processes events posted from fizmo task
 */
void FizmoEventQueue::onEvent(const FizmoEvent &event)
{
    FizmoBackend &backend = FizmoBackend::instance();

    switch (event.type) {
        case FizmoEventType::OutputText:
            backend.appendOutput(event.text);
            break;

        case FizmoEventType::InputRequested:
            backend.waitingForInput.setValue(true);
            break;

        case FizmoEventType::CharRequested:
            backend.waitingForChar.setValue(true);
            break;

        case FizmoEventType::StatusUpdate:
            strncpy(backend.m_statusRoom, event.statusRoom, sizeof(backend.m_statusRoom) - 1);
            backend.m_statusRoom[sizeof(backend.m_statusRoom) - 1] = '\0';
            strncpy(backend.m_statusScore, event.statusScore, sizeof(backend.m_statusScore) - 1);
            backend.m_statusScore[sizeof(backend.m_statusScore) - 1] = '\0';
            backend.statusVersion.setValue(backend.statusVersion.value() + 1);
            break;

        case FizmoEventType::GameExited:
            backend.gameExited.setValue(true);
            break;
    }
}

void FizmoBackend::appendCommandChar(const Qul::Private::String &key)
{
    // Extract the character from the string
    const char *utf8 = key.maybeUtf8();
    const char *latin1 = key.maybeLatin1();
    const char *str = utf8 ? utf8 : latin1;
    
    if (str && m_commandLength < (int)sizeof(m_commandBuffer) - 1) {
        // Append the character(s)
        int len = key.rawLength();
        int available = sizeof(m_commandBuffer) - m_commandLength - 1;
        if (len > available) len = available;
        
        memcpy(m_commandBuffer + m_commandLength, str, len);
        m_commandLength += len;
        m_commandBuffer[m_commandLength] = '\0';
        
        // Notify QML
        commandVersion.setValue(commandVersion.value() + 1);
    }
}

void FizmoBackend::commandBackspace()
{
    if (m_commandLength > 0) {
        // Simple backspace - remove last byte (assumes ASCII for now)
        m_commandLength--;
        m_commandBuffer[m_commandLength] = '\0';
        
        // Notify QML
        commandVersion.setValue(commandVersion.value() + 1);
    }
}

void FizmoBackend::submitCommand()
{
    // Mark where the new output will start (after existing content)
    m_currentOutputStart = m_outputLength;

    // Echo the command to output
    if (m_commandLength > 0) {
        char echoBuffer[260];
        echoBuffer[0] = ' ';
        memcpy(echoBuffer + 1, m_commandBuffer, m_commandLength);
        echoBuffer[m_commandLength + 1] = '\n';
        echoBuffer[m_commandLength + 2] = '\0';

        appendOutput(echoBuffer);
    }

    // Submit the accumulated command
    fizmo_submit_line(m_commandBuffer);

    // Clear the command buffer
    m_commandLength = 0;
    m_commandBuffer[0] = '\0';
    commandVersion.setValue(commandVersion.value() + 1);
}

const char* FizmoBackend::getCommandText() const
{
    return m_commandBuffer;
}

// Register the singleton
QUL_SINGLETON(FizmoBackend)
