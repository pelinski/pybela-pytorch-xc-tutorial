#include <Bela.h>
#include <Watcher.h>
#include <cmath>
#include <libraries/Scope/Scope.h>

float gFrequency = 15.0;
float gPhase;
float gInverseSampleRate;
float gAudioFramesPerAnalogFrame;

Watcher<float> pot("pot");
Scope scope;

bool setup(BelaContext *context, void *userData) {
  gui.setup(context->projectName);

  gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
  gInverseSampleRate = 1.0 / context->audioSampleRate;
  gPhase = 0.0;

  scope.setup(3, context->audioSampleRate);

  return true;
}

void render(BelaContext *context, void *userData) {

  pot = map(analogRead(context, 0 / gAudioFramesPerAnalogFrame, 0), 0,
                  0.84, 0, 3); // only check pot once per audio block

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    uint64_t frames = context->audioFramesElapsed + n;
		Bela_getDefaultWatcherManager()->tick(frames);
    
    float in = audioRead(context, n, 0);

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

    float out = lfo * in;

    scope.log(lfo, in, out);

    for (unsigned int channel = 0; channel < context->audioOutChannels;
         channel++) {
      audioWrite(context, n, channel, out);
    }
  }
}

void cleanup(BelaContext *context, void *userData) {}
