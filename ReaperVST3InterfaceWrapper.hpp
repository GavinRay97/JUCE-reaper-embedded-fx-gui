// Need to fix up the "reaper_vst3_interfaces.h" file to make it like this:
// https://gist.github.com/GavinRay97/2b2ec9bf2997ab13218166c871c4b6f8
#include "pluginterfaces/base/funknown.h"

using namespace Steinberg;
#include "./include/vendor/reaper-sdk/sdk/reaper_vst3_interfaces.h"

DEF_CLASS_IID(IReaperHostApplication)
DEF_CLASS_IID(IReaperUIEmbedInterface)
