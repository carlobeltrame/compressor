#ifndef __COMPRESSOR__
#define __COMPRESSOR__

#include "IPlug_include_in_plug_hdr.h"

#include <cmath>
#include <vector>

class Compressor : public IPlug
{
public:
  Compressor(IPlugInstanceInfo instanceInfo);
  ~Compressor();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  int numChannels_;
  double threshold_;
  double ratio_;
  double makeupGain_;
  double attack_;
  double release_;

  void UpdateThreshold(double value);
  void UpdateRatio(double value);
  void UpdateMakeupGain(double value);
  void UpdateAttack(double value);
  void UpdateRelease(double value);

  std::vector<double> envelope_;

  inline static double db2factor(double dbValue) { return pow(10., dbValue / 20.); }
  inline static double factor2db(double scalarValue) { return 20. * log10(scalarValue); }
};

#endif // __COMPRESSOR__
