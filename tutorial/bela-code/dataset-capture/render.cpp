#include <Bela.h>
#include <Watcher.h>
#include <cmath>
#include <libraries/AudioFile/AudioFile.h>
#include <libraries/Scope/Scope.h>

float gFrequency = 15.0;
float gPhase;
float gInverseSampleRate;
float gAudioFramesPerAnalogFrame;

Watcher<float> pot("pot");
Scope scope;

std::string gFilename = "waves.wav";
std::vector<std::vector<float> > gSampleData;
int gStartFrame = 44100;
int gEndFrame = 88200;
unsigned int gReadPtr; 

bool setup(BelaContext *context, void *userData) {
  
  Bela_getDefaultWatcherManager()->getGui().setup(context->projectName);
	Bela_getDefaultWatcherManager()->setup(context->audioSampleRate); // set sample rate in watcher

  gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
  gInverseSampleRate = 1.0 / context->audioSampleRate;
  gPhase = 0.0;

  scope.setup(3, context->audioSampleRate);

  // Load the audio file
	gSampleData = AudioFileUtilities::load(gFilename, gEndFrame - gStartFrame, gStartFrame);

  return true;
}

void render(BelaContext *context, void *userData) {

  pot = map(analogRead(context, 0 / gAudioFramesPerAnalogFrame, 0), 0,
                  0.84, 0, 3); // only check pot once per audio block

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    uint64_t frames = context->audioFramesElapsed + n;
		Bela_getDefaultWatcherManager()->tick(frames);
    
    // Increment read pointer and reset to 0 when end of file is reached
    if(++gReadPtr > gSampleData[0].size())
        gReadPtr = 0;

    gPhase += 2.0f * (float)M_PI * gFrequency * gInverseSampleRate;

    float tri;
    float sq;
    if (gPhase > M_PI)
      gPhase -= 2.0f * (float)M_PI;

    if (gPhase > 0) {
      tri = -1 + (2 * gPhase / (float)M_PI);
      sq = 1;

    } else {
      tri = -1 - (2 * gPhase / (float)M_PI);
      sq = -1;
    }

    float lfo = 0;
    if (pot <= 1) {
      lfo = (1 - pot) * sinf(gPhase) + pot * tri;
    } else if (pot <= 2) {
      lfo = (2 - pot) * tri + (1 - pot) * sq;
    } else if (pot <= 3) {
      float saw = 1 - (1 / (float)M_PI * gPhase);
      lfo = (3 - pot) * sq + (2 - pot) * saw;
    }

		// Multiply the audio sample by the LFO value
		float in = gSampleData[0][gReadPtr];
		float out =  lfo * gSampleData[0][gReadPtr];

    scope.log(lfo, in, out);

		// Write the audio input to left channel, output to the right channel
		audioWrite(context, n, 0, in);
		audioWrite(context, n, 1, out);
  }
}

void cleanup(BelaContext *context, void *userData) {}
