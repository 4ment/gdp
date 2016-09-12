//
//  Tree.hpp
//  PhyCPP
//
//  Created by Mathieu on 28/04/2015.
//

#ifndef __PhyCPP__Tree__
#define __PhyCPP__Tree__

#include <stdio.h>
#include <vector>
#include <string>

#include "Node.hpp"

namespace phycpp {
    
    class Tree{
        
    public:
        Tree(Node *node);
        
        Tree(const Tree& tree);
        
        virtual ~Tree();
        
        virtual int getNodeCount() const;
        
        virtual int getInternalNodeCount() const;
        
        virtual int getLeafNodeCount() const;
        
        virtual Node* getRoot()const;
        
        virtual Node* getNode(int nodeID)const;
        
        virtual const std::vector<Node*>& getNodes() const;
        
        std::string newick();
        
        void newick(const Node& node, std::string &newick);
        
//        // Iterator
//        tree_iterator begin();
//        // return an iterator pointing to the first item
//        // inorder
//        const_tree_iterator begin() const;
//        // constant version
//        tree_iterator end();
//        // return an iterator pointing just past the end of
//        // the tree data
//        const_tree_iterator end() const;
        
    private:
        void initIDs( Node *node, int *internalCount);
        
        std::vector<Node*> nodes_;
        int id_;
        Node* root_;
        int tipCount_;
        int nodeCount_;
        
    protected:
        
    };
}

#endif /* defined(__PhyCPP__Tree__) */
