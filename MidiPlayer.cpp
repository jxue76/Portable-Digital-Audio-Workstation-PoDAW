#include "MidiPlayer.hpp"

MidiPlayer::MidiPlayer(AudioHandler& audioHandler)
    : audio(audioHandler) {}

void MidiPlayer::play(const MidiRecording& recording, bool loop) {
    if (recording.events.empty()) return;

    stop(); // stop existing playback if any

    playing = true;

    playbackThread = std::thread([this, recording, loop]() {
        playbackThreadFunc(recording, loop);
    });
}

void MidiPlayer::stop() {
    playing = false;
    if (playbackThread.joinable())
        playbackThread.join();
}

void MidiPlayer::playbackThreadFunc(MidiRecording recording, bool loop) {
    do {
        auto start = Clock::now();

        for (const auto& ev : recording.events) {
            if (!playing) return;

            auto target = start + ev.timestamp;
            std::this_thread::sleep_until(target);

            const auto& msg = ev.msg;

            if (msg.isNoteOn()) {
                audio.addNoteToInstrument(msg.instrument, msg.note);
            } else if (msg.isNoteOff()) {
                audio.removeNoteFromInstrument(msg.instrument, msg.note);
            }
        }

    } while (loop && playing);

    playing = false;
}