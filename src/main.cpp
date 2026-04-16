#include "Inventory.hpp"

#include <iostream>
#include <limits>
#include <string>

using namespace ims;

namespace {

void printMenu() {
    std::cout << "\n================ INVENTORY MANAGEMENT SYSTEM ================\n";
    std::cout << "1. Urun ekle\n";
    std::cout << "2. Urunleri listele\n";
    std::cout << "3. Urun satisi yap\n";
    std::cout << "4. Stok arttir\n";
    std::cout << "5. Urun sil\n";
    std::cout << "6. Dusuk stok raporu\n";
    std::cout << "7. Islem gecmisi\n";
    std::cout << "8. Ozet\n";
    std::cout << "9. Kaydet ve cik\n";
    std::cout << "Secim: ";
}

int readInt(const std::string& prompt) {
    int value{};
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Gecersiz sayi, tekrar dene.\n";
    }
}

double readDouble(const std::string& prompt) {
    double value{};
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Gecersiz ondalik sayi, tekrar dene.\n";
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::cin >> std::ws;
    std::getline(std::cin, value);
    return value;
}

bool readBool(const std::string& prompt) {
    std::string value = readLine(prompt + " (yes/no): ");
    return toBool(value);
}

void addProductMenu(Inventory& inventory) {
    std::cout << "\nUrun turu sec:\n";
    std::cout << "1. Electronics\n";
    std::cout << "2. Food\n";
    std::cout << "3. OfficeSupply\n";
    const int type = readInt("Tur: ");

    const std::string name = readLine("Urun adi: ");
    const int quantity = readInt("Miktar: ");
    const double price = readDouble("Fiyat: ");
    const std::string supplier = readLine("Tedarikci: ");

    if (type == 1) {
        const std::string brand = readLine("Marka: ");
        const int warranty = readInt("Garanti ayi: ");
        inventory.addProduct(Electronics(name, quantity, price, supplier, brand, warranty));
    } else if (type == 2) {
        const std::string expiryDate = readLine("Son kullanma tarihi (YYYY-MM-DD): ");
        const std::string storage = readLine("Saklama bilgisi: ");
        inventory.addProduct(Food(name, quantity, price, supplier, expiryDate, storage));
    } else if (type == 3) {
        const std::string material = readLine("Malzeme: ");
        const bool reusable = readBool("Yeniden kullanilabilir mi");
        inventory.addProduct(OfficeSupply(name, quantity, price, supplier, material, reusable));
    } else {
        std::cout << "Gecersiz tur.\n";
        return;
    }

    std::cout << "Urun eklendi.\n";
}

void sellProductMenu(Inventory& inventory) {
    const int id = readInt("Urun ID: ");
    const int amount = readInt("Satilacak miktar: ");
    if (inventory.sellProduct(id, amount)) {
        std::cout << "Satis basarili.\n";
    } else {
        std::cout << "Satis basarisiz. Stok yetersiz olabilir.\n";
    }
}

void restockMenu(Inventory& inventory) {
    const int id = readInt("Urun ID: ");
    const int amount = readInt("Eklenecek miktar: ");
    if (inventory.restockProduct(id, amount)) {
        std::cout << "Stok guncellendi.\n";
    } else {
        std::cout << "Stok artirilamadi.\n";
    }
}

void deleteMenu(Inventory& inventory) {
    const int id = readInt("Silinecek urun ID: ");
    if (inventory.removeProduct(id)) {
        std::cout << "Urun silindi.\n";
    } else {
        std::cout << "Urun bulunamadi.\n";
    }
}

} // namespace

int main() {
    Inventory inventory;
    const std::string databasePath = "inventory_data.db";

    if (!inventory.loadFromFile(databasePath)) {
        inventory.seedDemoData();
    }

    std::cout << "\nInventory Management System basladi.\n";

    while (true) {
        printMenu();

        int choice{};
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Gecersiz secim.\n";
            continue;
        }

        switch (choice) {
            case 1:
                addProductMenu(inventory);
                break;
            case 2:
                inventory.listProducts(std::cout);
                break;
            case 3:
                sellProductMenu(inventory);
                break;
            case 4:
                restockMenu(inventory);
                break;
            case 5:
                deleteMenu(inventory);
                break;
            case 6:
                inventory.printLowStockReport(std::cout, 5);
                break;
            case 7:
                inventory.listTransactions(std::cout);
                break;
            case 8:
                inventory.printSummary(std::cout);
                break;
            case 9:
                if (inventory.saveToFile(databasePath)) {
                    std::cout << "Veri kaydedildi. Cikis yapiliyor.\n";
                } else {
                    std::cout << "Kayit basarisiz ama program kapatiliyor.\n";
                }
                return 0;
            default:
                std::cout << "Gecersiz secim.\n";
                break;
        }
    }
}
