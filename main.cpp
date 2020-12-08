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
    Reader(const std::string filename) {
        input_txt = filename;
    }
    virtual void execute(std::vector<std::string> &Text) override {
        std::ifstream input;
        input.open(input_txt);
        //std::cout << input_txt;
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
public:
    Greper(const std::string word) {

    }
    virtual void execute(std::vector<std::string> &Text) override {

    }
};
class Sorter : public Executor {
public:
    Sorter (){
        //output.open(filename);
    }
    virtual void execute(std::vector<std::string> &Text) override {
        std::sort(Text.begin(), Text.end());
    }
};
class Replacer : public Executor {
public:
    Replacer(const std::string word1, const std::string word2) {
        //output.open(filename);
    }
    virtual void execute(std::vector<std::string> &Text) override {

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
                    //std:: cout << block_number;
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
            blocks.push_back(create_block(type, par1, par2));
        }
        std::vector<std::string> Text;
        for (auto it : blocks) {
            it->execute(Text);
            //std::cout << Text.size();
        }
        /*while (std::getline(input, new_str)) { //определенный порядок
            static const std::regex to_extractor("[\\d]+");
            auto numbers_begin = std::sregex_iterator(new_str.begin(), new_str.end(), to_extractor);
            for (auto i = numbers_begin; i != std::sregex_iterator(); ++i) {

            }
        }*/
    }

private:
    std::list<Executor*> blocks;
};

int main(int argc, char* argv[]) {
    Workflow_Parser parse(argv[1]);
    return 0;
}
