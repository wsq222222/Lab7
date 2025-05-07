#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

// ====== Базовый класс ======
class Entity {
public:
    virtual ~Entity() {}
    virtual void display() const = 0;
    virtual void save(std::ofstream& out) const = 0;
    virtual void load(std::ifstream& in) = 0;
};

// ====== Класс Player ======
class Player : public Entity {
private:
    std::string name;
    int health;
    int level;

public:
    Player() : name(""), health(0), level(0) {}
    Player(const std::string& n, int h, int l) : name(n), health(h), level(l) {}

    void display() const override {
        std::cout << "Player: " << name << ", Health: " << health << ", Level: " << level << std::endl;
    }

    void save(std::ofstream& out) const override {
        out << "Player " << name << " " << health << " " << level << std::endl;
    }

    void load(std::ifstream& in) override {
        in >> name >> health >> level;
    }
};

// ====== Менеджер игры ======
template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    ~GameManager() {
        for (auto e : entities) {
            delete e;
        }
    }

    void addEntity(T entity) {
        entities.push_back(entity);
    }

    const std::vector<T>& getEntities() const {
        return entities;
    }

    void displayAll() const {
        for (auto e : entities) {
            e->display();
        }
    }

    void clear() {
        for (auto e : entities) {
            delete e;
        }
        entities.clear();
    }
};

// ====== Функция сохранения ======
void saveToFile(const GameManager<Entity*>& manager, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    const auto& entities = manager.getEntities();
    file << entities.size() << std::endl;

    for (auto entity : entities) {
        entity->save(file);
    }

    file.close();
}

// ====== Функция загрузки ======
void loadFromFile(GameManager<Entity*>& manager, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    manager.clear();

    size_t count;
    file >> count;

    for (size_t i = 0; i < count; ++i) {
        std::string type;
        file >> type;

        if (type == "Player") {
            Player* player = new Player();
            player->load(file);
            manager.addEntity(player);
        }
        else {
            throw std::runtime_error("Unknown entity type: " + type);
        }
    }

    file.close();
}

// ====== Главная функция ======
int main() {
    try {
        GameManager<Entity*> manager;

        // Создаем персонажей
        manager.addEntity(new Player("Hero", 100, 1));
        manager.addEntity(new Player("Mage", 80, 2));
        manager.addEntity(new Player("Warrior", 120, 3));

        // Сохраняем в файл
        saveToFile(manager, "game_save.txt");

        // Загружаем в новый менеджер
        GameManager<Entity*> loadedManager;
        loadFromFile(loadedManager, "game_save.txt");

        // Показываем загруженных персонажей
        std::cout << "Loaded characters:" << std::endl;
        loadedManager.displayAll();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
