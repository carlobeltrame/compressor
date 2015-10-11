#include "Compressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams {
  kThreshold = 0,
  kRatio,
  kMakeupGain,
  kAttack,
  kRelease,
  kNumParams
};

enum ELayout {
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kThresholdX = 10,
  kThresholdY = 100,
  kRatioX = 90,
  kRatioY = 100,
  kMakeupGainX = 170,
  kMakeupGainY = 100,
  kAttackX = 50,
  kAttackY = 150,
  kReleaseX = 130,
  kReleaseY = 150,
  kKnobFrames = 60
};

Compressor::Compressor(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), numChannels_(PLUG_CHANNELS), envelope_(numChannels_) {

  UpdateThreshold(0.);
  UpdateRatio(2.);
  UpdateMakeupGain(0.);
  UpdateAttack(20.);
  UpdateRelease(200.);

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kThreshold)->InitDouble("Threshold", 0., -30., 0., 0.01, "dBFS");
  GetParam(kThreshold)->SetShape(2);
  GetParam(kRatio)->InitDouble("Ratio", 2., 1., 30., 0.01, "");
  GetParam(kRatio)->SetShape(2);
  GetParam(kMakeupGain)->InitDouble("Makeup Gain", 0., -24., 24., 0.01, "dBFS");
  GetParam(kAttack)->InitDouble("Attack", 20., 0.1, 500., 0.01, "ms");
  GetParam(kAttack)->SetShape(2);
  GetParam(kRelease)->InitDouble("Release", 200., 0.1, 1500., 0.01, "ms");
  GetParam(kRelease)->SetShape(2);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_BLACK);

  IBitmap knobBMP = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  IKnobMultiControl* thresh = new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knobBMP);
  thresh->DisablePrompt(false);
  IKnobMultiControl* ratio = new IKnobMultiControl(this, kRatioX, kRatioY, kRatio, &knobBMP);
  ratio->DisablePrompt(false);
  IKnobMultiControl* gain = new IKnobMultiControl(this, kMakeupGainX, kMakeupGainY, kMakeupGain, &knobBMP);
  gain->DisablePrompt(false);
  IKnobMultiControl* attack = new IKnobMultiControl(this, kAttackX, kAttackY, kAttack, &knobBMP);
  attack->DisablePrompt(false);
  IKnobMultiControl* release = new IKnobMultiControl(this, kReleaseX, kReleaseY, kRelease, &knobBMP);
  release->DisablePrompt(false);
  pGraphics->AttachControl(thresh);
  pGraphics->AttachControl(ratio);
  pGraphics->AttachControl(gain);
  pGraphics->AttachControl(attack);
  pGraphics->AttachControl(release);

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

Compressor::~Compressor() {}

void Compressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
  // Mutex is already locked for us.

  for (int channel = 0; channel < numChannels_; ++channel) {
    double* in = inputs[channel];
    double* out = outputs[channel];
    double env = envelope_[channel];

    for (int s = 0; s < nFrames; ++s, ++in, ++out) {
      double rectified = abs(*in);
      // Is the sample above the envelope value?
      if (rectified > envelope_[channel]) {
        // Use the attack envelope
        env = env + attack_ * (rectified - env);
      }
      else {
        // Use the release envelope
        env = env - release_ * (env - rectified);
      }
      // The gain depends on how far the envelope is above the threshold (if at all)
      double gain = makeupGain_;
      if (env > threshold_) {
        double targetLevel = threshold_ + (env - threshold_) * ratio_;
        gain *= targetLevel / env;
      }
      *out = *in * gain;
    }

    envelope_[channel] = env;
  }
}

void Compressor::Reset() {
  IMutexLock lock(this);
}

void Compressor::OnParamChange(int paramIdx) {
  IMutexLock lock(this);

  switch (paramIdx) {
    case kThreshold:
      UpdateThreshold(GetParam(kThreshold)->Value());
      break;
    case kRatio:
      UpdateRatio(GetParam(kRatio)->Value());
      break;
    case kMakeupGain:
      UpdateMakeupGain(GetParam(kMakeupGain)->Value());
      break;
    case kAttack:
      UpdateAttack(GetParam(kAttack)->Value());
      break;
    case kRelease:
      UpdateRelease(GetParam(kRelease)->Value());
      break;
    default:
      break;
  }
}

void Compressor::UpdateThreshold(double value) {
  // Convert dB level to sample magnitude
  threshold_ = db2factor(value);
}

void Compressor::UpdateRatio(double value) {
  // Convert ratio x to 1:x
  ratio_ = 1. / value;
}

void Compressor::UpdateMakeupGain(double value) {
  // Convert dB ratio to sample ratio
  makeupGain_ = db2factor(value);
}

void Compressor::UpdateAttack(double value) {
  // Convert ms to number of samples
  double samples = GetSampleRate() * value / 1000.;
  // After the attack time, the envelope should be at most 1/1000 away from the target value
  attack_ = 1. - pow(1. / 1000., 1. / samples);
}

void Compressor::UpdateRelease(double value) {
  // Convert ms to number of samples
  double samples = GetSampleRate() * value / 1000.;
  // After the release time, the envelope should be at most 1/1000 away from the target value
  release_ = 1. - pow(1. / 1000., 1. / samples);
}
