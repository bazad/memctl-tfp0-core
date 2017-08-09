#include "memctl/core.h"
#include "memctl/memctl_error.h"

#include <assert.h>
#include <mach/host_priv.h>

/*
 * found_kernel_port
 *
 * Description:
 * 	Test if the kernel task port has been successfully retrieved.
 */
static bool found_kernel_port() {
	if (kernel_task == MACH_PORT_NULL) {
		return false;
	}
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
 * try_host_special_ports
 *
 * Description:
 * 	Try to obtain the kernel task port via any of the host special ports.
 */
static bool try_host_special_ports() {
	mach_port_t host = mach_host_self();
	if (host == MACH_PORT_NULL) {
		return false;
	}
	for (int port_id = 0; port_id <= HOST_MAX_SPECIAL_PORT; port_id++) {
		host_get_special_port(host, 0, port_id, &kernel_task);
		if (found_kernel_port()) {
			break;
		}
	}
	mach_port_deallocate(mach_task_self(), host);
	return (kernel_task != MACH_PORT_NULL);
}

/*
 * try_task_for_pid_0
 *
 * Description:
 * 	Try to obtain the kernel task port via task_for_pid.
 */
static bool try_task_for_pid_0() {
	task_for_pid(mach_task_self(), 0, &kernel_task);
	return found_kernel_port();
}

bool core_load() {
	if (try_host_special_ports() || try_task_for_pid_0()) {
		return true;
	}
	error_core("could not obtain kernel task port via host_get_special_port() or task_for_pid(0)");
	return false;
}
