#include "parser.h"

TopDownParser::TopDownParser(const std::string raw_rules, const std::string input):input_word(input){
    read_rules(raw_rules);
    start_parsing();

}

TopDownParser::~TopDownParser(){
    for (auto x : list_of_states){
        delete x;
    }
}


TopDownParser::ParsingState::ParsingState(){
    i=0;
    level=0;
    symbol_alternative=0;
}


TopDownParser::ParsingState::ParsingState(const ParsingState* ps){
    state=ps->state;
    i=ps->i;
    alpha=ps->alpha;
    beta=ps->beta;
    level=ps->level;
    symbol_alternative=ps->symbol_alternative;

}

TopDownParser::ParsingState* TopDownParser::ParsingState::new_parsing_state(){
    TopDownParser::ParsingState* ps_ptr=new TopDownParser::ParsingState();
    return ps_ptr;
}

TopDownParser::ParsingState* TopDownParser::ParsingState::copy(const ParsingState* ps){
    TopDownParser::ParsingState* copy_ptr=new TopDownParser::ParsingState(ps);
    return copy_ptr;
}


void TopDownParser::ParsingState::print_instance_to_std_err(){
    std::string st;
    switch(state){
        case TopDownParser::StateofAnalysis::Q:
            st="normal";
            break;
        case TopDownParser::StateofAnalysis::T:
            st="accept";
            break;
        case TopDownParser::StateofAnalysis::B:
            st="backtrack";
            break;
        
    }


    std::string alpha_top_string=alpha.top();
    
    std::cerr<<"Parser instance: "<<std::endl;

    std::cerr<<"state: "<<st<<std::endl;
    std::cerr<<"i: "<<i<<std::endl;
    std::cerr<<"alpha stack top element: "<<alpha_top_string<<std::endl;
    std::cerr<<"beta str: "<<beta<<std::endl;
    std::cerr<<"level: "<<level<<std::endl;
    std::cerr<<"symbol alternative: "<<symbol_alternative<<std::endl<<std::endl;

}

void TopDownParser::print_rules_content() {
    std::cout<<"Rules:"<<std::endl;
    for (const auto x : extract_rule_symbols()){
        std::cout<<x<<" -> ";
        for (const auto y : rules[x]){
            std::cout<<y<<", ";
        }
        std::cout<<std::endl;
    }
}

bool TopDownParser::is_capital(char letter){
    if (std::find(capitals.begin(),capitals.end(),letter)==capitals.end()){
        return false;
    }

    return true;
}


void TopDownParser::read_rules(const std::string &raw_rules){
    std::string curr_symbol;
    std::string curr_rule;
    bool after_arrow=false; // check if we are after the '>' in the rule declaration

    for (int i=0;i<raw_rules.length();++i){
        if (raw_rules[i]=='>'){
            after_arrow=true;
        }
        else if(raw_rules[i]=='-'){
            continue;
        }
        else if (raw_rules[i]==' '){
            rules[curr_symbol].push_back(curr_rule);
            curr_rule=curr_symbol="";
            after_arrow=false;
        }

        else if (after_arrow){
            curr_rule+=raw_rules[i];
                
        } 
        else {
            if (is_capital(raw_rules[i])){
                if (curr_symbol==""){
                    curr_symbol=raw_rules[i];
                }
                else {
                    std::cerr<<"Cannot give more than 1 symbols in one rule (S>adeSxa is valid, Sk>fnmekSlsS or SL>kjdfSlsK is not)"<<std::endl;
                    exit(-1);
                }
            }
            else {
                std::cerr<<"Symbol cannot be a terminal (lower case character): "<<raw_rules[i]<<std::endl;
                exit(-1);
            }

        }
    }

    if (curr_rule!="" && curr_symbol!=""){
        rules[curr_symbol].push_back(curr_rule);
        curr_rule=curr_symbol="";
        after_arrow=false;
    }

    std::vector<std::string> symbols=extract_rule_symbols();
    if(std::find(symbols.begin(), symbols.end(),"S")==symbols.end()){
        std::cerr<<"Starting symbol is required! (S)"<<std::endl;
        exit(-1);
    }

    for (const auto symb: extract_rule_symbols()){
        for (const auto rule:rules[symb]){
            for (const auto rule_char:rule){
                if (is_capital(rule_char)){
                    if (std::find(extract_rule_symbols().begin(),extract_rule_symbols().end(),std::string(1,rule_char))==extract_rule_symbols().end()){
                        std::cerr<<rule_char<<" has no member."<<std::endl;
                        exit(-1);
                    }
                }
            }
        }
    }
}


void TopDownParser::start_parsing(){
    TopDownParser::ParsingState* new_state=TopDownParser::ParsingState::new_parsing_state();
    // init the starting state of the parser
    new_state->state=TopDownParser::StateofAnalysis::Q;
    new_state->i=0;
    new_state->level=0;
    new_state->symbol_alternative=0;
    new_state->beta="S";

    list_of_states.push_back(new_state);
    
    std::cerr<<"Parsing has started..."<<std::endl;
    extend("S", 0);
}

void TopDownParser::extend(std::string non_terminal, unsigned int num_of_alternative){
    if (num_of_alternative>=rules[non_terminal].size()){
        if (list_of_states[list_of_states.size()-1]->beta=="S"){
            std::cerr<<"The input word is not an element of the given language"<<std::endl;
            std::cerr<<"Program exits..."<<std::endl;
            exit(-1);
        }
        backtrack_in_extension();
    }

    TopDownParser::ParsingState* new_state=TopDownParser::ParsingState::copy(list_of_states[list_of_states.size()-1]);
    list_of_states[list_of_states.size()-1]->symbol_alternative=num_of_alternative;
    new_state->alpha.push(non_terminal + std::to_string(num_of_alternative+1));
    new_state->beta.erase(0,1);
    new_state->beta=rules[non_terminal][num_of_alternative] + new_state->beta;
    new_state->level++;
    new_state->symbol_alternative=0;

    list_of_states.push_back(new_state);
    std::cerr<<non_terminal<<" was extended to the "<<num_of_alternative<<" alternative"<<std::endl;
    list_of_states[list_of_states.size()-1]->print_instance_to_std_err();
    match_input();

}


void TopDownParser::match_input(){
    std::cout<<"processed word length: "<<list_of_states[list_of_states.size()-1]->beta.length() + list_of_states[list_of_states.size()-1]->i<<std::endl;
    std::cout<<"input word length: "<<input_word.length()<<std::endl;
    if (list_of_states[list_of_states.size()-1]->beta.length() + list_of_states[list_of_states.size()-1]->i > input_word.length()){
        std::cerr<<"While matching the input the length of beta was bigger than the input word's length >> backtrack in the input..."<<std::endl;
        list_of_states[list_of_states.size()-1]->print_instance_to_std_err();
        backtrack_in_input();
    }
    else if (is_capital(list_of_states[list_of_states.size()-1]->beta[0])){
        std::cerr<<"While matching input the first char of beta was non-terminal >> extending further..."<<std::endl;
        list_of_states[list_of_states.size()-1]->print_instance_to_std_err();
        extend(std::string(1,list_of_states[list_of_states.size()-1]->beta[0]),0);
    }
    else if (list_of_states[list_of_states.size()-1]->beta[0]==input_word[list_of_states[list_of_states.size()-1]->i]){
        std::cerr<<"Succesful matching"<<std::endl;
        successful_matching();

    } 
    else if (list_of_states[list_of_states.size()-1]->beta[0]!=input_word[list_of_states[list_of_states.size()-1]->i]) {
        unsuccessful_matching();
    }
}


void TopDownParser::successful_matching(){
    TopDownParser::ParsingState* ps=TopDownParser::ParsingState::copy(list_of_states[list_of_states.size()-1]);
    ps->alpha.push(std::string(1,ps->beta[0]));
    ps->beta.erase(0,1);
    ps->i++;
    if (ps->i==input_word.length()){
        std::cout<<std::endl<<"The word is element of the grammar"<<std::endl;
        list_of_states[list_of_states.size()-1]->state=TopDownParser::StateofAnalysis::T;
        
        std::cout<<std::endl<<"Parse tree:"<<std::endl;
        std::vector<std::string> printable_tree;
        while (!list_of_states[list_of_states.size()-1]->alpha.empty()){
            if (list_of_states[list_of_states.size()-1]->alpha.top().length()!=1){
                // building up the parse tree
                printable_tree.push_back(list_of_states[list_of_states.size()-1]->alpha.top());

            }
            list_of_states[list_of_states.size()-1]->alpha.pop();
        }

        for (int i=printable_tree.size()-1;i>=0;i--){
            std::cout<<printable_tree[i];
        }
        std::cout<<std::endl;
    } else {
        list_of_states.push_back(ps);
        match_input();
    }
}


void TopDownParser::unsuccessful_matching(){
    std::cerr<<"The matching waws unsuccessful >> backtrack in the input"<<std::endl;
    list_of_states[list_of_states.size()-1]->print_instance_to_std_err();
    backtrack_in_input();
}

void TopDownParser::backtrack_in_input(){
    unsigned int lvl=list_of_states[list_of_states.size()-1]->level;
    while (true){
        if (list_of_states[list_of_states.size()-1]->level < lvl){
            list_of_states[list_of_states.size()-1]->state=TopDownParser::StateofAnalysis::B;
            extend(std::string(1,list_of_states[list_of_states.size()-1]->beta[0]),list_of_states[list_of_states.size()-1]->symbol_alternative+1);
            break;
        } else {
            delete list_of_states[list_of_states.size()-1];
            list_of_states.resize(list_of_states.size()-1);
        }
    }
}

void TopDownParser::backtrack_in_extension(){
    delete list_of_states[list_of_states.size()-1];
    list_of_states.resize(list_of_states.size()-1);
    extend(std::string(1,list_of_states[list_of_states.size()-1]->beta[0]),list_of_states[list_of_states.size()-1]->symbol_alternative+1);

}

std::vector<std::string> TopDownParser::extract_rule_symbols() const {
    std::vector<std::string> keys;

    for (auto const& element: rules){
        keys.push_back(element.first);
    }

    return keys;
}