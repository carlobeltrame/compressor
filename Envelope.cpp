#include "Envelope.h"

#include <cmath>

Envelope::Envelope(int numSamples) : factor_(SamplesToFactor(numSamples)) {}

double Envelope::SamplesToFactor(int numSamples) {
  // After the decay time, the envelope should be at most 1/1000 away from the target value
  return 1. - pow(1. / 1000., 1. / numSamples);
}

void Envelope::SetTime(int numSamples) {
  factor_ = SamplesToFactor(numSamples);
}

void Envelope::Process(double& state, double sample) const {
  state += factor_ * (sample - state);
}