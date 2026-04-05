#include "unity_fixture.h"

TEST_GROUP_RUNNER(Pipeline)
{
    RUN_TEST_CASE(Pipeline, InitNullContextReturnsNullInput);
    RUN_TEST_CASE(Pipeline, InitReturnsOkWithValidContext);
    RUN_TEST_CASE(Pipeline, InitSetsClientHandle);
    RUN_TEST_CASE(Pipeline, InitConnectCalledOnce);
    RUN_TEST_CASE(Pipeline, CleanupNullContextDoesNotCrash);
    RUN_TEST_CASE(Pipeline, CleanupDisconnectCalledOnce);
    RUN_TEST_CASE(Pipeline, CleanupNullsClientHandle);
    RUN_TEST_CASE(Pipeline, RunNullContextReturnsNullInput);
    RUN_TEST_CASE(Pipeline, RunPublishesForAllGridPoints);
    RUN_TEST_CASE(Pipeline, RunFetchCalledForAllGridPoints);
    RUN_TEST_CASE(Pipeline, RunWithForbiddenSkipsPublish);
    RUN_TEST_CASE(Pipeline, RunWithTimeoutRetriesAndSkipsPublish);
}