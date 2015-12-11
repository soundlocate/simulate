#ifndef _SOUNDPROCESSOR_H
#define _SOUNDPROCESSOR_H

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <limits>
#include <vector>

class SoundProcessor {
public:
	struct v3 {
		float x;
		float y;
		float z;

		v3(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		};
	};

	struct SoundObject{
		float x;
		float y;
		float z;
		float vx;
		float vy;
		float pitch;
		float freq;
		unsigned int sample;

	    SoundObject(float x, float y, float freq) {
			this->x = x;
			this->y = y;
			this->freq = freq;
		};
	};


	SoundProcessor(int samplerate, std::vector<v3> mics);

	int add(SoundObject o);
	int remove(float x, float y);

	double * sample(int count);
private:
	double * m_createSineWave(float freq);
	inline double m_sin(float freq, unsigned int sample);
	inline double m_dist(float x1, float y1, float z1, float x2, float y2, float z2);

	int m_samplerate = 0;
	int m_samples = 0;

	double m_speed = 340;

	std::vector<SoundObject> m_objs;
    std::vector<v3> m_dists;
	std::vector<double *> m_buffer;
};

#endif
