#pragma once

#include "models.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ims {

class Inventory {
private:
    std::vector<std::unique_ptr<Product>> products_;
    std::vector<Transaction> transactions_;

public:
    Inventory() = default;
    ~Inventory() = default;

    void addProduct(std::unique_ptr<Product> product);
    void addProduct(const Electronics& product);
    void addProduct(const Food& product);
    void addProduct(const OfficeSupply& product);

    Product* findProduct(int id);
    const Product* findProduct(int id) const;

    bool removeProduct(int id);
    bool restockProduct(int id, int amount);
    bool sellProduct(int id, int amount);

    void listProducts(std::ostream& out) const;
    void listTransactions(std::ostream& out) const;
    void printLowStockReport(std::ostream& out, int threshold) const;
    void printSummary(std::ostream& out) const;

    InventoryStats stats() const;

    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

    void seedDemoData();
    bool empty() const { return products_.empty(); }
    std::size_t size() const { return products_.size(); }
};

} // namespace ims
