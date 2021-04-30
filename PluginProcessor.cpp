#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include <cstddef>

#ifdef _WIN32
#include <basetsd.h>
#endif

// Provider IReaperHostApplication, IReaperUIEmbedInterface properly wrapped
#include "./ReaperVST3InterfaceWrapper.hpp"
DEF_CLASS_IID(IReaperHostApplication)
// Provides macro definitions for things like "REAPER_FXEMBED_WM_GETMINMAXINFO"
#include "include/vendor/reaper-sdk/sdk/reaper_plugin_fx_embed.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

void AudioPluginAudioProcessor::handleVST3HostContext(
  Steinberg::FUnknown* hostContext,
  Steinberg::Vst::IHostApplication* host,
  juce::JuceAudioProcessor* comPluginInstance,
  juce::JuceVST3EditController* juceVST3EditController) 
{
  // Credit: Robbert van der Helm (https://github.com/robbert-vdh)
  auto reaper = FUnknownPtr<IReaperHostApplication>(hostContext);
  auto showConsoleMsg = static_cast<void(*)(const char* msg)>(reaper->getReaperApi("ShowConsoleMsg"));
  showConsoleMsg("Test from IReaperHostApplication");
  return;
}


void AudioPluginAudioProcessor::timerCallback() {
	DBG("Timer callback invoked");
}

// Handle drawing embedded UI
Steinberg::TPtrInt AudioPluginAudioProcessor::handleReaperEmbedMessage(int msg, Steinberg::TPtrInt parm2, Steinberg::TPtrInt parm3)
{
	auto embedComponent = this->getActiveEditor();

	switch (msg)
	{
	/* return 1 if embedding is supported and available
	 * return -1 if embedding is supported and unavailable
	 * return 0 if embedding is not supported
	*/
	case REAPER_FXEMBED_WM_IS_SUPPORTED:
		return 1;

	// called when embedding begins (return value ignored)
	case REAPER_FXEMBED_WM_CREATE:
	{

	}
	return 0;

	// called when embedding ends (return value ignored)
	case REAPER_FXEMBED_WM_DESTROY:
	{

	}
	return 0;

	/*
	* get size hints. parm3 = (REAPER_FXEMBED_SizeHints*). return 1 if supported
	* note that these are just hints, the actual size may vary
	*/
	case REAPER_FXEMBED_WM_GETMINMAXINFO:
		if (parm3)
		{
			/*
			typedef struct REAPER_FXEMBED_SizeHints { // alias to MINMAXINFO
				int preferred_aspect; // 16.16 fixed point (65536 = 1:1, 32768 = 1:2, etc)
				int minimum_aspect;   // 16.16 fixed point
				int _res1, _res2, _res3, _res4;
				int min_width, min_height;
				int max_width, max_height;
			} REAPER_FXEMBED_SizeHints;
			*/
			auto mmi = (REAPER_FXEMBED_SizeHints*)parm3;
			mmi->min_width = 80;
			mmi->min_height = 90;
			mmi->max_width = 100;
			mmi->max_height = 110;
			mmi->preferred_aspect = 0;
			mmi->minimum_aspect = 0;
		}
		return 1;

	// [NOTE]: Not really sure what this does (Gavin)
	// parm3: REAPER_FXEMBED_DrawInfo*. set mouse cursor and return REAPER_FXEMBED_RETNOTIFY_HANDLED, or return 0.
	case REAPER_FXEMBED_WM_SETCURSOR:
	if (parm3)
	{
		auto drawInfo = (REAPER_FXEMBED_DrawInfo*)parm3;
		return REAPER_FXEMBED_RETNOTIFY_HANDLED;
	}
	return 0;

	/*
	* mouse messages
	* parm3 = (REAPER_FXEMBED_DrawInfo*)
	* capture is automatically set on mouse down, released on mouse up
	* when not captured, will always receive a mousemove when exiting the window
	*
	* if the mouse messages return with REAPER_FXEMBED_RETNOTIFY_INVALIDATE set, a non-optional
  * redraw is initiated (generally sooner than the next timer-based redraw)
	*/
	case REAPER_FXEMBED_WM_MOUSEMOVE:
	case REAPER_FXEMBED_WM_LBUTTONDOWN:
	case REAPER_FXEMBED_WM_LBUTTONUP:
	case REAPER_FXEMBED_WM_LBUTTONDBLCLK:
	case REAPER_FXEMBED_WM_RBUTTONDOWN:
	case REAPER_FXEMBED_WM_RBUTTONUP:
	case REAPER_FXEMBED_WM_RBUTTONDBLCLK:
	case REAPER_FXEMBED_WM_MOUSEWHEEL:
		if (parm3)
		{
			auto inf = (REAPER_FXEMBED_DrawInfo*)parm3;
			auto point = juce::Point<float>(inf->mouse_x, inf->mouse_y);

			juce::Desktop &desktop = juce::Desktop::getInstance();
			auto mms = desktop.getMainMouseSource();

			auto ev = juce::MouseEvent(mms, point, 0, 0,
				0, 0, 0, 0,
				embedComponent, embedComponent,
				juce::Time(0), 
				point, juce::Time(0),
				1, false);

			int w = inf->width, h = inf->height;
			if (inf->mouse_x < 0 || inf->mouse_y < 0)
				return 1;

			juce::Point<float> ptDesktop = mms.getScreenPosition();
			// calculate desktop position of BMP 
			juce::Point<float> mousePosDesktop = mms.getScreenPosition();
			juce::Point<float> mousePosBmp = juce::Point<float>(inf->mouse_x, inf->mouse_y);
			// posBmp = mousePosDesktop-mousePosBmp
			juce::Point<float> posBmp = mousePosDesktop - mousePosBmp;
			juce::Point<int> posBmpInt;
			//	posBmpInt.x = (int)posBmp.x;
			//  posBmpInt.y = (int)posBmp.y;
			//	posBmpInt.x = (int)0;
			//	posBmpInt.y = (int)0;

			int rim = 10;
			int lowerBorder_x = rim;
			int upperBorder_x = w - rim;
			int lowerBorder_y = rim;
			int upperBorder_y = h - rim;

			if ((lowerBorder_x < point.x && point.x < upperBorder_x) && (lowerBorder_y < point.y && point.y < upperBorder_y))
			{
				juce::Rectangle<int> rect = { (int)posBmp.x, (int)posBmp.y, w, h };
				//				embedComponent->setTopLeftPosition(posBmpInt);
				embedComponent->setBounds(rect);
				embedComponent->setVisible(true);
				embedComponent->addToDesktop(0);
				embedComponent->setAlwaysOnTop(true);
				Timer::startTimer(1000);
			}
			else
			{
				embedComponent->setVisible(false);
				embedComponent->removeFromDesktop();
				embedComponent->setAlwaysOnTop(false);
			}
		}
		return 1;

 /* draw embedded UI.
	* 
	* parm2: REAPER_FXEMBED_IBitmap * to draw into. note
	* parm3: REAPER_FXEMBED_DrawInfo *
	*
	* if flags has REAPER_FXEMBED_DRAWINFO_FLAG_PAINT_OPTIONAL set, update is optional. if no change since last draw, return 0.
	* if flags has REAPER_FXEMBED_DRAWINFO_FLAG_LBUTTON_CAPTURED set, left mouse button is down and captured
	* if flags has REAPER_FXEMBED_DRAWINFO_FLAG_RBUTTON_CAPTURED set, right mouse button is down and captured
	*
	* HiDPI:
	* if REAPER_FXEMBED_IBitmap::Extended(REAPER_FXEMBED_EXT_GET_ADVISORY_SCALING,NULL) returns nonzero, then it is a 24.8 scalefactor for UI drawing
	*
  * return 1 if drawing occurred, 0 otherwise.
	*/
	case REAPER_FXEMBED_WM_PAINT:
		if ((void*)parm2 && parm3)
		{
			auto inf = (REAPER_FXEMBED_DrawInfo*)parm3;
			//inf->context =  1=TCP, 2=MCP
			int inf_flags = (int)(INT_PTR)inf->flags;

			//if (inf_flags & 1)
			//	if (!embedded_updated)
			//		return 0;
			//embedded_updated = false;

			REAPER_FXEMBED_IBitmap* bmp = (REAPER_FXEMBED_IBitmap*)(void*)parm2;
			int w = bmp->getWidth();
			int h = bmp->getHeight();
			// bmp->resize(80, 90);

			//copy img->bmp;
			juce::Image img(juce::Image::PixelFormat::ARGB,
				bmp->getWidth(), bmp->getHeight(), true);

			juce::Graphics g(img);
			embedComponent->paintEntireComponent(g, false);

			int reaperBmpRowSpan = bmp->getRowSpan();
			unsigned int* reaperBmpBits = bmp->getBits();
			juce::Image::BitmapData juceBmpData{ img, juce::Image::BitmapData::readOnly };

			int ls = juceBmpData.lineStride;
			if (ls != (reaperBmpRowSpan * sizeof(unsigned int)))
			{
				uint8* lineStartPtr;
				w *= sizeof(unsigned int);
				for (int y = 0; y < h; y++)
				{
					lineStartPtr = juceBmpData.getLinePointer(y);
					for (int x = 0; x < w; x++)
					{
						memcpy(reaperBmpBits, lineStartPtr, w);
					}
					reaperBmpBits += reaperBmpRowSpan;
				}
			}
			else {
				uint8* lineStartPtr;
				lineStartPtr = juceBmpData.getLinePointer(0);
				memcpy(reaperBmpBits, lineStartPtr, ls * h);
			}
			return 1;
		}
		return 0;
	}
	return 0;
}


//==============================================================================

const juce::String AudioPluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName) {
  juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.
  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    auto *channelData = buffer.getWritePointer(channel);
    juce::ignoreUnused(channelData);
    // ..do something to the data...
  }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor() {
  return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new AudioPluginAudioProcessor();
}
