#include <Bela.h>
#include <Watcher.h>
#include <cmath>

unsigned int gAudioFramesPerAnalogFrame;

Watcher<float> pot("pot"); // the "pot" variable is "watched"

bool setup(BelaContext *context, void *userData) {

  Bela_getDefaultWatcherManager()->getGui().setup(context->projectName);
  Bela_getDefaultWatcherManager()->setup(
      context->audioSampleRate); // set sample rate in watcher

  gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

  return true;
}

void render(BelaContext *context, void *userData) {

  for (unsigned int n = 0; n < context->audioFrames; n++) {

    uint64_t analogFramesElapsed = int((context->audioFramesElapsed + n) / 2);
    Bela_getDefaultWatcherManager()->tick(
        analogFramesElapsed); // tick the watcher clock

    if (gAudioFramesPerAnalogFrame && !(n % gAudioFramesPerAnalogFrame)) {
      pot = analogRead(context, n / gAudioFramesPerAnalogFrame, 0);
    }
  }
}

void cleanup(BelaContext *context, void *userData) {}
