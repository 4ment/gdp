//
//  Tree.cpp
//  PhyCPP
//
//  Created by Mathieu on 28/04/2015.
//

#include "Tree.hpp"

#include <string>

//#include "Newick.h"

using namespace phycpp;
using namespace std;


Tree::Tree(Node* node){
    id_ = -1;
    root_ = node;
    int internalCount = 0;
    tipCount_ = 0;
    nodeCount_ = 0;
    initIDs(root_, &internalCount);
}

Tree::Tree(const Tree& tree){
    id_ = tree.id_;
    root_ = new Node(*tree.root_);
    int internalCount = 0;
    tipCount_ = 0;
    nodeCount_ = 0;
    initIDs(root_, &internalCount);
}

Tree::~Tree(){
    delete root_;
}

int Tree::getNodeCount() const{
    return nodeCount_;
}

int Tree::getInternalNodeCount() const{
    return nodeCount_ - tipCount_;
}

int Tree::getLeafNodeCount() const{
    return tipCount_;
}


Node* Tree::getNode(int nodeID)const{
    return nodes_.at(nodeID);
}

const std::vector<Node*>& Tree::getNodes() const{
    return nodes_;
}

Node* Tree::getRoot()const{
    return root_;
}

void Tree::initIDs( Node* node, int* internalCount){
    for ( int i = 0; i < node->childCount(); i++ ) {
        initIDs(node->childAt(i), internalCount);
    }
    if ( node->isLeaf() ) {
        node->setClassID(tipCount_++);
    }
    else {
        node->setClassID((*internalCount)++);
    }
    node->setID(nodeCount_++);
    nodes_.push_back(node);
}

void Tree::newick(const Node& node, string& newickStr){
    
    if( node.isLeaf() ){
        newickStr += node.getName()+":"+std::to_string(node.getDistance());
    }
    else {
        newickStr += "(";
    }
    for ( int i = 0; i < node.childCount(); i++ ) {
        
        newick(*(node.childAt(i)), newickStr);
        if( i == node.childCount()-1 ){
            newickStr += ")";
            if( !node.isRoot() ){
                newickStr += node.getName()+":"+std::to_string(node.getDistance());
            }
        }
        else{
            newickStr += ",";
        }
    }
}

string Tree::newick(){
    string newickStr;
    newick(*root_, newickStr);
    return newickStr;
}

