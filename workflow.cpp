#include "workflow.h"

Reader::Reader(const std::string &filename) {
    input_txt = filename;
}

void Reader::execute(std::optional<std::vector<std::string>> &Text) {
    try {
        if (Text)
        {
            throw "File is already open";
        }
        std::ifstream input;
        input.open(input_txt);
        if (!input.is_open()) {
            throw "Input file isn't exist's";
        }
        std::vector<std::string> vec;
        Text.emplace(vec);
        std::string str;
        while (std::getline(input, str))
        {
            Text->push_back(str);
        }
        input.close();
    }
    catch (const char* exception) {
        std::cout << exception;
    }
}

Writer::Writer(const std::string &filename){
    output_txt = filename;
}
void Writer::execute(std::optional<std::vector<std::string>> &Text) {
    try {
        if (!Text) {
            throw "File is already closed or hasn't ever opened";
        }
        std::ofstream output;
        output.open(output_txt);
        if (!output.is_open()) {
            throw "Output file isn't exist's";
        }
        for (const auto& iter : *Text)
        {
            output << iter << std::endl;
        }
        output.close();
        Text.reset();
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}

Greper::Greper(const std::string &word){
    fword = word;
}
void Greper::execute(std::optional<std::vector<std::string>> &Text) {
    try {
        if (!Text) {
            throw "File is already closed or hasn't ever opened";
        }
        auto iter = Text->begin();
        while (iter != Text->end()) {
            std::regex expression("\\b"+fword+"\\b");
            std::sregex_token_iterator first(iter->begin(), iter->end(), expression);
            std::sregex_token_iterator last;
            if (std::distance(first, last) == 0) {
                iter = Text->erase(iter);
            }
            else
                ++iter;
        }
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}

void Sorter::execute(std::optional<std::vector<std::string>> &Text) {
    try {
        if (!Text) {
            throw "File is already closed or hasn't ever opened";
        }
        std::sort(Text->begin(), Text->end());
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}

Replacer::Replacer(const std::string &word1, const std::string &word2){
    old_word = word1;
    new_word = word2;
}
void Replacer::execute(std::optional<std::vector<std::string>> &Text) {
    try {
        if (!Text) {
            throw "File is already closed or hasn't ever opened";
        }
        const std::regex space_extractor("\\b"+old_word+"\\b");
        auto iter = Text->begin();
        while (iter != Text->end()) {
            *iter = std::regex_replace(*iter, space_extractor, new_word);
            ++iter;
        }
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}

Dumper::Dumper(const std::string &file) {
    filename = file;
}
void Dumper::execute(std::optional<std::vector<std::string>> &Text) {
    try {
        if (!Text) {
            throw "File is already closed or hasn't ever opened";
        }
        std::ofstream out;
        out.open(filename);
        if (!out.is_open()) {
            throw "Output file isn't exist's";
        }
        for (const auto& iter : *Text)
        {
            out << iter << std::endl;
        }
        out.close();
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}


Executor* create_block(char type, std::string &par1, std::string par2) {
    if (type == 'i') {
        return new Reader(par1);
    } else if (type == 'o') {
        return new Writer(par1);
    } else if (type == 'g') {
        return new Greper(par1);
    } else if (type == 'd') {
        return new Dumper(par1);
    } else if (type == 's') {
        return new Sorter();
    } else if (type == 'r') {
        return new Replacer(par1, par2);
    } else
        return new Executor;
}
