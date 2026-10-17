// Programmer: Mohamed Aziz Chebbi
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "Product.h"
#include "Inventory.h"
#include "Utils.h"
using namespace std;

const string PROJECT_NAME = "StockFlow";
const string VERSION = "1.0";
const string PROGRAMMER = "Mohamed Aziz Chebbi";
const string PRODUCTS_FILE = "products.txt";
const string SALES_FILE = "sales.txt";

const int MAX_LOGIN_ATTEMPTS = 3;
const int DEFAULT_LOW_STOCK_THRESHOLD = 5;
const int MAX_SOLD_PRODUCTS = 500;      
const int MAX_ID = 999999;
const int MAX_STOCK = 1000000;
const double MAX_PRICE = 1000000.0;

struct User {
    string username;
    string password;
    string role;
};
const int USER_COUNT = 2;
const User USERS[USER_COUNT] = {
    {"admin", "admin123", "Admin"},
    {"staff", "staff123", "Staff"}
};

struct SalesSummary {
    int transactions;
    int invalidLines;
    int unitsSold;
    double revenue;
    string bestName;
    int bestQuantity;
};

void printBorder() {
    cout << "  +" << string(50, '-') << "+\n";
}

void printBoxLine(const string& text) {
    int width = 50;
    int left = (width - (int)text.length()) / 2;
    int right = width - (int)text.length() - left;
    cout << "  |" << string(left, ' ') << text << string(right, ' ') << "|\n";
}

void printFooter() {
    cout << "\n  ---- " << PROJECT_NAME << " | Version " << VERSION << " ----\n";
}

void showWelcomeScreen() {
    cout << "\n";
    printBorder();
    printBoxLine("");
    printBoxLine("[=]  [=]  [=]  [=]  [=]");
    printBoxLine("S T O C K F L O W");
    printBoxLine("Inventory Management System");
    printBoxLine("[=]  [=]  [=]  [=]  [=]");
    printBoxLine("");
    printBoxLine("Programmed by " + PROGRAMMER);
    printBoxLine("Version " + VERSION);
    printBoxLine("");
    printBorder();
}

void showProgrammerInfo() {
    cout << "\n=========== PROGRAMMER INFORMATION ===========\n";
    cout << "Programmer : " << PROGRAMMER << "\n";
    cout << "Project    : " << PROJECT_NAME << "\n";
    cout << "Language   : C++ (console application)\n";
    cout << "Version    : " << VERSION << "\n";
}

void printTableHeader() {
    cout << left << setw(8) << "ID" << setw(22) << "Name" << setw(16) << "Category"
         << setw(8) << "Stock" << setw(10) << "Price" << "\n";
    cout << string(64, '-') << "\n";
}

void printProductRow(const Product& p) {
    cout << left << setw(8) << p.getId()
         << setw(22) << shorten(p.getName(), 20)
         << setw(16) << shorten(p.getCategory(), 14)
         << setw(8) << p.getStock()
         << setw(10) << p.getPrice() << "\n";
}

void printEmptyInventoryMessage() {
    cout << "The inventory is empty. Ask an Admin to add products first.\n";
}

bool login(string& role) {
    cout << "\n=============== LOGIN ===============\n";
    cout << MAX_LOGIN_ATTEMPTS << " attempts.)\n";
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        string username = readLine("Username: ");
        string password = readLine("Password: ");
        for (int i = 0; i < USER_COUNT; i++) {
            if (USERS[i].username == username && USERS[i].password == password) {
                role = USERS[i].role;
                cout << "Login successful. Welcome, " << username << " (" << role << ").\n";
                return true;
            }
        }
        cout << "Error: incorrect username or password.";
        int remaining = MAX_LOGIN_ATTEMPTS - attempt;
        if (remaining > 0)
            cout << " Attempts remaining: " << remaining;
        cout << "\n";
    }
    cout << "Too many failed attempts. Access denied.\n";
    return false;
}

bool saveInventory(const Inventory& inventory) {
    if (!inventory.save(PRODUCTS_FILE)) {
        cout << "Error: could not write " << PRODUCTS_FILE << ". Changes are NOT saved on disk.\n";
        return false;
    }
    return true;
}

void loadInventory(Inventory& inventory) {
    int invalidLines = 0;
    int migratedLines = 0;
    if (!inventory.load(PRODUCTS_FILE, invalidLines, migratedLines)) {
        cout << "Info: " << PRODUCTS_FILE << " not found or cannot be opened. "
             << "Starting with an empty inventory (the file is created at the first save).\n";
        return;
    }
    cout << inventory.getCount() << " product(s) loaded from " << PRODUCTS_FILE << ".\n";
    if (migratedLines > 0)
        cout << "Info: " << migratedLines << " line(s) used the old format and were converted "
             << "(category set to 'General').\n";
    if (invalidLines > 0)
        cout << "Warning: " << invalidLines << " invalid line(s) were ignored.\n";
    if (migratedLines > 0 || invalidLines > 0) {
        cout << "A safety copy of the original file was saved as " << PRODUCTS_FILE << ".bak\n";
        if (saveInventory(inventory))
            cout << PRODUCTS_FILE << " was rewritten in the new format.\n";
    }
}

bool recordSale(const Product& product, int quantity, double total, const string& dateTime) {
    ofstream file(SALES_FILE.c_str(), ios::app);
    if (!file.is_open())
        return false;
    file << fixed << setprecision(2);
    file << dateTime << "|" << product.getId() << "|" << product.getName() << "|"
         << quantity << "|" << product.getPrice() << "|" << total << endl;
    bool ok = file.good();
    file.close();
    return ok;
}

bool readSalesSummary(SalesSummary& summary) {
    summary.transactions = 0;
    summary.invalidLines = 0;
    summary.unitsSold = 0;
    summary.revenue = 0.0;
    summary.bestName = "";
    summary.bestQuantity = 0;

    ifstream file(SALES_FILE.c_str());
    if (!file.is_open())
        return false;

    int ids[MAX_SOLD_PRODUCTS];
    string names[MAX_SOLD_PRODUCTS];
    int quantities[MAX_SOLD_PRODUCTS];
    int different = 0;

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty())
            continue;
        string f[6];
        if (splitLine(line, '|', f, 6) != 6 || !isInteger(f[1]) || !isInteger(f[3])
            || !isDecimal(f[4]) || !isDecimal(f[5])) {
            summary.invalidLines++;
            continue;
        }
        int id = toInt(f[1]);
        int quantity = toInt(f[3]);
        summary.transactions++;
        summary.unitsSold += quantity;
        summary.revenue += toDouble(f[5]);

        int position = -1;
        for (int i = 0; i < different; i++)
            if (ids[i] == id)
                position = i;
        if (position == -1 && different < MAX_SOLD_PRODUCTS) {
            position = different;
            ids[position] = id;
            names[position] = f[2];
            quantities[position] = 0;
            different++;
        }
        if (position != -1)
            quantities[position] += quantity;
    }
    file.close();

    for (int i = 0; i < different; i++) {
        if (quantities[i] > summary.bestQuantity) {
            summary.bestQuantity = quantities[i];
            summary.bestName = names[i];
        }
    }
    return true;
}

void addProductScreen(Inventory& inventory) {
    cout << "\n--- Add a product ---\n";
    if (inventory.isFull()) {
        cout << "Error: the inventory is full (maximum " << MAX_PRODUCTS << " products).\n";
        return;
    }
    int id = readInt("Product ID: ", 1, MAX_ID);
    if (inventory.findIndexById(id) != -1) {
        cout << "Error: a product with ID " << id << " already exists.\n";
        return;
    }
    string name = readText("Name: ", 20);
    string category = readText("Category: ", 14);
    int stock = readInt("Stock quantity: ", 0, MAX_STOCK);
    double price = readDouble("Selling price: ", 0.0, MAX_PRICE);

    inventory.addProduct(Product(id, name, category, stock, price));
    if (saveInventory(inventory))
        cout << "Success: product added.\n";
}

void viewProductsScreen(const Inventory& inventory) {
    cout << "\n=============== INVENTORY ===============\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    printTableHeader();
    for (int i = 0; i < inventory.getCount(); i++)
        printProductRow(inventory.getProduct(i));
    cout << "Total: " << inventory.getCount() << " product(s)\n";
}

void searchByIdScreen(const Inventory& inventory) {
    cout << "\n--- Search by ID ---\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    int id = readInt("Product ID: ", 1, MAX_ID);
    int index = inventory.findIndexById(id);
    if (index == -1) {
        cout << "Product not found.\n";
        return;
    }
    printTableHeader();
    printProductRow(inventory.getProduct(index));
}

void searchByNameScreen(const Inventory& inventory) {
    cout << "\n--- Search by name ---\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    string text = readText("Name (or part of it): ", 20);
    int results[MAX_PRODUCTS];
    int found = inventory.findByName(text, results);
    if (found == 0) {
        cout << "Product not found.\n";
        return;
    }
    printTableHeader();
    for (int i = 0; i < found; i++)
        printProductRow(inventory.getProduct(results[i]));
    cout << found << " product(s) found.\n";
}

void updateProductScreen(Inventory& inventory) {
    cout << "\n--- Update a product ---\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    int id = readInt("ID of the product to update: ", 1, MAX_ID);
    int index = inventory.findIndexById(id);
    if (index == -1) {
        cout << "Product not found.\n";
        return;
    }
    Product& product = inventory.getProduct(index);
    printTableHeader();
    printProductRow(product);

    cout << "1. Name\n2. Category\n3. Stock quantity\n4. Selling price\n0. Cancel\n";
    int choice = readInt("Field to update: ", 0, 4);
    switch (choice) {
        case 1: product.setName(readText("New name: ", 20)); break;
        case 2: product.setCategory(readText("New category: ", 14)); break;
        case 3: product.setStock(readInt("New stock quantity: ", 0, MAX_STOCK)); break;
        case 4: product.setPrice(readDouble("New selling price: ", 0.0, MAX_PRICE)); break;
        default: cout << "Update cancelled.\n"; return;
    }
    if (saveInventory(inventory))
        cout << "Success: product updated.\n";
}

void deleteProductScreen(Inventory& inventory) {
    cout << "\n--- Delete a product ---\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    int id = readInt("ID of the product to delete: ", 1, MAX_ID);
    int index = inventory.findIndexById(id);
    if (index == -1) {
        cout << "Product not found. Nothing was deleted.\n";
        return;
    }
    printTableHeader();
    printProductRow(inventory.getProduct(index));
    if (!confirm("Do you really want to delete this product?")) {
        cout << "Deletion cancelled.\n";
        return;
    }
    inventory.removeById(id);
    if (saveInventory(inventory))
        cout << "Success: product deleted.\n";
}

void productMenu(Inventory& inventory, const string& role) {
    bool isAdmin = (role == "Admin");
    int choice;
    do {
        cout << "\n========== PRODUCT MANAGEMENT ==========\n";
        cout << "1. Add a product            [Admin]\n";
        cout << "2. Display all products\n";
        cout << "3. Search by ID\n";
        cout << "4. Search by name\n";
        cout << "5. Update a product         [Admin]\n";
        cout << "6. Delete a product         [Admin]\n";
        cout << "0. Back to main menu\n";
        printFooter();
        choice = readInt("Your choice: ", 0, 6);

        if ((choice == 1 || choice == 5 || choice == 6) && !isAdmin) {
            cout << "Access denied: this operation is reserved for Admin users.\n";
            continue;
        }
        switch (choice) {
            case 1: addProductScreen(inventory); break;
            case 2: viewProductsScreen(inventory); break;
            case 3: searchByIdScreen(inventory); break;
            case 4: searchByNameScreen(inventory); break;
            case 5: updateProductScreen(inventory); break;
            case 6: deleteProductScreen(inventory); break;
        }
    } while (choice != 0);
}

void printReceipt(const Product& product, int quantity, double total, const string& dateTime) {
    cout << "\n========== RECEIPT ==========\n";
    cout << "Date       : " << dateTime << "\n";
    cout << "Product    : " << product.getName() << " (ID " << product.getId() << ")\n";
    cout << "Quantity   : " << quantity << "\n";
    cout << "Unit price : " << product.getPrice() << "\n";
    cout << "TOTAL      : " << total << "\n";
    cout << "=============================\n";
}

void sellProductScreen(Inventory& inventory) {
    cout << "\n--- Sell a product ---\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    int id = readInt("Product ID: ", 1, MAX_ID);
    int index = inventory.findIndexById(id);
    if (index == -1) {
        cout << "Product not found. No sale was made.\n";
        return;
    }
    Product& product = inventory.getProduct(index);
    printTableHeader();
    printProductRow(product);

    int quantity = readInt("Quantity to sell: ", -MAX_STOCK, MAX_STOCK);
    if (quantity <= 0) {
        cout << "Error: the quantity must be at least 1. No sale was made.\n";
        return;
    }
    if (quantity > product.getStock()) {
        cout << "Error: not enough stock (available: " << product.getStock()
             << "). No sale was made.\n";
        return;
    }

    double total = quantity * product.getPrice();
    string dateTime = currentDateTime();

    if (!recordSale(product, quantity, total, dateTime)) {
        cout << "Error: could not write " << SALES_FILE << ". No sale was made.\n";
        return;
    }
    product.setStock(product.getStock() - quantity);
    saveInventory(inventory);
    printReceipt(product, quantity, total, dateTime);
}

void lowStockScreen(const Inventory& inventory, int threshold) {
    cout << "\n=========== LOW STOCK ALERT ===========\n";
    cout << "Threshold: stock <= " << threshold << "\n";
    if (inventory.isEmpty()) {
        printEmptyInventoryMessage();
        return;
    }
    int found = 0;
    for (int i = 0; i < inventory.getCount(); i++) {
        if (inventory.getProduct(i).getStock() <= threshold) {
            if (found == 0)
                printTableHeader();
            printProductRow(inventory.getProduct(i));
            found++;
        }
    }
    if (found == 0)
        cout << "No product is below the threshold.\n";
    else
        cout << found << " product(s) need restocking.\n";
}

void salesListScreen() {
    cout << "\n============= SALES REPORT =============\n";
    ifstream file(SALES_FILE.c_str());
    if (!file.is_open()) {
        cout << "No sales recorded yet (" << SALES_FILE << " does not exist).\n";
        return;
    }
    cout << left << setw(21) << "Date" << setw(8) << "ID" << setw(20) << "Product"
         << setw(6) << "Qty" << setw(10) << "Unit" << setw(10) << "Total" << "\n";
    cout << string(75, '-') << "\n";

    int shown = 0;
    int invalid = 0;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty())
            continue;
        string f[6];
        if (splitLine(line, '|', f, 6) != 6 || !isInteger(f[1]) || !isInteger(f[3])
            || !isDecimal(f[4]) || !isDecimal(f[5])) {
            invalid++;
            continue;
        }
        cout << left << setw(21) << f[0] << setw(8) << f[1] << setw(20) << shorten(f[2], 18)
             << setw(6) << f[3] << setw(10) << toDouble(f[4]) << setw(10) << toDouble(f[5]) << "\n";
        shown++;
    }
    file.close();
    if (shown == 0)
        cout << "No valid sales found.\n";
    else
        cout << shown << " transaction(s).\n";
    if (invalid > 0)
        cout << "Warning: " << invalid << " unreadable line(s) ignored (old format?).\n";
}

void statisticsScreen(const Inventory& inventory) {
    cout << "\n============== STATISTICS ==============\n";
    cout << "Products in inventory : " << inventory.getCount() << "\n";
    cout << "Inventory value       : " << inventory.totalValue()
         << "  (current stock x selling price)\n";

    SalesSummary summary;
    if (!readSalesSummary(summary)) {
        cout << "No sales recorded yet.\n";
    } else {
        cout << "Sales transactions    : " << summary.transactions << "\n";
        cout << "Units sold            : " << summary.unitsSold << "\n";
        cout << "Total revenue         : " << summary.revenue << "\n";
        if (summary.bestQuantity > 0)
            cout << "Best-selling product  : " << summary.bestName
                 << " (" << summary.bestQuantity << " units)\n";
        if (summary.invalidLines > 0)
            cout << "Warning: " << summary.invalidLines << " unreadable line(s) ignored.\n";
    }
    cout << "Note: revenue is NOT profit. Purchase costs are not stored, so profit is not calculated.\n";
}

void reportsMenu(const Inventory& inventory, int& lowStockThreshold) {
    int choice;
    do {
        cout << "\n============= REPORTS =============\n";
        cout << "1. Low-stock alert (current threshold: " << lowStockThreshold << ")\n";
        cout << "2. Display all sales\n";
        cout << "3. Statistics (revenue, inventory value, best seller)\n";
        cout << "4. Change the low-stock threshold\n";
        cout << "0. Back to main menu\n";
        printFooter();
        choice = readInt("Your choice: ", 0, 4);
        switch (choice) {
            case 1: lowStockScreen(inventory, lowStockThreshold); break;
            case 2: salesListScreen(); break;
            case 3: statisticsScreen(inventory); break;
            case 4:
                lowStockThreshold = readInt("New threshold (0 to 100000): ", 0, 100000);
                cout << "Threshold set to " << lowStockThreshold << ".\n";
                break;
        }
    } while (choice != 0);
}

int main() {
    cout << fixed << setprecision(2);   

    showWelcomeScreen();

    string role;
    if (!login(role))
        return 0;

    Inventory inventory;
    loadInventory(inventory);
    int lowStockThreshold = DEFAULT_LOW_STOCK_THRESHOLD;

    bool running = true;
    while (running) {
        cout << "\n=============== MAIN MENU ===============\n";
        cout << "User role: " << role << "\n";
        cout << "1. Product management\n";
        cout << "2. Sell a product\n";
        cout << "3. Reports and statistics\n";
        cout << "4. Programmer information\n";
        cout << "0. Exit\n";
        printFooter();

        int choice = readInt("Your choice: ", 0, 4);
        switch (choice) {
            case 1: productMenu(inventory, role); break;
            case 2: sellProductScreen(inventory); break;
            case 3: reportsMenu(inventory, lowStockThreshold); break;
            case 4: showProgrammerInfo(); break;
            case 0:
                if (confirm("Do you really want to exit?")) {
                    cout << "Goodbye! Data is saved in " << PRODUCTS_FILE
                         << " and " << SALES_FILE << ".\n";
                    running = false;
                }
                break;
        }
    }
    return 0;
}
