#include "event_dispatcher.h"
#include <gtest/gtest.h>

TEST(EventDispatcher, RegisterAndDispatch)
{
    EventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.registerHandler(0, [&]() { callCount++; });
    dispatcher.dispatch(0);
    EXPECT_EQ(callCount, 1);
}

TEST(EventDispatcher, DispatchUnregisteredButtonIsSafe)
{
    EventDispatcher dispatcher;
    EXPECT_NO_THROW(dispatcher.dispatch(2));
}

TEST(EventDispatcher, ClearHandlersStopsCallbacks)
{
    EventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.registerHandler(1, [&]() { callCount++; });
    dispatcher.clearHandlers();
    dispatcher.dispatch(1);
    EXPECT_EQ(callCount, 0);
}

TEST(EventDispatcher, DispatchOneButtonDoesNotFireAnother)
{
    EventDispatcher dispatcher;
    int count0 = 0, count1 = 0;
    dispatcher.registerHandler(0, [&]() { count0++; });
    dispatcher.registerHandler(1, [&]() { count1++; });
    dispatcher.dispatch(0);
    EXPECT_EQ(count0, 1);
    EXPECT_EQ(count1, 0);
}

TEST(EventDispatcher, SelectButtonIndex4IsValid)
{
    EventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.registerHandler(4, [&]() { callCount++; });
    dispatcher.dispatch(4);
    EXPECT_EQ(callCount, 1);
}

TEST(EventDispatcher, DispatchMultipleTimes)
{
    EventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.registerHandler(3, [&]() { callCount++; });
    dispatcher.dispatch(3);
    dispatcher.dispatch(3);
    EXPECT_EQ(callCount, 2);
}

// ---------------------------------------------------------------------------
// Release handler
// ---------------------------------------------------------------------------

TEST(EventDispatcher, RegisterReleaseAndDispatchRelease)
{
    EventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.registerReleaseHandler(0, [&]() { callCount++; });
    dispatcher.dispatchRelease(0);
    EXPECT_EQ(callCount, 1);
}

TEST(EventDispatcher, DispatchReleaseWithNoHandlerIsSafe)
{
    EventDispatcher dispatcher;
    EXPECT_NO_THROW(dispatcher.dispatchRelease(2));
}

TEST(EventDispatcher, DispatchReleaseDoesNotFirePressHandler)
{
    EventDispatcher dispatcher;
    int pressCount = 0, releaseCount = 0;
    dispatcher.registerHandler(1, [&]() { pressCount++; });
    dispatcher.registerReleaseHandler(1, [&]() { releaseCount++; });
    dispatcher.dispatchRelease(1);
    EXPECT_EQ(pressCount, 0);
    EXPECT_EQ(releaseCount, 1);
}

TEST(EventDispatcher, ClearHandlersClearsReleaseToo)
{
    EventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.registerReleaseHandler(0, [&]() { callCount++; });
    dispatcher.clearHandlers();
    dispatcher.dispatchRelease(0);
    EXPECT_EQ(callCount, 0);
}
