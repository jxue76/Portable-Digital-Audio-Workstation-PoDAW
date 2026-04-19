#include "MidiPlayer.hpp"
#include <algorithm>

MidiPlayer::MidiPlayer(AudioHandler& audioHandler)
    : audio(audioHandler) {}

void MidiPlayer::play(const MidiRecording& recording, bool loop) {
    if (recording.getEvents().empty()) return;

    stop(); // stop existing playback if any

    // Create a copy and sort by timestamp
    MidiRecording sortedRecording = recording;
    std::sort(sortedRecording.getEvents().begin(), sortedRecording.getEvents().end(), 
              [](const TimedMidiMessage& a, const TimedMidiMessage& b) {
                  return a.getTimestamp() < b.getTimestamp();
              });

    playing = true;

    playbackThread = std::thread([this, sortedRecording, loop]() {
        playbackThreadFunc(sortedRecording, loop);
    });
}

void MidiPlayer::stop() {
    playing = false;
    if (playbackThread.joinable())
        playbackThread.join();
}

void MidiPlayer::playbackThreadFunc(MidiRecording recording, bool loop = false) {
    do {
        auto start = Clock::now();

        for (const auto& ev : recording.getEvents()) {
            if (!playing) return;

            auto target = start + ev.getTimestamp();
            std::this_thread::sleep_until(target);

            if (ev.isOn()) {
                audio.addNoteToInstrument(recording.getInstrument(), ev.getNote());
            } else {
                audio.removeNoteFromInstrument(recording.getInstrument(), ev.getNote());
            }
        }

    } while (loop && playing);

    playing = false;
}