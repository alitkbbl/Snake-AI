#include "ai.cpp"
#include <thread>
#include <chrono>

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void drawGame(const Game& game) {
    clearScreen();
    
    for (int y = 0; y < Game::HEIGHT + 2; y++) {
        for (int x = 0; x < Game::WIDTH + 2; x++) {
            if (y == 0 || y == Game::HEIGHT + 1 || x == 0 || x == Game::WIDTH + 1) {
                std::cout << "█";
            } else {
                Point p = {x - 1, y - 1};
                
                if (p == game.snake.getHead()) {
                    std::cout << "●";
                } else if (p == game.food) {
                    std::cout << "◆";
                } else if (game.isOnSnake(p)) {
                    std::cout << "○";
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "\nScore: " << game.snake.score 
              << " | Steps: " << game.snake.steps 
              << " | Alive: " << (game.snake.alive ? "Yes" : "No") << "\n";
}

void watchBestAI(NeuralNet& brain) {
    Game game;
    
    while (game.snake.alive) {
        drawGame(game);
        
        std::vector<float> inputs = game.getInputs();
        Direction dir = brain.predict(inputs);
        game.snake.dir = dir;
        game.update();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    drawGame(game);
    std::cout << "\nGame Over! Final Score: " << game.snake.score << "\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

int main() {
    srand(time(0));
    
    GeneticAlgorithm ga;
    
    std::cout << "=== Snake AI Training ===\n";
    std::cout << "Population: " << GeneticAlgorithm::POPULATION_SIZE << "\n";
    std::cout << "Neural Network: 16 -> 12 -> 4\n\n";
    
    for (int gen = 0; gen < 100; gen++) {
        ga.evaluatePopulation();
        
        int avgFitness = 0;
        for (int f : ga.fitness) avgFitness += f;
        avgFitness /= GeneticAlgorithm::POPULATION_SIZE;
        
        std::cout << "Gen " << std::setw(3) << gen + 1 
                  << " | Best Score: " << std::setw(3) << ga.bestScore
                  << " | Best Fitness: " << std::setw(6) << ga.bestFitness
                  << " | Avg Fitness: " << std::setw(6) << avgFitness << "\n";
        
        if ((gen + 1) % 10 == 0) {
            std::cout << "\n>>> Watching best AI from generation " << gen + 1 << "...\n";
            std::cout << "Press Enter to watch...";
            std::cin.get();
            
            NeuralNet best = ga.getBest();
            watchBestAI(best);
        }
        
        ga.evolve();
    }
    
    std::cout << "\n=== Training Complete ===\n";
    std::cout << "Best Score Achieved: " << ga.bestScore << "\n";
    std::cout << "Best Fitness: " << ga.bestFitness << "\n\n";
    
    std::cout << "Watch final best AI? (y/n): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice == 'y' || choice == 'Y') {
        NeuralNet best = ga.getBest();
        watchBestAI(best);
    }
    
    return 0;
}
