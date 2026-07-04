#pragma once

#include "store/key_value_store.hpp"

#include <string>
#include <sstream>

namespace key_value_store {

enum class ProtocolStatus {
    ok,
    value,
    not_found,
    error,
    count,
    bye
};

struct ProtocolResponse {
    ProtocolStatus status;
    std::string data;

    std::string serialize() const;
    static ProtocolResponse parse(const std::string& line);
};

class Protocol {
public:
    explicit Protocol(KeyValueStore& store);

    ProtocolResponse execute(const std::string& request);

    std::string getHelp() const;

private:
    ProtocolResponse handleSet(const std::string& args);
    ProtocolResponse handleGet(const std::string& args);
    ProtocolResponse handleDelete(const std::string& args);
    ProtocolResponse handleExists(const std::string& args);
    ProtocolResponse handleCount();
    ProtocolResponse handleClear();
    ProtocolResponse handlePing();
    ProtocolResponse handleHelp();
    ProtocolResponse handleUnknown(const std::string& command);

    KeyValueStore& store_;
};

} // namespace key_value_store
