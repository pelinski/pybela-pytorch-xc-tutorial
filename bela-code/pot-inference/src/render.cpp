#include <Bela.h>
#include <torch/script.h>
#include <libraries/AudioFile/AudioFile.h>
#include <cmath>
#include <vector>
#include "AppOptions.h"

const int gInputWindowSize = 32;
const int gOutputWindowSize = 32;
const int gHopSize = 32;
const int gBufferSize = 128;

float gFrequency = 15.0;
float gPhase;

std::vector<float> gInputBuffer(gBufferSize);
int gInputBufferPointer = 0;
int gHopCounter = 0;

std::vector<float> gOutputBuffer(gBufferSize);
int gOutputBufferWritePointer = 2 * gHopSize;
int gOutputBufferReadPointer = 0;

torch::jit::script::Module model;
std::vector<float> unwrappedBuffer(gOutputWindowSize);
AuxiliaryTask gInferenceTask;
int gCachedInputBufferPointer = 0;
unsigned int gAudioFramesPerAnalogFrame;
float gInverseSampleRate;

void inference_task_background(void *);

std::string gFilename = "waves.wav";
std::vector<std::vector<float> > gSampleData;
int gStartFrame = 44100;
int gEndFrame = 88200;
unsigned int gReadPtr; 

bool setup(BelaContext *context, void *userData) {

    printf("analog sample rate: %.1f\n", context->analogSampleRate);

	// Better to calculate the inverse sample rate here and store it in a variable so it can be reused
	gInverseSampleRate = 1.0 / context->audioSampleRate;
	
	// Set up the thread for the inference
	gInferenceTask = Bela_createAuxiliaryTask(inference_task_background, 50, "bela-inference");

	// rate of audio frames per analog frame
	if (context->analogFrames) gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

    //gHopCounter = gHopSize;
    // Load PyTorch model
    AppOptions *opts = reinterpret_cast<AppOptions *>(userData);
    try {
        model = torch::jit::load(opts->modelPath.c_str());
    } catch (const c10::Error& e) {
        std::cerr << "Error loading the model: " << e.msg() << std::endl;
        return false;
    }

	// Load the audio file
	gSampleData = AudioFileUtilities::load(gFilename, gEndFrame - gStartFrame, gStartFrame);

    return true;
}

void inference_task(std::vector<float> const& inBuffer, unsigned int inPointer, std::vector<float>& outBuffer, unsigned int outPointer) {
    for (int n = 0; n < gInputWindowSize; n++) {
        int circularBufferIndex = (inPointer + n - gInputWindowSize + gBufferSize) % gBufferSize;
        //rt_printf("%d\n" , circularBufferIndex);
        unwrappedBuffer[n] = inBuffer[circularBufferIndex];
    }

    // Convert unwrappedBuffer to a Torch tensor
    torch::Tensor inputTensor = torch::from_blob(unwrappedBuffer.data(), {1,gInputWindowSize,1}).clone();
    

    // Perform inference
    torch::Tensor outputTensor = model.forward({inputTensor}).toTensor();
    outputTensor = outputTensor.view(-1);


    // Copy outputTensor to outBuffer
    for (int n = 0; n < gOutputWindowSize; n++) {
        int circularBufferIndex = (outPointer + n) % gBufferSize;
        outBuffer[circularBufferIndex] = outputTensor[n].item<float>();
    }
}

void inference_task_background(void *) {
    inference_task(gInputBuffer, gInputBufferPointer, gOutputBuffer, gOutputBufferWritePointer);
    gOutputBufferWritePointer = (gOutputBufferWritePointer + gHopSize) % gBufferSize;
}

void render(BelaContext *context, void *userData) {
	float pot = map(analogRead(context, 0 / gAudioFramesPerAnalogFrame, 0), 0, 0.84, 0, 3); // only check pot every 16 audio frames
    rt_printf("in: %f\n", pot);
	gInputBuffer[gInputBufferPointer++] = pot;
	if(gInputBufferPointer >= gBufferSize) {
		// Wrap the circular buffer
		// Notice: this is not the condition for starting a new inference
		gInputBufferPointer = 0;
	}

	// Get the output sample from the output buffer
	float outPot = gOutputBuffer[gOutputBufferReadPointer];
    rt_printf("out: %f\n", outPot);

	// Increment the read pointer in the output circular buffer
    gOutputBufferReadPointer++;
	if(gOutputBufferReadPointer >= gBufferSize)
		gOutputBufferReadPointer = 0;
	
			
    if (++gHopCounter >= gHopSize) {
        gHopCounter = 0;
        //gInputBufferPointer = (gInputBufferPointer + gHopSize) % gBufferSize;
        gCachedInputBufferPointer = gInputBufferPointer;
        Bela_scheduleAuxiliaryTask(gInferenceTask);
    }

   

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		
        // Increment read pointer and reset to 0 when end of file is reached
        if(++gReadPtr > gSampleData[0].size())
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
		if (outPot <= 1) {
		lfo = (1 - outPot) * sinf(gPhase) + outPot * tri;
		} else if (outPot <= 2) {
		lfo = (2 - outPot) * tri + (1 - outPot) * sq;
		} else if (outPot <= 3) {
		float saw = 1 - (1 / (float)M_PI * gPhase);
		lfo = (3 - outPot) * sq + (2 - outPot) * saw;
		}

		// Multiply the audio sample by the LFO value
		float in = gSampleData[0][gReadPtr];
		float out =  lfo * gSampleData[0][gReadPtr];

		// Write the audio input to left channel, output to the right channel
		audioWrite(context, n, 0, in);
		audioWrite(context, n, 1, out);
	}
}

void cleanup(BelaContext *context, void *userData) {
    // Clean up resources
}
