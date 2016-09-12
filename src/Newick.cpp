//
//  Newick.cpp
//  PhyCPP
//
//  Created by Mathieu on 29/04/2015.
//

#include "Newick.hpp"

#include "Utils.hpp"

using namespace phycpp;
using namespace std;

Node * parse_newick( const string &nexus );


Newick::Newick(std::istream *inputstream){
    _inputstream = inputstream;
}

Tree * Newick::stringToTree(const std::string &newick){
    Node *root = parse_newick(newick);
    Tree *tree = new Tree(root);
    
    return tree;
}

// read first tree file
Tree * Newick::readTree() const{
    string newick,buffer;
    while ( ! _inputstream->eof() ) {
        getline(*_inputstream, buffer, '\n');
        string::size_type index = buffer.find(";");
        if(index != string::npos){
            newick += buffer.substr(0, index + 1);
            break;
        }
        else newick += buffer;
    }
    return Newick::stringToTree(newick);
}

vector<Tree*> * Newick::readTrees(size_t start, size_t end) const {
    string newick,buffer;
    vector<Tree*> *trees = new std::vector<Tree*>();
    int count = 0;
    while ( ! _inputstream->eof() ) {
        getline(*_inputstream, buffer, '\n');
        string::size_type index = buffer.find(";");
        if(index != string::npos){
            
            if(count > end) break;
            if(count >= start){
                newick += buffer.substr(0, index + 1);
                Tree *tree = Newick::stringToTree(newick);
                trees->push_back(tree);
                if( index + 1 == newick.size() ){
                    newick = "";
                }
                else {
                    newick.erase(newick.begin(),newick.begin()+index+1);
                }
                count++;
            }
        }
        else newick += buffer;
    }
    return trees;
}

// starts on a [
// finish with a ]
void _parse_comment( Node *node, const string &newick, int &i ){
    ++i;
    int start = i;
    string key;
    string value;
    //cout << &newick[i] << endl;
    
    while ( newick[i] != ']'){
        // it's an array
        if ( newick[i] == '{' ) {
            while ( newick[i] != '}'){
                ++(i);
            }
        }
        // it is a new record
        else if( newick[i] == ',' ){
            value = newick.substr(start,i-start);
            start = i+1;
            node->addAnnotation(key, value);
        }
        // we just parsed a key
        else if( newick[i] == '=' ){
            if( newick[start] == '&'){
                start++;
            }
            key = newick.substr(start,i-start);
            start = i+1;
        }
        ++i;
    }
    if ( value == "" ) {
        value = newick.substr(start,i-start-1);
        //cout << node->getName()<< " " << key << " "<<value<<endl;
        node->addAnnotation(key, value);
        
    }
}

// called just after a ) or a leaf name
// can start  with bootstrap value, node name, :, [
// return i on ')' or ',' or ;
void _parse_description( Node *node, const string &newick, int &i ){
    //cout << &newick[i]<< endl;
    while ( isspace( newick[i] ) ) ++i;
    
    if( newick[i] == ',' || newick[i] == ')' || newick[i] == ';' ) return;
    
    if( newick[i] == '[' ){
        _parse_comment(node, newick, i);
        i++;
    }
    
    // bootstrap or internal node name are ignored for now
    while ( newick[i] != ':' && newick[i] != ',' && newick[i] != ')' && newick[i] != ';' ) {
        i++;
    }
    
    double d = 0;
    string buffer;
    
    if( newick[i] == ':' ){
        i++;
        if( newick[i] == '[' ){
            _parse_comment(node, newick, i);
            ++i;
        }
        
        buffer.clear();
        
        while ( newick[i] != ',' && newick[i] != ')' && newick[i] != ';' ){
            buffer.append(&newick[i],1);
            i++;
        }
        d = stof(buffer);
    }
    
    node->setDistance(d);
}

Node * parse_newick( const string &nexus ){
    
    Node *current = 0;
    int i = 0;
    unsigned long l = nexus.size();
    
    string *buffer = new string();
    
    int count = 0;
    for ( i = 0; i < l; i++) {
        if ( nexus[i] == '(' ) {
            count++;
        }
        else if ( nexus[i] == ')' ) {
            count--;
        }
    }
    
    if ( count != 0 ){
        cerr << "The newick tree is malformed: Number opening parenthesis != number closing parenthesis" << endl;
        exit(1);
    }
    
    Node *root = current = new Node("node0", current);
    count = 1;
    
    for ( i = 1; i < l; i++) {
        // Internal node
        if( nexus[i] == '(' ){
            Node *n = new Node("node"+std::to_string(++count), current);
            current->addChild(n);
            current = n;
        }
        else if( nexus[i] == ',' ){
            //fprintf(stderr, "%c\n", nexus[i]);
        }
        else if( nexus[i] == ')' ){
            // we have reached the end and no info after the last )
            if( root == current && (i == l-1 || nexus[i] == ';') ){
                break;
            }
            // we have reached the last ) but there is some info for the root
            else if(root == current ){
                i++;
                _parse_description(current, nexus, i);
                break;
            }
            // internal node
            else {
                i++;
                _parse_description(current, nexus, i);
                i--;
                
                current = current->getParent();
            }
            
        }
        // Node ID: tip or internal
        // does not parse bootstrap or other values at each node
        else{
            buffer->clear();
            // read name
            while ( nexus[i] != ':' && nexus[i] != ',' && nexus[i] != ')' && nexus[i] != '[' ){
                buffer->append(&nexus[i],1);
                i++;
            }
            if ( (buffer->at(0) == '\'' && buffer->at(buffer->size()-1) == '\'')
                || (buffer->at(0) == '"' && buffer->at(buffer->size()-1) == '"') ) {
                buffer->at(buffer->size()-1) = '\0';
                buffer->erase(buffer->begin());
                buffer->erase(buffer->end());
            }
            Node *n = new Node(*buffer, current);
            
            _parse_description(n, nexus, i);
            --i;
            current->addChild(n);
            current = n->getParent();
        }
    }
    
    if( root != current ){
        std::cerr <<"Error creating tree (current different from root): "<<root->getName() << " != " << current->getName() <<endl;
        //exit(1);
    }
    
    delete buffer;
    
    return root;
}
