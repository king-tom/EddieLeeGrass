#pragma once

#include <stdlib.h>

class CPerlinNoise
{
public:

    CPerlinNoise();
    CPerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

    double GetHeight(double x, double y) const;

	double Persistence() const { return persistence; }
	double Frequency()   const { return frequency;   }
	double Amplitude()   const { return amplitude;   }
	int    Octaves()     const { return octaves;     }
	int    RandomSeed()  const { return randomseed;  }

	void Set(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

	void SetPersistence(double _persistence) { persistence = _persistence; }
	void SetFrequency(  double _frequency)   { frequency = _frequency;     }
	void SetAmplitude(  double _amplitude)   { amplitude = _amplitude;     }
	void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
	void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

private:

	double Total(double i, double j) const;
	double GetValue(double x, double y) const;
	double Interpolate(double x, double y, double a) const;
	double Noise(int x, int y) const;

	double persistence, frequency, amplitude;
	int octaves, randomseed;
};