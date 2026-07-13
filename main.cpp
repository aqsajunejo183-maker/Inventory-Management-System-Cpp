/*
 * Inventory Management System
 * ----------------------------
 * A console-based CRUD application to manage product inventory.
 * Demonstrates: OOP (classes, encapsulation), file I/O (CSV persistence),
 * exception handling, and clean menu-driven design.
 *
 * Author: <your name here>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// ------------------- Product Class -------------------
class Product {
public:
    int id;
    string name;
    string category;
    int quantity;
    double price;

    Product() : id(0), quantity(0), price(0.0) {}

    Product(int id, string name, string category, int quantity, double price)
        : id(id), name(move(name)), category(move(category)),
          quantity(quantity), price(price) {}

    double totalValue() const {
        return quantity * price;
    }

    // Convert product to a CSV line for saving
    string toCSV() const {
        ostringstream oss;
        oss << id << "," << name << "," << category << ","
            << quantity << "," << fixed << setprecision(2) << price;
        return oss.str();
    }

    // Parse a CSV line into a Product
    static Product fromCSV(const string& line) {
        stringstream ss(line);
        string idStr, name, category, qtyStr, priceStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, category, ',');
        getline(ss, qtyStr, ',');
        getline(ss, priceStr, ',');
        return Product(stoi(idStr), name, category, stoi(qtyStr), stod(priceStr));
    }
};

// ------------------- Inventory Manager Class -------------------
class InventoryManager {
private:
    vector<Product> products;
    const string filename = "inventory.csv";
    int nextId = 1;

public:
    InventoryManager() {
        loadFromFile();
    }

    ~InventoryManager() {
        saveToFile();
    }

    void addProduct() {
        string name, category;
        int quantity;
        double price;

        cout << "\n--- Add New Product ---\n";
        cout << "Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Category: ";
        getline(cin, category);
        quantity = readInt("Quantity: ");
        price = readDouble("Price: ");

        products.emplace_back(nextId++, name, category, quantity, price);
        cout << "Product added successfully (ID: " << products.back().id << ")\n";
    }

    void viewAllProducts() const {
        if (products.empty()) {
            cout << "\nNo products in inventory.\n";
            return;
        }
        printHeader();
        for (const auto& p : products) {
            printRow(p);
        }
        cout << "-----------------------------------------------------------\n";
        cout << "Total inventory value: $" << fixed << setprecision(2)
             << totalInventoryValue() << "\n";
    }

    void searchProduct() const {
        cout << "\nSearch by (1) ID or (2) Name: ";
        int choice = readInt("");
        cin.ignore();

        if (choice == 1) {
            int id = readInt("Enter ID: ");
            auto it = find_if(products.begin(), products.end(),
                               [id](const Product& p) { return p.id == id; });
            if (it != products.end()) {
                printHeader();
                printRow(*it);
            } else {
                cout << "Product not found.\n";
            }
        } else {
            string name;
            cout << "Enter name (or part of it): ";
            getline(cin, name);
            bool found = false;
            printHeader();
            for (const auto& p : products) {
                if (p.name.find(name) != string::npos) {
                    printRow(p);
                    found = true;
                }
            }
            if (!found) cout << "No matching products found.\n";
        }
    }

    void updateProduct() {
        int id = readInt("\nEnter ID of product to update: ");
        auto it = find_if(products.begin(), products.end(),
                           [id](const Product& p) { return p.id == id; });
        if (it == products.end()) {
            cout << "Product not found.\n";
            return;
        }

        cout << "Leave blank to keep current value.\n";
        cin.ignore();

        cout << "Name [" << it->name << "]: ";
        string name;
        getline(cin, name);
        if (!name.empty()) it->name = name;

        cout << "Category [" << it->category << "]: ";
        string category;
        getline(cin, category);
        if (!category.empty()) it->category = category;

        cout << "Quantity [" << it->quantity << "] (enter -1 to skip): ";
        int qty;
        cin >> qty;
        if (qty != -1) it->quantity = qty;

        cout << "Price [" << it->price << "] (enter -1 to skip): ";
        double price;
        cin >> price;
        if (price != -1) it->price = price;

        cout << "Product updated successfully.\n";
    }

    void deleteProduct() {
        int id = readInt("\nEnter ID of product to delete: ");
        auto it = find_if(products.begin(), products.end(),
                           [id](const Product& p) { return p.id == id; });
        if (it == products.end()) {
            cout << "Product not found.\n";
            return;
        }
        products.erase(it);
        cout << "Product deleted successfully.\n";
    }

    void lowStockReport() const {
        int threshold = readInt("\nShow products with quantity below: ");
        printHeader();
        bool found = false;
        for (const auto& p : products) {
            if (p.quantity < threshold) {
                printRow(p);
                found = true;
            }
        }
        if (!found) cout << "No low-stock products found.\n";
    }

    void saveToFile() const {
        ofstream file(filename);
        for (const auto& p : products) {
            file << p.toCSV() << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) return;

        string line;
        int maxId = 0;
        while (getline(file, line)) {
            if (line.empty()) continue;
            Product p = Product::fromCSV(line);
            products.push_back(p);
            maxId = max(maxId, p.id);
        }
        nextId = maxId + 1;
        file.close();
    }

private:
    double totalInventoryValue() const {
        double total = 0;
        for (const auto& p : products) total += p.totalValue();
        return total;
    }

    void printHeader() const {
        cout << "\n" << left << setw(5) << "ID" << setw(20) << "Name"
             << setw(15) << "Category" << setw(10) << "Qty"
             << setw(10) << "Price" << "Total\n";
        cout << "-----------------------------------------------------------\n";
    }

    void printRow(const Product& p) const {
        cout << left << setw(5) << p.id << setw(20) << p.name
             << setw(15) << p.category << setw(10) << p.quantity
             << "$" << setw(9) << fixed << setprecision(2) << p.price
             << "$" << p.totalValue() << "\n";
    }

    int readInt(const string& prompt) const {
        int value;
        cout << prompt;
        while (!(cin >> value)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return value;
    }

    double readDouble(const string& prompt) const {
        double value;
        cout << prompt;
        while (!(cin >> value)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return value;
    }
};

// ------------------- Main Menu -------------------
void printMenu() {
    cout << "\n=================================\n";
    cout << "   INVENTORY MANAGEMENT SYSTEM\n";
    cout << "=================================\n";
    cout << "1. Add Product\n";
    cout << "2. View All Products\n";
    cout << "3. Search Product\n";
    cout << "4. Update Product\n";
    cout << "5. Delete Product\n";
    cout << "6. Low Stock Report\n";
    cout << "7. Save & Exit\n";
    cout << "Choose an option: ";
}

int main() {
    InventoryManager manager;
    int choice;

    do {
        printMenu();
        while (!(cin >> choice)) {
            cout << "Invalid input. Try again: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: manager.addProduct(); break;
            case 2: manager.viewAllProducts(); break;
            case 3: manager.searchProduct(); break;
            case 4: manager.updateProduct(); break;
            case 5: manager.deleteProduct(); break;
            case 6: manager.lowStockReport(); break;
            case 7:
                manager.saveToFile();
                cout << "Inventory saved. Goodbye!\n";
                break;
            default:
                cout << "Invalid option, please try again.\n";
        }
    } while (choice != 7);

    return 0;
}
