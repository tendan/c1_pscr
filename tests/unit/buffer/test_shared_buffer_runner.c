#include <unity_fixture.h>

TEST_GROUP_RUNNER(SharedBuffer)
{
    RUN_TEST_CASE(SharedBuffer, InitNullReturnsNullInput);
    RUN_TEST_CASE(SharedBuffer, InitReturnsOk);
    RUN_TEST_CASE(SharedBuffer, ProduceNullBufferReturnsNullInput);
    RUN_TEST_CASE(SharedBuffer, ProduceNullDataReturnsNullInput);
    RUN_TEST_CASE(SharedBuffer, ConsumeNullBufferReturnsNullInput);
    RUN_TEST_CASE(SharedBuffer, ConsumeNullOutputReturnsNullInput);
    RUN_TEST_CASE(SharedBuffer, ProduceReturnsOk);
    RUN_TEST_CASE(SharedBuffer, ConsumeAfterProduceReturnsOk);
    RUN_TEST_CASE(SharedBuffer, ConsumedDataMatchesProducedData);
    RUN_TEST_CASE(SharedBuffer, ConsumeWithoutProduceReturnsTimeout);
    RUN_TEST_CASE(SharedBuffer, ProducerConsumerInSeparateThreads);
    RUN_TEST_CASE(SharedBuffer, ProducerBlocksUntilConsumed);
}