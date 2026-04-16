#pragma once

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace ims {

inline std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    const auto last = value.find_last_not_of(" \t\n\r");
    return value.substr(first, last - first + 1);
}

inline std::vector<std::string> split(const std::string& value, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(value);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

inline std::string join(const std::vector<std::string>& values, char delimiter) {
    std::ostringstream output;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            output << delimiter;
        }
        output << values[index];
    }
    return output.str();
}

inline bool toBool(const std::string& value) {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return lower == "1" || lower == "true" || lower == "yes";
}

inline std::string boolToString(bool value) {
    return value ? "true" : "false";
}

inline std::string currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif
    std::ostringstream output;
    output << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

struct InventoryStats {
    int totalProducts{};
    int lowStockProducts{};
    double totalValue{};
};

class Product {
protected:
    int id_{};
    std::string name_;
    int quantity_{};
    double price_{};
    std::string supplier_;

    static inline int nextId_ = 1;
    static inline int createdCount_ = 0;

    Product(int id, std::string name, int quantity, double price, std::string supplier)
        : id_(id), name_(std::move(name)), quantity_(quantity), price_(price), supplier_(std::move(supplier)) {
        ++createdCount_;
        if (id_ >= nextId_) {
            nextId_ = id_ + 1;
        }
    }

public:
    Product(std::string name, int quantity, double price, std::string supplier)
        : Product(nextId_++, std::move(name), quantity, price, std::move(supplier)) {}

    Product() : Product("Unnamed", 0, 0.0, "Unknown") {}

    virtual ~Product() = default;

    Product(const Product&) = default;
    Product& operator=(const Product&) = default;
    Product(Product&&) noexcept = default;
    Product& operator=(Product&&) noexcept = default;

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    int getQuantity() const { return quantity_; }
    double getPrice() const { return price_; }
    const std::string& getSupplier() const { return supplier_; }

    void setQuantity(int quantity) { quantity_ = quantity < 0 ? 0 : quantity; }
    void setPrice(double price) { price_ = price < 0.0 ? 0.0 : price; }
    void setSupplier(const std::string& supplier) { supplier_ = supplier; }

    void restock(int amount) { quantity_ += amount > 0 ? amount : 0; }

    bool consume(int amount) {
        if (amount <= 0 || amount > quantity_) {
            return false;
        }
        quantity_ -= amount;
        return true;
    }

    double totalValue() const { return quantity_ * price_; }

    static int getCreatedCount() { return createdCount_; }
    static int peekNextId() { return nextId_; }
    static void syncNextId(int loadedId) {
        if (loadedId >= nextId_) {
            nextId_ = loadedId + 1;
        }
    }

    virtual std::string getType() const = 0;
    virtual std::string serialize() const = 0;
    virtual std::unique_ptr<Product> clone() const = 0;
    virtual void printDetails(std::ostream& out) const = 0;

    friend std::ostream& operator<<(std::ostream& out, const Product& product) {
        product.printDetails(out);
        return out;
    }
};

class Electronics : public Product {
private:
    std::string brand_;
    int warrantyMonths_{};

public:
    Electronics(std::string name, int quantity, double price, std::string supplier, std::string brand, int warrantyMonths)
        : Product(std::move(name), quantity, price, std::move(supplier)), brand_(std::move(brand)), warrantyMonths_(warrantyMonths) {}

    Electronics(int id, std::string name, int quantity, double price, std::string supplier, std::string brand, int warrantyMonths)
        : Product(id, std::move(name), quantity, price, std::move(supplier)), brand_(std::move(brand)), warrantyMonths_(warrantyMonths) {}

    std::string getType() const override { return "Electronics"; }

    std::string getBrand() const { return brand_; }
    int getWarrantyMonths() const { return warrantyMonths_; }

    std::string serialize() const override {
        return join({"electronics", std::to_string(getId()), getName(), std::to_string(getQuantity()),
                     std::to_string(getPrice()), getSupplier(), brand_, std::to_string(warrantyMonths_)}, '|');
    }

    std::unique_ptr<Product> clone() const override {
        return std::make_unique<Electronics>(*this);
    }

    void printDetails(std::ostream& out) const override {
        out << std::left << std::setw(12) << getType() << " | "
            << std::setw(4) << getId() << " | "
            << std::setw(18) << getName() << " | "
            << std::setw(7) << getQuantity() << " | "
            << std::setw(9) << std::fixed << std::setprecision(2) << getPrice() << " | "
            << std::setw(12) << brand_ << " | "
            << "Warranty: " << warrantyMonths_ << " months | "
            << "Supplier: " << getSupplier();
    }
};

class Food : public Product {
private:
    std::string expiryDate_;
    std::string storageInfo_;

public:
    Food(std::string name, int quantity, double price, std::string supplier, std::string expiryDate, std::string storageInfo)
        : Product(std::move(name), quantity, price, std::move(supplier)), expiryDate_(std::move(expiryDate)), storageInfo_(std::move(storageInfo)) {}

    Food(int id, std::string name, int quantity, double price, std::string supplier, std::string expiryDate, std::string storageInfo)
        : Product(id, std::move(name), quantity, price, std::move(supplier)), expiryDate_(std::move(expiryDate)), storageInfo_(std::move(storageInfo)) {}

    std::string getType() const override { return "Food"; }

    std::string getExpiryDate() const { return expiryDate_; }
    std::string getStorageInfo() const { return storageInfo_; }

    std::string serialize() const override {
        return join({"food", std::to_string(getId()), getName(), std::to_string(getQuantity()),
                     std::to_string(getPrice()), getSupplier(), expiryDate_, storageInfo_}, '|');
    }

    std::unique_ptr<Product> clone() const override {
        return std::make_unique<Food>(*this);
    }

    void printDetails(std::ostream& out) const override {
        out << std::left << std::setw(12) << getType() << " | "
            << std::setw(4) << getId() << " | "
            << std::setw(18) << getName() << " | "
            << std::setw(7) << getQuantity() << " | "
            << std::setw(9) << std::fixed << std::setprecision(2) << getPrice() << " | "
            << std::setw(12) << expiryDate_ << " | "
            << "Storage: " << storageInfo_ << " | "
            << "Supplier: " << getSupplier();
    }
};

class OfficeSupply : public Product {
private:
    std::string material_;
    bool reusable_{};

public:
    OfficeSupply(std::string name, int quantity, double price, std::string supplier, std::string material, bool reusable)
        : Product(std::move(name), quantity, price, std::move(supplier)), material_(std::move(material)), reusable_(reusable) {}

    OfficeSupply(int id, std::string name, int quantity, double price, std::string supplier, std::string material, bool reusable)
        : Product(id, std::move(name), quantity, price, std::move(supplier)), material_(std::move(material)), reusable_(reusable) {}

    std::string getType() const override { return "Office"; }

    std::string getMaterial() const { return material_; }
    bool isReusable() const { return reusable_; }

    std::string serialize() const override {
        return join({"office", std::to_string(getId()), getName(), std::to_string(getQuantity()),
                     std::to_string(getPrice()), getSupplier(), material_, boolToString(reusable_)}, '|');
    }

    std::unique_ptr<Product> clone() const override {
        return std::make_unique<OfficeSupply>(*this);
    }

    void printDetails(std::ostream& out) const override {
        out << std::left << std::setw(12) << getType() << " | "
            << std::setw(4) << getId() << " | "
            << std::setw(18) << getName() << " | "
            << std::setw(7) << getQuantity() << " | "
            << std::setw(9) << std::fixed << std::setprecision(2) << getPrice() << " | "
            << std::setw(12) << material_ << " | "
            << "Reusable: " << boolToString(reusable_) << " | "
            << "Supplier: " << getSupplier();
    }
};

class Transaction {
private:
    int id_{};
    int productId_{};
    std::string productName_;
    std::string action_;
    int quantity_{};
    double unitPrice_{};
    double totalPrice_{};
    std::string timestamp_;

    static inline int nextId_ = 1;

public:
    Transaction(std::string action, int productId, std::string productName, int quantity, double unitPrice)
        : id_(nextId_++), productId_(productId), productName_(std::move(productName)), action_(std::move(action)), quantity_(quantity),
          unitPrice_(unitPrice), totalPrice_(quantity * unitPrice), timestamp_(currentTimestamp()) {}

    int getId() const { return id_; }
    int getProductId() const { return productId_; }
    const std::string& getProductName() const { return productName_; }
    const std::string& getAction() const { return action_; }
    int getQuantity() const { return quantity_; }
    double getUnitPrice() const { return unitPrice_; }
    double getTotalPrice() const { return totalPrice_; }
    const std::string& getTimestamp() const { return timestamp_; }

    std::string serialize() const {
        return join({std::to_string(id_), action_, std::to_string(productId_), productName_, std::to_string(quantity_),
                     std::to_string(unitPrice_), std::to_string(totalPrice_), timestamp_}, '|');
    }

    friend std::ostream& operator<<(std::ostream& out, const Transaction& transaction) {
        out << std::left << std::setw(4) << transaction.id_ << " | "
            << std::setw(10) << transaction.action_ << " | "
            << std::setw(4) << transaction.productId_ << " | "
            << std::setw(18) << transaction.productName_ << " | "
            << std::setw(7) << transaction.quantity_ << " | "
            << std::setw(8) << std::fixed << std::setprecision(2) << transaction.unitPrice_ << " | "
            << std::setw(9) << transaction.totalPrice_ << " | "
            << transaction.timestamp_;
        return out;
    }
};

} // namespace ims
