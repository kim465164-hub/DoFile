#ifndef DOFILE_LIBRARY_HPP
#define DOFILE_LIBRARY_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// Variable wrapper with automatic type conversion
struct Variable {
    string type;
    string rawValue;
    
    // Convert to int
    int asInt() const {
        return stoi(rawValue);
    }
    
    // Convert to float
    float asFloat() const {
        return stof(rawValue);
    }
    
    // Convert to string (remove quotes)
    string asString() const {
        if (rawValue.length() >= 2 && rawValue[0] == '"' && rawValue.back() == '"') {
            return rawValue.substr(1, rawValue.length() - 2);
        }
        return rawValue;
    }
    
    // Convert to bool
    bool asBool() const {
        return rawValue == "true" || rawValue == "1";
    }
    
    // Get raw array/list/map value
    string getRawValue() const {
        return rawValue;
    }
    
    // Auto conversion operators for easy usage
    operator int() const { return asInt(); }
    operator float() const { return asFloat(); }
    operator double() const { return stod(rawValue); }
    operator string() const { return asString(); }
    operator bool() const { return asBool(); }
};

struct Token {
    string type;
    string value;
};

struct TokenLine {
    vector<Token> tokens;

    void push_back(const Token& token) {
        tokens.push_back(token);
    }

    Token& operator[](size_t idx) {
        return tokens[idx];
    }

    const Token& operator[](size_t idx) const {
        return tokens[idx];
    }

    size_t size() const {
        return tokens.size();
    }

    bool empty() const {
        return tokens.empty();
    }

    Token& back() {
        return tokens.back();
    }

    const Token& back() const {
        return tokens.back();
    }

    auto begin() {
        return tokens.begin();
    }

    auto end() {
        return tokens.end();
    }

    auto begin() const {
        return tokens.begin();
    }

    auto end() const {
        return tokens.end();
    }

    void clear() {
        tokens.clear();
    }
};

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

    vector<TokenLine> lexer() {
        vector<TokenLine> tokens;
        for (const auto& line : lines) {
            TokenLine lineTokens;
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
    string extractCollectionContent(const TokenLine& lineTokens, size_t startIdx) {
        string content = "";
        bool inCollection = false;
        int bracketDepth = 0;
        int braceDepth = 0;
        bool firstElement = true;
        
        for (size_t i = startIdx; i < lineTokens.size(); i++) {
            if (lineTokens[i].type == "DELIM") {
                if (!inCollection && (lineTokens[i].value == "{" || lineTokens[i].value == "[")) {
                    inCollection = true;
                    if (lineTokens[i].value == "[") bracketDepth++;
                    else braceDepth++;
                    continue;
                }
                
                if (inCollection) {
                    // Track nesting depth
                    if (lineTokens[i].value == "[") {
                        bracketDepth++;
                        content += lineTokens[i].value;
                    } else if (lineTokens[i].value == "]") {
                        bracketDepth--;
                        if (bracketDepth <= 0 && braceDepth <= 0) {
                            break;  // End of collection
                        }
                        content += lineTokens[i].value;
                    } else if (lineTokens[i].value == "{") {
                        braceDepth++;
                        content += lineTokens[i].value;
                    } else if (lineTokens[i].value == "}") {
                        braceDepth--;
                        if (bracketDepth <= 0 && braceDepth <= 0) {
                            break;  // End of collection
                        }
                        content += lineTokens[i].value;
                    } else if (lineTokens[i].value == ",") {
                        content += lineTokens[i].value;
                    } else if (lineTokens[i].value == ";") {
                        break;  // Statement ended
                    }
                }
            } else if (inCollection && lineTokens[i].type != "DELIM") {
                content += lineTokens[i].value;
            }
        }
        
        return content;
    }

    // Helper to parse complex generic types like array<array<int>>
    pair<string, size_t> parseGenericType(const TokenLine& lineTokens, size_t startIdx) {
        string typeStr = "";
        int angleDepth = 0;
        size_t i = startIdx;
        
        while (i < lineTokens.size()) {
            if (lineTokens[i].type == "KEYWORD" || lineTokens[i].type == "IDENT") {
                typeStr += lineTokens[i].value;
            } else if (lineTokens[i].value == "<") {
                angleDepth++;
                typeStr += "<";
            } else if (lineTokens[i].value == ">") {
                angleDepth--;
                typeStr += ">";
                if (angleDepth <= 0) {
                    i++;
                    break;
                }
            } else if (lineTokens[i].value == ",") {
                typeStr += ",";
            } else {
                break;  // End of type declaration
            }
            i++;
        }
        
        return {typeStr, i};
    }

    // Execute tokens and store variables
    void execute(const TokenLine& lineTokens, map<string, Variable>& context, int lineNumber) {
        if (lineTokens.empty()) return;
        
        // Handle variable assignment/update: x = value;
        if (lineTokens.size() >= 4 && lineTokens[0].type == "IDENT" && lineTokens[1].value == "=") {
            string varName = lineTokens[0].value;
            string rawValue = lineTokens[2].value;
            
            if (context.find(varName) != context.end()) {
                context[varName].rawValue = rawValue;
                if (debug) cout << "DEBUG: Updated " << varName << " = " << rawValue << endl;
            }
            return;
        }
        
        // Handle: var x = 10; or var x = y; or var<int> x = 10;
        if (lineTokens[0].value == "var") {
            string varName, varType = "", rawValue;
            size_t idx = 1;
            
            // Check if typed var<type>
            if (lineTokens.size() > 1 && lineTokens[1].type == "OPERATOR" && lineTokens[1].value == "<") {
                auto [type, nextIdx] = parseGenericType(lineTokens, 1);
                varType = type;
                idx = nextIdx;
                
                if (idx < lineTokens.size()) varName = lineTokens[idx].value;
                idx++;  // skip variable name
            } else {
                varName = lineTokens[1].value;
                idx = 2;
            }
            
            while (idx < lineTokens.size() && lineTokens[idx].value != "=") idx++;
            if (idx + 1 < lineTokens.size()) {
                rawValue = lineTokens[idx + 1].value;
                context[varName] = {varType, rawValue};
                if (debug) {
                    cout << "DEBUG: Declared " << varName << " = " << rawValue;
                    if (!varType.empty()) cout << " (" << varType << ")";
                    cout << endl;
                }
            }
        }
        // Handle: array<...> arr = [...]; list<...> lst = [...]; map<...> m = [...];
        else if ((lineTokens[0].value == "array" || lineTokens[0].value == "list" || lineTokens[0].value == "map") && 
                 lineTokens.size() >= 4) {
            auto [typeStr, typeEndIdx] = parseGenericType(lineTokens, 0);
            
            if (typeEndIdx < lineTokens.size()) {
                string varName = lineTokens[typeEndIdx].value;
                
                size_t assignIdx = typeEndIdx + 1;
                while (assignIdx < lineTokens.size() && lineTokens[assignIdx].value != "=") assignIdx++;
                
                if (assignIdx + 1 < lineTokens.size()) {
                    string content = extractCollectionContent(lineTokens, assignIdx + 1);
                    context[varName] = {typeStr, content};
                    if (debug) cout << "DEBUG: Declared " << varName << " (" << typeStr << ") = " << content << endl;
                }
            }
        }
        // Handle: int x = 10; float y = 3.14; string z = hello;
        else if (variables["KEYWORD"].find(lineTokens[0].value) != variables["KEYWORD"].end() && lineTokens.size() >= 5) {
            string varType = lineTokens[0].value;
            string varName = lineTokens[1].value;
            string rawValue = lineTokens[3].value;
            context[varName] = {varType, rawValue};
            if (debug) cout << "DEBUG: Declared " << varName << " = " << rawValue << " (" << varType << ")" << endl;
        }
    }

    map<string, Variable> run() {
        map<string, Variable> context;
        vector<TokenLine> tokens = this->lexer();
        
        vector<TokenLine> mergedTokens;
        TokenLine currentStatement;
        
        for (size_t i = 0; i < tokens.size(); i++) {
            for (const auto& token : tokens[i]) {
                currentStatement.push_back(token);
            }
            
            if (!tokens[i].empty() && tokens[i].back().type == "DELIM" && tokens[i].back().value == ";") {
                mergedTokens.push_back(currentStatement);
                currentStatement.clear();
            }
        }
        
        if (!currentStatement.empty()) {
            mergedTokens.push_back(currentStatement);
        }
        
        for (size_t i = 0; i < mergedTokens.size(); i++) {
            execute(mergedTokens[i], context, i + 1);
        }
        
        // Store context for writing
        this->lastContext = context;
        
        // Output as JSON map
        cout << "{" << endl;
        size_t count = 0;
        for (const auto& entry : context) {
            cout << "  \"" << entry.first << "\": {\"type\": \"" << entry.second.type
                 << "\", \"value\": \"" << entry.second.rawValue << "\"}";
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
            file << "  \"" << entry.first << "\": {\"type\": \"" << entry.second.type 
                 << "\", \"value\": \"" << entry.second.rawValue << "\"}";
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
            string varType = entry.second.type;
            string varValue = entry.second.rawValue;
            
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
    map<string, Variable> lastContext;
};

#endif // DOFILE_LIBRARY_HPP
