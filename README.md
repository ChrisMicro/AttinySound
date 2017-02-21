# AttinySound
sound generation on the AVR-Attiny85

This synthesizer is based on the famous [8BitMixTape](https://github.com/8BitMixtape/8bitMixTape-SoundProg2085) 

<p align="left">
  <img src="/doc/NeoCycloSynth.PNG" width="480"/>
</p>


## AudioBootLoader
You can download very easily different synthizer apps when you have installed the AudioBootLoader on your Attiny85.

* AudioBootLoader for Attiny85: https://github.com/ChrisMicro/TinyAudioBoot

The AudioBootloader was orignially programmed for an Atmega8 or Atmega168. 
Special thanks to [Ioyk](https://github.com/ATtinyTeenageRiot) for reworking it for Attiny85.

Here is a simple schematic of how to connect a PC and an Attiny85 for programming it. 

<p align="left">
  <img src="/doc/audioInputSchematic.PNG" width="480"/>
</p>

There are precompiled wav files for the Attiny85 which you can download:

* precompiled wav files for audio boot: https://github.com/ChrisMicro/AttinySound/tree/master/wavForAudioBoot

The benefit of precompiled wav files: you don't need any programming software. 
You can just click on it and the sound played will programm your Attiny85. 


## schematic
This synth is mainly compatible to the 8BitMixTape-SoundProg2085
![](https://raw.githubusercontent.com/wiki/8BitMixtape/8bitMixTape-SoundProg2085/Schematics_85SoundProg_MixTape_V08.png)


## additional links

* the_synth for Attiny85: https://github.com/ChrisMicro/the_synth
* 100 Melodien Klingel: http://www.mikrocontroller.net/topic/120289
