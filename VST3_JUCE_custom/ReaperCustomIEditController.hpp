#pragma once
#include <juce_core/juce_core.h>

#include "../include/vendor/reaper-sdk/sdk/reaper_plugin_fx_embed.h"
#include "../ReaperVST3InterfaceWrapper.hpp"
#include "base/source/fobject.h"
#include <juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ftypes.h>

// I'm defining the same class IID as the IReaperUIEmbedInterface so that this implementation class
// gets resolved by this IID during lookups
DEF_CLASS_IID(IReaperUIEmbedInterface)
DECLARE_CLASS_IID(ReaperVST3EditController, 0x049bf9e7, 0xbc74ead0, 0xc4101e86, 0x7f725981)

// IReaperUIEmbedInterface has to be private, because it extends FUnknown and FObject extends FUnknown
// so an ambiguous base would occur when trying to cast ReaperVST3EditController -> FUnknown
// (The problem: Which FUnknown -- FObject's or IReaperUIEmbed's?)
class ReaperVST3EditController : public FObject,
                                 private IReaperUIEmbedInterface {
private:
    virtual Steinberg ::uint32 __stdcall addRef() override { return Steinberg ::FObject ::addRef(); }
    virtual Steinberg ::uint32 __stdcall release() override { return Steinberg ::FObject ::release(); }

public:
    Steinberg::TPtrInt embed_message(int msg, Steinberg::TPtrInt parm2, Steinberg::TPtrInt parm3) override
    {
        DBG("[ReaperVST3EditController::embed_message] msg = " << msg << " parm2 = " << parm2 << " parm3 = " << parm3);
        switch (msg) {
        case REAPER_FXEMBED_WM_IS_SUPPORTED:
            return 0;
        }
        return 0;
    };

    tresult queryInterface(const TUID _iid, void** obj) override
    {
        return Steinberg::kResultOk;
    };

    static const FUID iid;
};

DEF_CLASS_IID(ReaperVST3EditController)
