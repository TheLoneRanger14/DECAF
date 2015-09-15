/*
 * vmi_callback.h
 *
 *  Created on: Dec 11, 2013
 *      Author: Xunchao hu
 */

#ifndef VMI_CALLBACK_H_
#define VMI_CALLBACK_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "DECAF_shared/DECAF_types.h"
typedef enum {
  VMI_CREATEPROC_CB = 0,
  VMI_REMOVEPROC_CB,
  VMI_LOADMODULE_CB,
  VMI_REMOVEMODULE_CB,
  VMI_LOADMAINMODULE_CB,
  DS_DALVIK_INSN_BEGIN_CB,
  DS_DALVIK_METHOD_BEGIN_CB,
  VMI_PROCESSBEGIN_CB = VMI_LOADMAINMODULE_CB, //alias
  VMI_LAST_CB, //place holder for the last position, no other uses.
} VMI_callback_type_t;

typedef struct _VMI_CreateProc_Params {
	  uint32_t pid;
	  uint32_t cr3;
	  char *name;
	  char *short_name;
}VMI_CreateProc_Params;

typedef VMI_CreateProc_Params VMI_RemoveProc_Params;

typedef struct _VMI_LoadModule_Params {
	  uint32_t pid;
	  uint32_t cr3;
	  char* name;
	  gva_t base;
	  uint32_t size;
	  char* full_name;
}VMI_LoadModule_Params;

typedef VMI_LoadModule_Params VMI_RemoveModule_Params;

typedef struct _DalvikInsnBegin_Params
{
  CPUState* env;
  gva_t dalvik_pc;
  uint32_t opcode; 
} DalvikInsnBegin_Params;

typedef struct _DalvikMethodBegin_Params
{
  CPUState* env;
  gva_t dalvik_pc;
} DalvikMethodBegin_Params;


typedef union _VMI_Callback_Params
{
  VMI_CreateProc_Params cp;
  VMI_RemoveProc_Params rp;
  VMI_LoadModule_Params lm;
  VMI_RemoveModule_Params rm;

  DalvikInsnBegin_Params ib;
  DalvikMethodBegin_Params mb;
  //LoadMainModule_Params lmm;
} VMI_Callback_Params;


typedef void (*VMI_callback_func_t) (VMI_Callback_Params* params);

DECAF_Handle VMI_register_callback(
                VMI_callback_type_t cb_type,
                VMI_callback_func_t cb_func,
                int *cb_cond
                );

DECAF_Handle Dalvik_VMI_register_callback(
                VMI_callback_type_t cb_type,
                VMI_callback_func_t cb_func,
                int *cb_cond
                );

int Dalvik_VMI_unregister_callback(VMI_callback_type_t cb_type, DECAF_Handle handle);

int VMI_unregister_callback(VMI_callback_type_t cb_type, DECAF_Handle handle);

#ifdef __cplusplus
}
#endif
#endif /* VMI_CALLBACK_H_ */
