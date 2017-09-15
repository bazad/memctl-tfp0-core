## memctl-tfp0-core

<!-- Brandon Azad -->

memctl-tfp0-core is a [memctl] core for iOS that tries to access the kernel task port using
`task_for_pid(0)` and `host_get_special_port()` for all special port numbers (although most
jailbreaks use port 4).

[memctl]: https://github.com/bazad/memctl

It has been tested with [mach_portal] on iOS 10.1.1 and [yalu102] on iOS 10.2.

[mach_portal]: https://bugs.chromium.org/p/project-zero/issues/detail?id=965
[yalu102]: https://github.com/kpwn/yalu102

### Building

To build memctl using this core:

	$ git clone https://github.com/bazad/memctl
	$ cd memctl
	$ git clone https://github.com/bazad/memctl-tfp0-core
	$ ln -s memctl-tfp0-core core
	$ cd memctl-tfp0-core
	$ make
	$ cd ..
	$ make

### Running

To run memctl, first copy it to the device you will analyze. On the device, run memctl with no
arguments to drop into a REPL.

	$ memctl
	memctl> vm _memcpy
	          START - END             [ VSIZE ] PRT/MAX SHRMOD DEPTH RESIDENT REFCNT TAG
	fffffff022000000-fffffff124000000 [  4.0G ] ---/---    NUL     0        0      0   0

### License

memctl-tfp0-core is released under the MIT license.
