#include "pubsub.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <atomic>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_publish_subscribe() {
    pubsub::Publisher<std::string> pub;

    std::string received;
    pub.subscribe("topic1", [&received](const std::string& msg) {
        received = msg;
    });

    pub.publish("topic1", "hello");
    expect(received == "hello", "expected hello");
}

void test_multiple_subscribers() {
    pubsub::Publisher<std::string> pub;

    int count = 0;
    pub.subscribe("topic1", [&count](const std::string&) { count++; });
    pub.subscribe("topic1", [&count](const std::string&) { count++; });
    pub.subscribe("topic1", [&count](const std::string&) { count++; });

    pub.publish("topic1", "msg");
    expect(count == 3, "expected 3 subscribers to receive");
}

void test_multiple_topics() {
    pubsub::Publisher<std::string> pub;

    std::string topic1Msg, topic2Msg;
    pub.subscribe("topic1", [&topic1Msg](const std::string& msg) { topic1Msg = msg; });
    pub.subscribe("topic2", [&topic2Msg](const std::string& msg) { topic2Msg = msg; });

    pub.publish("topic1", "hello");
    pub.publish("topic2", "world");

    expect(topic1Msg == "hello", "expected hello on topic1");
    expect(topic2Msg == "world", "expected world on topic2");
    expect(topic1Msg != "world", "expected topic1 not affected by topic2");
}

void test_unsubscribe() {
    pubsub::Publisher<std::string> pub;

    int count = 0;
    int id = pub.subscribe("topic1", [&count](const std::string&) { count++; });

    pub.publish("topic1", "msg");
    expect(count == 1, "expected 1");

    pub.unsubscribe(id);
    pub.publish("topic1", "msg");
    expect(count == 1, "expected still 1 after unsubscribe");
}

void test_subscriber_count() {
    pubsub::Publisher<std::string> pub;

    expect(pub.subscriberCount("topic1") == 0, "expected 0 subscribers");

    int id1 = pub.subscribe("topic1", [](const std::string&) {});
    int id2 = pub.subscribe("topic1", [](const std::string&) {});

    expect(pub.subscriberCount("topic1") == 2, "expected 2 subscribers");
    expect(pub.totalSubscribers() == 2, "expected 2 total subscribers");

    pub.unsubscribe(id1);
    expect(pub.subscriberCount("topic1") == 1, "expected 1 subscriber");
}

void test_no_subscribers() {
    pubsub::Publisher<std::string> pub;

    // Should not crash
    pub.publish("nonexistent", "msg");
    expect(pub.subscriberCount("nonexistent") == 0, "expected 0");
}

void test_message_order() {
    pubsub::Publisher<int> pub;

    std::vector<int> received;
    pub.subscribe("topic", [&received](const int& msg) {
        received.push_back(msg);
    });

    pub.publish("topic", 1);
    pub.publish("topic", 2);
    pub.publish("topic", 3);

    expect(received.size() == 3, "expected 3 messages");
    expect(received[0] == 1, "expected 1");
    expect(received[1] == 2, "expected 2");
    expect(received[2] == 3, "expected 3");
}

} // namespace

int main() {
    try {
        std::cout << "Publish-Subscribe Tests:\n";
        test_basic_publish_subscribe();
        std::cout << "  test_basic_publish_subscribe: PASS\n";

        test_multiple_subscribers();
        std::cout << "  test_multiple_subscribers: PASS\n";

        test_multiple_topics();
        std::cout << "  test_multiple_topics: PASS\n";

        test_unsubscribe();
        std::cout << "  test_unsubscribe: PASS\n";

        test_subscriber_count();
        std::cout << "  test_subscriber_count: PASS\n";

        test_no_subscribers();
        std::cout << "  test_no_subscribers: PASS\n";

        test_message_order();
        std::cout << "  test_message_order: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "pubsub_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\npubsub_tests passed" << std::endl;
    return 0;
}
