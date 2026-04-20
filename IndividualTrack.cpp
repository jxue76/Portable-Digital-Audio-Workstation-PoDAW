#include "imgui.h"
#include "IndividualTrack.hpp"

static constexpr float HEADER_W  = 80.0f;
static constexpr float GRID_X0   = 90.0f;
static constexpr float GRID_Y0   = 10.0f;
static constexpr float CELL_W    = 22.0f;
static constexpr float CELL_H    = 40.0f;
static constexpr float TRACK_PAD = 55.0f;
static constexpr float BAR_Y     = 280.0f;
static constexpr float BAR_H     = 40.0f;
static constexpr float WIN_W     = 480.0f;

void IndividualTrackUI::render(Sequencer& seq, Inputs& inputs, float dt, bool isPlayback, MidiRecording& recordedNotes) {
    cursor_increment = 440.0f/(static_cast<float>(seq.timeSigNum)*4.0f);
    //ppn = 280.0f/26.0f; //pixels per note
    ppb = cursor_increment;
    handleInputs(seq, inputs, dt, recordedNotes);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p       = ImGui::GetCursorScreenPos();
    ImVec2 wp      = ImGui::GetWindowPos();

    // Draw notes here
    // 13 notes an octave, 7 octaves max on a piano
    // Number of notes per note chunk: 26, for two octaves per chunk
    // note_selection_vert will move selection by half octaves(7 notes), max note_selection_vert = 10
    drawNotes(recordedNotes, seq);
    /*for (int i = 0; i < 26; i++) {
        if (i%2==0) dl->AddRectFilled(ImVec2(-10000, i*ppn), ImVec2(10000, i*ppn+ppn), IM_COL32(90,80,30,255));
    }*/

    if (fastMovement) dl->AddRectFilled(ImVec2(0,0),ImVec2(40,280),IM_COL32(180,130,70,255));
    if (!fastMovement) dl->AddRectFilled(ImVec2(0,0), ImVec2(40, 280), IM_COL32(70, 130, 180, 255));
    char track_num[16];
    snprintf(track_num, sizeof(track_num), "%d", seq.currentTrack);
    dl->AddText(ImVec2(10, 20), IM_COL32(255,255,255,255), track_num);
    // Next is track instrument
    snprintf(track_num, sizeof(track_num), "%s", recordedNotes.getInstrument().get()->getName());
    dl->AddText(ImVec2(10,30), IM_COL32(255,255,255,255), track_num);
    if (isPlayback) {
        snprintf(track_num, sizeof(track_num), "PLAY");
        dl->AddText(ImVec2(10,40), IM_COL32(255,255,255,255), track_num);
    } else {
        snprintf(track_num, sizeof(track_num), "REC");
        dl->AddText(ImVec2(10,40), IM_COL32(255,255,255,255), track_num);
    }

    // Selection for notes from y = 40 -> 280, x width of 40
    float ppo = 240.0f/7.0f; // 7 octaves, two octaves displayed at a time
    dl->AddRectFilled(ImVec2(0,280-(note_selection_vert*ppo/2)),
                        ImVec2(40, 280-(note_selection_vert*ppo/2)-(ppo*2)),
                        IM_COL32(0,0,0,60));

    // Cursor
    dl->AddRectFilled(ImVec2(cursor_pos, 0),
                            ImVec2(cursor_pos+10, 470),
                            IM_COL32(90,80,30,200));

    float segmentLength = 480/max_segments;
    dl->AddRectFilled(ImVec2(0,280), ImVec2(480,320), IM_COL32(30,30,30,255));
    dl->AddRectFilled(ImVec2(current_segment*segmentLength,280),
                        ImVec2(current_segment*segmentLength+segmentLength, 320),
                        IM_COL32(255,255,255,50));
}

void IndividualTrackUI::handleInputs(Sequencer& seq, Inputs& inputs, float dt, MidiRecording& recordedNotes) {
    if (seq.isStopped()) {
        if (fastMovement) {
            if (leftKey.check(inputs.isLeftPressed(), dt)) {
                current_segment -= 1;
                if (current_segment <= 0) {
                    current_segment = 0;
                }
            }
            if (rightKey.check(inputs.isRightPressed(), dt)) {
                current_segment += 1;
                if (current_segment >= max_segments-1) {
                    current_segment = max_segments-1;
                }
            }
        } else {
            if (leftKey.check(inputs.isLeftPressed(), dt)) {
                cursor_pos -= cursor_increment;
                if (cursor_pos <= 40) {
                    if (current_segment <= 0) {
                        cursor_pos = 40;
                    } else {
                        current_segment -= 1;
                        cursor_pos = 480;
                    }
                }
            }
            if (rightKey.check(inputs.isRightPressed(), dt)) {
                cursor_pos += cursor_increment;
                if (cursor_pos >= 480) {
                    if (current_segment >= max_segments-1) {
                        cursor_pos = 480;
                    } else {
                        current_segment += 1;
                        cursor_pos = 40;
                    }
                }
            }
        }
    }
    if (fastMovement) {
        if (upKey.check(inputs.isUpPressed(), dt)) {
            seq.currentTrack -= 1;
            if (seq.currentTrack <= 1) {
                seq.currentTrack = 1;
            }
        }
        if (downKey.check(inputs.isDownPressed(), dt)) {
            seq.currentTrack += 1;
            if (seq.currentTrack >= seq.NUM_TRACKS) {
                seq.currentTrack = seq.NUM_TRACKS;
            }
        }
    } else {
        if (upKey.check(inputs.isUpPressed(), dt)) {
            note_selection_vert += 1;
            if (note_selection_vert >= 10) note_selection_vert = 10;
        }
        if (downKey.check(inputs.isDownPressed(), dt)) {
            note_selection_vert -= 1;
            if (note_selection_vert <= 0) note_selection_vert = 0;
        }
    }

    if (fastKey.check(inputs.isXPressed(), dt)) {
        fastMovement = !fastMovement;
    }
}

void IndividualTrackUI::drawNotes(MidiRecording& recordedNotes, Sequencer& seq) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p       = ImGui::GetCursorScreenPos();
    ImVec2 wp      = ImGui::GetWindowPos();

    /*
        Top of notes: 0
        Bottom-most note: ppn*25
        Leftmost: 40
        Rightmost: 40 + ppb*(timeSigNum*4)
    */

    ImU32 white = IM_COL32(255,255,255,255);

    std::vector<float> x(26, -1.0f);

    float segmentOffset = -440.0f*current_segment;
    // Vertoffset is based on 0 as lowest note, with each difference being a distance of
    // ppn*7
    float verticalOffset = note_selection_vert*ppn*(-7.0f);

    /*
        Select which notes will be displayed here
        vert_selection = 0; 0-25
        vert_selection = 1; 7-32
    */
    int noteMin = note_selection_vert*7;
    int noteMax = noteMin+25;

    /*
    dl->AddRectFilled(ImVec2(0, 4*ppn), ImVec2(2*ppb+40, 5*ppn+ppn), white);
    dl->AddRectFilled(ImVec2(2*ppb+40, 0), ImVec2(2*ppb+ppb+40, 2*ppn), white);
    dl->AddRectFilled(ImVec2(3*ppb+40, 6*ppn), ImVec2(6*ppb+40, 7*ppn+ppn), white);
    dl->AddRectFilled(ImVec2(7*ppb+40, 20*ppn), ImVec2(10*ppb+40, 21*ppn+ppn), white);
    dl->AddRectFilled(ImVec2(11*ppb+40, 14*ppn), ImVec2(15*ppb+40, 15*ppn+ppn), white);
    */

    std::vector<TimedMidiMessage>& events = recordedNotes.getEvents();

    // Equation for timestamp to beat conversion:
    // timestamp*0.1^6*seq.tempo/60
    float t = 0.1f*0.1f*0.1f*0.1f*0.1f*0.1f;
    float ttb = t*seq.tempo/60; // timestamp to beats

    noteCol = IM_COL32(255,255,255,255);

    for (TimedMidiMessage& noteEvent : events) {
        int note = noteEvent.getNote().getMidiNote();
        if (note >= noteMin && note <= noteMax && noteEvent.isOn()) {
            // Save start x position
            x[note-noteMin] = 40.0f+segmentOffset+ noteEvent.getTimestamp().count()*ttb;
        }
        if (note >= noteMin && note <= noteMax && !noteEvent.isOn() && x[note-noteMin] != -1.0f) {
            // Draw note
            y = (280 - (note-noteMin)*ppn);
            x_end = 40.0f+segmentOffset+ noteEvent.getTimestamp().count()*ttb;
            dl->AddRectFilled(ImVec2(x[note-noteMin],y), ImVec2(x_end,y+ppn),noteCol);
            x[note-noteMin] = -1.0f;
        }
    }
}