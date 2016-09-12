//
//  Nexus.hpp
//  PhyCPP
//
//  Created by Mathieu on 19/05/2015.
//

#ifndef __PhyCPP__Nexus__
#define __PhyCPP__Nexus__

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

#include "Tree.hpp"
#include "TreeReader.hpp"

namespace phycpp {
    
    class Nexus : public TreeReader{
        
    public:
        Nexus(std::istream *inputstream);
        
        virtual ~Nexus(){};
        
        virtual Tree * readTree() const;
        
        virtual std::vector<Tree*> * readTrees(unsigned int start=0, unsigned int end=UINT_MAX) const;
        
        bool findBlock(const std::string &blockName)const;
        
        std::string nextLineUncommented()const;
        
    private:
        std::istream *_inputstream;
        
        void readTreesBlock(std::vector<Tree*> *trees, unsigned int start, unsigned int end)const;
        
        
    protected:
        
    };
}

#endif /* defined(__PhyCPP__Nexus__) */
