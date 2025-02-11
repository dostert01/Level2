/**
 * @file pipelineapi.h
 * @brief This file declares the three exported functions of a level2 worker module
 */
#ifndef lEVEL2_PIPELINE_API_H
#define lEVEL2_PIPELINE_API_H

#include "apistructs.h"
#include "payloadnames.h"

/** declare as extern "C" or not **/
#ifdef BEGIN_DECL
#undef BEGIN_DECL
#endif
#ifdef END_DECL
#undef END_DECL
#endif
#ifdef __cplusplus
#define BEGIN_DECL      extern "C" {
#define END_DECL        }
#else
#define BEGIN_DECL              /* none */
#define END_DECL                /* none */
#endif

BEGIN_DECL
/**
 * @brief initialization function of the module
 * 
 * Called once during the lifetime of the module directly after loading
 * 
 * @returns Returnvalue is currently not used. Usually return 0.
 */
int pipeline_step_module_init(level2::PipelineStepInitData& initData);
/**
 * @brief processing function of the module
 * 
 * Called once for any execution of the module in a pipeline
 * 
 * @returns Returnvalue is currently not used. Usually return 0.
 * Any errors, that might have occurred during processing should be returned
 * to the calling process by calling `level2::PipelineProcessingData::addError`.
 */
int pipeline_step_module_process(level2::PipelineProcessingData& processData);
/**
 * @brief finalization function of the module
 * 
 * Called once during the lifetime of the module directly before unloading.
 * This function should be used for cleaning up any resources, that might have
 * been reserved during `pipeline_step_module_init` or have been left over from
 * previous calls to `pipeline_step_module_process`.
 * 
 * @returns Returnvalue is currently not used. Usually return 0.
 */
int pipeline_step_module_finish();
END_DECL

#endif //lEVEL2_PIPELINE_API_H