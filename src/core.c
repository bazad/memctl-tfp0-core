#include "memctl/core.h"
#include "memctl/memctl_error.h"

#include <assert.h>
#include <mach/host_priv.h>

/*
 * found_kernel_port
 *
 * Description:
 * 	Test if the kernel port has been successfully retrieved.
 */
static bool found_kernel_port() {
	pid_t pid;
	kern_return_t kr = pid_for_task(kernel_task, &pid);
	if (kr != KERN_SUCCESS || pid != 0) {
		mach_port_deallocate(mach_task_self(), kernel_task);
		kernel_task = MACH_PORT_NULL;
		return false;
	}
	return true;
}

/*
 * try_host_special_port_4
 *
 * Description:
 * 	Try to obtain the kernel task port via host special port 4.
 */
static bool try_host_special_port_4() {
	mach_port_t host = mach_host_self();
	if (host == MACH_PORT_NULL) {
		return false;
	}
	kern_return_t kr = host_get_special_port(host, 0, 4, &kernel_task);
	mach_port_deallocate(mach_task_self(), host);
	if (kr != KERN_SUCCESS || !found_kernel_port()) {
		return false;
	}
	assert(kernel_task != 0);
	return true;
}

/*
 * try_task_for_pid_0
 *
 * Description:
 * 	Try to obtain the kernel task port via task_for_pid.
 */
static bool try_task_for_pid_0() {
	kern_return_t kr = task_for_pid(mach_task_self(), 0, &kernel_task);
	if (kr != KERN_SUCCESS || !found_kernel_port()) {
		return false;
	}
	assert(kernel_task != 0);
	return true;
}

bool core_load() {
	if (try_host_special_port_4() || try_task_for_pid_0()) {
		return true;
	}
	error_core("could not obtain kernel task port via host_get_special_port(4) or task_for_pid(0)");
	return false;
}
