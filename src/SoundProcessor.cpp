#include "SoundProcessor.h"

SoundProcessor::SoundProcessor(int samplerate, std::vector<v3> dists) {
  m_samplerate = samplerate;
  m_dists = dists;
  m_speed /= (m_samplerate);
}

//ToDo(robin): add veclocity and pitch support
int SoundProcessor::add(SoundProcessor::SoundObject o) {
	o.sample = m_samples;
	m_objs.push_back(o);

	return 0;
}

int SoundProcessor::remove(float x, float y) {
	for(int i = 0; i < m_objs.size(); i++) {
		if((m_objs[i].x == x) && (m_objs[i].x == x)) {
			m_objs.erase(m_objs.begin() + i - 1);
			return 0;
		}
	}
	return 1;
}

double * SoundProcessor::sample(int count) {
	double * ret = (double *) malloc(sizeof(double) * (m_dists.size() / 3) * count);
	double value = 0;

	std::cout << __PRETTY_FUNCTION__ << "ret = " << ret << count << std::endl;

	for(int i = 0; i < count; i++) {
		m_samples++;
		int k = 0;

		for(auto mic : m_dists) {
			value = 0;

			for(auto source : m_objs) {
				int travelSamples = m_dist(mic.x, mic.y, mic.z, source.x, source.y, source.z) / m_speed;
				if(travelSamples < (m_samples - source.sample)) {
					value += m_sin(source.freq, m_samples - source.sample - travelSamples);
				}
			}

			ret[i * 8 + k++] = value;
		}
	}

	std::cout << __PRETTY_FUNCTION__ << "ret = " << ret << count << std::endl;

	return ret;
}

inline double SoundProcessor::m_dist(float x1, float y1, float z1, float x2, float y2, float z2) {
	double dx =	x2 - x1;
	double dy =	y2 - y1;
	double dz =	z2 - z1;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

double * SoundProcessor::m_createSineWave(float freq) {
    double * data = (double *) malloc(sizeof(double) * (int) ((float) m_samplerate / freq));

	for(int i = 0; i < (unsigned int) ((float) m_samplerate / freq) + 1; i++)
		data[i] = sin((((float) i * freq) / (float) m_samplerate) * 2.0 * M_PI);

	return data;
}

inline double SoundProcessor::m_sin(float freq, unsigned int sample) {
	return sin((((float) sample * freq) / (float) m_samplerate) * 2.0 * M_PI);
}
