#ifndef __ENVELOPE__
#define __ENVELOPE__



class Envelope
{
public:
  Envelope(int numSamples);

  void SetTime(int numSamples);
  void Process(double& state, double sample) const;

private:

  double factor_;

  static double SamplesToFactor(int numSamples);
};

#endif // __ENVELOPE__
