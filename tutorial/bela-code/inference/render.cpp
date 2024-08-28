#include <Bela.h>
#include <RtThread.h>
#include <cmath>
#include <libraries/AudioFile/AudioFile.h>
#include <torch/script.h>
#include <vector>

#define N_VARS 2

// torch buffers
const int gWindowSize = 512;
const int gBufferSize = 100 * gWindowSize;
int gOutputBufferWritePointer = 50 * gWindowSize;
int gOutputBufferReadPointer = 0;
int gInputBufferPointer = 0;
int gWindowFramesCounter = 0;
std::vector<std::vector<float>> gInputBuffer(N_VARS,
                                             std::vector<float>(gBufferSize));
std::vector<std::vector<float>> gOutputBuffer(N_VARS,
                                              std::vector<float>(gBufferSize));
torch::jit::script::Module model;
std::vector<std::vector<float>> unwrappedBuffer(gWindowSize,
                                                std::vector<float>(N_VARS));

AuxiliaryTask gInferenceTask;
int gCachedInputBufferPointer = 0;

// LFO
float gFrequency = 15.0;
float gPhase;
unsigned int gAudioFramesPerAnalogFrame;
float gInverseSampleRate;

void inference_task_background(void *);

std::string gFilename = "waves.wav";
std::vector<std::vector<float>> gSampleData;
int gStartFrame = 44100;
int gEndFrame = 88200;
unsigned int gReadPtr;

std::string gModelPath = "model.jit";

int start = 1;

bool setup(BelaContext *context, void *userData) {

  printf("analog sample rate: %.1f\n", context->analogSampleRate);

  // Better to calculate the inverse sample rate here and store it in a variable
  // so it can be reused
  gInverseSampleRate = 1.0 / context->audioSampleRate;

  // Set up the thread for the inference
  gInferenceTask =
      Bela_createAuxiliaryTask(inference_task_background, 99, "bela-inference");

  // rate of audio frames per analog frame
  if (context->analogFrames)
    gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

  if (userData != 0)
    gModelPath = *(std::string *)userData;

  try {
    model = torch::jit::load(gModelPath);
  } catch (const c10::Error &e) {
    std::cerr << "Error loading the model: " << e.msg() << std::endl;
    return false;
  }
  // Dummy inference
  try {
    // Create a dummy input tensor
    torch::Tensor dummy_input = torch::rand({1, 512, 2});

    // Perform a forward pass
    auto output = model.forward({dummy_input}).toTensor();

    // Print the result
    std::cout << "Dummy inference output: "
              << output.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << std::endl;
  } catch (const c10::Error &e) {
    std::cerr << "Error during dummy inference: " << e.msg() << std::endl;
    return false;
  }

  // Load the audio file
  gSampleData =
      AudioFileUtilities::load(gFilename, gEndFrame - gStartFrame, gStartFrame);

  return true;
}

void inference_task(const std::vector<std::vector<float>> &inBuffer,
                    unsigned int inPointer,
                    std::vector<std::vector<float>> &outBuffer,
                    unsigned int outPointer) {
  //   if (start) {
  //     start = 0;
  //     RtThread::setThisThreadPriority(1);
  //   }

  for (int n = 0; n < gWindowSize; n++) {
    int circularBufferIndex =
        (inPointer + n - gWindowSize + gBufferSize) % gBufferSize;
    // rt_printf("%d\n" , circularBufferIndex);
    for (int i = 0; i < N_VARS; i++) {
      unwrappedBuffer[n][i] = inBuffer[i][circularBufferIndex];
    }
  }

  // Convert unwrappedBuffer to a Torch tensor
  torch::Tensor inputTensor =
      torch::from_blob(unwrappedBuffer.data(), {1, gWindowSize, N_VARS})
          .clone();

  // Perform inference
  torch::Tensor outputTensor = model.forward({inputTensor}).toTensor();
  outputTensor = outputTensor.squeeze(0);

  // Copy outputTensor to outBuffer
  for (int n = 0; n < gWindowSize; n++) {
    int circularBufferIndex = (outPointer + n) % gBufferSize;
    for (int i = 0; i < N_VARS; i++) {
      outBuffer[i][circularBufferIndex] = outputTensor[n][i].item<float>();
    }
  }
}

void inference_task_background(void *) {
  if (start) {
    start = 0;
    RtThread::setThisThreadPriority(1);
  }
  inference_task(gInputBuffer, gInputBufferPointer, gOutputBuffer,
                 gOutputBufferWritePointer);
  gOutputBufferWritePointer =
      (gOutputBufferWritePointer + gWindowSize) % gBufferSize;
}

void render(BelaContext *context, void *userData) {

  float pot1;
  float pot2;
  float outpot1;
  float outpot2;

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    if (gAudioFramesPerAnalogFrame && !(n % gAudioFramesPerAnalogFrame)) {

      if (n == 0) { // only update the pot values once per block{
        pot1 = map(analogRead(context, n / gAudioFramesPerAnalogFrame, 0), 0,
                   0.84, 0, 3);
        pot2 = map(analogRead(context, n / gAudioFramesPerAnalogFrame, 1), 0,
                   0.84, 0, 1);

        // -- pytorch buffer
        gInputBuffer[0][gInputBufferPointer] = pot1;
        gInputBuffer[1][gInputBufferPointer] = pot2;
        if (++gInputBufferPointer >= gBufferSize) {
          // Wrap the circular buffer
          // Notice: this is not the condition for starting a new inference
          gInputBufferPointer = 0;
        }

        if (++gWindowFramesCounter >= gWindowSize) {
          gWindowFramesCounter = 0;
          // gInputBufferPointer = (gInputBufferPointer + gWindowSize) %
          // gBufferSize;
          gCachedInputBufferPointer = gInputBufferPointer;
          Bela_scheduleAuxiliaryTask(gInferenceTask);
        }
        // Get the output sample from the output buffer
        outpot1 = gOutputBuffer[0][gOutputBufferReadPointer];
        outpot2 = gOutputBuffer[1][gOutputBufferReadPointer];

        // rt_printf("read pointer: %d, write pointer %d \n",
        //           gOutputBufferReadPointer, gOutputBufferWritePointer);
        // Increment the read pointer in the output circular buffer
        if ((gOutputBufferReadPointer + 1) % gBufferSize ==
            gOutputBufferWritePointer) {
          rt_printf("Warning: output buffer overrun\n");
        } else {
          gOutputBufferReadPointer++;
        }
        if (gOutputBufferReadPointer >= gBufferSize)
          gOutputBufferReadPointer = 0;

        // --
      }
      // Increment read pointer and reset to 0 when end of file is reached
      if (++gReadPtr > gSampleData[0].size())
        gReadPtr = 0;

      // LFO code
      gPhase += 2.0f * (float)M_PI * gFrequency * gInverseSampleRate;
      if (gPhase > M_PI)
        gPhase -= 2.0f * (float)M_PI;

      float tri;
      float sq;
      if (gPhase > 0) {
        tri = -1 + (2 * gPhase / (float)M_PI);
        sq = 1;
      } else {
        tri = -1 - (2 * gPhase / (float)M_PI);
        sq = -1;
      }

      float lfo;
      if (outpot1 <= 1) {
        lfo = (1 - outpot1) * sinf(gPhase) + outpot1 * tri;
      } else if (outpot1 <= 2) {
        lfo = (2 - outpot1) * tri + (1 - outpot1) * sq;
      } else if (outpot1 <= 3) {
        float saw = 1 - (1 / (float)M_PI * gPhase);
        lfo = (3 - outpot1) * sq + (2 - outpot1) * saw;
      }

      // Multiply the audio sample by the LFO value
      float in = gSampleData[0][gReadPtr];
      float out = outpot2 * lfo * gSampleData[0][gReadPtr];

      // Write the audio input to left channel, output to the right channel
      audioWrite(context, n, 0, out);
      audioWrite(context, n, 1, out);

      //   if (n % 64) { // debug every 64 frames
      //     rt_printf("pot1: %.2f, pot2: %.2f\n", pot1, pot2);
      //     rt_printf("outpot1: %.2f, outpot2: %.2f\n", outpot1, outpot2);
      //   }
    }
  }
}

void cleanup(BelaContext *context, void *userData) {
  // Clean up resources
}
