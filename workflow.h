#ifndef WORKFLOW_EXECUTOR_WORKFLOW_H
#define WORKFLOW_EXECUTOR_WORKFLOW_H
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <list>
class Executor {
public:
    virtual void execute(std::vector<std::string> &Text){}
};

class Reader : public Executor {
    std::string input_txt;
public:
    explicit Reader(const std::string &filename);
    void execute(std::vector<std::string> &Text) override;
};

class Writer : public Executor {
    std::string output_txt;
public:
    explicit Writer(const std::string &filename);
    void execute(std::vector<std::string> &Text) override;
};

class Greper : public Executor {
    std::string fword;
public:
    explicit Greper(const std::string &word);
    void execute(std::vector<std::string> &Text) override ;
};

class Sorter : public Executor {
public:
    Sorter ()= default;
    void execute(std::vector<std::string> &Text) override;
};

class Replacer : public Executor {
    std::string old_word;
    std::string new_word;
public:
    Replacer(const std::string &word1, const std::string &word2);
    void execute(std::vector<std::string> &Text) override;
};

class Dumper : public Executor {
    std::string filename;
public:
    explicit Dumper(const std::string &file);
    void execute(std::vector<std::string> &Text) override;
};

Executor* create_block(char type, std::string &par1, std::string par2 = nullptr);

class Workflow_Parser {
private:
    struct Block {
        int index;
        Executor* block_n;
    };
    std::list<Block> blocks;
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
            int block_number;
            char type;
            std::string par1 = "", par2 = "";
            for (auto i = newstr_begin; i != std::sregex_iterator(); ++i) {
                if (iteration == 0)
                {
                    std::string qwe = i -> str();
                    block_number = std::atoi(qwe.c_str());
                    iteration++;
                } else if (iteration == 1) {
                    std::string str = i->str();
                    type = str[0];
                    if (str == "readfile")
                    {
                        type = 'i';
                    }
                    if (str == "writefile")
                    {
                        type = 'o';
                    }
                    iteration++;
                } else if (iteration == 2)
                {
                    std::string str = i->str();
                    par1 = str;
                    iteration++;
                } else if (iteration == 3)
                {
                    std::string str = i->str();
                    par2 = str;
                    iteration++;
                }
                //std::cout << iteration << i -> str() << std::endl;
            }
            //std::cout << par1 << par2;
            Block b{};
            b.index = block_number;
            b.block_n = create_block(type, par1, par2);
            auto iter = blocks.begin();
            while (iter != blocks.end() && iter->index < b.index)
            {
                ++iter;
            }
            if (iter == blocks.begin()) {
                blocks.push_back(b);
            }
            else {
                blocks.insert(iter, b);
            }
        }
    }
    void do_execute() {
        std::vector<std::string> Text;
        std::string new_str;
        while (std::getline(input, new_str)) { //определенный порядок
            static const std::regex to_extractor("[\\d]+");
            auto numbers_begin = std::sregex_iterator(new_str.begin(), new_str.end(), to_extractor);
            for (auto i = numbers_begin; i != std::sregex_iterator(); ++i) {
                std::string id_number = i->str();
                int id = std::atoi(id_number.c_str());
                auto block_iter = blocks.begin();
                while (block_iter != blocks.end()) {
                    if (block_iter->index == id) {
                        //std::cout << id;
                        block_iter->block_n->execute(Text);
                        break;
                    }
                    ++block_iter;
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
