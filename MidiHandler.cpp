#include "MidiHandler.hpp"

MidiHandler::MidiHandler() {
    midiIn = std::make_unique<RtMidiIn>();
    if (midiIn->getPortCount() > 0) {
        midiIn->openPort(0); // Open the first available MIDI port
        midiIn->ignoreTypes(true, true, true); // Ignore sysex, timing, and active sensing messages
        midiIn->setCallback(&MidiHandler::midiCallback, this);
    }
}

MidiHandler::~MidiHandler() {
    if (midiIn && midiIn->isPortOpen()) {
        midiIn->closePort();
    }
}

void MidiHandler::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    MidiHandler* handler = static_cast<MidiHandler*>(userData);
    
    if (message->size() >= 3) {
        unsigned char status = message->at(0);
        unsigned char data1 = message->at(1);
        unsigned char data2 = message->at(2);
        
        // Check if it's a note on or note off message
        bool isNoteOn = (status >= 0x90 && status <= 0x9F);
        bool isNoteOff = (status >= 0x80 && status <= 0x8F) || (isNoteOn && data2 == 0);
        
        if (isNoteOn || isNoteOff) {
            // Extract channel (lower 4 bits of status)
            unsigned char channel = status & 0x0F;
            
            // Create Note object
            Note note(data1, 1.0f);
            
            // Create MidiMessage
            MidiMessage midiMsg(note, isNoteOn && data2 > 0);
            
            // Add to queue (thread-safe if needed)
            handler->midiMessages.push_back(midiMsg);
        }
        // Ignore all other message types
    }
}

MidiMessage MidiHandler::popMessage() {
    if (!midiMessages.empty()) {
        MidiMessage msg = midiMessages.front();
        midiMessages.erase(midiMessages.begin());
        return msg;
    }
    return MidiMessage(); // Return empty message if none available
}

bool MidiHandler::hasMessages() const {
    return !midiMessages.empty();
}