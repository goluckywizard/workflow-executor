#include "workflow.h"

Reader::Reader(const std::string &filename) {
    input_txt = filename;
}

void Reader::execute(Option_text &Text) {
    if (Text.isOpen)
    {
        throw WorkflowException(10, "Error with reading: Text is already opened");
    }
    std::ifstream input;
    input.open(input_txt);
    if (!input.is_open()) {
        throw WorkflowException(15, "Error with reading: Input file isn't exist's");
    }
    std::vector<std::string> vec;
    //Text.emplace(vec);
    std::string str;
    while (std::getline(input, str))
    {
        Text.text.push_back(str);
    }
    input.close();
    Text.isOpen = true;
}

Writer::Writer(const std::string &filename){
    output_txt = filename;
}
void Writer::execute(Option_text &Text) {
    if (!Text.isOpen) {
        throw WorkflowException(33, "Error with writing: File has already closed or hasn't ever opened");
    }
    std::ofstream output;
    output.open(output_txt);
    if (!output.is_open()) {
        throw WorkflowException(38, "Error with writing: Output file isn't exist's");
    }
    for (const auto& iter : Text.text)
    {
        output << iter << std::endl;
    }
    output.close();
    Text.text.clear();
    Text.isOpen = false;
}

Greper::Greper(const std::string &word){
    fword = word;
}
void Greper::execute(Option_text &Text) {
    if (!Text.isOpen) {
        if (Text.wasAltInput || !Text.hasAltInput) {
            throw WorkflowException(55, "Error with execute: File is already closed or hasn't ever opened");
        }
        Text.alt_read();
    }
    auto iter = Text.text.begin();
    while (iter != Text.text.end()) {
        std::regex expression("\\b"+fword+"\\b");
        std::sregex_token_iterator first(iter->begin(), iter->end(), expression);
        std::sregex_token_iterator last;
        if (std::distance(first, last) == 0) {
            iter = Text.text.erase(iter);
        }
        else
            ++iter;
    }
}

void Sorter::execute(Option_text &Text) {
    if (!Text.isOpen) {
        if (Text.wasAltInput || !Text.hasAltInput) {
            throw WorkflowException(75, "Error with sort: File is already closed or hasn't ever opened");
        }
        Text.alt_read();
    }
    std::sort(Text.text.begin(), Text.text.end());
}

Replacer::Replacer(const std::string &word1, const std::string &word2){
    old_word = word1;
    new_word = word2;
}
void Replacer::execute(Option_text &Text) {
    if (!Text.isOpen) {
        if (Text.wasAltInput || !Text.hasAltInput) {
            throw WorkflowException(89, "Error with replace: File is already closed or hasn't ever opened");
        }
        Text.alt_read();
    }
    const std::regex space_extractor("\\b"+old_word+"\\b");
    auto iter = Text.text.begin();
    while (iter != Text.text.end()) {
        *iter = std::regex_replace(*iter, space_extractor, new_word);
        ++iter;
    }
}

Dumper::Dumper(const std::string &file) {
    filename = file;
}
void Dumper::execute(Option_text &Text) {
    if (!Text.isOpen) {
        if (Text.wasAltInput || !Text.hasAltInput) {
            throw WorkflowException(108, "Error with dump: File is already closed or hasn't ever opened");
        }
        Text.alt_read();
    }
    std::ofstream out;
    out.open(filename);
    if (!out.is_open()) {
        throw WorkflowException(115, "Error with dump: Output file isn't exist's");
    }
    for (const auto& iter : Text.text)
    {
        out << iter << std::endl;
    }
    out.close();
}


std::shared_ptr <Executor> create_block(std::string type, std::string par1, std::string par2) {
    if (type == "readfile") {
        return std::make_shared<Reader>(par1);
    } else if (type == "writefile") {
        return std::make_shared<Writer>(par1);
    } else if (type == "grep") {
        return std::make_shared<Greper>(par1);
    } else if (type == "dump") {
        return std::make_shared<Dumper>(par1);
    } else if (type == "sort") {
        return std::make_shared<Sorter>();
    } else if (type == "replace") {
        return std::make_shared<Replacer>(par1, par2);
    }
    else {
        throw WorkflowException(139, "Error with creating new block");
    }
}
