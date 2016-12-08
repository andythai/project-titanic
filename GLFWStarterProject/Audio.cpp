//
//  Audio.cpp
//  CSE167Final
//
// Based on: https://ffainelli.github.io/openal-example/
// Wav file processing: http://www.gamedev.net/topic/669381-openal-sound-dependent-on-object/#entry5236587
//

#include "Audio.h"

/**
 Constructor
 filename - path to a wav file
 */
Audio::Audio(std::vector<std::string> filenames) {
    device = alcOpenDevice(NULL);
    if (!device) {
        fprintf(stderr, "unable to open default device\n");
        return;
    }

    fprintf(stdout, "Device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

    alGetError();

    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context)) {
        fprintf(stderr, "failed to make default context\n");
        return;
    }
    TEST_ERROR("make default context");
    
    long numOfFiles = filenames.size();
    buffers.resize(numOfFiles);
    sources.resize(numOfFiles);
    for (int i = 0; i < numOfFiles; i++) {
        alGenSources((ALuint)1, &sources[i]);
        TEST_ERROR("source generation");
        
        alSourcef(sources[i], AL_PITCH, 1);
        TEST_ERROR("source pitch");
        alSourcef(sources[i], AL_GAIN, 1);
        TEST_ERROR("source gain");
        alSource3f(sources[i], AL_POSITION, 0, 0, 0);
        TEST_ERROR("source position");
        alSource3f(sources[i], AL_VELOCITY, 0, 0, 0);
        TEST_ERROR("source velocity");
        alSourcei(sources[i], AL_LOOPING, AL_FALSE);
        TEST_ERROR("source looping");
        
        alGenBuffers(1, &buffers[i]);
        TEST_ERROR("buffer generation");
        
        // read in wav file
        char charbuff[18];
        std::ifstream in(filenames[i], std::ios::binary);
        in.read(charbuff, 4);
        if (strncmp(charbuff, "RIFF", 4) != 0) {
            std::cout << "Error:  not a valid wav file" << std::endl;
            return;
        }
        in.read(charbuff, 18);
        in.read(charbuff, 2);
        int chan = convertToInt(charbuff, 2);
        in.read(charbuff, 4);
        int samplerate = convertToInt(charbuff, 4);
        in.read(charbuff, 6);
        in.read(charbuff, 2);
        int bps = convertToInt(charbuff, 2);
        in.read(charbuff, 4);
        in.read(charbuff, 4);
        int size = convertToInt(charbuff, 4);
        char* data = new char[size];
        in.read(data, size);
        
        // check format
        if (chan == 1) {
            if (bps == 8) {
                format = AL_FORMAT_MONO8;
            }
            else {
                format = AL_FORMAT_MONO16;
            }
        }
        else {
            if (bps == 8) {
                format = AL_FORMAT_STEREO8;
            }
            else {
                format = AL_FORMAT_STEREO16;
            }
        }
        
        alBufferData(buffers[i], format, data, size, samplerate);
        TEST_ERROR("failed to load buffer data");
        
        alSourcei(sources[i], AL_BUFFER, buffers[i]);
        TEST_ERROR("buffer binding");
    }
}

/**
 Play the audio file. If called when playing, do nothing.
 */
void Audio::play(int index) {
    alGetSourcei(sources[index], AL_SOURCE_STATE, &source_state);
    TEST_ERROR("source state get");

    if (source_state == AL_PLAYING) {
        return;
    }

    alSourcePlay(sources[index]);
    TEST_ERROR("source playing");
}

/**
 Destructor.
 */
Audio::~Audio() {
    for (int i = 0; i < sources.size(); i++) {
        alDeleteSources(1, &sources[i]);
        alDeleteBuffers(1, &buffers[i]);
    }
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void Audio::toggleMuteAll() {
	muteAll = !muteAll;
	float gain = muteAll ? 0 : 1;
	for (ALuint source : sources) {
		alSourcef(source, AL_GAIN, gain);
		TEST_ERROR("source gain");
	}
}

