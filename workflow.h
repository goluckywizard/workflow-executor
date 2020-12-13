#ifndef WORKFLOW_EXECUTOR_WORKFLOW_H
#define WORKFLOW_EXECUTOR_WORKFLOW_H
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <list>
#include <optional>
#include <memory>
#include <map>
class Option_text {
public:
    bool isOpen = false;
    bool hasAltInput = false;
    bool hasAltOutput = false;
    bool wasAltInput = false;
    bool wasAltOutput = false;
    std::string altInputName;
    std::string altOutputName;
    void set_input(std::string &filename) {
        hasAltInput = true;
        altInputName = filename;
    }
    void set_output(std::string &filename) {
        hasAltOutput = true;
        altOutputName = filename;
    }
    void alt_read() {
        try {
            std::ifstream input;
            input.open(altInputName);
            if (!input.is_open()) {
                throw "Alternative input file isn't exist's";
            }
            std::vector<std::string> vec;
            //Text.emplace(vec);
            std::string str;
            while (std::getline(input, str))
            {
                text.push_back(str);
            }
            input.close();
            isOpen = true;
            wasAltInput = true;
        }
        catch (const char *exception) {
            std::cout << exception;
        }
    }
    void alt_write() {
        try {
            std::ofstream output;
            output.open(altOutputName);
            if (!output.is_open()) {
                throw "Alternative output file isn't exist's";
            }
            for (const auto& iter : text)
            {
                output << iter << std::endl;
            }
            output.close();
            text.clear();
            isOpen = false;
            wasAltOutput = true;
        }
        catch (const char *exception) {
            std::cout << exception;
        }
    }
    std::vector<std::string> text;
};

class Executor {
public:
    virtual void execute(Option_text &Text){}
};

class Reader : public Executor {
    std::string input_txt;
public:
    explicit Reader(const std::string &filename);
    void execute(Option_text &Text) override;
};

class Writer : public Executor {
    std::string output_txt;
public:
    explicit Writer(const std::string &filename);
    void execute(Option_text &Text) override;
};

class Greper : public Executor {
    std::string fword;
public:
    explicit Greper(const std::string &word);
    void execute(Option_text &Text) override ;
};

class Sorter : public Executor {
public:
    Sorter ()= default;
    void execute(Option_text &Text) override;
};

class Replacer : public Executor {
    std::string old_word;
    std::string new_word;
public:
    Replacer(const std::string &word1, const std::string &word2);
    void execute(Option_text &Text) override;
};

class Dumper : public Executor {
    std::string filename;
public:
    explicit Dumper(const std::string &file);
    void execute(Option_text &Text) override;
};


std::shared_ptr <Executor> create_block(char type, std::string &par1, std::string par2 = nullptr);

class Workflow_Parser {
private:
    std::map<int, std::shared_ptr<Executor>> blocks;
    std::list<std::shared_ptr<Executor>> block_list;
    std::ifstream input;
    //char *tag1 = nullptr;
    bool hasAlternaiveInput = false;
    char *alt_i;
    bool hasAlternaiveOutput = false;
    char *alt_o;

    void blocks_parser() {
        std::string new_str;
        while (std::getline(input, new_str)) {
            if (new_str == "desc")
                break;
        }
        while (std::getline(input, new_str)) {
            if (new_str == "csed")
                break;
            static const std::regex space_extractor("[\\w,\\.]+");
            auto newstr_begin = std::sregex_iterator(new_str.begin(), new_str.end(), space_extractor);
            int iteration = 0;
            std::optional<int> block_number;
            std::optional<char> type;
            std::optional<std::string> par1, par2;
            par2.emplace("");
            for (auto i = newstr_begin; i != std::sregex_iterator(); ++i) {
                if (iteration == 0)
                {
                    std::string qwe = i -> str();
                    block_number.emplace(std::atoi(qwe.c_str()));
                    iteration++;
                } else if (iteration == 1) {
                    std::string str = i->str();
                    if (str == "readfile")
                    {
                        type.emplace('i');
                    }
                    if (str == "writefile")
                    {
                        type.emplace('o');
                    }
                    if (str == "replace")
                    {
                        type.emplace('r');
                    }
                    if (str == "dump")
                    {
                        type.emplace('d');
                    }
                    if (str == "sort")
                    {
                        type.emplace('s');
                    }
                    if (str == "grep")
                    {
                        type.emplace('g');
                    }
                    iteration++;
                } else if (iteration == 2)
                {
                    std::string str = i->str();
                    par1.emplace(str);
                    iteration++;
                } else if (iteration == 3)
                {
                    std::string str = i->str();
                    par2.emplace(str);
                    iteration++;
                }
            }
            try {
                if (!(type && block_number && par1)) {
                    throw "Not enough arguments";
                }
                std::shared_ptr<Executor> temp_pointer (create_block(*type, *par1, *par2));
                if (blocks[*block_number])
                {
                    throw "Blocks can't be overloaded";
                }
                else
                {

                    blocks[*block_number] = std::move(temp_pointer);
                }
            }
            catch (char* exception) {
                std::cout << std::endl;
            }
        }
        //std::string new_str;
        while (std::getline(input, new_str)) {
            static const std::regex to_extractor("[\\d]+");
            auto numbers_begin = std::sregex_iterator(new_str.begin(), new_str.end(), to_extractor);
            for (auto i = numbers_begin; i != std::sregex_iterator(); ++i) {
                std::string id_number = i->str();
                int id = std::atoi(id_number.c_str());
                if (!blocks[id])
                {
                    std::cout << "Haven't description for this index: " << id << std::endl;
                }
                else
                {
                    //blocks[id]->execute(Text);
                    block_list.push_back(blocks[id]);
                }
            }
        }
    }
    void do_execute() {

        Option_text text;
        if (hasAlternaiveInput) {
            text.hasAltInput = true;
            text.altInputName = alt_i;
        }
        if (hasAlternaiveOutput) {
            text.hasAltOutput = true;
            text.altOutputName = alt_o;
        }

        for (auto &it : block_list) {
            it->execute(text);
        }
        if (text.isOpen)
        {
            if (text.hasAltOutput && !text.wasAltOutput) {
                text.alt_write();
            }
            else {
                std::cout << "Text aren't closed!";
            }
        }
        //auto iter = block_list.begin();
        //if (*iter )
    }
public:
    Workflow_Parser(char *workflow[], int argc) {
        if (argc > 2) {
            /*if (strncmp(workflow[2], "-i", 2) == 0){
                hasAlternaiveInput = true;
                alt_i = workflow[3];
            }*/
            if (strncmp(workflow[2], "-o", 2) == 0){
                hasAlternaiveOutput = true;
                alt_o = workflow[3];
            }
        }
        if (argc > 4) {
            if (strncmp(workflow[4], "-i", 2) == 0){
                hasAlternaiveInput = true;
                alt_i = workflow[5];
            }
            if (strncmp(workflow[4], "-o", 2) == 0){
                hasAlternaiveOutput = true;
                alt_o = workflow[5];
            }
        }

        input.open(workflow[1]);
        blocks_parser();
        do_execute();
    }
};
#endif
