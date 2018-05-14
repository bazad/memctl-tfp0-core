#include "memctl/core.h"
#include "memctl/memctl_error.h"

#include <assert.h>
#include <mach/host_priv.h>
#include <stdio.h>

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

/*
 * try_processor_set_tasks
 *
 * Description:
 * 	Try to obtain the kernel task port via processor_set_tasks().
 */
static bool try_processor_set_tasks() {
	bool success = false;
	mach_port_t host = mach_host_self();
	if (host == MACH_PORT_NULL) {
		goto fail_0;
	}
	processor_set_name_t pset_name;
	kern_return_t kr = processor_set_default(host, &pset_name);
	if (kr != KERN_SUCCESS) {
		goto fail_1;
	}
	processor_set_t pset;
	kr = host_processor_set_priv(host, pset_name, &pset);
	if (kr != KERN_SUCCESS) {
		goto fail_2;
	}
	task_array_t tasks;
	mach_msg_type_number_t task_count;
	kr = processor_set_tasks(pset, &tasks, &task_count);
	if (kr != KERN_SUCCESS) {
		goto fail_3;
	}
	for (size_t i = 1; i < task_count; i++) {
		mach_port_deallocate(mach_task_self(), tasks[i]);
	}
	kernel_task = tasks[0];
	success = found_kernel_port();
	vm_deallocate(mach_task_self(), (vm_address_t)tasks, task_count * sizeof(*tasks));
fail_3:
	mach_port_deallocate(mach_task_self(), pset);
fail_2:
	mach_port_deallocate(mach_task_self(), pset_name);
fail_1:
	mach_port_deallocate(mach_task_self(), host);
fail_0:
	return success;
}

static size_t format_core_error(char *buffer, size_t size, error_handle error) {
	assert(error->size > 0);
	int len = snprintf(buffer, size, "%s", (char *) error->data);
	return (len > 0 ? len : 0);
}

struct error_type core_error = {
	.static_description = "core error",
	.destroy_error_data = NULL,
	.format_description = format_core_error,
};

static void error_core(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	error_push_printf(&core_error, fmt, ap);
	va_end(ap);
}

bool core_load() {
	if (try_task_for_pid_0()
			|| try_host_special_ports()
			|| try_processor_set_tasks()) {
		return true;
	}
	error_core("could not obtain kernel task port via "
			"task_for_pid(0), host_get_special_port() or processor_set_tasks()");
	return false;
}
