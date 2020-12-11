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
class Executor {
public:
    virtual void execute(std::optional<std::vector<std::string>> &Text){}
};

class Reader : public Executor {
    std::string input_txt;
public:
    explicit Reader(const std::string &filename);
    void execute(std::optional<std::vector<std::string>> &Text) override;
};

class Writer : public Executor {
    std::string output_txt;
public:
    explicit Writer(const std::string &filename);
    void execute(std::optional<std::vector<std::string>> &Text) override;
};

class Greper : public Executor {
    std::string fword;
public:
    explicit Greper(const std::string &word);
    void execute(std::optional<std::vector<std::string>> &Text) override ;
};

class Sorter : public Executor {
public:
    Sorter ()= default;
    void execute(std::optional<std::vector<std::string>> &Text) override;
};

class Replacer : public Executor {
    std::string old_word;
    std::string new_word;
public:
    Replacer(const std::string &word1, const std::string &word2);
    void execute(std::optional<std::vector<std::string>> &Text) override;
};

class Dumper : public Executor {
    std::string filename;
public:
    explicit Dumper(const std::string &file);
    void execute(std::optional<std::vector<std::string>> &Text) override;
};

std::unique_ptr <Executor> create_block(char type, std::string &par1, std::string par2 = nullptr);

class Workflow_Parser {
private:
    std::map<int, std::unique_ptr<Executor>> blocks;
    std::ifstream input;
    void listblocks_parser() {
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
                std::unique_ptr<Executor> temp_pointer (create_block(*type, *par1, *par2));
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
    }
    void do_execute() {
        std::optional<std::vector<std::string>> Text;
        std::string new_str;
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
                    blocks[id]->execute(Text);
                }
            }
        }
    }
public:
    explicit Workflow_Parser(char *in) {
        input.open(in);
        listblocks_parser();
        do_execute();
    }
};
#endif
