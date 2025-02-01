#pragma once

namespace batteries
{
  struct PostProcessEffectSettings
  {
    bool active = true;
  };

  struct BasePostProcessEffect
  {
    virtual ~BasePostProcessEffect() = default;
  };

  template <typename T>
  struct PostProcessEffect : public BasePostProcessEffect
  {
    T settings;
  };
}