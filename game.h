#ifndef GAME_H
#define GAME_H

#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
public:
    std::vector<Point> body;
    Direction dir;
    bool alive;
    int score;
    int steps;
    int stepsSinceFood;
    
    Snake(int startX, int startY) : dir(RIGHT), alive(true), score(0), steps(0), stepsSinceFood(0) {
        body.push_back({startX, startY});
        body.push_back({startX - 1, startY});
        body.push_back({startX - 2, startY});
    }
    
    Point getHead() const { return body[0]; }
    
    void move() {
        if (!alive) return;
        
        Point head = getHead();
        Point newHead = head;
        
        switch (dir) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }
        
        body.insert(body.begin(), newHead);
        body.pop_back();
        
        steps++;
        stepsSinceFood++;
    }
    
    void grow() {
        Point tail = body.back();
        body.push_back(tail);
        score++;
        stepsSinceFood = 0;
    }
    
    bool checkSelfCollision() const {
        Point head = getHead();
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i] == head) return true;
        }
        return false;
    }
};

class Game {
public:
    static const int WIDTH = 20;
    static const int HEIGHT = 20;
    
    Snake snake;
    Point food;
    
    Game() : snake(WIDTH / 2, HEIGHT / 2) {
        srand(time(0));
        spawnFood();
    }
    
    void spawnFood() {
        do {
            food.x = rand() % WIDTH;
            food.y = rand() % HEIGHT;
        } while (isOnSnake(food));
    }
    
    bool isOnSnake(const Point& p) const {
        for (const auto& segment : snake.body) {
            if (segment == p) return true;
        }
        return false;
    }
    
    void update() {
        if (!snake.alive) return;
        
        snake.move();
        
        Point head = snake.getHead();
        
        if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
            snake.alive = false;
            return;
        }
        
        if (snake.checkSelfCollision()) {
            snake.alive = false;
            return;
        }
        
        if (head == food) {
            snake.grow();
            spawnFood();
        }
        
        if (snake.stepsSinceFood > 100) {
            snake.alive = false;
        }
    }
    
    std::vector<float> getInputs() const {
        Point head = snake.getHead();
        
        float dx = (food.x - head.x) / (float)WIDTH;
        float dy = (food.y - head.y) / (float)HEIGHT;
        
        float angle = atan2(dy, dx) / M_PI;
        
        float dangerUp = isDanger(head.x, head.y - 1) ? 1.0f : 0.0f;
        float dangerDown = isDanger(head.x, head.y + 1) ? 1.0f : 0.0f;
        float dangerLeft = isDanger(head.x - 1, head.y) ? 1.0f : 0.0f;
        float dangerRight = isDanger(head.x + 1, head.y) ? 1.0f : 0.0f;
        
        float dangerUpLeft = isDanger(head.x - 1, head.y - 1) ? 1.0f : 0.0f;
        float dangerUpRight = isDanger(head.x + 1, head.y - 1) ? 1.0f : 0.0f;
        float dangerDownLeft = isDanger(head.x - 1, head.y + 1) ? 1.0f : 0.0f;
        float dangerDownRight = isDanger(head.x + 1, head.y + 1) ? 1.0f : 0.0f;
        
        float dirUp = (snake.dir == UP) ? 1.0f : 0.0f;
        float dirDown = (snake.dir == DOWN) ? 1.0f : 0.0f;
        float dirLeft = (snake.dir == LEFT) ? 1.0f : 0.0f;
        float dirRight = (snake.dir == RIGHT) ? 1.0f : 0.0f;
        
        float length = snake.body.size() / 100.0f;
        
        return {
            dx, dy, angle,
            dangerUp, dangerDown, dangerLeft, dangerRight,
            dangerUpLeft, dangerUpRight, dangerDownLeft, dangerDownRight,
            dirUp, dirDown, dirLeft, dirRight,
            length
        };
    }
    
    bool isDanger(int x, int y) const {
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return true;
        
        Point p = {x, y};
        for (size_t i = 0; i < snake.body.size(); i++) {
            if (snake.body[i] == p) return true;
        }
        
        return false;
    }
    
    int getFitness() const {
        int fitness = 0;
        
        fitness += snake.score * snake.score * 500;
        
        if (snake.steps < 50) fitness -= 1000;
        
        Point head = snake.getHead();
        int distToFood = abs(head.x - food.x) + abs(head.y - food.y);
        fitness += (WIDTH + HEIGHT - distToFood) * 10;
        
        fitness += snake.steps;
        
        return fitness;
    }
    
    void reset() {
        snake = Snake(WIDTH / 2, HEIGHT / 2);
        spawnFood();
    }
};

#endif
