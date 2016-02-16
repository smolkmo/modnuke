Do you run Linux and do you want to see the world burn?

## modnuke

modnuke is a Linux kernel module that instantly and without question removes any process from existence by overwriting all of its allocated memory (code and data) with zeros.

Since it runs in kernel space, be assured it really doesn't have to ask twice.

It was created because the author did not accept "You should never try to kill init" as an answer for their question "Why cant I kill init with the kill command?".

Consider using modnuke if you think that:

* The "kill" command is for softies
* Your system is too stable
* init (and possibly your whole system) must die

WARNING: Using this is pretty much equal to poking a totally pissed bear with a stick. I do not understand the Linux kernel enough to tell you if/what things may go wrong. It will most likely cause bad things to happen, with data loss being the least of your worries. I advise against using this on a real system and cant be held accountable for the damage you inflict with modnuke. 

## Install

```make all```

## Usage
After loading the kernel module (if you do not know how to do that, then you may want to rethink if you really want to try this), you can obliterate processes in the shell with:

```echo PID > /proc/modnuke```

With PID corresponding to the PID of the unlucky bastard.

## Credits
This module was proudly inspired by the Magic the Gathering card Path to Exile.

## License
MIT