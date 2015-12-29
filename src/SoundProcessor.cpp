#include "SoundProcessor.h"

SoundProcessor::SoundProcessor(int samplerate, std::vector<v3> dists) {
	m_samplerate = samplerate;
	m_dists = dists;
	//m_speed /= (m_samplerate);
}

//ToDo(robin): add veclocity and pitch support
int SoundProcessor::add(SoundProcessor::SoundObject * o) {
	o->sample = m_samples;
	m_objs.push_back(o);

	return 0;
}

int SoundProcessor::remove(float x, float y) {
	for(int i = 0; i < m_objs.size(); i++) {
		if((fabs(m_objs[i]->x - x) <= 0.0001) && fabs(m_objs[i]->y - y) <= 0.0001) {
			m_objs.erase(m_objs.begin() + i);
			return 0;
		}
	}

	m_objs.clear();

	return 1;
}

double * SoundProcessor::sample(int count) {
	double * ret = (double *) malloc(sizeof(double) * m_dists.size() * count);
	double value = 0;

	int k = 0;

	int dists[4] = {0};

	for(int i = 0; i < count; i++) {
		m_samples++;

		for(int j = 0; j < m_dists.size(); j++) {
		    v3 mic = m_dists[j];
			value = 0;

			for(auto source : m_objs) {
				int travelSamples = (m_dist(mic.x, mic.y, mic.z, source->x, source->y, source->z) / m_speed) * m_samplerate;

				dists[j] = travelSamples;

				if(travelSamples < (m_samples - source->sample)) {
					value += m_sin(source->freq, m_samples - source->sample - travelSamples);
				}
			}

			ret[k++] = value / ((m_objs.size() != 0) ? m_objs.size() : 1);
		}

		if(i == 0) {
//			std::cout << "dx12 " << m_speed * (float) ((float) (dists[0] - dists[1]) / (float) m_samplerate) << std::endl;
//			std::cout << "dx13 " << m_speed * (float) ((float) (dists[0] - dists[2]) / (float) m_samplerate) << std::endl;
		}
	}

	return ret;
}

inline double SoundProcessor::m_dist(double x1, double y1, double z1, double x2, double y2, double z2) {
	double dx =	x2 - x1;
	double dy =	y2 - y1;
	double dz =	z2 - z1;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

double * SoundProcessor::m_createSineWave(float freq) {
    double * data = (double *) malloc(sizeof(double) * (int) ((float) m_samplerate / freq));

	for(int i = 0; i <= (unsigned int) ((float) m_samplerate / freq); i++)
		data[i] = sin((((float) i * freq) / (float) m_samplerate) * 2.0 * M_PI);

	return data;
}

inline double SoundProcessor::m_sin(double freq, unsigned int sample) {
	return sin((((double) sample * freq) / (double) m_samplerate) * 2.0 * M_PI);
}
