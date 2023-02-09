GenericUSBMIDI
==============

This is an OS X driver for MIDI devices connected to USB based off of one by the same name by Yves Salathe, which in turn was based off a very convoluted OOP monstrosity of sample Apple code. In any case, it works. The goal of this one is to update as necessary for modern OS X tool support and for addition of multiple devices rather than needing to compile a new driver for each USB MIDI device that needs support.

Currently supported are:
    * Roland PC-300
    * Roland SC-D70
    * Roland/EDIROL SD-90

After compiling the source with Xcode simply copy the resulting bundle GenericUSBMidi.plugin to the Folder /Library/Audio/MIDI Drivers/. This is most simply done with the following bash (Terminal) commands:

    cd $HOME/Library/Developer/Xcode/DerivedData/GenericUSBMidi*/Build/Products/Release
    sudo cp -r GenericUSBMidi.plugin /Library/Audio/MIDI\ Drivers/

Note: to find out where the "GenericUSBMidi.plugin" file has been created, right-click onto Products->GenericUSBMidi.plugin in Xcode and then click "Show in Finder".

After that it is necessary to reboot the MIDIServer process (or the entire OS). 

For debugging (e.g. using break points) attach your debugger to the MIDIServer process.
In Xcode this is done via the Menu Product -> Attach to Process -> MIDIServer.
