//
//  TreeReader.hpp
//  PhyCPP
//
//  Created by Mathieu on 26/05/2015.
//

#ifndef PhyCPP_TreeReader_h
#define PhyCPP_TreeReader_h

#include <vector>

#include "Tree.hpp"

namespace phycpp {
    
    class TreeReader{

    public:
        
        virtual ~TreeReader(){}
        
        virtual Tree * readTree() const = 0;
        
        virtual std::vector<Tree*>* readTrees(size_t start = 0, size_t end = UINT_MAX) const = 0;

    };
}
#endif
