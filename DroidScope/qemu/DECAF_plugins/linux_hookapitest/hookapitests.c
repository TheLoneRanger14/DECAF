/*
Copyright (C) <2012> <Syracuse System Security (Sycure) Lab>
This is a plugin of DECAF. You can redistribute and modify it
under the terms of BSD license but it is made available
WITHOUT ANY WARRANTY. See the top-level COPYING file for more details.

For more information about DECAF and other softwares, see our
web site at:
http://sycurelab.ecs.syr.edu/

If you have any questions about DECAF,please post it on
http://code.google.com/p/decaf-platform/
*/
/**
 * @author Abhishek VB
 * @date June 22 2015
 */

#include <sys/time.h>

#include "DECAF_types.h"
#include "DECAF_main.h"
#include "DECAF_callback.h"
#include "DECAF_shared/vmi_callback.h"
#include "vmi_c_wrapper.h"

#define DECAF_printf(...) monitor_printf(default_mon, __VA_ARGS__)

static plugin_interface_t hookapitests_interface;
static DECAF_Handle modulebegin_handle = DECAF_NULL_HANDLE;
static DECAF_Handle processbegin_handle = DECAF_NULL_HANDLE;

extern void DECAF_output_init(Monitor* mon);

static char targetname[512];
static uint32_t targetpid = -1;
static uint32_t targetcr3 = 0;
static int monitoring = 1;
static tmodinfo_t module_info;

static void handle_module_load(VMI_Callback_Params* params)
{
	if(params->lm.cr3 != targetcr3) return; 
	
	monitor_printf(default_mon,"module name=%s loaded, base address-%016llX, size is 0x%08x \n",params->lm.name,params->lm.base,params->lm.size);
}

static void register_hooks()
{
	//modulebegin_handle=VMI_register_callback(VMI_LOADMODULE_CB,handle_module_load,&monitoring);
}

static void createproc_callback(VMI_Callback_Params* params)
{
    if(targetcr3 != 0) //if we have found the process, return immediately
    	return;

    monitor_printf(default_mon, "process %s started!\n", params->cp.name);

	if (strlen(targetname) > 1 && strstr(params->cp.name, targetname) != 0) {
		targetpid = params->cp.pid;
		targetcr3 = params->cp.cr3;
		register_hooks();
		DECAF_printf("Process found: pid=%08x, cr3=%08x, name = %s\n", targetpid, targetcr3, params->cp.name);
	}
}

static void do_hookapitests(Monitor* mon, const char* proc_name)
{
	if(strlen(proc_name) < 512)
		strncpy(targetname, proc_name, strlen(proc_name));
	else
		return;

	targetname[511] = '\0';
}


static int hookapitests_init(void)
{
	//DECAF_output_init(NULL);
	targetname[0] = '\0';

	//register for process create and process remove events
	processbegin_handle = VMI_register_callback(VMI_CREATEPROC_CB,
			&createproc_callback, NULL);
	
	
	return (0);
}

static void hookapitests_cleanup(void)
{

	if (processbegin_handle != DECAF_NULL_HANDLE) {
		VMI_unregister_callback(VMI_CREATEPROC_CB,
				processbegin_handle);
		processbegin_handle = DECAF_NULL_HANDLE;
	}

	if(modulebegin_handle != DECAF_NULL_HANDLE)
		VMI_unregister_callback(VMI_LOADMODULE_CB,modulebegin_handle);

}

static mon_cmd_t hookapitests_term_cmds[] = {
#include "plugin_cmds.h"
		{ NULL, NULL, }, };

plugin_interface_t* init_plugin(void) {
	hookapitests_interface.mon_cmds = hookapitests_term_cmds;
	hookapitests_interface.plugin_cleanup = &hookapitests_cleanup;

	//initialize the plugin
	hookapitests_init();
	return (&hookapitests_interface);
}

