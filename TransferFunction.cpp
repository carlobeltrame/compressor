#include "TransferFunction.h"

#include <cmath>

TransferFunction::TransferFunction(double threshold, double ratio) : threshold_(threshold), ratio_(ratio) {}

void TransferFunction::SetThreshold(double threshold) {
  threshold_ = threshold;
}
void TransferFunction::SetRatio(double ratio) {
  ratio_ = ratio;
}

double TransferFunction::Process(double sideChain) const {
  // The output gain depends on how far the envelope is above the threshold (if at all)
  if (sideChain > threshold_) {
    double targetLevel = threshold_ + (sideChain - threshold_) * ratio_;
    return targetLevel / sideChain;
  }
  return 1.;
}