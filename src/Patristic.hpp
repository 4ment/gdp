//
//  Patristic.hpp
//  PhyCPP
//
//  Created by Mathieu on 23/09/2015.
//

#ifndef __PhyCPP__Patristic__
#define __PhyCPP__Patristic__

#include <set>
#include <vector>

#include "Tree.hpp"

namespace phycpp {
    
    class Patristic{
        
    public:
        Patristic(const Tree& tree);
        
        ~Patristic();
        
        void calculate();
        
        const std::vector<std::vector<double>>& getMatrix();
        
        const std::vector<Node*>& getNodes();
        
    private:
        void populateMap(Node& ode);
        
    private:
        const Tree& tree_;
        std::vector<std::vector<double> > matrix_;
        std::vector<std::set<Node*> > map_;
        std::vector<Node*> nodes_;
    };
}

#endif /* defined(__PhyCPP__Patristic__) */
