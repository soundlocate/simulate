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

	class SoundObject{
	public:
		float x = 0;
		float y = 0;
		float z = 0;
		float vx = 0;
		float vy = 0;
		float pitch = 0;
		float freq = 0;
		unsigned int sample = 0;

	    SoundObject(float x, float y, float freq) {
			this->x = x;
			this->y = y;
			this->freq = freq;
		};
	};


	SoundProcessor(int samplerate, std::vector<v3> mics);

	int add(SoundObject * o);
	int remove(float x, float y);

	double * sample(int count);
private:
	double * m_createSineWave(float freq);
	inline double m_sin(double freq, unsigned int sample);
	inline double m_dist(double x1, double y1, double z1, double x2, double y2, double z2);

	int m_samplerate = 0;
	unsigned int m_samples = 0;

	double m_speed = 340;

	std::vector<SoundObject *> m_objs;
    std::vector<v3> m_dists;
	std::vector<double *> m_buffer;
};

#endif
