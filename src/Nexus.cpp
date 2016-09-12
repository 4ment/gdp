//
//  Nexus.cpp
//  PhyCPP
//
//  Created by Mathieu on 19/05/2015.
//

#include "Nexus.hpp"

#include <regex>


#include "Newick.hpp"
#include "Utils.hpp"

using namespace phycpp;
using namespace std;
using namespace std::regex_constants;


Nexus::Nexus(std::istream *inputstream){
    _inputstream = inputstream;
}

// read first tree file
Tree * Nexus::readTree() const{
    vector<Tree*> * trees = readTrees();
    Tree * tree = NULL;
    if(trees->size() > 0 ){
        tree = trees->at(0);
        delete trees;
    }
    return tree;
}

vector<Tree*> * Nexus::readTrees(unsigned int start, unsigned int end) const {
    regex treesBlockRegex ("^begin\\s+trees;.*", ECMAScript|icase);
    vector<Tree*> * trees = new vector<Tree*>();
    string buffer;
    while ( ! _inputstream->eof() ) {
        getline(*_inputstream, buffer, '\n');
        if( regex_match(buffer,treesBlockRegex) ){
            readTreesBlock(trees, start, end);
        }
    }
    return trees;
}

void Nexus::readTreesBlock(vector<Tree*> *trees, unsigned int start, unsigned int end)const {
    map<string, string>* translate = new map<string, string>();

    string line;
    regex treeRegex ("^tree.*", ECMAScript|icase);
    regex translateRegex ("^translate.*", ECMAScript|icase);
    regex endBlockRegex ("^end;.*", ECMAScript|icase);
    
    int count = 0;
    
    while ( ! _inputstream->eof() ){
        getline(*_inputstream, line, '\n');
        trim(line);
        
        if( line.size() == 0 ){
            continue;
        }
        
        if( regex_match(line, endBlockRegex) ){
            break;
        }
        
        
        if( regex_match(line,treeRegex) ){
            
            if(count >= start){
                if(count > end) break;
                
                int i = 4;
                while ( line[i] != '(') {
                    if( line[i] == '['){
                        while ( line[i] != ']') {
                            i++;
                        }
                    }
                    i++;
                }
                // we don't want comments at the end if any
                std::size_t index = line.find(";");
                if ( index == std::string::npos ) {
                    index = line.size();
                }
                string newick = line.substr(i,index-i);
                //cout << newick <<endl;
                Tree *tree = Newick::stringToTree(newick);
                trees->push_back(tree);
            }
            count++;
        }
        else if( regex_match(line, translateRegex) ){

            bool done = false;
            while( !_inputstream->eof() && !done ) {
                line = nextLineUncommented();
                trim(line);
                //cout << "translate "<<line <<endl;
                
                if( line == ";" ) break;
                
                std::size_t found = line.find(";");
                
                if( found != std::string::npos ){
                    line = line.substr(0,found);
                    done = true;
                }
                
                found = line.find(",");
                
                if(found == std::string::npos){
                    std::size_t index = line.find_first_of("\t ");
                    string shorthand = line.substr(0,index);
                    line.erase(0,index+1);
                    if( line[0] == '\'' || line[0] == '\"'){
                        line = line.substr(1, line.size()-2);
                    }
                    //cout << shorthand << " = "<<line<<endl;
                    translate->insert(std::pair<string,string>(shorthand, line));
                }
                else {
                    std::size_t start = 0;
                    
                    while (found != std::string::npos) {
                        string temp = line.substr(start,found);
                        if(temp.size() != 0 ){
                            std::size_t index = temp.find_first_of("\t ");
                            string shorthand = temp.substr(0,index);
                            temp.erase(0,index+1);
                            trim(temp);
                            if( temp[0] == '\'' || temp[0] == '\"'){
                                temp = temp.substr(1, temp.size()-2);
                            }
                            //cout << shorthand << " = "<<temp<<endl;
                            translate->insert(std::pair<string,string>(shorthand, temp));
                        }
                        start = found+1;
                        found = line.find(",", start);
                    }
                }
            }
        }
    }
    
    if( translate->size() > 0 ){
        for ( int i = 0; i < trees->size(); i++ ) {
            Tree *tree = trees->at(i);
            for ( int i = 0; i < tree->getNodeCount(); i++ ) {
                Node *node = tree->getNode(i);
                if( node->isLeaf() ){
                    //cout <<"node name "<<node->getName() << endl;
                    node->setName(translate->at(node->getName()));
                }
            }
        }
    }
    delete translate;
}

// it can return an empty string if it is a (single or multiple line) comment with nothing around
// assumes one comment per line
// assumes it is not eof
string Nexus::nextLineUncommented()const{

    string buffer;
    
    getline(*_inputstream, buffer, '\n');
    
    std::size_t indexStart = buffer.find_first_of("[");
    
    if( indexStart == std::string::npos ){
        return buffer;
    }
    
    string line = buffer;
    
    std::size_t indexClose = buffer.find_first_of("]");
    
    // single line comment
    if( indexClose != std::string::npos ){
        line.erase(indexStart,indexClose-indexStart);
    }
    // multiple line comment
    else {
        while ( indexClose == std::string::npos ) {
            getline(*_inputstream, buffer, '\n');
            indexClose = buffer.find_first_of("]");
        }
        buffer.erase(0,indexClose);
        line += buffer;
    }
    
    return line;
}

bool Nexus::findBlock(const string &blockName)const{
    bool found = false;
    string regexString = "^begin\\s+" + blockName+".*";
    regex blockRegex (regexString, ECMAScript|icase);
    string buffer;
    while ( ! _inputstream->eof() ) {
        getline(*_inputstream, buffer, '\n');
        if( regex_match(buffer,blockRegex) ){
            found = true;
            break;
        }
    }
    return found;
}

