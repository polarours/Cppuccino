#include "actor_mailbox.hpp"

#include <iostream>
#include <thread>

int main() {
    struct Msg { int from; std::string body; };

    actor::Mailbox<Msg> mailbox(64);
    long totalLen = 0;
    mailbox.start([&](Msg& m) {
        totalLen += static_cast<long>(m.body.size());
        std::cout << "actor handled #" << m.from << "\n";
    });

    // send from multiple producer threads
    std::vector<std::thread> producers;
    for (int t = 0; t < 4; ++t) {
        producers.emplace_back([&mailbox, t] {
            for (int i = 0; i < 5; ++i)
                mailbox.send(Msg{t * 10 + i, "hello-world"});
        });
    }
    for (auto& p : producers) p.join();

    // wait for drain
    for (int i = 0; i < 100 && mailbox.pending() > 0; ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mailbox.stop();

    std::cout << "handled=" << mailbox.handled()
              << " totalLen=" << totalLen << " (expect 20 * 11 = 220)\n";
    return 0;
}
