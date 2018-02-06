# memctl-tfp0-core

<!-- Brandon Azad -->

memctl-tfp0-core is a [memctl] core for iOS that tries to access the kernel task port using
`task_for_pid(0)` and `host_get_special_port()` for all special port numbers (although most
jailbreaks use port 4).

[memctl]: https://github.com/bazad/memctl

It has been tested with [Electra] on iOS 11.1.2 and [yalu102] on iOS 10.2.

[Electra]: https://github.com/coolstar/electra
[yalu102]: https://github.com/kpwn/yalu102

## Signing and entitlements

Some jailbreaks need binaries to be signed with special entitlements for them to run properly. Be
sure to add any such entitlements to `entitlements.plist` before building memctl.

## Building

To build memctl using this core:

	$ git clone https://github.com/bazad/memctl
	$ cd memctl
	$ git clone https://github.com/bazad/memctl-tfp0-core
	$ ln -s memctl-tfp0-core core
	$ cd memctl-tfp0-core
	$ make
	$ cd ..
	$ make

## Running

To run memctl, first copy it to the device you will analyze. On the device, run memctl with no
arguments to drop into a REPL.

	$ memctl
	memctl> vm _memcpy
	          START - END             [ VSIZE ] PRT/MAX SHRMOD DEPTH RESIDENT REFCNT TAG
	fffffff022000000-fffffff124000000 [  4.0G ] ---/---    NUL     0        0      0   0

## License

memctl-tfp0-core is released under the MIT license.
