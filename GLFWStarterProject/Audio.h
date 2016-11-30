//
//  Audio.h
//  CSE167Final
//

#ifndef Audio_hpp
#define Audio_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <unistd.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <al.h>
#include <alc.h>
#endif

#include <iostream>
#include <string>
#include <fstream>

#define TEST_ERROR(_msg)		\
error = alGetError();		\
if (error != AL_NO_ERROR) {	\
fprintf(stderr, _msg "\n");	\
return;		\
}

class Audio {
public:
	ALCdevice* device;
	ALCcontext* context;
	ALenum format;
	ALuint buffer;
	ALuint source;
	ALCenum error;
	ALint source_state;

	Audio(std::string filename);
	~Audio();
	void play();

	static bool checkBigEndian() {
		int i = 1;
		return !((char*)&i)[0];
	}

	static int convertToInt(char* buffer, int len) {
		int a = 0;
		if (!checkBigEndian())
			for (int i = 0; i < len; i++)
				((char*)&a)[i] = buffer[i];
		else
			for (int i = 0; i < len; i++)
				((char*)&a)[3 - i] = buffer[i];
		return a;
	}
};

#endif /* Audio_hpp */
