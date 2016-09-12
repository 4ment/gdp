//
//  Newick.hpp
//  PhyCPP
//
//  Created by Mathieu on 29/04/2015.
//

#ifndef __PhyCPP__Newick__
#define __PhyCPP__Newick__

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

#include "Tree.hpp"
#include "TreeReader.hpp"

namespace phycpp {
    
    class Newick : public TreeReader{
        
    public:
        Newick(std::istream *inputstream);
        
        virtual Tree * readTree() const;
        
        virtual std::vector<Tree*> * readTrees(size_t start = 0, size_t end = UINT_MAX) const;
        
        static Tree *stringToTree(const std::string &newick);
        
    private:
        std::istream *_inputstream;
    protected:
        
    };
}
#endif /* defined(__PhyCPP__Newick__) */
