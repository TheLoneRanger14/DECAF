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
static DECAF_Handle insn_cb_handle = DECAF_NULL_HANDLE;

static int tracing = 0;

extern void DECAF_output_init(Monitor* mon);

static char targetname[512];
static uint32_t targetpid = -1;
static uint32_t targetcr3 = 0;
static int monitoring = 1;
static tmodinfo_t module_info;

static void hook_all(DECAF_Callback_Params* param) {

	char modname[512];
	char functionname[512];

	CPUState* env = param->ib.env;
 	
 	if(DECAF_getPGD(env) != targetcr3) return; 

 	target_ulong base;

 	VMI_locate_module_c(DECAF_getPC(env) /* gva_t eip*/,DECAF_getPGD(env) /* gva_t cr3 */, targetname  /* char proc[] */,&module_info /* tmodinfo_t* */);

 	if(strstr(module_info.name, "libdvm.so") == NULL)
 		return;
 
	if(funcmap_get_name_c(DECAF_getPC(env), DECAF_getPGD(env), modname, functionname) == 0)
		monitor_printf(default_mon, "[FUNCTION CALL] [Module:] %s [Function:] %s\n", modname, functionname);
}

static void handle_module_load(VMI_Callback_Params* params)
{
	if(params->lm.cr3 != targetcr3) return; 
	
	monitor_printf(default_mon,"module name=%s loaded, base address-%016llX, size is 0x%08x \n",params->lm.name,params->lm.base,params->lm.size);
}

static void register_hooks()
{
	//modulebegin_handle = VMI_register_callback(VMI_LOADMODULE_CB,handle_module_load,&monitoring);
	insn_cb_handle = DECAF_register_callback(DECAF_INSN_BEGIN_CB, hook_all, NULL);
}

static void createproc_callback(VMI_Callback_Params* params)
{
    if(targetcr3 != 0) //if we have found the process, return immediately
    	return;

	if (tracing) { // && strstr(targetname, params->cp.short_name) != NULL) {
		targetpid = params->cp.pid;
		targetcr3 = params->cp.cr3;
		register_hooks();
		DECAF_printf("Process found: pid=%08x, cr3=%08x\n", targetpid, targetcr3);
		tracing = 0;
	}
}

static void do_hookapitests(Monitor* mon, const QDict* qdict)
{
	/*
	const char *procname;
	if (qdict != NULL) {

		procname = qdict_get_str(qdict, "proc_name");
		strncpy(targetname, procname, 511);
	}
	targetname[511] = '\0'; */
	//DECAF_printf("waiting for process %s to start \n", targetname);

	tracing = 1;
}


static int hookapitests_init(void)
{
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

	if(insn_cb_handle != DECAF_NULL_HANDLE)
 		DECAF_unregister_callback(DECAF_INSN_BEGIN_CB, insn_cb_handle);


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

