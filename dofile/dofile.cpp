#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

template<typename T>
class DoFile {
    private:
    map<string, map<string, T>> variables = {
        {"KEYWORD", {
            {"int", "INT"},
            {"float", "FLOAT"},
            {"string", "STRING"},
            {"bool", "BOOL"},
            {"char", "CHAR"},
            {"void", "VOID"},
            {"map", "MAP"},
            {"array", "ARRAY"},
            {"list", "LIST"},
            {"var", "VAR"}
        }},
        {"OPERATOR", {
            {"+", "PLUS"},
            {"-", "MINUS"},
            {"*", "MULTIPLY"},
            {"/", "DIVIDE"},
            {"=", "ASSIGN"},
            {"==", "EQUALS"},
            {"!=", "NOT_EQUALS"},
            {"<", "LESS_THAN"},
            {">", "GREATER_THAN"},
            {"<=", "LESS_THAN_OR_EQUAL"},
            {">=", "GREATER_THAN_OR_EQUAL"}
        }},
        {"DELIMITER", {
            {";", "SEMICOLON"},
            {",", "COMMA"},
            {"(", "LEFT_PAREN"},
            {")", "RIGHT_PAREN"},
            {"{", "LEFT_BRACE"},
            {"}", "RIGHT_BRACE"},
            {"[", "LEFT_BRACKET"},
            {"]", "RIGHT_BRACKET"},
            {"<", "LEFT_ANGLE"},
            {">", "RIGHT_ANGLE"}
        }}
    };

    public:
    bool debug = false;
    vector<string> lines;
    void ReadFile(const string& filepath) {
        ifstream file(filepath);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filepath << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            lines.push_back(line);
        }
    }

    vector<vector<pair<string, string>>> lexer() {
        vector<vector<pair<string, string>>> tokens;
        for (const auto& line : lines) {
            vector<pair<string, string>> lineTokens;
            string buffer = "";
            
            for (size_t i = 0; i < line.length(); i++) {
                char ch = line[i];
                
                // Handle quoted strings
                if (ch == '"') {
                    if (!buffer.empty()) {
                        if (variables["KEYWORD"].find(buffer) != variables["KEYWORD"].end()) {
                            lineTokens.push_back({"KEYWORD", buffer});
                        } else {
                            lineTokens.push_back({"IDENT", buffer});
                        }
                        buffer = "";
                    }
                    string strValue = "\"";
                    i++;
                    while (i < line.length() && line[i] != '"') {
                        strValue += line[i];
                        i++;
                    }
                    if (i < line.length()) {
                        strValue += "\"";
                    }
                    lineTokens.push_back({"STRING", strValue});
                    continue;
                }
                
                // Handle whitespace
                if (isspace(ch)) {
                    if (!buffer.empty()) {
                        if (variables["KEYWORD"].find(buffer) != variables["KEYWORD"].end()) {
                            lineTokens.push_back({"KEYWORD", buffer});
                        } else {
                            lineTokens.push_back({"IDENT", buffer});
                        }
                        buffer = "";
                    }
                    continue;
                }
                
                // Check for two-character operators
                string twoChar = "";
                if (i + 1 < line.length()) {
                    twoChar = string(1, ch) + line[i + 1];
                }
                
                if (!twoChar.empty() && variables["OPERATOR"].find(twoChar) != variables["OPERATOR"].end()) {
                    if (!buffer.empty()) {
                        if (variables["KEYWORD"].find(buffer) != variables["KEYWORD"].end()) {
                            lineTokens.push_back({"KEYWORD", buffer});
                        } else {
                            lineTokens.push_back({"IDENT", buffer});
                        }
                        buffer = "";
                    }
                    lineTokens.push_back({"OPERATOR", twoChar});
                    i++;
                    continue;
                }
                
                // Check for single-character operators and delimiters
                if (variables["OPERATOR"].find(string(1, ch)) != variables["OPERATOR"].end()) {
                    if (!buffer.empty()) {
                        if (variables["KEYWORD"].find(buffer) != variables["KEYWORD"].end()) {
                            lineTokens.push_back({"KEYWORD", buffer});
                        } else {
                            lineTokens.push_back({"IDENT", buffer});
                        }
                        buffer = "";
                    }
                    lineTokens.push_back({"OPERATOR", string(1, ch)});
                    continue;
                }
                
                if (variables["DELIMITER"].find(string(1, ch)) != variables["DELIMITER"].end()) {
                    if (!buffer.empty()) {
                        if (variables["KEYWORD"].find(buffer) != variables["KEYWORD"].end()) {
                            lineTokens.push_back({"KEYWORD", buffer});
                        } else {
                            lineTokens.push_back({"IDENT", buffer});
                        }
                        buffer = "";
                    }
                    lineTokens.push_back({"DELIM", string(1, ch)});
                    continue;
                }
                
                // Build buffer for identifiers/keywords/numbers
                buffer += ch;
            }
            
            // Handle remaining buffer
            if (!buffer.empty()) {
                if (variables["KEYWORD"].find(buffer) != variables["KEYWORD"].end()) {
                    lineTokens.push_back({"KEYWORD", buffer});
                } else {
                    lineTokens.push_back({"IDENT", buffer});
                }
            }
            
            if (!lineTokens.empty()) {
                tokens.push_back(lineTokens);
            }
        }
        return tokens;
    }

    // Helper function to resolve variable value (if it's a variable name, get its value)
    string resolveValue(const string& value, const map<string, pair<string, string>>& context) {
        if (context.find(value) != context.end()) {
            return context.at(value).second;  // Return the actual value of the variable
        }
        return value;  // Return literal value
    }

    // Helper function to extract array/list/map contents with nesting support
    string extractCollectionContent(const vector<pair<string, string>>& lineTokens, size_t startIdx) {
        string content = "";
        bool inCollection = false;
        int bracketDepth = 0;
        int braceDepth = 0;
        bool firstElement = true;
        
        for (size_t i = startIdx; i < lineTokens.size(); i++) {
            if (lineTokens[i].first == "DELIM") {
                if (!inCollection && (lineTokens[i].second == "{" || lineTokens[i].second == "[")) {
                    inCollection = true;
                    if (lineTokens[i].second == "[") bracketDepth++;
                    else braceDepth++;
                    continue;
                }
                
                if (inCollection) {
                    // Track nesting depth
                    if (lineTokens[i].second == "[") {
                        bracketDepth++;
                        content += lineTokens[i].second;
                    } else if (lineTokens[i].second == "]") {
                        bracketDepth--;
                        if (bracketDepth <= 0 && braceDepth <= 0) {
                            break;  // End of collection
                        }
                        content += lineTokens[i].second;
                    } else if (lineTokens[i].second == "{") {
                        braceDepth++;
                        content += lineTokens[i].second;
                    } else if (lineTokens[i].second == "}") {
                        braceDepth--;
                        if (bracketDepth <= 0 && braceDepth <= 0) {
                            break;  // End of collection
                        }
                        content += lineTokens[i].second;
                    } else if (lineTokens[i].second == ",") {
                        content += lineTokens[i].second;
                    } else if (lineTokens[i].second == ";") {
                        break;  // Statement ended
                    }
                }
            } else if (inCollection && lineTokens[i].first != "DELIM") {
                content += lineTokens[i].second;
            }
        }
        
        return content;
    }

    // Helper to parse complex generic types like array<array<int>>
    pair<string, size_t> parseGenericType(const vector<pair<string, string>>& lineTokens, size_t startIdx) {
        string typeStr = "";
        int angleDepth = 0;
        size_t i = startIdx;
        
        while (i < lineTokens.size()) {
            if (lineTokens[i].first == "KEYWORD" || lineTokens[i].first == "IDENT") {
                typeStr += lineTokens[i].second;
            } else if (lineTokens[i].second == "<") {
                angleDepth++;
                typeStr += "<";
            } else if (lineTokens[i].second == ">") {
                angleDepth--;
                typeStr += ">";
                if (angleDepth <= 0) {
                    i++;
                    break;
                }
            } else if (lineTokens[i].second == ",") {
                typeStr += ",";
            } else {
                break;  // End of type declaration
            }
            i++;
        }
        
        return {typeStr, i};
    }

    // Execute tokens and store variables
    void execute(const vector<pair<string, string>>& lineTokens, map<string, pair<string, string>>& context, int lineNumber) {
        if (lineTokens.empty()) return;
        
        // Handle variable assignment/update: x = value;
        if (lineTokens.size() >= 4 && lineTokens[0].first == "IDENT" && lineTokens[1].second == "=") {
            string varName = lineTokens[0].second;
            string varValue = resolveValue(lineTokens[2].second, context);
            
            if (context.find(varName) != context.end()) {
                // Update existing variable
                context[varName].second = varValue;
                if (debug) cout << "DEBUG: Updated " << varName << " = " << varValue << endl;
            }
            return;
        }
        
        // Handle: var x = 10; or var x = y; or var<int> x = 10;
        if (lineTokens[0].second == "var") {
            string varName, varType = "", varValue;
            size_t idx = 1;
            
            // Check if typed var<type>
            if (lineTokens.size() > 1 && lineTokens[1].first == "OPERATOR" && lineTokens[1].second == "<") {
                auto [type, nextIdx] = parseGenericType(lineTokens, 1);
                varType = type;
                idx = nextIdx;
                
                if (idx < lineTokens.size()) varName = lineTokens[idx].second;
                idx++;  // skip variable name
            } else {
                varName = lineTokens[1].second;
                idx = 2;
            }
            
            // Find = and extract value
            while (idx < lineTokens.size() && lineTokens[idx].second != "=") idx++;
            if (idx + 1 < lineTokens.size()) {
                varValue = resolveValue(lineTokens[idx + 1].second, context);
                context[varName] = {varType, varValue};
                if (debug) {
                    cout << "DEBUG: Declared " << varName << " = " << varValue;
                    if (!varType.empty()) cout << " (" << varType << ")";
                    cout << endl;
                }
            }
        }
        // Handle: array<...> arr = [...]; list<...> lst = [...]; map<...> m = [...];
        else if ((lineTokens[0].second == "array" || lineTokens[0].second == "list" || lineTokens[0].second == "map") && 
                 lineTokens.size() >= 4) {
            auto [typeStr, typeEndIdx] = parseGenericType(lineTokens, 0);
            
            if (typeEndIdx < lineTokens.size()) {
                string varName = lineTokens[typeEndIdx].second;
                
                // Find = and extract content
                size_t assignIdx = typeEndIdx + 1;
                while (assignIdx < lineTokens.size() && lineTokens[assignIdx].second != "=") assignIdx++;
                
                if (assignIdx + 1 < lineTokens.size()) {
                    string content = extractCollectionContent(lineTokens, assignIdx + 1);
                    context[varName] = {typeStr, content};
                    if (debug) cout << "DEBUG: Declared " << varName << " (" << typeStr << ") = " << content << endl;
                }
            }
        }
        // Handle: int x = 10; float y = 3.14; string z = hello;
        else if (variables["KEYWORD"].find(lineTokens[0].second) != variables["KEYWORD"].end() && lineTokens.size() >= 5) {
            string varType = lineTokens[0].second;
            string varName = lineTokens[1].second;
            string varValue = resolveValue(lineTokens[3].second, context);
            context[varName] = {varType, varValue};
            if (debug) cout << "DEBUG: Declared " << varName << " = " << varValue << " (" << varType << ")" << endl;
        }
    }

    map<string, pair<string, string>> run() {
        map<string, pair<string, string>> context;  // var_name -> (type, value)
        vector<vector<pair<string, string>>> tokens = this->lexer();
        
        // Merge multiline statements
        vector<vector<pair<string, string>>> mergedTokens;
        vector<pair<string, string>> currentStatement;
        
        for (size_t i = 0; i < tokens.size(); i++) {
            // Add tokens from current line to the statement
            for (const auto& token : tokens[i]) {
                currentStatement.push_back(token);
            }
            
            // Check if statement ends with semicolon
            if (!tokens[i].empty() && tokens[i].back().first == "DELIM" && tokens[i].back().second == ";") {
                mergedTokens.push_back(currentStatement);
                currentStatement.clear();
            }
        }
        
        // Add any remaining tokens as final statement
        if (!currentStatement.empty()) {
            mergedTokens.push_back(currentStatement);
        }
        
        // Execute each merged statement
        for (size_t i = 0; i < mergedTokens.size(); i++) {
            execute(mergedTokens[i], context, i + 1);
        }
        
        // Store context for writing
        this->lastContext = context;
        
        // Output as JSON map
        cout << "{" << endl;
        size_t count = 0;
        for (const auto& entry : context) {
            cout << "  \"" << entry.first << "\": {\"type\": \"" << entry.second.first 
                 << "\", \"value\": \"" << entry.second.second << "\"}";
            if (count < context.size() - 1) cout << ",";
            cout << endl;
            count++;
        }
        cout << "}" << endl;
        
        return context;
    }
    
    // Write to JSON file
    void WriteToJSON(const string& outputPath) {
        ofstream file(outputPath);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file " << outputPath << " for writing" << endl;
            return;
        }
        
        file << "{" << endl;
        size_t count = 0;
        for (const auto& entry : lastContext) {
            file << "  \"" << entry.first << "\": {\"type\": \"" << entry.second.first 
                 << "\", \"value\": \"" << entry.second.second << "\"}";
            if (count < lastContext.size() - 1) file << ",";
            file << endl;
            count++;
        }
        file << "}" << endl;
        file.close();
        
        cout << "✓ JSON output written to: " << outputPath << endl;
    }
    
    // Write to source code format
    void WriteToSource(const string& outputPath) {
        ofstream file(outputPath);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file " << outputPath << " for writing" << endl;
            return;
        }
        
        for (const auto& entry : lastContext) {
            string varName = entry.first;
            string varType = entry.second.first;
            string varValue = entry.second.second;
            
            if (varType.empty()) {
                file << "var " << varName << " = " << varValue << ";" << endl;
            } else if (varType.find("<") != string::npos && varType.find(">") != string::npos) {
                // Collection type
                if (varType.find("array") != string::npos) {
                    file << varType << " " << varName << " = [" << varValue << "];" << endl;
                } else if (varType.find("list") != string::npos) {
                    file << varType << " " << varName << " = [" << varValue << "];" << endl;
                } else if (varType.find("map") != string::npos) {
                    file << varType << " " << varName << " = [" << varValue << "];" << endl;
                }
            } else {
                file << varType << " " << varName << " = " << varValue << ";" << endl;
            }
        }
        file.close();
        
        cout << "✓ Source code written to: " << outputPath << endl;
    }
    
    // Write both JSON and Source
    void WriteAll(const string& jsonPath, const string& sourcePath) {
        WriteToJSON(jsonPath);
        WriteToSource(sourcePath);
    }
    
    protected:
    map<string, pair<string, string>> lastContext;
};
