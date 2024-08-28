#include <Bela.h>
#include <Watcher.h>
#include <cmath>
#include <libraries/AudioFile/AudioFile.h>
#include <libraries/Scope/Scope.h>

float gFrequency = 15.0;
float gPhase;
float gInverseSampleRate;
unsigned int gAudioFramesPerAnalogFrame;

Watcher<float> pot1("pot1");
Watcher<float> pot2("pot2");
Scope scope;

std::string gFilename = "waves.wav";
std::vector<std::vector<float>> gSampleData;
int gStartFrame = 44100;
int gEndFrame = 88200;
unsigned int gReadPtr;

bool setup(BelaContext *context, void *userData) {

  Bela_getDefaultWatcherManager()->getGui().setup(context->projectName);
  Bela_getDefaultWatcherManager()->setup(
      context->audioSampleRate); // set sample rate in watcher

  gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
  gInverseSampleRate = 1.0 / context->audioSampleRate;
  gPhase = 0.0;

  scope.setup(3, context->audioSampleRate);

  // Load the audio file
  gSampleData =
      AudioFileUtilities::load(gFilename, gEndFrame - gStartFrame, gStartFrame);

  return true;
}

void render(BelaContext *context, void *userData) {

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    uint64_t frames = int((context->audioFramesElapsed + n) / 2);
    if (gAudioFramesPerAnalogFrame && !(n % gAudioFramesPerAnalogFrame)) {

      if (n == 0) { // only update the pot values once per block
        Bela_getDefaultWatcherManager()->tick(frames);
        pot1 = map(analogRead(context, n / gAudioFramesPerAnalogFrame, 0), 0,
                   0.84, 0, 3);
        pot2 = map(analogRead(context, n / gAudioFramesPerAnalogFrame, 1), 0,
                   0.84, 0, 3);
      }

      // Increment read pointer and reset to 0 when end of file is reached
      if (++gReadPtr > gSampleData[0].size())
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
      if (pot1 <= 1) {
        lfo = (1 - pot1) * sinf(gPhase) + pot1 * tri;
      } else if (pot1 <= 2) {
        lfo = (2 - pot1) * tri + (1 - pot1) * sq;
      } else if (pot1 <= 3) {
        float saw = 1 - (1 / (float)M_PI * gPhase);
        lfo = (3 - pot1) * sq + (2 - pot1) * saw;
      }

      // Multiply the audio sample by the LFO value
      float in = gSampleData[0][gReadPtr];
      float out = pot2 * lfo * gSampleData[0][gReadPtr];

      scope.log(lfo, in, out);

      // Write the audio input to left channel, output to the right channel
      audioWrite(context, n, 0, out);
      audioWrite(context, n, 1, out);
    }
  }
}

void cleanup(BelaContext *context, void *userData) {}
