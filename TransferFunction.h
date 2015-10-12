#ifndef __TRANSFERFUNCTION__
#define __TRANSFERFUNCTION__



class TransferFunction
{
public:
  TransferFunction(double threshold, double ratio);

  void SetThreshold(double threshold);
  void SetRatio(double ratio);
  double Process(double sideChain) const;

private:
  double threshold_;
  double ratio_;
};

#endif // __TRANSFERFUNCTION__
