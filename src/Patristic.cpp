//
//  Patristic.cpp
//  PhyCPP
//
//  Created by Mathieu on 23/09/2015.
//

#include "Patristic.hpp"

#include <iostream>

using namespace phycpp;

Patristic::Patristic(const Tree &tree): tree_(tree),matrix_(tree.getLeafNodeCount()), map_(tree.getNodeCount()), nodes_(tree.getLeafNodeCount()){
    for ( int i = 0; i < tree.getLeafNodeCount(); i++ ) {
        matrix_[i].resize(tree.getLeafNodeCount());
    }
}

Patristic::~Patristic(){

}

void Patristic::populateMap(Node &node){
    
    std::set<Node*> nodes;
    map_[node.getID()] = nodes;
    for ( int i = 0; i < node.childCount(); i++) {
        populateMap(*node.childAt(i));
    }
    if( !node.isLeaf() ){
        for ( int i = 0; i < node.childCount(); i++) {
            map_[node.getID()].insert(map_[node.childAt(i)->getID()].begin(), map_[node.childAt(i)->getID()].end());
        }
    }
    else{
        map_[node.getID()].insert(&node);
    }
}

void Patristic::calculate(){
    
    populateMap(*tree_.getRoot());
    
    for ( int i = 0; i < tree_.getNodeCount(); i++ ) {
        Node *node = tree_.getNode(i);
        if( node->isLeaf() ){
            matrix_[node->getClassID()][node->getClassID()] = 0;
            nodes_[node->getClassID()] = node;
        }
    }
    
    for ( int i = 0; i < tree_.getNodeCount(); i++ ) {
        Node *node1 = tree_.getNode(i);
        
        if( node1->isLeaf() ){
            for ( std::vector<Node*>::const_iterator nodeIter2 = nodes_.begin(); nodeIter2 != nodes_.end(); ++nodeIter2 ) {
                Node *node2 = *nodeIter2;
                if( node1 != node2 ){
                    matrix_[node1->getClassID()][node2->getClassID()] += node1->getDistance();
                    matrix_[node2->getClassID()][node1->getClassID()] = matrix_[node1->getClassID()][node2->getClassID()];
                    
                }
            }
        }
        else {
            std::set<Node*> nodes = map_[node1->getID()];
            // Iterate over leaf vector
            for ( std::vector<Node*>::const_iterator nodeIter2 = nodes_.begin(); nodeIter2 != nodes_.end(); ++nodeIter2 ) {
                Node *node2 = *nodeIter2;
                // node2 is not below node1
                if( nodes.find(node2) == nodes.end() ){
                    // add branch length of node1 to pairwise distance between node2 and every leaf below node1
                    for ( std::set<Node*>::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter ) {
                        matrix_[(*iter)->getClassID()][node2->getClassID()] += node1->getDistance();
                        matrix_[node2->getClassID()][(*iter)->getClassID()] = matrix_[(*iter)->getClassID()][node2->getClassID()];
                    }
                }
            }
        }
    }
}

const std::vector<std::vector<double>>& Patristic::getMatrix(){
    return matrix_;
}

const std::vector<Node*>& Patristic::getNodes(){
    return nodes_;
}
