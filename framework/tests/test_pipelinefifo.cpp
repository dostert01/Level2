#include <gtest/gtest.h>
#include <memory>
#include "pipelinefifo.h"
#include "../logger/logger.h"

using namespace std;
using namespace event_forge;

TEST(PipeLineFifo, canGetAnInstance) {
    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();
    PipelineFiFo *p = dynamic_cast<PipelineFiFo*>(fifo.get());
    EXPECT_TRUE(p != nullptr);
}

TEST(PipeLineFifo, canQueueAndDequeueSomeData) {
    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();
    fifo->enqueue("testData", "text/plain", "hello world 01");
    fifo->enqueue("testData", "text/plain", "hello world 02");

    shared_ptr<PipelineFiFo> p = dynamic_pointer_cast<PipelineFiFo>(fifo);

    optional<shared_ptr<PipelineProcessingData>> outData = p->dequeue();
    EXPECT_EQ("hello world 01", outData.value()->getPayload("testData").value()->payloadAsString());
    outData = p->dequeue();
    EXPECT_EQ("hello world 02", outData.value()->getPayload("testData").value()->payloadAsString());
    outData = p->dequeue();
    EXPECT_EQ(nullopt, outData);
}

