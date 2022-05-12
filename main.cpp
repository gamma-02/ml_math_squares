// C++ program to create target string, starting from
// random string using Genetic Algorithm

//#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#include <fstream>
#include "ShuntingYard.h"


using namespace std;



// Number of individuals in each generation
#define POPULATION_SIZE 100

// Valid Genes
const vector<int> GENES({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});


// Target answers to be found for
const vector<int> TARGET({-24, 27, -61, 153, 140, 196, 11, 82});

// Pattern of answers
const string PATTERN = "-/*-++*+/+++++--***++*--";

bool correct(vector<int> &vec){
    for (int i = 0; i < vec.size(); ++i) {
        for(int j = 0; j < vec.size(); ++j){
            if(i != j){
                if(vec.at(i) == vec.at(j)){
                    return false;
                }
            }
        }
    }

    return true;
}

ostream& operator<<(ostream &os, const vector<int> &vec){
    for(auto iter = vec.begin(); iter != vec.end(); iter++){
        os << *iter << (++iter != vec.end() ? ", " : "");
        iter--;
    }
    return os;
}



// Function to generate random numbers in given range
int random_num(int start, int end)
{
//    int range = (end-start)+1;
//    int random_int = start+(rand()%range);
//    return random_int;
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(start, end);
    return dist(mt);
}
// Create random genes for mutation
vector<int> mutate_genes(vector<int> &input, int mutationAmount){
    vector<int> genes_copy(input);
//    std::random_device rd;
//    std::mt19937 g(rd());

//    std::shuffle(genes_copy.begin(), genes_copy.end(), g);
    auto iter = genes_copy.begin();
    auto revIter = genes_copy.rbegin();
    for(int i = 0; i < mutationAmount; i++) {
        for(int e = 0; e < random_num(1, 16); e++){
            iter++;
            if(iter == genes_copy.end()){
                iter = genes_copy.begin();
            }

        }
        for(int e = 0; e < random_num(1, 16); e++){
            revIter++;
            if(revIter == genes_copy.rend()){
                revIter = genes_copy.rbegin();
            }
        }


        iter_swap(iter, revIter);

    }
    return genes_copy;


}

vector<int> mutate_genes(vector<int> &input){
    vector<int> genes_copy(input);
//    std::random_device rd;
//    std::mt19937 g(rd());

//    std::shuffle(genes_copy.begin(), genes_copy.end(), g);
    int mutations = random_num(1, 4);
    auto iter = genes_copy.begin();
    auto revIter = genes_copy.rbegin();
    for(int i = 0; i < mutations; i++) {
        for(int e = 0; e < random_num(1, 16); e++){
            iter++;
            if(iter == genes_copy.end()){
                iter = genes_copy.begin();
            }

        }
        for(int e = 0; e < random_num(1, 16); e++){
            revIter++;
            if(revIter == genes_copy.rend()){
                revIter = genes_copy.rbegin();
            }
        }


        iter_swap(iter, revIter);

    }
    return genes_copy;


}



// create chromosome or vector of genes
vector<int> create_gnome() {
    vector<int> genes_copy(GENES);
    std::random_device rd;
    std::mt19937 g(rd());

    shuffle(genes_copy.begin(), genes_copy.end(), g);
    return genes_copy;
}

// Class representing individual in population
class Individual
{
public:
    vector<int> chromosome;
    int fitness;
    Individual(vector<int> &chromosome);
    Individual mate(Individual parent2);
    int cal_fitness();



};



Individual::Individual(vector<int> &chromosome)
{
    this->chromosome = chromosome;
    this->fitness = cal_fitness();
};
ostream& operator<<(ostream &os, const Individual &ind){
    os << "fitness: " << ind.fitness << ", genes: " << ind.chromosome;
    return os;
}

// Perform mating and produce new offspring
Individual Individual::mate(Individual par2)
{
    // chromosome for offspring
//    string child_chromosome = "";
    vector<int> child_chromosome;
    auto myIter = this->chromosome.begin();
    auto otherIter = par2.chromosome.begin();

    for(int i = 0; i < (this->chromosome.size()/par2.chromosome.size())/*should be 16*/; i++){
        int r = random_num(1, 2);
        if(r == 1){
            child_chromosome.push_back(*myIter);
        }else{
            child_chromosome.push_back(*otherIter);
        }
        myIter++;
        otherIter++;
    }


    vector<int> temp;
    int missed = 0;
    for(int &i : child_chromosome){
        if(std::find(temp.begin(), temp.end(), i) == temp.end()){
            temp.push_back(i);
        }else{
            missed++;
        }
    }
    vector<int> missing;
    for(const int &i : GENES){
        if(std::find(temp.begin(), temp.end(), i) == temp.end()){
            missing.push_back(i);
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    shuffle(missing.begin(), missing.end(), g);
    for(int i : missing){
        temp.push_back(i);
    }
    temp = mutate_genes(temp);


    return Individual(temp);
};


// Calculate fitness score, it is the number of
// characters in string which differ from target
// string.
int Individual::cal_fitness()
{
    int fitness = 0;

    if(!correct(this->chromosome))
        return INT32_MAX;

    string pattern = PATTERN;


    vector<vector<string>> patternSquare;
    int notChars = 0;
    int number = 0;
    vector<int> vec = this->chromosome;

    for(int i = 0; i < 7; i++){
        vector<string> temp;
        for(int j = 0; j < 7; j++){
            bool evenRow = i % 2 == 0;
//            cout << "evenRow: " << (evenRow ? "true" : "false") << endl;
            bool evenColumn = j % 2 == 0;
//            cout << "evenCol: " << (evenColumn ? "true" : "false") << endl;
            if(evenRow){

                if(!evenColumn){
                    temp.emplace_back(pattern.substr(notChars, 1));
                    notChars++;
                }else{
                    temp.push_back(to_string(vec.at(number)));
                    number++;
                }
//                cout << notChars << endl;
            }else if(evenColumn){
                temp.emplace_back(pattern.substr(notChars, 1));
                notChars++;
//                cout << notChars << endl;

            }else{
                temp.emplace_back("\0");//technically a char
            }
        }
        patternSquare.push_back(temp);
    }
    //pattern square construted, do calcuations.
    vector<string> expressions {"", "", "", "", "", "", "", ""};
    int iter = 0;
    int exp = 0;
    for(vector<string> &st : patternSquare){
        if(iter % 2 == 0){//not an iteration with null squares
            for(string &s : st){
                expressions.at(exp) += s;
            }
            exp++;
        }
        iter++;
    }
    int downRow = 4;
    for(int i = 0; i < 7; i++){
        for(vector<string> &st : patternSquare){
            if(i % 2 == 0){
                expressions.at(downRow) += st.at(i);
            }
        }
        if(i%2 == 0){
            downRow++;
        }
    }

    vector<int> square = calcuate(expressions);
    int at = 0;

    for(int i = 0; i < 8; i++){
        int sq = square.at(i);
        int an = TARGET.at(i);

        if(sq > an){
            fitness += abs(sq - an);
        }else if(an > sq){
            fitness += abs(an - sq);
        }else if(an == sq){
            fitness += 0;
        }
    }


    return fitness;
};

// Overloading < operator
bool operator<(const Individual &ind1, const Individual &ind2)
{
    return ind1.fitness < ind2.fitness;
}


void doMate(vector<Individual> &population, vector<Individual> &new_generation){

    int r = random_num(0, 50);
    Individual parent1 = population[r];
    r = random_num(0, 50);
    Individual parent2 = population[r];
    Individual offspring = parent1.mate(parent2);
    new_generation.push_back(offspring);
}

// Driver code
void executeSimulation(string name)
{
//    srand((unsigned)(time(0)));

    ofstream out("/Users/danielposselt/Desktop/ml_output/"+name+".txt", fstream::app);

    // current generation
    int generation = 0;

    vector<Individual> population;
    bool found = false;

    // create initial population
    for(int i = 0;i<POPULATION_SIZE;i++)
    {
        vector<int> gnome = create_gnome();
        population.push_back(Individual(gnome));
    }

    int lastGeneration = 0;
    int same = 0;


    while(!found)
    {

        // sort the population in increasing order of fitness score
        sort(population.begin(), population.end());

        // if the individual having lowest fitness score ie.
        // 0 then we know that we have reached to the target
        // and break the loop
        if(population[0].fitness <= 0)
        {
            found = true;
            break;
        }

        // Otherwise generate new offsprings for new generation
        vector<Individual> new_generation;
//        if(same <= 1000) {

            // Perform Elitism, that mean 10% of fittest population
            // goes to the next generation
            int s = (10*POPULATION_SIZE)/100;
            for(int i = 0;i<s;i++)
                new_generation.push_back(population[i]);

            // From 50% of fittest population, Individuals
            // will mate to produce offspring
            int tracker = 0;
            s = (90 * POPULATION_SIZE) / 100;
            for (int i = 0; i < s; i++) {
                int rand = random_num(1, 5);
                if (rand != 1) {
                    doMate(population, new_generation);
                } else {
                    vector<int> vec = create_gnome();
                    new_generation.push_back(Individual(vec));
                }
            }
//            vector<thread> matingThreads;
//            auto f = [](pair<vector<Individual>, vector<Individual>> p){
//                doMate(p.first, p.second);
//            };
//
//            for(int i = 0; i < tracker; i++){
//                thread ts(f, pair<vector<Individual>, vector<Individual>>(population, new_generation));
//            }
        /*} else {
            cout << "REFRESHING GENE POOL" << endl;
            out << "REFRESHING GENE POOL: " << population[0] << endl;
            // Perform Elitism, that mean 30% of fittest population
            // goes to the next, new, generation with 1 mutation
            int s = (30*POPULATION_SIZE)/100;
            for(int i = 0;i<s;i++) {
                vector<int> e = mutate_genes(population[i].chromosome, 1);
                Individual a(e);
                new_generation.push_back(a);
            }

            // the other 60% gets completly replaced, to "freshen up" the gene pool

            s = (60 * POPULATION_SIZE) / 100;
            for (int i = 0; i < s; i++) {
                vector<int> vec = create_gnome();
                new_generation.push_back(Individual(vec));
            }
            same = 0;

        }*/
        population = new_generation;
        cout << "Thread: " << name << '\t';
        cout<< "Generation: " << generation << "\t";
        cout<< "Answers: "<< population[0].chromosome <<"\t";
        cout<< "Fitness: "<< population[0].fitness << "\n";

        if(lastGeneration == population[0].fitness){
            same++;
        }else{
            out << population[0].fitness << endl;
            same = 0;
        }

        lastGeneration = population[0].fitness;

        generation++;
    }
    cout<< "Generation: " << generation << "\t";
    cout<< "String: "<< population[0].chromosome <<"\t";
    cout<< "Fitness: "<< population[0].fitness << "\n";
    out << endl << "Generation: " << generation << "\t";
    out<< "String: "<< population[0].chromosome <<"\t";
    out<< "Fitness: "<< population[0].fitness << "\n";
    out.close();
    cout << name << "FINISHED.";


}

int main(){

    cout << "STARTING THREAD ONE " << endl;
    thread th(executeSimulation, "simulation_one");

    cout << "STARTING THREAD TWO " << endl;
    thread th2(executeSimulation, "simulation_two");

    cout << "STARTING THREAD THREE " << endl;
    thread th3(executeSimulation, "simulation_thr");


    th.join();
    th2.join();
    th3.join();

}
