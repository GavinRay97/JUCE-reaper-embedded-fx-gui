DIFF_FLAGS =

diff_juce_patches:
	cd JUCE
	git diff $(DIFF_FLAGS) origin/develop -- modules/juce_audio_plugin_client/VST3/juce_VST3_Wrapper.cpp
	git diff $(DIFF_FLAGS) origin/develop -- modules/juce_audio_processors/processors/juce_AudioProcessor.h

copy_juce_patches:
  cp  ./JUCE/modules/juce_audio_plugin_client/VST3/juce_VST3_Wrapper.cpp ./patches/JUCE
	cp  ./JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.h ./patches/JUCE
