#include "Drums.hpp"
#include "Drummer.h"

Drums::Drums()
    : Instrument("Drums", []() { return std::make_unique<stk::Drummer>(); }, 36, 81, 1.0f) {
}
Drums::Drums(std::string name)
    : Instrument(std::move(name), []() { return std::make_unique<stk::Drummer>(); }, 36, 81, 1.0f) {
}
void Drums::noteOn(const Note& note) {
    float offset = 1.5f; // Adjust the MIDI note number to better match the drum sounds
    Instrument::noteOn(Note(note.getMidiNote() + offset, note.getAmplitude()));
}
void Drums::noteOff(const Note& note) {
    float offset = 1.5f; // Adjust the MIDI note number to better match the drum sounds
    Instrument::noteOff(Note(note.getMidiNote() + offset, note.getAmplitude()));
}
stk::StkFloat Drums::midiToFrequency(int midiNote) const {
    midiNote = mapBetween(midiNote, midiMin, midiMax);
    return static_cast<stk::StkFloat>(midiNote); // For drums, we can just return the MIDI note number as a float, since each note corresponds to a different drum sound rather than a pitch
}