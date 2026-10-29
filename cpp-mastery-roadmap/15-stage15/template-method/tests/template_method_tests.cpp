#include "template_method.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <sstream>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The classes under test report what they do by printing, so the output
// is what the test looks at.
class CapturedOutput {
public:
    CapturedOutput() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturedOutput() { std::cout.rdbuf(previous_); }

    CapturedOutput(const CapturedOutput&) = delete;
    CapturedOutput& operator=(const CapturedOutput&) = delete;

    std::string text() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

void test_csv_miner() {
    template_method::CSVDataMiner miner;
    CapturedOutput output;
    miner.mine("data.csv");

    const std::string text = output.text();
    expect(text.find("[DataMiner] Opening file: data.csv") != std::string::npos,
           "expected the file to be opened: " + text);
    expect(text.find("[CSV] Extracting CSV data") != std::string::npos,
           "expected the data to be extracted: " + text);
    expect(text.find("[CSV] Parsing CSV format") != std::string::npos,
           "expected the data to be parsed: " + text);
    expect(text.find("[CSV] Analyzing CSV data") != std::string::npos,
           "expected the data to be analysed: " + text);
    expect(text.find("[DataMiner] Closing file") != std::string::npos,
           "expected the file to be closed: " + text);

    // The template method runs the steps in this order.
    expect(text.find("Opening file") < text.find("Extracting"),
           "expected opening before extracting: " + text);
    expect(text.find("Extracting") < text.find("Parsing"),
           "expected extracting before parsing: " + text);
    expect(text.find("Parsing") < text.find("Analyzing"),
           "expected parsing before analysing: " + text);
    expect(text.find("Analyzing") < text.find("Closing file"),
           "expected analysing before closing: " + text);
}

void test_json_miner() {
    template_method::JSONDataMiner miner;
    CapturedOutput output;
    miner.mine("data.json");

    const std::string text = output.text();
    expect(text.find("[JSON] Opening JSON file: data.json") != std::string::npos,
           "expected the json hook to open the file: " + text);
    expect(text.find("[JSON] Extracting JSON data") != std::string::npos,
           "expected the data to be extracted: " + text);
    expect(text.find("[JSON] Analyzing JSON data") != std::string::npos,
           "expected the data to be analysed: " + text);
}

void test_xml_miner() {
    template_method::XMLDataMiner miner;
    CapturedOutput output;
    miner.mine("data.xml");

    const std::string text = output.text();
    // The xml miner does not override the hook, so the base class runs.
    expect(text.find("[DataMiner] Opening file: data.xml") != std::string::npos,
           "expected the base hook to open the file: " + text);
    expect(text.find("[XML] Extracting XML data") != std::string::npos,
           "expected the data to be extracted: " + text);
    expect(text.find("[XML] Parsing XML format") != std::string::npos,
           "expected the data to be parsed: " + text);
    expect(text.find("[XML] Analyzing XML data") != std::string::npos,
           "expected the data to be analysed: " + text);
    expect(text.find("[DataMiner] Closing file") != std::string::npos,
           "expected the file to be closed: " + text);
}

void test_hook_override() {
    template_method::JSONDataMiner miner;
    CapturedOutput output;
    miner.mine("test.json");

    const std::string text = output.text();
    // The json miner overrides the hook, so the base class does not run.
    expect(text.find("[JSON] Opening JSON file: test.json") != std::string::npos,
           "expected the overridden hook to run: " + text);
    expect(text.find("[DataMiner] Opening file") == std::string::npos,
           "expected the base hook not to run: " + text);
}

} // namespace

int main() {
    try {
        std::cout << "Template Method Tests:\n";
        test_csv_miner();
        std::cout << "  test_csv_miner: PASS\n";

        test_json_miner();
        std::cout << "  test_json_miner: PASS\n";

        test_xml_miner();
        std::cout << "  test_xml_miner: PASS\n";

        test_hook_override();
        std::cout << "  test_hook_override: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "template_method_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ntemplate_method_tests passed" << std::endl;
    return 0;
}
