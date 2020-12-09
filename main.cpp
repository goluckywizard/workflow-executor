#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <list>
#include <iterator>
class Executor {
public:
    virtual void execute(std::vector<std::string> &Text){}
};
class Reader : public Executor {
    std::string input_txt;
public:
    Reader(const std::string filename) {
        input_txt = filename;
    }
    virtual void execute(std::vector<std::string> &Text) override {
        std::ifstream input;
        input.open(input_txt);
        std::string str;
        while (std::getline(input, str))
        {
            Text.push_back(str);
        }
        input.close();
    }
};
class Writer : public Executor {
    std::string output_txt;
public:
    Writer(const std::string filename) {
        output_txt = filename;
    }
    virtual void execute(std::vector<std::string> &Text) override {
        std::ofstream output;
        output.open(output_txt);
        for (const auto& iter : Text)
        {
            output << iter << std::endl;
        }
        output.close();
    }
};
class Greper : public Executor {
    std::string fword;
public:
    Greper(const std::string word) {
        fword = word;
    }
    virtual void execute(std::vector<std::string> &Text) override {
        auto iter = Text.begin();
        while (iter != Text.end()) {
            std::regex expression("\\b"+fword+"\\b");
            std::sregex_token_iterator first(iter->begin(), iter->end(), expression);
            std::sregex_token_iterator last;
            //std::cout << iter << " " << std::endl;
            if (std::distance(first, last) == 0) {
                iter = Text.erase(iter);
            }
            else
                ++iter;
        }
    }
};
class Sorter : public Executor {
public:
    Sorter (){}
    virtual void execute(std::vector<std::string> &Text) override {
        std::sort(Text.begin(), Text.end());
    }
};
class Replacer : public Executor {
    std::string old_word;
    std::string new_word;
public:
    Replacer(const std::string word1, const std::string word2) {
        old_word = word1;
        new_word = word2;
        //std::cout << "constr:" << word1 << "," << word2 << std::endl;
    }
    virtual void execute(std::vector<std::string> &Text) override {
        const std::regex space_extractor("\\b"+old_word+"\\b");
        auto iter = Text.begin();
        while (iter != Text.end()) {
            //std::cout << "old_word:" << old_word << " new_word:" << new_word << "\\\\\\\\";
            //auto newstr_begin = std::sregex_iterator(iter->begin(), iter->end(), space_extractor);
            *iter = std::regex_replace(*iter, space_extractor, new_word);
            ++iter;
        }
    }
};
class Dumper : public Executor {
    std::string filename;
public:
    Dumper(const std::string file) {
        filename = file;
    }
    virtual void execute(std::vector<std::string> &Text) override {
        std::ofstream out;
        out.open(filename);
        for (const auto& iter : Text)
        {
            out << iter << std::endl;
        }
        out.close();
    }
};

Executor* create_block(char type, std::string &par1, std::string par2 = nullptr) {
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
class Workflow_Parser {
    struct Block {
        int index;
        Executor* block_n;
    };
public:
    Workflow_Parser(char *in) {
        //std::cout << in;
        std::ifstream input;
        input.open(in);
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
        std::vector<std::string> Text;
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

private:
    std::list<Block> blocks;
};

int main(int argc, char* argv[]) {
    Workflow_Parser parse(argv[1]);
    return 0;
}
