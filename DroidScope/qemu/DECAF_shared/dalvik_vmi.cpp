/*
    Copyright (C) <2012> <Syracuse System Security (Sycure) Lab>

    DECAF is based on QEMU, a whole-system emulator. You can redistribute
    and modify it under the terms of the GNU GPL, version 3 or later,
    but it is made available WITHOUT ANY WARRANTY. See the top-level
    README file for more details.

    For more information about DECAF and other softwares, see our
    web site at:
    http://sycurelab.ecs.syr.edu/

    If you have any questions about DECAF,please post it on
    http://code.google.com/p/decaf-platform/
*/
/*
* dalvik_vmi.cpp
*
*  Created on: September 15, 2015
* 	Author : Abhishek V B
*/

#include <inttypes.h>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <queue>
#include <sys/time.h>
#include <math.h>
#include <mcheck.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "cpu.h"
#include "config.h"
#include "hw/hw.h" // AWH
#include "qemu-timer.h"
#ifdef __cplusplus
};
#endif /* __cplusplus */

#include "DECAF_shared/DECAF_main.h"
#include "DECAF_shared/vmi.h"
#include "DECAF_shared/linux_vmi_.h"
#include "DECAF_shared/linux_procinfo.h"
#include "DECAF_shared/utils/SimpleCallback.h"
#include "DECAF_shared/DECAF_callback.h"

#include "DECAF_shared/vmi_callback.h"
#include "DECAF_shared/vmi_c_wrapper.h"
#include "DECAF_shared/function_map.h"


#define pgd_strip(_pgd) (_pgd & ~0xC0000FFF)

using namespace std;
using namespace std::tr1;

#define BREAK_IF(x) if(x) break

//Global variable used to read values from the stack
int monitored = 0;

static DECAF_Handle method_begin_cb_handle = DECAF_NULL_HANDLE;
static target_ulong libdvm_call_method_addr = 0x00;
static process *system_server_process = nullptr;

void update_system_server_ptr(process *incoming)
{
    system_server_process = incoming;
}

static void method_begin_cb(Decaf_Callback_Params* params)
{

}

DECAF_Handle Dalvik_VMI_register_callback(VMI_callback_type_t cb_type, VMI_callback_func_t cb_func, int *cb_cond) 
{

    switch(VMI_callback_type_t)
    {
        case DS_DALVIK_METHOD_BEGIN_CB :
            if(method_begin_cb_handle == DECAF_NULL_HANDLE)
            {
                libdvm_call_method_addr = funcmap_get_pc("libdvm.so","dvmCallMethod(Thread*, Method const*, Object*, JValue*, ...)", pe->cr3);
            }

    }
}