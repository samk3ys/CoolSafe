#include "Particle.h"
// Plays a melody - Connect small speaker to speakerPin
// Notes defined in microseconds (Period/2) 
// from note C to B, Octaves 3 through 7

#define BPM         100
#define RELEASE     20
 
#define NOTE_C3 	3817
#define NOTE_C4 	1908
#define NOTE_C5 	956
#define NOTE_C6 	478
#define NOTE_C7 	239
#define NOTE_CS3 	3597
#define NOTE_CS4 	1805
#define NOTE_CS5 	903
#define NOTE_CS6 	451
#define NOTE_CS7 	226
#define NOTE_D3 	3401
#define NOTE_D4 	1701
#define NOTE_D5 	852
#define NOTE_D6 	426
#define NOTE_D7 	213
#define NOTE_DS3    3205
#define NOTE_DS4    1608
#define NOTE_DS5    804
#define NOTE_DS6    402
#define NOTE_DS7    201
#define NOTE_E3     3030
#define NOTE_E4     1515
#define NOTE_E5     759
#define NOTE_E6     379
#define NOTE_E7     190
#define NOTE_F3     2857
#define NOTE_F4     1433
#define NOTE_F5     716
#define NOTE_F6     358
#define NOTE_F7     179
#define NOTE_FS3    2703
#define NOTE_FS4    1351
#define NOTE_FS5    676
#define NOTE_FS6    338
#define NOTE_FS7    169
#define NOTE_G3     2551
#define NOTE_G4     1276
#define NOTE_G5     638
#define NOTE_G6     319
#define NOTE_G7     159
#define NOTE_GS3    2404
#define NOTE_GS4    1205
#define NOTE_GS5    602
#define NOTE_GS6    301
#define NOTE_GS7    151
#define NOTE_A3     2273
#define NOTE_A4     1136
#define NOTE_A5     568
#define NOTE_A6     284
#define NOTE_A7     142
#define NOTE_AS3    2146
#define NOTE_AS4    1073
#define NOTE_AS5    536
#define NOTE_AS6    268
#define NOTE_AS7    134
#define NOTE_B3     2024
#define NOTE_B4     1012
#define NOTE_B5     506
#define NOTE_B6     253
#define NOTE_B7     127

// EXAMPLE USAGE
//int numNotes = 12;	// number of notes to play
//int noteFrequencies[] = {NOTE_E5, NOTE_E5, 0, NOTE_E5, 0, NOTE_C5, NOTE_E5, 0, NOTE_G5, 0, 0, NOTE_G4};	// notes in the melody
//int noteDurations[] = 	{4,		  4, 	   4, 4,	   4, 4,	   4,		4, 4,		2, 4, 4		 };	// note durations: 4 = quarter note, 2 = half note, etc.

// Play "numNotes" notes from given "noteFrequencies" for "noteDurations" amount of time on speaker/buzzer connected to "speakerPin"
void play(int speakerPin, int numNotes, int* noteFrequencies, int* noteDurations);