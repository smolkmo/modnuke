Do you run Linux, and do you want to see the world burn?

## modnuke

modnuke is a Linux kernel module that can destroy processes by overwriting their memory with zeros.

It is, by the very definition, to a process as the death star is to a planet.

Since it runs in kernel space, it does not have to ask for permission or check for sanity first. It just fires at whatever you point it at.

It can also kill the process with PID 1.

Consider using modnuke if you think that:

* The "kill" command is too soft
* SIGNALs are redundant
* Your system is too stable

WARNING: Using this is pretty much equal to poking a pissed bear with a stick. I do not understand the Linux kernel enough to tell you if/what things may go wrong! It will most likely cause bad things to happen, with data loss being the least of your worries. I advise against using this on a real system and cant be held accountable for the damage you inflict with modnuke. 

## Install

```make all```

## Usage
After loading the kernel module (if you do not know how to do that, then you may want to rethink if you really want to try this), you can obliterate processes in the shell with:

```echo PID > /proc/modnuke```

With PID corresponding to the PID of the unlucky bastard.

You need to have root priviledges to write to /proc/modnuke, so consider using:

```sudo bash -c "echo PID > /proc/modnuke"```

## License
GPL
