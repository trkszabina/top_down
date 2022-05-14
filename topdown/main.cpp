#include <iostream>
#include <string>
#include "parser.h"



int main(){
    std::string rules, input;

    std::cout<<"The inpus must follow the following rules"<<std::endl;
    std::cout<<"It has to have a starting symbol which is marked with ```S```"<<std::endl;
    std::cout<<"The rules follow the the following format: D>DkLsdk where D is a"<<std::endl;
    std::cout<<"non-terminal (all capitals are considered non-terminals)"<<std::endl<<std::endl;
    std::cout<<"Every rule must be given separately which means you cannot do for "<<std::endl;
    std::cout<<"for EG. S>sd,Lksd -> this has to be given in the following format: S>sd S>Lksd"<<std::endl;
    std::cout<<"The input must be only one line as well as the input word"<<std::endl<<std::endl;
    
    std::cout<<"Enter the rules:"<<std::endl;
    getline(std::cin,rules);

    std::cout<<"Enter the input word"<<std::endl;
    getline(std::cin,input);

    TopDownParser TDP(rules,input);

    TDP.print_rules_content();
 

    return 0;
}