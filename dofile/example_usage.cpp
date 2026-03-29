#include <iostream>
#include <map>
#include <string>

using namespace std;

// Example showing how to use DoFile with the new return value pattern
// This demonstrates: auto data = dofile.run();

int main() {
    cout << "=== DoFile Return Value Usage Examples ===" << endl << endl;
    
    // Example 1: Basic usage - get data and iterate
    cout << "Example 1: Get data and iterate through all variables" << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "Code:" << endl;
    cout << "  DoFile<string> dofile;" << endl;
    cout << "  dofile.ReadFile(\"file.txt\");" << endl;
    cout << "  auto data = dofile.run();" << endl;
    cout << "" << endl;
    cout << "  for (const auto& entry : data) {" << endl;
    cout << "      cout << entry.first << \": \" << entry.second.first" << endl;
    cout << "           << \" = \" << entry.second.second << endl;" << endl;
    cout << "  }" << endl;
    cout << "" << endl << endl;
    
    // Example 2: Access specific variables
    cout << "Example 2: Access specific variable by name" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Code:" << endl;
    cout << "  auto data = dofile.run();" << endl;
    cout << "" << endl;
    cout << "  if (data.find(\"age\") != data.end()) {" << endl;
    cout << "      auto type = data[\"age\"].first;" << endl;
    cout << "      auto value = data[\"age\"].second;" << endl;
    cout << "      cout << \"Age type: \" << type << endl;" << endl;
    cout << "  }" << endl;
    cout << "" << endl << endl;
    
    // Example 3: Store and use data
    cout << "Example 3: Store data for later use" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Code:" << endl;
    cout << "  auto variables = dofile.run();" << endl;
    cout << "  " << endl;
    cout << "  // Use variables later in your program" << endl;
    cout << "  string userType = variables[\"username\"].first;" << endl;
    cout << "  string userName = variables[\"username\"].second;" << endl;
    cout << "" << endl << endl;
    
    // Example 4: Filter variables by type
    cout << "Example 4: Filter variables by type" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Code:" << endl;
    cout << "  auto data = dofile.run();" << endl;
    cout << "" << endl;
    cout << "  cout << \"String variables:\" << endl;" << endl;
    cout << "  for (const auto& entry : data) {" << endl;
    cout << "      if (entry.second.first == \"string\") {" << endl;
    cout << "          cout << entry.first << \" = \" << entry.second.second << endl;" << endl;
    cout << "      }" << endl;
    cout << "  }" << endl;
    cout << "" << endl << endl;
    
    // Example 5: Copy data for modification
    cout << "Example 5: Store data in a local variable" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Code:" << endl;
    cout << "  map<string, pair<string, string>> myData = dofile.run();" << endl;
    cout << "" << endl;
    cout << "  // Now you can process or pass myData around" << endl;
    cout << "  ProcessData(myData);" << endl;
    cout << "  LogData(myData);" << endl;
    cout << "" << endl << endl;
    
    // Example 6: Check if variable exists
    cout << "Example 6: Check if variable exists" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Code:" << endl;
    cout << "  auto data = dofile.run();" << endl;
    cout << "" << endl;
    cout << "  if (data.count(\"userEmail\")) {" << endl;
    cout << "      cout << \"Email found: \" << data[\"userEmail\"].second << endl;" << endl;
    cout << "  } else {" << endl;
    cout << "      cout << \"Email not found\" << endl;" << endl;
    cout << "  }" << endl;
    cout << "" << endl;
    
    return 0;
}

/*
 * SUMMARY OF DATA STRUCTURE:
 * =========================
 * 
 * run() returns: map<string, pair<string, string>>
 * 
 * Structure:
 * {
 *     "variableName": {
 *         first:  "dataType"  (e.g., "int", "string", "array<int>")
 *         second: "value"     (e.g., "25", "\"hello\"", "[1,2,3]")
 *     }
 * }
 * 
 * Access patterns:
 * - data["varName"].first   -> gets the type
 * - data["varName"].second  -> gets the value
 * 
 * Iteration:
 * for (const auto& entry : data) {
 *     entry.first             -> variable name
 *     entry.second.first      -> variable type
 *     entry.second.second     -> variable value
 * }
 */
