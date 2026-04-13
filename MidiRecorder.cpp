#include "MidiRecording.hpp"
void MidiRecorder::start() {
    std::lock_guard<std::mutex> lock(mtx);
    events.clear();
    startTime = Clock::now();
    recording = true;
}

MidiRecording MidiRecorder::stop() {
    std::lock_guard<std::mutex> lock(mtx);
    recording = false;

    MidiRecording rec;
    rec.events = events;

    if (!events.empty()) {
        rec.length = events.back().timestamp;
    }

    return rec;
}

void MidiRecorder::process(const MidiMessage& msg) {
    if (!recording) return;

    auto now = Clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);

    std::lock_guard<std::mutex> lock(mtx);
    events.push_back({msg, delta});
}