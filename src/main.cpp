//
//  main.cpp
//  PhyCPP
//
//  Created by Mathieu on 23/09/2015.
//

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <regex>
#include <random>

#include "Nexus.hpp"
#include "Newick.hpp"
#include "Patristic.hpp"
#include "Multinomial.hpp"
#include "CmdLine.hpp"

using namespace phycpp;
using namespace std;
using namespace std::regex_constants;

static double meanDistance( const vector<Node*> &nodes, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& matrix ){
    double d = 0.0;
    for ( int i = 0; i < nodes.size(); i++ ) {
        int ii = nodeToIndex.at(nodes[i]);
        for ( int j = i+1; j < nodes.size(); j++ ) {
            d += matrix[ii][nodeToIndex.at(nodes[j])];
        }
    }
    d /= nodes.size()*(nodes.size()-1)/2;
    return d;
}

static double meanDistance2( const vector<Node*> &nodes1, const vector<Node*> &nodes2, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& matrix ){
    double d = 0.0;
    for ( int i = 0; i < nodes1.size(); i++ ) {
        int ii = nodeToIndex.at(nodes1[i]);
        for ( int j = 0; j < nodes2.size(); j++ ) {
            d += matrix[ii][nodeToIndex.at(nodes2[j])];
        }
    }
    d /= nodes1.size()*nodes2.size();
    return d;
}

static double ssn(const vector<Node*> &nodes1, const vector<Node*> &nodes2, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& matrix){
    double snn = 0.0;
    
    for ( int i = 0; i < nodes1.size(); i++ ) {
        int ii = nodeToIndex.at(nodes1[i]);
        double min = std::numeric_limits<double>::max();
        double Xj = 0.0;
        double Wj = 0.0;
        
        for ( int j = 0; j < nodes1.size(); j++ ) {
            if( j != i ){
                int jj = nodeToIndex.at(nodes1[j]);
                double d = matrix[ii][jj];
                
                if( min > d ){
                    Wj = 1.0;
                    Xj = 1.0;
                    min = d;
                }
                else if( min == d ){
                    Wj++;
                    Xj++;
                }
            }
        }
        for ( int j = 0; j < nodes2.size(); j++ ) {
            if( j != i ){
                int jj = nodeToIndex.at(nodes2[j]);
                double d = matrix[ii][jj];
                
                if( min > d ){
                    Wj = 1.0;
                    Xj = 0.0;
                    min = d;
                }
                else if( min == d ){
                    Wj++;
                }
            }
        }
        snn += Xj/Wj;
    }
    
    return snn;
    
}

void permutations(vector<string> s, vector<string> p, map<vector<string>,bool>& m){
    for ( int i = 0; i < s.size(); i++ ) {
        vector<string> np(p);
        vector<string> ns;
        ns.resize(s.size()-1);
        int y = 0;
        for (int j = 0; j < s.size(); j++) {
            if ( j!= i ) {
                ns[y++] = s[j];
            }
        }
        np.push_back(s[i]);
        if(ns.size() == 0 ){
            m[np] = true;
        }
        else {
            permutations(ns, np,m);
        }
    }
}

// Hudson, Boos and Kaplan 1992
// Hudson, Slatkin and Maddison 1992
// Slatkin 1993
tuple<double, double, double> calculateAllFsts( const map<string,vector<Node*>>& traitToNodes, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& matrix){
    
    vector<string> traits;
    for(auto& p: traitToNodes){
        traits.push_back(p.first);
    }
    
    double Ht1  = 0.0; // sum_i(pi*pi*dij)
    double Ht2  = 0.0; // sum_i>j(pi*pj*dij)
    double pipi = 0.0; // sum_i(pi*pi)
    double pij  = 0.0; // sum_i>j(pi*pj)
    
    for ( int i = 0; i < traits.size() ; i++) {
        double ci = traitToNodes.at(traits[i]).size();
        double pi = ci/(matrix.size());
        pipi += pi*pi;
        
        Ht1 += pi*pi * meanDistance(traitToNodes.at(traits[i]), nodeToIndex, matrix);
        for ( int j = i+1; j < traits.size(); j++ ) {
            double cj = traitToNodes.at(traits[j]).size();
            double pj = cj/(matrix.size());
            
            Ht2 += pi*pj * meanDistance2(traitToNodes.at(traits[i]), traitToNodes.at(traits[j]), nodeToIndex, matrix);
            pij += pi*pj;
        }
    }
    double Ht = Ht1 + 2 * Ht2; // total diversity
    double Hs = Ht1/pipi;      // within diversity
    double Hb = Ht2/pij;       // bewteen diversity
    
    return make_tuple(1.0-(Hs/Ht), (Hb-Hs)/Hb, (Hb-Hs)/(Hs+Hb));
}

void bootstrapDistanceMatrix(int bootstrapCount, mt19937& mt_rand, const map<string,vector<Node*>>& traitToNodes, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& mat){
    
    vector<double> randomFstHBK92;
    vector<double> randomFstHSM92;
    vector<double> randomFstS93;
    
    for (int b = 0; b < bootstrapCount; b++ ) {
        map<Node*,int> randomizedNodeToIndex;
        for(auto& p: traitToNodes){
            const vector<Node*> &nodes = traitToNodes.at(p.first);
            vector<int> indexes;
            for ( int j = 0; j < nodes.size(); j++) {
                indexes.push_back(nodeToIndex.at(nodes[j]));
            }
            //auto int_rand = std::bind(std::uniform_int_distribution<int>(0,(int)indexes.size()-1), mt_rand);
            std::uniform_int_distribution<int> distribution(0, (int)indexes.size()-1);
            for ( int j = 0; j < nodes.size(); j++) {
                randomizedNodeToIndex[nodes[j]] = indexes[distribution(mt_rand)];
                //randomizedNodeToIndex[nodes[j]] = indexes[int_rand()];
            }
        }
        auto fsts = calculateAllFsts(traitToNodes, randomizedNodeToIndex, mat);

        randomFstHBK92.push_back(get<0>(fsts));
        randomFstHSM92.push_back(get<1>(fsts));
        randomFstS93.push_back(get<2>(fsts));
        //randomSnn.push_back(Snn);
    }
    
    std::sort (randomFstHBK92.begin(), randomFstHBK92.end());
    double medianFstHBK92 = randomFstHBK92[randomFstHBK92.size()/2];
    double meanFstHBK92 = std::accumulate(randomFstHBK92.begin(), randomFstHBK92.end(), 0.0) / randomFstHBK92.size();
    cout << "F_ST HBK92 Median: "<<medianFstHBK92<<endl;
    cout << "F_ST HBK92 Mean: "<< meanFstHBK92 <<endl;
    cout << "F_ST HBK92 95\% CI: ["<< randomFstHBK92[randomFstHBK92.size()*0.025]<< " - " << randomFstHBK92[randomFstHBK92.size()*0.975]<<"]" <<endl;
    cout<<endl;
    
    std::sort (randomFstHSM92.begin(), randomFstHSM92.end());
    double medianFstHSM92 = randomFstHSM92[randomFstHSM92.size()/2];
    double meanFstHSM92 = std::accumulate(randomFstHSM92.begin(), randomFstHSM92.end(), 0.0) / randomFstHSM92.size();
    cout << "F_ST HSM92 Median: "<<medianFstHSM92<<endl;
    cout << "F_ST HSM92 Mean: "<< meanFstHSM92 <<endl;
    cout << "F_ST HSM92 95\% CI: ["<< randomFstHSM92[randomFstHSM92.size()*0.025]<< " - " << randomFstHSM92[randomFstHSM92.size()*0.975]<<"]" <<endl;
    cout<<endl;
    
    std::sort (randomFstS93.begin(), randomFstS93.end());
    double medianFstS93 = randomFstS93[randomFstS93.size()/2];
    double meanFstS93 = std::accumulate(randomFstS93.begin(), randomFstS93.end(), 0.0) / randomFstS93.size();
    cout << "F_ST S93 Median: "<<medianFstS93<<endl;
    cout << "F_ST S93 Mean: "<< meanFstS93 <<endl;
    cout << "F_ST S93 95\% CI: ["<< randomFstS93[randomFstS93.size()*0.025]<< " - " << randomFstS93[randomFstS93.size()*0.975]<<"]" <<endl;
}

void permuteDistanceMatrix(int bootstrapCount, mt19937& mt_rand, const vector<double>& obsFst, const map<string,vector<Node*>>& traitToNodes, const vector<string> &traits, const map<Node*,int> &nodeToIndex, const vector<vector<double>> &mat){
    
    vector<double> randomFstHBK92;
    vector<double> randomFstHSM92;
    vector<double> randomFstS93;
    //vector<double> randomSnn;
    
    map<string,bool> unique;
    
    for (int b = 0; b < bootstrapCount; b++ ) {
        map<Node*,int> randomizedNodeToIndex;
        vector<int> indexes;
        string indexesString;
        
        for(auto& n: nodeToIndex) {
            indexes.push_back(n.second);
            indexesString.append(std::to_string(n.second));
        }
        if(b==0){
            unique[indexesString] = true;
            cout << indexesString<<endl;
        }
        indexesString.clear();
        
        shuffle(indexes.begin(), indexes.end(), mt_rand);
        
        int i = 0;
        for (auto& n: nodeToIndex) {
            indexesString.append(std::to_string(indexes[i]));
            randomizedNodeToIndex[n.first] = indexes[i++];
        }
        
        if(unique.find(indexesString) != unique.end()){
            cout <<"dup"<<endl;
            continue;
        }
        
        unique[indexesString] = true;
        
        auto fsts = calculateAllFsts(traitToNodes, randomizedNodeToIndex, mat);
        
        randomFstHBK92.push_back(get<0>(fsts));
        randomFstHSM92.push_back(get<1>(fsts));
        randomFstS93.push_back(get<2>(fsts));
    }
    
    double pFstHNBK92 = 0.0;
    double pFstHSM92 = 0.0;
    double pFstS93 = 0.0;
    for (int b = 0; b < randomFstHBK92.size(); b++ ) {
        if(randomFstHBK92[b] > obsFst[0] ){
            pFstHNBK92++;
        }
        if(randomFstHSM92[b] > obsFst[1] ){
            pFstHSM92++;
        }
        if(randomFstS93[b] > obsFst[2] ){
            pFstS93++;
        }
    }
    pFstHNBK92 /= randomFstHBK92.size();
    pFstHSM92  /= randomFstHSM92.size();
    pFstS93    /= randomFstS93.size();
    
    cout << "F_ST (Hudson, Boos, Kaplan 1992) p-value " << pFstHNBK92 << endl;
    cout << "F_ST (Hudson, Slatkin, Maddison 1992) p-value " << pFstHSM92 << endl;
    cout << "F_ST (Slatkin 1993) p-value " << pFstS93 << endl;
}

void permuteTraits( int n, mt19937& mt_rand, const vector<double>& obsFst, const map<Node*,string>& nodeToTrait, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& mat){
    
    vector<double> randomFstHBK92;
    vector<double> randomFstHSM92;
    vector<double> randomFstS93;
    
    vector<string> traits;
    for(auto& n: nodeToTrait) {
        traits.push_back(n.second);
    }
    
    for ( int i = 0 ; i < n; i++ ) {
        vector<string> randomTraits = traits;
        
        shuffle(randomTraits.begin(), randomTraits.end(), mt_rand);
        
        map<string,vector<Node*>> randomTraitToNodes;
        int index = 0;
        for (auto& n: nodeToTrait) {
            randomTraitToNodes[randomTraits[index++]].push_back(n.first);
        }
        
        auto fsts = calculateAllFsts(randomTraitToNodes, nodeToIndex, mat);

        randomFstHBK92.push_back(get<0>(fsts));
        randomFstHSM92.push_back(get<1>(fsts));
        randomFstS93.push_back(get<2>(fsts));
    }
    
    double pFstHBK92 = 0.0;
    double pFstHSM92 = 0.0;
    double pFstS93 = 0.0;
    for (int b = 0; b < randomFstHBK92.size(); b++ ) {
        if(randomFstHBK92[b] > obsFst[0] ){
            pFstHBK92++;
        }
        if(randomFstHSM92[b] > obsFst[1] ){
            pFstHSM92++;
        }
        if(randomFstS93[b] > obsFst[2] ){
            pFstS93++;
        }
    }

    pFstHBK92 /= randomFstHBK92.size();
    pFstHSM92 /= randomFstHSM92.size();
    pFstS93   /= randomFstS93.size();
    
    cout << "F_ST (Hudson, Boos, Kaplan 1992) p-value " << pFstHBK92 << endl;
    cout << "F_ST (Hudson, Slatkin, Maddison 1992) p-value " << pFstHSM92 << endl;
    cout << "F_ST (Slatkin 1993) p-value " << pFstS93 << endl;
}

void permuteTraitsExact( const vector<double>& obsFst, const map<Node*,string>& nodeToTrait, const map<Node*,int>& nodeToIndex, const vector<vector<double>>& mat){
    
    vector<double> randomFstHBK92;
    vector<double> randomFstHSM92;
    vector<double> randomFstS93;
    //vector<double> randomSnn;
    
    vector<string> ts;
    for(auto& n: nodeToTrait) {
        ts.push_back(n.second);
    }
    map<vector<string>,bool> m;
    vector<string> p;
    
    permutations(ts, p, m);
    
    for ( map<vector<string>, bool>::const_iterator iter = m.begin(); iter != m.end(); iter++ ) {
        vector<string> traits = iter->first;
        
        map<string,vector<Node*>> randomTraitToNodes;
        int index = 0;
        for(auto& n: nodeToTrait) {
            randomTraitToNodes[traits[index++]].push_back(n.first);
        }
        
        auto fsts = calculateAllFsts(randomTraitToNodes, nodeToIndex, mat);
        
        randomFstHBK92.push_back(get<0>(fsts));
        randomFstHSM92.push_back(get<1>(fsts));
        randomFstS93.push_back(get<2>(fsts));
    }
    
    double pFstHNBK92 = 0.0;
    double pFstHSM92 = 0.0;
    double pFstS93 = 0.0;
    for (int b = 0; b < randomFstHBK92.size(); b++ ) {
        
        if(randomFstHBK92[b] > obsFst[0] ){
            pFstHNBK92++;
        }
        if(randomFstHSM92[b] > obsFst[1] ){
            pFstHSM92++;
        }
        if(randomFstS93[b] > obsFst[2] ){
            pFstS93++;
        }
    }

    pFstHNBK92 /= randomFstHBK92.size();
    pFstHSM92  /= randomFstHSM92.size();
    pFstS93    /= randomFstS93.size();
    
    cout << "F_ST (Hudson, Boos, Kaplan 1992) p-value " << pFstHNBK92 << endl;
    cout << "F_ST (Hudson, Slatkin, Maddison 1992) p-value " << pFstHSM92 << endl;
    cout << "F_ST (Slatkin 1993) p-value " << pFstS93 << endl;
}

int main(int argc, char * argv[]) {
    
    string inputFile = "/Users/mathieu/Google Drive/flyway/aiv-flyways/MP/mp.tree";
    string regexString = ".+_(\\w+)$";
    bool exactTest = false;
    int numberOfPermuations = 000;
    int numberOfBootstraps = 000;
    
    mt19937::result_type seed = static_cast<mt19937::result_type>(chrono::high_resolution_clock::now().time_since_epoch().count()); //time(0);
    
    getCmdOption(argv,argv+argc, "-i", inputFile);
    getCmdOption(argv,argv+argc, "-r", regexString);
    getCmdOptionUnsignedInt(argv,argv+argc, "-s", seed);
    getCmdOptionInt(argv,argv+argc, "-b", numberOfPermuations);
    
    if(numberOfPermuations == 0){
        getCmdOptionInt(argv,argv+argc, "-p", numberOfBootstraps);
    }
    if(numberOfPermuations == 0 && numberOfBootstraps == 0){
        exactTest = containCmdOption(argv, argv+argc, "-e");
    }
    
    mt19937 mt_rand(seed);
    
    ifstream inputStream;
    inputStream.open(inputFile);
    
    ofstream myfile;
    myfile.open (inputFile+".fst.csv");
    
    if( !inputStream.good() ){
        cerr << "Cannot open file: " << inputFile << endl;
        exit(1);
    }
    
    TreeReader *reader = new Newick(&inputStream);
    unique_ptr<Tree> tree(reader->readTree());
    
    inputStream.close();
    delete reader;
    
    int leafCount = tree->getLeafNodeCount();
    
    cout << "Number of sequences: " << leafCount <<endl;
    
    map<string,vector<Node*>> traitToNodes; // key is a trait and value is vector of nodes associated with this trait
    map<Node*,int> nodeToIndex;             // key is a node and value is its index in the distance matrix mat
    map<Node*,string> nodeToTrait;          // key is a node and value is its trait
    vector<string> traits;                  // list of traits (keys of traitToNodes)
    regex traitRegex(regexString, ECMAScript|icase);
    
    for (auto node: tree->getNodes() ) {
        std::smatch sm;
        if( node->isLeaf() && regex_match(node->getName(), sm, traitRegex) ){
            traitToNodes[sm[1]].push_back(node);
            nodeToTrait[node] = sm[1];
        }
    }
    cout << "Number of traits: " << traitToNodes.size() <<endl;
    for (auto& n: traitToNodes) {
        cout << "  " << n.first << ": "<< n.second.size() << " (" << (static_cast<double>(n.second.size())/leafCount) << ")"<<endl;
    }
    cout << endl;
    cout << "Computing patristic distances...";
    cout.flush();
    
    Patristic patristic(*tree);
    patristic.calculate();
    
    cout <<" done"<<endl<<endl;
    
    vector<vector<double> > mat = patristic.getMatrix();
    vector<Node*> nodes = patristic.getNodes();
    
    for (int i = 0; i < nodes.size(); i++ ) {
        nodeToIndex[nodes[i]] = i;
    }
    
    for(auto& trait: traitToNodes){
        traits.push_back(trait.first);
    }
    
    std::sort(traits.begin(), traits.end());
    
    myfile << "trait1,trait2,Fst HBK92,Fst HSM92,Fst S93,Fst unweighted,Snn" << endl;
    
    for ( int i = 0; i < traits.size() ; i++) {
        double di = meanDistance(traitToNodes[traits[i]],nodeToIndex,mat);
        double ci = traitToNodes[traits[i]].size();
        
        for ( int j = i+1; j < traits.size() ; j++) {
            double dj = meanDistance(traitToNodes[traits[j]],nodeToIndex,mat);
            double cj = traitToNodes[traits[j]].size();
            double pi = ci/(ci+cj);
            double pj = 1.0-pi;
            double dij = meanDistance2(traitToNodes[traits[i]],traitToNodes[traits[j]],nodeToIndex,mat);
            
            double Ht = pi*pi*di + pj*pj*dj + 2.0*pi*pj*dij;   // total diversity
            double Hs = (pi*pi*di + pj*pj*dj)/(pi*pi + pj*pj); // within diversity
            double Hb = dij;                                   // bewteen diversity
            
            double FstHBK92 = 1.0-(Hs/Ht);   // Hudson, Boos and Kaplan 1992
            double FstHSM92 = (Hb-Hs)/Hb;    // Hudson, Slatkin and Maddison 1992
            double FstS93 = (Hb-Hs)/(Hs+Hb); // Slatkin 1993
            double Snn = (ssn(traitToNodes[traits[i]],traitToNodes[traits[j]],nodeToIndex,mat) + ssn(traitToNodes[traits[j]],traitToNodes[traits[i]],nodeToIndex,mat)) / (traitToNodes[traits[i]].size()+traitToNodes[traits[j]].size()); // Hudson 2000
            
            cout << traits[i] << " " << traits[j] << " " << (1.0-(di+dj)/2.0/dij) << " F_ST (Hudson, Boos, Kaplan 1992): " << FstHBK92 << " F_ST (Hudson, Slatkin, Maddison 1992): "<< FstHSM92 << " F_ST (Slatkin 1993): "<< FstS93 << " H_S: " << Hs << " H_T: " << Ht << " S_nn: " << Snn<<endl;
            myfile << traits[i] << "," << traits[j] << "," << FstHBK92 << "," << FstHSM92 <<"," << FstS93 << "," << (1.0-(di+dj)/2.0/dij) << "," << Snn << endl;
        }
    }
    myfile.close();
    cout<<endl;
    
    auto fsts = calculateAllFsts(traitToNodes, nodeToIndex, mat);
    
    cout<<"F_ST (Hudson, Boos, Kaplan 1992): "<< get<0>(fsts) <<" F_ST (Hudson, Slatkin, Maddison 1992): "<< get<1>(fsts) <<" F_ST (Slatkin 1993): " << get<2>(fsts) << endl;
    
    vector<double> obsFst = {get<0>(fsts), get<1>(fsts), get<2>(fsts)};

    if( exactTest){
        try{
            multinomial::SVI v(traits.size());
            for (string trait: traits) {
                v.push_back(traitToNodes[trait].size());
            }
            unsigned long long combinations = multinomial::multi<unsigned long long>(v);
            cout<<"Exact test with " << combinations << " combinations"<<endl;
            permuteTraitsExact(obsFst, nodeToTrait, nodeToIndex, mat);
        }
        catch (std::overflow_error& ex){
            std::cerr << ex.what() << std::endl;
        }
    }
    else if( numberOfPermuations > 0 ) {
        cout<< numberOfPermuations << " permutations" << endl;
        permuteTraits(numberOfPermuations, mt_rand, obsFst, nodeToTrait, nodeToIndex, mat);
    }
    else if( numberOfBootstraps > 0 ){
        cout<< numberOfBootstraps <<" bootstrap replicates"<< endl;
        bootstrapDistanceMatrix(numberOfBootstraps, mt_rand, traitToNodes, nodeToIndex, mat);
    }
    
    return 0;
}
