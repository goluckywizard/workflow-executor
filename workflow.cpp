#include "workflow.h"

Reader::Reader(const std::string &filename) {
    input_txt = filename;
}

void Reader::execute(Option_text &Text) {
    try {
        if (Text.isOpen)
        {
            throw "File is already open";
        }
        std::ifstream input;
        input.open(input_txt);
        if (!input.is_open()) {
            throw "Input file isn't exist's";
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
    catch (const char* exception) {
        std::cout << exception;
    }
}

Writer::Writer(const std::string &filename){
    output_txt = filename;
}
void Writer::execute(Option_text &Text) {
    try {
        if (!Text.isOpen) {
            throw "File is already closed or hasn't ever opened";
        }
        std::ofstream output;
        output.open(output_txt);
        if (!output.is_open()) {
            throw "Output file isn't exist's";
        }
        for (const auto& iter : Text.text)
        {
            output << iter << std::endl;
        }
        output.close();
        Text.text.clear();
        Text.isOpen = false;
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}

Greper::Greper(const std::string &word){
    fword = word;
}
void Greper::execute(Option_text &Text) {
    try {
        if (!Text.isOpen) {
            if (Text.wasAltInput || !Text.hasAltInput) {
                throw "File is already closed or hasn't ever opened";
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
    catch (const char *exception) {
        std::cout << exception;
    }
}

void Sorter::execute(Option_text &Text) {
    try {
        if (!Text.isOpen) {
            if (Text.wasAltInput || !Text.hasAltInput) {
                throw "File is already closed or hasn't ever opened";
            }
            Text.alt_read();
        }
        std::sort(Text.text.begin(), Text.text.end());
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}

Replacer::Replacer(const std::string &word1, const std::string &word2){
    old_word = word1;
    new_word = word2;
}
void Replacer::execute(Option_text &Text) {
    try {
        if (!Text.isOpen) {
            if (Text.wasAltInput || !Text.hasAltInput) {
                throw "File is already closed or hasn't ever opened";
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
    catch (const char *exception) {
        std::cout << exception;
    }
}

Dumper::Dumper(const std::string &file) {
    filename = file;
}
void Dumper::execute(Option_text &Text) {
    try {
        if (!Text.isOpen) {
            if (Text.wasAltInput || !Text.hasAltInput) {
                throw "File is already closed or hasn't ever opened";
            }
            Text.alt_read();
        }
        std::ofstream out;
        out.open(filename);
        if (!out.is_open()) {
            throw "Output file isn't exist's";
        }
        for (const auto& iter : Text.text)
        {
            out << iter << std::endl;
        }
        out.close();
    }
    catch (const char *exception) {
        std::cout << exception;
    }
}


std::shared_ptr <Executor> create_block(char type, std::string &par1, std::string par2) {
    if (type == 'i') {
        return std::make_shared<Reader>(par1);
    } else if (type == 'o') {
        return std::make_shared<Writer>(par1);
    } else if (type == 'g') {
        return std::make_shared<Greper>(par1);
    } else if (type == 'd') {
        return std::make_shared<Dumper>(par1);
    } else if (type == 's') {
        return std::make_shared<Sorter>();
    } else if (type == 'r') {
        return std::make_shared<Replacer>(par1, par2);
    } else
        return std::make_shared<Executor>();
}
