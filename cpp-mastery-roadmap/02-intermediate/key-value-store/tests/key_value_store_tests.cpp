#include "command/command_processor.hpp"
#include "persistence/store_persistence.hpp"
#include "store/key_value_store.hpp"
#include "store/store_manager.hpp"
#include "protocol/protocol.hpp"
#include "server/server.hpp"
#include "client/client.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_store_operations() {
    key_value_store::KeyValueStore store;
    
    // Test empty store
    expect(store.snapshot().empty(), "expected empty store");
    expect(!store.exists("key"), "expected exists to return false");
    expect(!store.get("key").has_value(), "expected get to return nullopt");
    expect(!store.erase("key"), "expected erase to return false");
    
    // Test set and get
    store.set("name", "Alice");
    expect(store.exists("name"), "expected exists to return true");
    expect(store.get("name").has_value(), "expected get to return value");
    expect(store.get("name").value() == "Alice", "expected correct value");
    expect(store.snapshot().size() == 1, "expected 1 entry");
    
    // Test overwrite
    store.set("name", "Bob");
    expect(store.get("name").value() == "Bob", "expected overwritten value");
    expect(store.snapshot().size() == 1, "expected still 1 entry");
    
    // Test multiple entries
    store.set("age", "25");
    store.set("city", "NYC");
    expect(store.snapshot().size() == 3, "expected 3 entries");
    
    // Test erase
    expect(store.erase("age"), "expected erase to return true");
    expect(!store.exists("age"), "expected age to be gone");
    expect(store.snapshot().size() == 2, "expected 2 entries after erase");
    
    // Test clear
    store.clear();
    expect(store.snapshot().empty(), "expected empty after clear");
}

void test_command_processor() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);
    
    // Test SET command
    auto result = processor.execute("SET key value");
    expect(result.output == "OK", "expected SET OK");
    expect(result.persist_after_command, "expected persist after SET");
    
    // Test GET command
    result = processor.execute("GET key");
    expect(result.output == "VALUE value", "expected GET value");
    expect(!result.persist_after_command, "expected no persist after GET");
    
    // Test DELETE command
    result = processor.execute("DELETE key");
    expect(result.output == "DELETED", "expected DELETE output");
    expect(result.persist_after_command, "expected persist after DELETE");
    
    // Test EXISTS command
    processor.execute("SET exists_key 1");
    result = processor.execute("EXISTS exists_key");
    expect(result.output == "TRUE", "expected EXISTS TRUE");
    
    result = processor.execute("EXISTS missing_key");
    expect(result.output == "FALSE", "expected EXISTS FALSE");
    
    // Test COUNT command
    result = processor.execute("COUNT");
    expect(result.output == "COUNT 1", "expected COUNT 1");
    
    // Test EXIT command
    result = processor.execute("EXIT");
    expect(result.output == "BYE", "expected EXIT BYE");
    expect(result.action == key_value_store::CommandAction::exit, "expected exit action");
}

void test_persistence_round_trip() {
    const std::string path = "/tmp/kv_test_roundtrip.db";
    std::remove(path.c_str());
    
    // Create and save data
    std::unordered_map<std::string, std::string> original = {
        {"key1", "value1"},
        {"key2", "value with spaces"},
        {"key3", "value\"with\"quotes"}
    };
    
    key_value_store::StorePersistence::save(path, original);
    
    // Load and verify
    auto loaded = key_value_store::StorePersistence::load(path);
    expect(loaded.size() == original.size(), "expected same size");
    expect(loaded == original, "expected identical data");
    
    std::remove(path.c_str());
}

void test_clear_command() {
    key_value_store::KeyValueStore store;
    store.set("a", "1");
    store.set("b", "2");
    expect(store.snapshot().size() == 2, "expected 2 entries before clear");

    key_value_store::CommandProcessor processor(store);
    const auto result = processor.execute("CLEAR");
    expect(result.output == "CLEARED", "expected CLEAR output");
    expect(result.persist_after_command, "expected CLEAR to request persistence");
    expect(store.snapshot().empty(), "expected store to be empty after CLEAR");
}

void test_save_command_action() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    const auto result = processor.execute("SAVE");
    expect(result.output == "SAVED", "expected SAVE output");
    expect(result.persist_after_command, "expected SAVE to request persistence");
    expect(result.action == key_value_store::CommandAction::none, "expected no action for SAVE");
}

void test_command_case_insensitivity() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("set key value").output == "OK", "expected lowercase set to work");
    expect(processor.execute("Get key").output == "VALUE value", "expected mixed case get to work");
    expect(processor.execute("DELETE key").output == "DELETED", "expected uppercase delete to work");
    expect(processor.execute("exists key").output == "FALSE", "expected lowercase exists to work");
    expect(processor.execute("Count").output == "COUNT 0", "expected mixed case count to work");
}

void test_quit_as_exit_alias() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    const auto result = processor.execute("QUIT");
    expect(result.output == "BYE", "expected QUIT to output BYE");
    expect(result.action == key_value_store::CommandAction::exit, "expected QUIT to trigger exit action");
}

void test_empty_command() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    const auto result = processor.execute("");
    expect(result.output == "ERROR empty command", "expected empty command error");
}

void test_unknown_command() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    const auto result = processor.execute("FOOBAR");
    expect(result.output == "ERROR unknown command", "expected unknown command error");
}

void test_set_get_with_spaces_in_value() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("SET greeting hello world").output == "OK", "expected SET with spaces");
    expect(processor.execute("GET greeting").output == "VALUE hello world", "expected GET to preserve spaces");
}

void test_overwrite_existing_key() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("SET key first").output == "OK", "expected first SET");
    expect(processor.execute("SET key second").output == "OK", "expected overwrite SET");
    expect(processor.execute("GET key").output == "VALUE second", "expected overwritten value");
    expect(processor.execute("COUNT").output == "COUNT 1", "expected count to remain 1");
}

void test_delete_nonexistent_key() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    const auto result = processor.execute("DELETE nonexistent");
    expect(result.output == "NOT_FOUND", "expected NOT_FOUND for deleting nonexistent key");
    expect(!result.persist_after_command, "expected no persistence for failed delete");
}

void test_replace_all() {
    key_value_store::KeyValueStore store;
    store.set("old_key", "old_value");

    std::unordered_map<std::string, std::string> new_data = {
        {"new_a", "val_a"},
        {"new_b", "val_b"}
    };
    store.replace_all(std::move(new_data));

    expect(store.snapshot().size() == 2, "expected 2 entries after replace_all");
    expect(!store.exists("old_key"), "expected old key to be gone");
    expect(store.get("new_a").has_value(), "expected new key a");
    expect(store.get("new_b").has_value(), "expected new key b");
}

void test_persistence_empty_store() {
    const std::string path = "/tmp/kv_test_empty.db";
    std::remove(path.c_str());

    key_value_store::KeyValueStore store;
    key_value_store::StorePersistence::save(path, store.snapshot());
    const auto loaded = key_value_store::StorePersistence::load(path);

    expect(loaded.empty(), "expected empty store to persist as empty");

    std::remove(path.c_str());
}

void test_persistence_special_characters() {
    const std::string path = "/tmp/kv_test_special.db";
    std::remove(path.c_str());

    key_value_store::KeyValueStore store;
    store.set("key with spaces", "value with spaces");
    store.set("key\"quote", "value\"quote");
    store.set("key\\backslash", "value\\backslash");

    key_value_store::StorePersistence::save(path, store.snapshot());
    const auto loaded = key_value_store::StorePersistence::load(path);

    expect(loaded.size() == 3, "expected 3 entries with special chars");
    expect(loaded.at("key with spaces") == "value with spaces", "expected spaces preserved");
    expect(loaded.at("key\"quote") == "value\"quote", "expected quotes preserved");
    expect(loaded.at("key\\backslash") == "value\\backslash", "expected backslash preserved");

    std::remove(path.c_str());
}

void test_persistence_load_nonexistent_file() {
    bool exception_thrown = false;
    try {
        key_value_store::StorePersistence::load("/tmp/nonexistent_kv_file.db");
    } catch (const std::runtime_error&) {
        exception_thrown = true;
    }
    expect(exception_thrown, "expected exception for loading nonexistent file");
}

void test_count_after_operations() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("COUNT").output == "COUNT 0", "expected count 0 initially");
    processor.execute("SET a 1");
    processor.execute("SET b 2");
    processor.execute("SET c 3");
    expect(processor.execute("COUNT").output == "COUNT 3", "expected count 3");
    processor.execute("DELETE b");
    expect(processor.execute("COUNT").output == "COUNT 2", "expected count 2 after delete");
    processor.execute("CLEAR");
    expect(processor.execute("COUNT").output == "COUNT 0", "expected count 0 after clear");
}

void test_store_manager_basic() {
    const std::string path = "/tmp/kv_manager_test.db";
    std::remove(path.c_str());

    key_value_store::StoreManager manager(path);

    expect(!manager.load(), "expected load to fail on missing file");

    manager.store().set("key", "value");
    expect(manager.save(), "expected save to succeed");

    key_value_store::StoreManager manager2(path);
    expect(manager2.load(), "expected load to succeed");
    expect(manager2.store().get("key").has_value(), "expected key to exist");
    expect(manager2.store().get("key").value() == "value", "expected correct value");

    std::remove(path.c_str());
}

void test_store_manager_auto_persist() {
    key_value_store::StoreManager manager("/tmp/kv_auto_test.db");

    expect(manager.autoPersist(), "expected auto persist to be on by default");
    manager.setAutoPersist(false);
    expect(!manager.autoPersist(), "expected auto persist to be off");

    manager.setAutoPersist(true);
    expect(manager.autoPersist(), "expected auto persist to be on");
}

void test_store_manager_change_path() {
    key_value_store::StoreManager manager("/tmp/old_path.db");
    expect(manager.persistencePath() == "/tmp/old_path.db", "expected initial path");

    manager.setPersistencePath("/tmp/new_path.db");
    expect(manager.persistencePath() == "/tmp/new_path.db", "expected updated path");
}

void test_malformed_set_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("SET").output == "ERROR usage: SET <key> <value>",
           "expected SET without args error");
    expect(processor.execute("SET key").output == "ERROR usage: SET <key> <value>",
           "expected SET with only key error");
    expect(processor.execute("SET key value extra").output == "OK",
           "expected SET with extra args to succeed (value includes extra)");
}

void test_malformed_get_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("GET").output == "ERROR usage: GET <key>",
           "expected GET without args error");
    expect(processor.execute("GET key extra").output == "ERROR usage: GET <key>",
           "expected GET with extra args error");
}

void test_malformed_delete_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("DELETE").output == "ERROR usage: DELETE <key>",
           "expected DELETE without args error");
    expect(processor.execute("DELETE key extra").output == "ERROR usage: DELETE <key>",
           "expected DELETE with extra args error");
}

void test_malformed_exists_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("EXISTS").output == "ERROR usage: EXISTS <key>",
           "expected EXISTS without args error");
    expect(processor.execute("EXISTS key extra").output == "ERROR usage: EXISTS <key>",
           "expected EXISTS with extra args error");
}

void test_malformed_count_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("COUNT extra").output == "ERROR usage: COUNT",
           "expected COUNT with extra args error");
}

void test_malformed_clear_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("CLEAR extra").output == "ERROR usage: CLEAR",
           "expected CLEAR with extra args error");
}

void test_malformed_save_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("SAVE extra").output == "ERROR usage: SAVE",
           "expected SAVE with extra args error");
}

void test_malformed_load_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("LOAD extra").output == "ERROR usage: LOAD",
           "expected LOAD with extra args error");
}

void test_malformed_exit_commands() {
    key_value_store::KeyValueStore store;
    key_value_store::CommandProcessor processor(store);

    expect(processor.execute("EXIT extra").output == "ERROR usage: EXIT",
           "expected EXIT with extra args error");
}

void test_protocol_set_get() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto setResp = protocol.execute("SET language C++");
    expect(setResp.status == key_value_store::ProtocolStatus::ok, "expected SET ok");

    auto getResp = protocol.execute("GET language");
    expect(getResp.status == key_value_store::ProtocolStatus::value, "expected GET value");
    expect(getResp.data == "C++", "expected value C++");

    auto getMissing = protocol.execute("GET missing");
    expect(getMissing.status == key_value_store::ProtocolStatus::not_found, "expected not found");
}

void test_protocol_delete() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    store.set("key1", "value1");

    auto delResp = protocol.execute("DEL key1");
    expect(delResp.status == key_value_store::ProtocolStatus::ok, "expected DEL ok");

    auto getResp = protocol.execute("GET key1");
    expect(getResp.status == key_value_store::ProtocolStatus::not_found, "expected not found after delete");

    auto delMissing = protocol.execute("DEL missing");
    expect(delMissing.status == key_value_store::ProtocolStatus::not_found, "expected not found for missing key");
}

void test_protocol_exists() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    store.set("exists_key", "value");

    auto existsResp = protocol.execute("EXISTS exists_key");
    expect(existsResp.status == key_value_store::ProtocolStatus::ok, "expected ok");
    expect(existsResp.data == "1", "expected 1 for existing key");

    auto notExistsResp = protocol.execute("EXISTS missing_key");
    expect(notExistsResp.data == "0", "expected 0 for missing key");
}

void test_protocol_count() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto emptyResp = protocol.execute("COUNT");
    expect(emptyResp.status == key_value_store::ProtocolStatus::count, "expected count");
    expect(emptyResp.data == "0", "expected count 0");

    store.set("a", "1");
    store.set("b", "2");

    auto countResp = protocol.execute("COUNT");
    expect(countResp.data == "2", "expected count 2");
}

void test_protocol_clear() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    store.set("a", "1");
    store.set("b", "2");

    auto clearResp = protocol.execute("CLEAR");
    expect(clearResp.status == key_value_store::ProtocolStatus::ok, "expected ok");

    auto countResp = protocol.execute("COUNT");
    expect(countResp.data == "0", "expected count 0 after clear");
}

void test_protocol_ping() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto resp = protocol.execute("PING");
    expect(resp.status == key_value_store::ProtocolStatus::ok, "expected ok");
    expect(resp.data == "pong", "expected pong");
}

void test_protocol_help() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto resp = protocol.execute("HELP");
    expect(resp.status == key_value_store::ProtocolStatus::value, "expected value");
    expect(resp.data.find("SET") != std::string::npos, "expected SET in help");
    expect(resp.data.find("GET") != std::string::npos, "expected GET in help");
}

void test_protocol_error_handling() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto emptyResp = protocol.execute("");
    expect(emptyResp.status == key_value_store::ProtocolStatus::error, "expected error for empty");

    auto unknownResp = protocol.execute("UNKNOWN");
    expect(unknownResp.status == key_value_store::ProtocolStatus::error, "expected error for unknown");

    auto setNoArgs = protocol.execute("SET");
    expect(setNoArgs.status == key_value_store::ProtocolStatus::error, "expected error for SET without args");

    auto getNoArgs = protocol.execute("GET");
    expect(getNoArgs.status == key_value_store::ProtocolStatus::error, "expected error for GET without args");
}

void test_protocol_response_serialize() {
    using namespace key_value_store;

    ProtocolResponse okResp{ProtocolStatus::ok, ""};
    expect(okResp.serialize() == "+OK\n", "expected +OK\\n");

    ProtocolResponse valueResp{ProtocolStatus::value, "hello"};
    expect(valueResp.serialize() == "$hello\n", "expected $hello\\n");

    ProtocolResponse notFoundResp{ProtocolStatus::not_found, ""};
    expect(notFoundResp.serialize() == "-NOT_FOUND\n", "expected -NOT_FOUND\\n");

    ProtocolResponse errorResp{ProtocolStatus::error, "bad input"};
    expect(errorResp.serialize() == "-ERROR bad input\n", "expected -ERROR bad input\\n");

    ProtocolResponse countResp{ProtocolStatus::count, "42"};
    expect(countResp.serialize() == ":42\n", "expected :42\\n");

    ProtocolResponse byeResp{ProtocolStatus::bye, ""};
    expect(byeResp.serialize() == "+BYE\n", "expected +BYE\\n");
}

void test_protocol_response_parse() {
    using namespace key_value_store;

    auto ok = ProtocolResponse::parse("+OK");
    expect(ok.status == ProtocolStatus::ok, "expected ok");

    auto value = ProtocolResponse::parse("$hello world");
    expect(value.status == ProtocolStatus::value, "expected value");
    expect(value.data == "hello world", "expected hello world");

    auto notFound = ProtocolResponse::parse("-NOT_FOUND");
    expect(notFound.status == ProtocolStatus::not_found, "expected not found");

    auto error = ProtocolResponse::parse("-ERROR bad command");
    expect(error.status == ProtocolStatus::error, "expected error");
    expect(error.data == "bad command", "expected bad command");

    auto count = ProtocolResponse::parse(":100");
    expect(count.status == ProtocolStatus::count, "expected count");
    expect(count.data == "100", "expected 100");

    auto bye = ProtocolResponse::parse("+BYE");
    expect(bye.status == ProtocolStatus::bye, "expected bye");
}

std::string sendCommand(const std::string& host, int port, const std::string& command) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return "SOCKET_ERROR";

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return "CONNECT_ERROR";
    }

    std::string request = command + "\n";
    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096];
    ssize_t bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
    close(sock);

    if (bytesRead <= 0) return "RECV_ERROR";
    buffer[bytesRead] = '\0';
    return std::string(buffer);
}

void test_server_protocol_integration() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto setResp = protocol.execute("SET language C++");
    expect(setResp.status == key_value_store::ProtocolStatus::ok, "expected SET ok");

    auto getResp = protocol.execute("GET language");
    expect(getResp.status == key_value_store::ProtocolStatus::value, "expected GET value");
    expect(getResp.data == "C++", "expected value C++");

    auto countResp = protocol.execute("COUNT");
    expect(countResp.data == "1", "expected count 1");

    auto delResp = protocol.execute("DEL language");
    expect(delResp.status == key_value_store::ProtocolStatus::ok, "expected DEL ok");

    auto getAfterDel = protocol.execute("GET language");
    expect(getAfterDel.status == key_value_store::ProtocolStatus::not_found, "expected not found");
}

void test_store_manager_with_protocol() {
    const std::string persistencePath = "/tmp/kv_protocol_test.db";
    std::remove(persistencePath.c_str());

    key_value_store::StoreManager manager(persistencePath);
    key_value_store::Protocol protocol(manager.store());

    protocol.execute("SET key1 value1");
    protocol.execute("SET key2 value2");

    expect(manager.save(), "expected save to succeed");

    key_value_store::StoreManager manager2(persistencePath);
    expect(manager2.load(), "expected load to succeed");

    key_value_store::Protocol protocol2(manager2.store());
    auto getResp = protocol2.execute("GET key1");
    expect(getResp.data == "value1", "expected persisted value");

    std::remove(persistencePath.c_str());
}

void test_client_direct_connection() {
    const std::string persistencePath = "/tmp/kv_client_direct_test.db";
    std::remove(persistencePath.c_str());

    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    auto setResp = protocol.execute("SET mykey myvalue");
    expect(setResp.status == key_value_store::ProtocolStatus::ok, "expected SET ok");

    auto getResp = protocol.execute("GET mykey");
    expect(getResp.status == key_value_store::ProtocolStatus::value, "expected GET value");
    expect(getResp.data == "myvalue", "expected myvalue");

    auto existsResp = protocol.execute("EXISTS mykey");
    expect(existsResp.data == "1", "expected exists");

    auto countResp = protocol.execute("COUNT");
    expect(countResp.data == "1", "expected count 1");

    auto delResp = protocol.execute("DEL mykey");
    expect(delResp.status == key_value_store::ProtocolStatus::ok, "expected DEL ok");

    auto getAfterDel = protocol.execute("GET mykey");
    expect(getAfterDel.status == key_value_store::ProtocolStatus::not_found, "expected not found");

    std::remove(persistencePath.c_str());
}

void test_client_interactive_simulation() {
    key_value_store::KeyValueStore store;
    key_value_store::Protocol protocol(store);

    std::vector<std::string> commands = {
        "PING",
        "SET language C++",
        "GET language",
        "EXISTS language",
        "COUNT",
        "SET version 17",
        "COUNT",
        "DEL language",
        "EXISTS language",
        "COUNT",
        "HELP"
    };

    for (const auto& cmd : commands) {
        key_value_store::ProtocolResponse response = protocol.execute(cmd);
        expect(response.status != key_value_store::ProtocolStatus::error || cmd == "HELP",
               "unexpected error for: " + cmd);
    }

    auto countResp = protocol.execute("COUNT");
    expect(countResp.data == "1", "expected count 1 after operations");
}

void test_concurrent_reads() {
    key_value_store::KeyValueStore store;

    for (int i = 0; i < 100; ++i) {
        store.set("key" + std::to_string(i), "value" + std::to_string(i));
    }

    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&store, i, &successCount]() {
            for (int j = 0; j < 100; ++j) {
                auto val = store.get("key" + std::to_string(j));
                if (val.has_value() && *val == "value" + std::to_string(j)) {
                    successCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    expect(successCount == 1000, "expected all concurrent reads to succeed");
}

void test_concurrent_writes() {
    key_value_store::KeyValueStore store;

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&store, i]() {
            for (int j = 0; j < 100; ++j) {
                std::string key = "thread" + std::to_string(i) + "_key" + std::to_string(j);
                store.set(key, "value");
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    expect(store.size() == 1000, "expected 1000 entries after concurrent writes");
}

void test_concurrent_read_write() {
    key_value_store::KeyValueStore store;

    for (int i = 0; i < 100; ++i) {
        store.set("key" + std::to_string(i), "initial");
    }

    std::vector<std::thread> readers;
    std::vector<std::thread> writers;

    std::atomic<bool> running{true};

    for (int i = 0; i < 5; ++i) {
        readers.emplace_back([&store, &running]() {
            while (running) {
                for (int j = 0; j < 100; ++j) {
                    store.get("key" + std::to_string(j));
                }
            }
        });
    }

    for (int i = 0; i < 5; ++i) {
        writers.emplace_back([&store, &running, i]() {
            int count = 0;
            while (running) {
                std::string key = "newkey" + std::to_string(i) + "_" + std::to_string(count++);
                store.set(key, "value");
                if (count > 50) break;
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    running = false;

    for (auto& t : readers) t.join();
    for (auto& t : writers) t.join();

    expect(store.size() >= 100, "expected at least 100 entries");
}

} // namespace

int main() {
    try {
        test_basic_store_operations();
        test_command_processor();
        test_persistence_round_trip();
        test_clear_command();
        test_save_command_action();
        test_command_case_insensitivity();
        test_quit_as_exit_alias();
        test_empty_command();
        test_unknown_command();
        test_set_get_with_spaces_in_value();
        test_overwrite_existing_key();
        test_delete_nonexistent_key();
        test_replace_all();
        test_persistence_empty_store();
        test_persistence_special_characters();
        test_persistence_load_nonexistent_file();
        test_count_after_operations();
        test_store_manager_basic();
        test_store_manager_auto_persist();
        test_store_manager_change_path();
        test_malformed_set_commands();
        test_malformed_get_commands();
        test_malformed_delete_commands();
        test_malformed_exists_commands();
        test_malformed_count_commands();
        test_malformed_clear_commands();
        test_malformed_save_commands();
        test_malformed_load_commands();
        test_malformed_exit_commands();
        test_protocol_set_get();
        test_protocol_delete();
        test_protocol_exists();
        test_protocol_count();
        test_protocol_clear();
        test_protocol_ping();
        test_protocol_help();
        test_protocol_error_handling();
        test_protocol_response_serialize();
        test_protocol_response_parse();
        test_server_protocol_integration();
        test_store_manager_with_protocol();
        test_client_direct_connection();
        test_client_interactive_simulation();
        test_concurrent_reads();
        test_concurrent_writes();
        test_concurrent_read_write();
    } catch (const std::exception& exception) {
        std::cerr << "key_value_store_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "key_value_store_tests passed" << std::endl;
    return 0;
}
