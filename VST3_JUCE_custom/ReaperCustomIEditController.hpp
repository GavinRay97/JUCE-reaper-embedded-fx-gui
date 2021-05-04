#pragma once
#include <juce_core/juce_core.h>

#include "../ReaperVST3InterfaceWrapper.hpp"

#include <juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ftypes.h>
#include <juce_audio_processors/format_types/VST3_SDK/base/source/fdebug.h>
#include "base/source/fobject.h"

#include <basetsd.h>
#include "../include/vendor/reaper-sdk/sdk/reaper_plugin_fx_embed.h"
#include "pluginterfaces/base/funknown.h"

// DEF_CLASS_IID(IReaperUIEmbedInterface) = (Expanded macro below)
const ::Steinberg::FUID IReaperUIEmbedInterface::iid(IReaperUIEmbedInterface_iid);

class ReaperVST3EditController final : public IReaperUIEmbedInterface {
private:
    Steinberg::int32 refCount = 1;

public:
    ReaperVST3EditController()
    {
        DBG("[ReaperVST3EditController::Constructor()] ReaperVST3EditController created");
    }

    virtual ~ReaperVST3EditController() = default;

    auto PLUGIN_API addRef() -> Steinberg::uint32 override
    {
        DBG("[ReaperVST3EditController::addRef()] CALLED");
        return (Steinberg::uint32)++ this->refCount;
    }

    auto PLUGIN_API release() -> Steinberg::uint32 override
    {
        DBG("[ReaperVST3EditController::release()] CALLED");
        const int r = --this->refCount;
        if (r == 0)
            delete this;
        return (Steinberg::uint32)r;
    }

    auto embed_message(int msg, Steinberg::TPtrInt parm2, Steinberg::TPtrInt parm3) -> Steinberg::TPtrInt override
    {
        DBG("[ReaperVST3EditController::embed_message] msg = " << msg << " parm2 = " << parm2 << " parm3 = " << parm3);
        switch (msg) {
        case REAPER_FXEMBED_WM_IS_SUPPORTED:
            return 1;
        }
        return 0;
    };

    auto PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) -> Steinberg::tresult override
    {
        DBG("[ReaperVST3EditController::queryInterface()] iid=" << _iid);
        jassertfalse;
        *obj = nullptr;
        return Steinberg::kNotImplemented;
    }

    // https://stackoverflow.com/a/61519399
    // "In former times, it was usual to declare the static member variable in the header but to define it in the .cpp-file"
    // "Since C++17, a new alternative is avaibable – using the keyword inline"
    // > "A static data member may be declared inline. An inline static data member can be defined in the class definition and may specify an initializer.
    // > "It does not need an out-of-class definition"
    //inline static const FUID iid = IReaperUIEmbedInterface::iid;
};