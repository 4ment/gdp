//
//  Node.hpp
//  PhyCPP
//
//  Created by Mathieu on 28/04/2015.
//

#ifndef __PhyCPP__Node__
#define __PhyCPP__Node__

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

namespace phycpp {
    class Node{

    public:
        Node(const std::string& name, Node* parent = nullptr);

        Node(const Node& node);
        
        virtual ~Node();
                
        const std::string& getName() const;
        
        void setName(const std::string& name);
        
        int getID() const;
        
        void setID(int ID);
        
        int getClassID() const;
        
        void setClassID(int ID);
        
        double getDistance() const;
        
        void setDistance(double distance);
        
        Node* getParent() const;
        
        void setParent(Node* parent);
        
        unsigned long childCount() const;
        
        Node* childAt(unsigned long i) const;
        
        bool addChild(Node* node);
        
        bool removeChild(Node* node);
        
        void swap( Node& node1, Node& node2 );
        
        bool isRoot() const;
        
        bool isLeaf() const;
        
        bool  containsAnnotation(const std::string& key) const;
        
        void addAnnotation(const std::string& key, const std::string& value);
        
        const std::string& getAnnotation(const std::string& key);
        
        void removeAnnotation(const std::string& key);
        
    private:
        std::string name_;
        int id_;
        int class_id_;
        Node* parent_;
        std::vector<Node*> children_;
        std::map<std::string,std::string> annotations_;
        double distance_;
        
    protected:
        
    };
}

#endif /* defined(__PhyCPP__Node__) */
