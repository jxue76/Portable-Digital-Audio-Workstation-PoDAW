#pragma once
#include "Note.hpp"

class MidiMessage
{
public:
    MidiMessage() = default;
    MidiMessage(const Note& note, bool on) : note(note), on(on) {}
    inline const Note& getNote() const { return note; }
    inline bool isOn() const { return on; }

private:
    Note note;
    bool on;
};