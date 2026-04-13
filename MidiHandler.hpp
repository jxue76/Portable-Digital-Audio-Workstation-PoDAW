#pragma once
#include "RtMidi.h"
#include <memory>
#include <vector>
#include "MidiMessage.hpp"

class MidiHandler {
public:
    MidiHandler();
    ~MidiHandler();

    MidiMessage popMessage(); // Pops the next MIDI message from the queue, returns an empty message if none are available
    bool hasMessages() const; // Checks if there are MIDI messages in the queue
private:
    static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
    std::unique_ptr<RtMidiIn> midiIn;
    std::vector<MidiMessage> midiMessages;
};