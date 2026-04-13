#pragma once
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include "MidiRecording.hpp"

class MidiRecorder {
public:
    void start();
    MidiRecording stop(); // returns finished recording

    void process(const MidiMessage& msg);

    bool isRecording() const { return recording; }

private:
    std::vector<TimedMidiMessage> events;
    std::chrono::time_point<Clock> startTime;

    std::atomic<bool> recording{false};
    std::mutex mtx;
};