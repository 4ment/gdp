//
//  Node.cpp
//  PhyCPP
//
//  Created by Mathieu on 28/04/2015.
//

#include "Node.hpp"

#include <algorithm>
#include <iostream>

using namespace phycpp;
using namespace std;


Node::Node(const string &name, Node *parent){
    name_ = name;
    parent_ = parent;
    distance_ = 0;
    id_ = -1;
    class_id_ = -1;
}

Node::Node(const Node& node){
    name_ = node.name_;
    parent_ = node.parent_;
    annotations_ = node.annotations_;
    distance_ = node.distance_;
    id_ = node.id_;
    class_id_ = node.class_id_;
    
    for (int i = 0; i < node.childCount(); i++) {
        children_.push_back(new Node(*node.children_.at(i)));
    }
    
}

Node::~Node(){
    for (int i = 0; i < childCount(); i++) {
        delete children_.at(i);
    }
}

const string & Node::getName() const{
    return name_;
}

void Node::setName(const string &name){
    name_ = name;
}

int Node::getID() const{
    return id_;
}

void Node::setID(int ID){
    id_ = ID;
}

int Node::getClassID() const{
    return class_id_;
}

void Node::setClassID(int ID){
    class_id_ = ID;
}

double Node::getDistance() const{
    return distance_;
}

void Node::setDistance(double distance){
    distance_ = distance;
}

Node * Node::getParent() const{
    return parent_;
}

void Node::setParent( Node *parent ){
    parent_ = parent;
}

unsigned long Node::childCount() const{
    return children_.size();
}

Node * Node::childAt(unsigned long i) const{
    return children_.at(i);
}

bool Node::addChild(Node *node){
    if ( find(children_.begin(), children_.end(), node) == children_.end() ) {
        children_.push_back(node);
        return true;
    }
    return false;
}

bool Node::removeChild(Node *node){
    for( unsigned long i = 0; i < children_.size(); i++) {
        if(children_.at(i) == node){
            children_.erase(children_.begin() + i);
            return true;
        }
    }
    return false;
}

void Node::swap( Node &node1, Node &node2 ){
    std::swap(node1, node2);
}

bool Node::isRoot() const{
    return parent_ == nullptr;
}

bool Node::isLeaf() const{
    return children_.size() == 0;
}

void Node::addAnnotation(const std::string &key, const std::string &value){
    annotations_.insert(pair<string, string>(key,value));
}

const std::string & Node::getAnnotation(const std::string &key){
    return annotations_.at(key);
}

bool  Node::containsAnnotation(const std::string &key) const{
    map<string,string>::const_iterator it = annotations_.find(key);
    return !( it == annotations_.end() );
}

void Node::removeAnnotation(const std::string &key){
    annotations_.erase(key);
}

