#include <gtest/gtest.h>
#include <memory>
#include "pipelinefifo.h"
#include "../logger/logger.h"

using namespace event_forge;

TEST(PipeLineFifo, canGetAnInstance) {
    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();
    PipelineFiFo *p = dynamic_cast<PipelineFiFo*>(fifo.get());
    EXPECT_TRUE(p != nullptr);
}

