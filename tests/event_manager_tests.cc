#include <gtest/gtest.h>

#include "rosewood/core/event.h"

using namespace rosewood::core;

struct TestEvent : public Event<TestEvent> {
    int data;

    TestEvent(int data) : data(data) { }
};

struct Sender {

};

struct Listener {
    int n_received;
    Sender *last_sender;
    TestEvent last_event;

    int n_global_received;
    Sender *last_global_sender;
    TestEvent last_global_event;

    Listener()
    : n_received(0), last_sender(nullptr), last_event(0)
    , n_global_received(0), last_global_sender(nullptr), last_global_event(0) { }

    void did_receive_event(Sender *sender, const TestEvent *event) {
        ++n_received;
        last_sender = sender;
        last_event = *event;
    }

    void did_receive_global_event(Sender *sender, const TestEvent *event) {
        ++n_global_received;
        last_global_sender = sender;
        last_global_event = *event;
    }
};

class EventManagerTests : public ::testing::Test {
protected:
    EventManager<Sender*> _events;
};

TEST_F(EventManagerTests, ListenToSender) {
    Sender sender;
    Listener listener;

    _events.add_listener<TestEvent>(&sender, &listener, &Listener::did_receive_event);

    _events.send_event(&sender, TestEvent(3));

    ASSERT_EQ(1, listener.n_received);
    EXPECT_EQ(&sender, listener.last_sender);
    EXPECT_EQ(3, listener.last_event.data);

    EXPECT_EQ(0, listener.n_global_received);

    _events.remove_listener(&listener);

    _events.send_event(&sender, TestEvent(4));

    EXPECT_EQ(1, listener.n_received);
    EXPECT_EQ(0, listener.n_global_received);
}

TEST_F(EventManagerTests, ListenToGlobal) {
    Sender sender;
    Listener listener;

    _events.add_listener<TestEvent>(&listener, &Listener::did_receive_global_event);

    _events.send_event(&sender, TestEvent(3));

    ASSERT_EQ(1, listener.n_global_received);
    EXPECT_EQ(&sender, listener.last_global_sender);
    EXPECT_EQ(3, listener.last_global_event.data);

    EXPECT_EQ(0, listener.n_received);

    _events.remove_listener(&listener);

    _events.send_event(&sender, TestEvent(2));

    EXPECT_EQ(1, listener.n_global_received);
    EXPECT_EQ(0, listener.n_received);
}
