#ifndef TOP_DOWN_PARSER
#define TOP_DOWN_PARSER

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <cctype>
#include <memory>
#include <utility>
#include <algorithm>

class TopDownParser{
    enum StateofAnalysis{
        Q, // normal
        T, // acceptable
        B  // backtrack
    };

    struct ParsingState{
        StateofAnalysis state;
        unsigned int i; // pointer in the input word
        std::stack<std::string> alpha;
        std::string beta;

        // divide the operations into levels to make backtrack easier
        unsigned int level;

        // helping in the backtrack keeping track of which alternative are we using
        // currently
        // in case of backtrack we only increase this number to try
        // the next alternative of the sybol
        unsigned int symbol_alternative;

        ParsingState();
        ParsingState(const ParsingState* ps); // copy constructor
        static ParsingState* new_parsing_state();
        static ParsingState* copy(const ParsingState* ps);
        void print_instance_to_std_err();
        // TODO: print out the tree in the end
    };

    std::vector<char> capitals={'Q','W','E','R','T','Y','U','I','O','P',
                                'A','S','D','F','G','H','J','K','L','Z','X','C',
                                'V','B','N','M'};

    
    bool is_capital(char letter);
    

    std::map<std::string,std::vector<std::string>> rules;

    void read_rules(const std::string &raw_rules);
    std::vector<std::string> extract_rule_symbols() const; // extract the key element from the rules


    std::vector<ParsingState*> list_of_states; // for backtrack purposes

    void start_parsing();
    void match_input();
    void extend(std::string non_terminal, unsigned int num_of_alternative);
    void backtrack_in_extension();
    void backtrack_in_input();
    void successful_matching();
    void unsuccessful_matching();


    std::string input_word;

public:

    TopDownParser(const std::string raw_rules, const std::string input);
    ~TopDownParser();
    void print_rules_content();

};

#endif