#pragma once
#include <vector>
#include <chrono>
#include "MidiMessage.hpp"

using Clock = std::chrono::steady_clock;

class TimedMidiMessage : public MidiMessage {
public:
    TimedMidiMessage() = default;
    TimedMidiMessage(const Note& note, bool on, std::chrono::microseconds timestamp)
        : MidiMessage(note, on), timestamp(timestamp) {}
    std::chrono::microseconds getTimestamp() const { return timestamp; }
private:    
    std::chrono::microseconds timestamp;
}

struct MidiRecording {
    std::vector<TimedMidiMessage> events;
    std::chrono::microseconds length{0};
};