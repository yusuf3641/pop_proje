#include "Inventory.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ims {

void Inventory::addProduct(std::unique_ptr<Product> product) {
    if (product) {
        products_.push_back(std::move(product));
    }
}

void Inventory::addProduct(const Electronics& product) {
    addProduct(product.clone());
}

void Inventory::addProduct(const Food& product) {
    addProduct(product.clone());
}

void Inventory::addProduct(const OfficeSupply& product) {
    addProduct(product.clone());
}

Product* Inventory::findProduct(int id) {
    for (auto& product : products_) {
        if (product && product->getId() == id) {
            return product.get();
        }
    }
    return nullptr;
}

const Product* Inventory::findProduct(int id) const {
    for (const auto& product : products_) {
        if (product && product->getId() == id) {
            return product.get();
        }
    }
    return nullptr;
}

bool Inventory::removeProduct(int id) {
    const auto iterator = std::remove_if(products_.begin(), products_.end(), [id](const auto& product) {
        return product && product->getId() == id;
    });
    if (iterator == products_.end()) {
        return false;
    }
    products_.erase(iterator, products_.end());
    return true;
}

bool Inventory::restockProduct(int id, int amount) {
    Product* product = findProduct(id);
    if (product == nullptr || amount <= 0) {
        return false;
    }
    product->restock(amount);
    transactions_.emplace_back("RESTOCK", product->getId(), product->getName(), amount, product->getPrice());
    return true;
}

bool Inventory::sellProduct(int id, int amount) {
    Product* product = findProduct(id);
    if (product == nullptr || amount <= 0) {
        return false;
    }
    if (!product->consume(amount)) {
        return false;
    }
    transactions_.emplace_back("SALE", product->getId(), product->getName(), amount, product->getPrice());
    return true;
}

void Inventory::listProducts(std::ostream& out) const {
    out << "\n==================== URUN LISTESI ====================\n";
    out << std::left << std::setw(12) << "TYPE" << " | "
        << std::setw(4) << "ID" << " | "
        << std::setw(18) << "NAME" << " | "
        << std::setw(7) << "QTY" << " | "
        << std::setw(9) << "PRICE" << " | "
        << "DETAILS\n";
    out << "---------------------------------------------------------------\n";
    if (products_.empty()) {
        out << "Stok bos.\n";
    }
    for (const auto& product : products_) {
        if (product) {
            out << *product << '\n';
        }
    }
}

void Inventory::listTransactions(std::ostream& out) const {
    out << "\n==================== ISLEM GECMISI ====================\n";
    out << std::left << std::setw(4) << "ID" << " | "
        << std::setw(10) << "ACTION" << " | "
        << std::setw(4) << "PID" << " | "
        << std::setw(18) << "PRODUCT" << " | "
        << std::setw(7) << "QTY" << " | "
        << std::setw(8) << "UNIT" << " | "
        << std::setw(9) << "TOTAL" << " | TIME\n";
    out << "--------------------------------------------------------------------------\n";
    if (transactions_.empty()) {
        out << "Henuz islem yok.\n";
    }
    for (const auto& transaction : transactions_) {
        out << transaction << '\n';
    }
}

InventoryStats Inventory::stats() const {
    InventoryStats result;
    result.totalProducts = static_cast<int>(products_.size());
    for (const auto& product : products_) {
        if (!product) {
            continue;
        }
        result.totalValue += product->totalValue();
        if (product->getQuantity() <= 5) {
            ++result.lowStockProducts;
        }
    }
    return result;
}

void Inventory::printSummary(std::ostream& out) const {
    const InventoryStats summary = stats();
    out << "\n==================== OZET ====================\n";
    out << "Toplam urun cesidi : " << summary.totalProducts << '\n';
    out << "Dusuk stoklu urun  : " << summary.lowStockProducts << '\n';
    out << "Toplam stok degeri : " << std::fixed << std::setprecision(2) << summary.totalValue << '\n';
    out << "Olusturulan nesne   : " << Product::getCreatedCount() << '\n';
}

void Inventory::printLowStockReport(std::ostream& out, int threshold) const {
    int matchCount = 0;
    for (const auto& product : products_) {
        if (product && product->getQuantity() <= threshold) {
            ++matchCount;
        }
    }

    out << "\n==================== DUSUK STOK RAPORU ====================\n";
    if (matchCount == 0) {
        out << "Esik degerin altinda urun yok.\n";
        return;
    }

    int* ids = new int[matchCount];
    int* quantities = new int[matchCount];
    std::string* names = new std::string[matchCount];

    int index = 0;
    for (const auto& product : products_) {
        if (product && product->getQuantity() <= threshold) {
            ids[index] = product->getId();
            quantities[index] = product->getQuantity();
            names[index] = product->getName();
            ++index;
        }
    }

    out << std::left << std::setw(6) << "ID" << " | "
        << std::setw(18) << "NAME" << " | "
        << "QTY\n";
    out << "------------------------------------------------\n";
    for (int i = 0; i < matchCount; ++i) {
        out << std::left << std::setw(6) << ids[i] << " | "
            << std::setw(18) << names[i] << " | "
            << quantities[i] << '\n';
    }

    delete[] ids;
    delete[] quantities;
    delete[] names;
}

bool Inventory::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& product : products_) {
        if (product) {
            file << product->serialize() << '\n';
        }
    }
    return true;
}

bool Inventory::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    products_.clear();
    transactions_.clear();

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> parts = split(line, '|');
        if (parts.size() < 2) {
            continue;
        }

        const std::string type = parts[0];
        const int id = std::stoi(parts[1]);
        Product::syncNextId(id);

        if (type == "electronics" && parts.size() >= 8) {
            addProduct(std::make_unique<Electronics>(id, parts[2], std::stoi(parts[3]), std::stod(parts[4]), parts[5], parts[6], std::stoi(parts[7])));
        } else if (type == "food" && parts.size() >= 8) {
            addProduct(std::make_unique<Food>(id, parts[2], std::stoi(parts[3]), std::stod(parts[4]), parts[5], parts[6], parts[7]));
        } else if (type == "office" && parts.size() >= 8) {
            addProduct(std::make_unique<OfficeSupply>(id, parts[2], std::stoi(parts[3]), std::stod(parts[4]), parts[5], parts[6], toBool(parts[7])));
        }
    }

    return true;
}

void Inventory::seedDemoData() {
    if (!products_.empty()) {
        return;
    }

    addProduct(Electronics("Laptop", 8, 37999.90, "TeknoMarket", "Lenovo", 24));
    addProduct(Food("Milk", 3, 32.50, "FreshFarm", "2026-04-30", "Keep refrigerated"));
    addProduct(OfficeSupply("Notebook", 12, 24.75, "OfficePlus", "Paper", true));
}

} // namespace ims
