// Plays a melody - Connect small speaker to speakerPin
// Notes defined in microseconds (Period/2) 
// from note C to B, Octaves 3 through 7

#include "playTones.h"

void play(int speakerPin, int numNotes, int* noteFrequencies, int* noteDurations) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < numNotes; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 60*1000/BPM/noteDurations[thisNote];
    tone(speakerPin, (noteFrequencies[thisNote]!=0)?(500000/noteFrequencies[thisNote]):0,noteDuration-RELEASE);

    // blocking delay needed because tone() does not block
    delay(noteDuration);
  }

  noTone(speakerPin);
}