#include <Bela.h>
#include <Watcher.h>
#include <cmath>
#include <libraries/AudioFile/AudioFile.h>

unsigned int gAudioFramesPerAnalogFrame;
std::string gFilename = "waves.wav";
std::vector<std::vector<float>> gSampleData;
int gStartFrame = 44100;
int gEndFrame = 88200;
unsigned int gReadPtr;

Watcher<float> pot("pot"); // the "pot" variable is "watched"

bool setup(BelaContext *context, void *userData) {

  Bela_getDefaultWatcherManager()->getGui().setup(context->projectName);
  Bela_getDefaultWatcherManager()->setup(
      context->audioSampleRate); // set sample rate in watcher

  gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

  // Load the audio file
  gSampleData =
      AudioFileUtilities::load(gFilename, gEndFrame - gStartFrame, gStartFrame);

  return true;
}

void render(BelaContext *context, void *userData) {

  for (unsigned int n = 0; n < context->audioFrames; n++) {

    uint64_t analogFramesElapsed = int((context->audioFramesElapsed + n) / 2);
    Bela_getDefaultWatcherManager()->tick(
        analogFramesElapsed); // tick the watcher clock

    if (gAudioFramesPerAnalogFrame && !(n % gAudioFramesPerAnalogFrame)) {
      pot = map(analogRead(context, n / gAudioFramesPerAnalogFrame, 0), 0, 0.84,
                0, 1);
    }

    // Increment read pointer and reset to 0 when end of file is reached
    if (++gReadPtr > gSampleData[0].size())
      gReadPtr = 0;
    float sound = gSampleData[0][gReadPtr];

    // the pot controls the volume of the sound
    float out = pot * sound;

    // Write the audio to all out channels
    for (unsigned int channel = 0; channel < context->audioOutChannels;
         channel++) {
      audioWrite(context, n, channel, out);
    }
  }
}

void cleanup(BelaContext *context, void *userData) {}
