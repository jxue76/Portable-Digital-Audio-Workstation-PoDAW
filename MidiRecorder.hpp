#pragma once
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <memory>
#include "MidiRecording.hpp"
#include "Instrument.hpp"

class MidiRecorder {
public:
    void start(); // Clears existing events and starts recording
    MidiRecording stop(); // returns finished recording

    void process(const MidiMessage& msg); // Add midi message to recording with timestamp

    void setInstrument(std::shared_ptr<Instrument> instr) { currentInstrument = instr; }

    bool isRecording() const { return recording; }

private:
    std::vector<TimedMidiMessage> events;
    std::chrono::time_point<Clock> startTime;
    std::shared_ptr<Instrument> currentInstrument;

    std::atomic<bool> recording{false};
    std::mutex mtx;
};