#ifndef SECOND_TAKE_PIPELINE_API_H
#define SECOND_TAKE_PIPELINE_API_H

#include "apistructs.h"

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
int pipeline_step_module_init(PipelineStepInitData& initData);
int pipeline_step_module_process(PipelineProcessingData& processData);
void processTheProcessingData(PipelineProcessingData & processData);
void processArgumentsFromJson();
int pipeline_step_module_finish();
END_DECL

#endif //SECOND_TAKE_PIPELINE_API_H