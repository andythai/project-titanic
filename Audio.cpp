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
Audio::Audio(std::string filename) {
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

    alGenSources((ALuint)1, &source);
    TEST_ERROR("source generation");

    alSourcef(source, AL_PITCH, 1);
    TEST_ERROR("source pitch");
    alSourcef(source, AL_GAIN, 1);
    TEST_ERROR("source gain");
    alSource3f(source, AL_POSITION, 0, 0, 0);
    TEST_ERROR("source position");
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    TEST_ERROR("source velocity");
    alSourcei(source, AL_LOOPING, AL_FALSE);
    TEST_ERROR("source looping");

    alGenBuffers(1, &buffer);
    TEST_ERROR("buffer generation");

    // read in wav file
    char charbuff[18];
    std::ifstream in(filename, std::ios::binary);
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

    alBufferData(buffer, format, data, size, samplerate);
    TEST_ERROR("failed to load buffer data");

    alSourcei(source, AL_BUFFER, buffer);
    TEST_ERROR("buffer binding");
}

/**
 Play the audio file. If called when playing, do nothing.
 */
void Audio::play() {
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    TEST_ERROR("source state get");

    if (source_state == AL_PLAYING) {
        return;
    }

    alSourcePlay(source);
    TEST_ERROR("source playing");
}

/**
 Destructor.
 */
Audio::~Audio() {
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
