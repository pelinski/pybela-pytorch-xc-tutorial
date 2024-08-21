
#include <Bela.h>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <getopt.h>
#include <string.h>

// Handle Ctrl-C by requesting that the audio rendering stop
void interrupt_handler(int var)
{
    Bela_requestStop();
}

// Print usage information
void usage(const char * processName)
{
	std::cerr << "Usage: " << processName << " [options]" << std::endl;

	Bela_usage();

	std::cerr << "   --modelpath [-m] model path:        path to torch .jit model\n";
	std::cerr << "   --help [-h]:                        Print this menu\n";
}


int main(int argc, char *argv[])
{
    BelaInitSettings *settings = Bela_InitSettings_alloc();
    std::string modelPath = "model.jit";

    struct option customOptions[] =
	{
		{"help", 0, NULL, 'h'},
		{"modelpath", 1, NULL, 'm'},
		{NULL, 0, NULL, 0}
	};

    // Set default settings
    Bela_defaultSettings(settings);
    settings->setup = setup;
    settings->render = render;
    settings->cleanup = cleanup;

	{
		char* nameWithSlash = strrchr(argv[0], '/');
		settings->projectName = nameWithSlash ? nameWithSlash + 1 : argv[0];
	}

	// Parse command-line arguments
	while (1) {
		int c = Bela_getopt_long(argc, argv, "hm:", customOptions, settings);
		if (c < 0)
		{
			break;
		}
		int ret = -1;
		switch (c) {
			case 'h':
				usage(basename(argv[0]));
				ret = 0;
				break;
			case 'm':
				modelPath = optarg;
				break;
			default:
				usage(basename(argv[0]));
				ret = 1;
				break;
		}
		if(ret >= 0)
		{
			Bela_InitSettings_free(settings);
			return ret;
		}
	}


    // Initialise the PRU audio device
    if (Bela_initAudio(settings, &modelPath) != 0)
    {
        Bela_InitSettings_free(settings);
        fprintf(stderr, "Error: unable to initialise audio\n");
        return 1;
    }
    Bela_InitSettings_free(settings);


    // Start the audio device running
    if (Bela_startAudio())
    {
        fprintf(stderr, "Error: unable to start real-time audio\n");
        // Stop the audio device
        Bela_stopAudio();
        // Clean up any resources allocated for audio
        Bela_cleanupAudio();
        return 1;
    }

    // Set up interrupt handler to catch Control-C and SIGTERM
    signal(SIGINT, interrupt_handler);
    signal(SIGTERM, interrupt_handler);

    // Run until told to stop
    while (!Bela_stopRequested())
    {
        usleep(100000);
    }

    // Stop the audio device
    Bela_stopAudio();

    // Clean up any resources allocated for audio
    Bela_cleanupAudio();

    // All done!
    return 0;

}