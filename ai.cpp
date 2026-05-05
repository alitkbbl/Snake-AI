#include "game.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

class NeuralNet {
public:
    std::vector<float> weightsInputHidden;
    std::vector<float> weightsHiddenOutput;
    std::vector<float> biasHidden;
    std::vector<float> biasOutput;
    
    static const int INPUT_SIZE = 16;
    static const int HIDDEN_SIZE = 12;
    static const int OUTPUT_SIZE = 4;
    
    NeuralNet() {
        weightsInputHidden.resize(INPUT_SIZE * HIDDEN_SIZE);
        weightsHiddenOutput.resize(HIDDEN_SIZE * OUTPUT_SIZE);
        biasHidden.resize(HIDDEN_SIZE);
        biasOutput.resize(OUTPUT_SIZE);
        randomize();
    }
    
    void randomize() {
        for (auto& w : weightsInputHidden) w = randFloat(-1, 1);
        for (auto& w : weightsHiddenOutput) w = randFloat(-1, 1);
        for (auto& b : biasHidden) b = randFloat(-1, 1);
        for (auto& b : biasOutput) b = randFloat(-1, 1);
    }
    
    float randFloat(float min, float max) {
        return min + (rand() / (float)RAND_MAX) * (max - min);
    }
    
    float relu(float x) {
        return x > 0 ? x : 0;
    }
    
    float sigmoid(float x) {
        return 1.0f / (1.0f + exp(-x));
    }
    
    Direction predict(const std::vector<float>& inputs) {
        std::vector<float> hidden(HIDDEN_SIZE, 0);
        for (int h = 0; h < HIDDEN_SIZE; h++) {
            float sum = biasHidden[h];
            for (int i = 0; i < INPUT_SIZE; i++) {
                sum += inputs[i] * weightsInputHidden[i * HIDDEN_SIZE + h];
            }
            hidden[h] = relu(sum);
        }
        
        std::vector<float> outputs(OUTPUT_SIZE, 0);
        for (int o = 0; o < OUTPUT_SIZE; o++) {
            float sum = biasOutput[o];
            for (int h = 0; h < HIDDEN_SIZE; h++) {
                sum += hidden[h] * weightsHiddenOutput[h * OUTPUT_SIZE + o];
            }
            outputs[o] = sigmoid(sum);
        }
        
        int maxIdx = 0;
        for (int i = 1; i < OUTPUT_SIZE; i++) {
            if (outputs[i] > outputs[maxIdx]) maxIdx = i;
        }
        
        return (Direction)maxIdx;
    }
    
    void mutate(float rate) {
        for (auto& w : weightsInputHidden) {
            if (randFloat(0, 1) < rate) w += randFloat(-0.5, 0.5);
        }
        for (auto& w : weightsHiddenOutput) {
            if (randFloat(0, 1) < rate) w += randFloat(-0.5, 0.5);
        }
        for (auto& b : biasHidden) {
            if (randFloat(0, 1) < rate) b += randFloat(-0.5, 0.5);
        }
        for (auto& b : biasOutput) {
            if (randFloat(0, 1) < rate) b += randFloat(-0.5, 0.5);
        }
    }
    
    NeuralNet crossover(const NeuralNet& other) {
        NeuralNet child;
        for (size_t i = 0; i < weightsInputHidden.size(); i++) {
            child.weightsInputHidden[i] = (rand() % 2) ? weightsInputHidden[i] : other.weightsInputHidden[i];
        }
        for (size_t i = 0; i < weightsHiddenOutput.size(); i++) {
            child.weightsHiddenOutput[i] = (rand() % 2) ? weightsHiddenOutput[i] : other.weightsHiddenOutput[i];
        }
        for (size_t i = 0; i < biasHidden.size(); i++) {
            child.biasHidden[i] = (rand() % 2) ? biasHidden[i] : other.biasHidden[i];
        }
        for (size_t i = 0; i < biasOutput.size(); i++) {
            child.biasOutput[i] = (rand() % 2) ? biasOutput[i] : other.biasOutput[i];
        }
        return child;
    }
};

class GeneticAlgorithm {
public:
    std::vector<NeuralNet> population;
    std::vector<int> fitness;
    int generation;
    int bestFitness;
    int bestScore;
    
    static const int POPULATION_SIZE = 50;
    static const float MUTATION_RATE = 0.1f;
    
    GeneticAlgorithm() : generation(0), bestFitness(0), bestScore(0) {
        population.resize(POPULATION_SIZE);
    }
    
    void evaluatePopulation() {
        fitness.clear();
        fitness.resize(POPULATION_SIZE);
        
        for (int i = 0; i < POPULATION_SIZE; i++) {
            Game game;
            
            while (game.snake.alive) {
                std::vector<float> inputs = game.getInputs();
                Direction dir = population[i].predict(inputs);
                game.snake.dir = dir;
                game.update();
            }
            
            fitness[i] = game.getFitness();
            
            if (fitness[i] > bestFitness) {
                bestFitness = fitness[i];
                bestScore = game.snake.score;
            }
        }
    }
    
    void evolve() {
        std::vector<NeuralNet> newPopulation;
        
        std::vector<int> indices(POPULATION_SIZE);
        for (int i = 0; i < POPULATION_SIZE; i++) indices[i] = i;
        std::sort(indices.begin(), indices.end(), [this](int a, int b) {
            return fitness[a] > fitness[b];
        });
        
        for (int i = 0; i < 5; i++) {
            newPopulation.push_back(population[indices[i]]);
        }
        
        while (newPopulation.size() < POPULATION_SIZE) {
            int parent1 = selectParent(indices);
            int parent2 = selectParent(indices);
            
            NeuralNet child = population[parent1].crossover(population[parent2]);
            child.mutate(MUTATION_RATE);
            
            newPopulation.push_back(child);
        }
        
        population = newPopulation;
        generation++;
    }
    
    int selectParent(const std::vector<int>& sortedIndices) {
        int tournamentSize = 5;
        int best = sortedIndices[rand() % POPULATION_SIZE];
        
        for (int i = 1; i < tournamentSize; i++) {
            int candidate = sortedIndices[rand() % POPULATION_SIZE];
            if (fitness[candidate] > fitness[best]) {
                best = candidate;
            }
        }
        
        return best;
    }
    
    NeuralNet getBest() {
        int bestIdx = 0;
        for (int i = 1; i < POPULATION_SIZE; i++) {
            if (fitness[i] > fitness[bestIdx]) bestIdx = i;
        }
        return population[bestIdx];
    }
};
