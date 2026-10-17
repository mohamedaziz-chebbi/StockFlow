// Programmer: Mohamed Aziz Chebbi
#include "Inventory.h"
#include "Utils.h"
#include <fstream>
#include <iomanip>
using namespace std;

Inventory::Inventory() {
    count = 0;
}

int Inventory::getCount() const { return count; }
bool Inventory::isEmpty() const { return count == 0; }
bool Inventory::isFull() const { return count >= MAX_PRODUCTS; }

Product& Inventory::getProduct(int index) { return products[index]; }
const Product& Inventory::getProduct(int index) const { return products[index]; }

int Inventory::findIndexById(int id) const {
    for (int i = 0; i < count; i++)
        if (products[i].getId() == id)
            return i;
    return -1;
}

int Inventory::findByName(const string& text, int results[]) const {
    string wanted = toLower(text);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (toLower(products[i].getName()).find(wanted) != string::npos) {
            results[found] = i;
            found++;
        }
    }
    return found;
}

bool Inventory::addProduct(const Product& product) {
    if (isFull())
        return false;
    if (findIndexById(product.getId()) != -1)
        return false;
    products[count] = product;
    count++;
    return true;
}

bool Inventory::removeById(int id) {
    int index = findIndexById(id);
    if (index == -1)
        return false;
    for (int i = index; i < count - 1; i++)     
        products[i] = products[i + 1];
    count--;
    return true;
}

double Inventory::totalValue() const {
    double total = 0.0;
    for (int i = 0; i < count; i++)
        total += products[i].getStock() * products[i].getPrice();
    return total;
}

void Inventory::backupFile(const string& filename) const {
    ifstream source(filename.c_str());
    ofstream backup((filename + ".bak").c_str());
    if (!source.is_open() || !backup.is_open())
        return;
    char c;
    while (source.get(c))
        backup.put(c);
}


bool Inventory::load(const string& filename, int& invalidLines, int& migratedLines) {
    invalidLines = 0;
    migratedLines = 0;
    count = 0;

    ifstream file(filename.c_str());
    if (!file.is_open())
        return false;

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty())
            continue;

        string f[5];
        int id = 0, stock = 0;
        double price = 0.0;
        string name, category;
        bool readable = false;

        if (line.find('|') != string::npos) {            
            if (splitLine(line, '|', f, 5) == 5
                && isInteger(f[0]) && isInteger(f[3]) && isDecimal(f[4])
                && !f[1].empty() && !f[2].empty()) {
                id = toInt(f[0]); name = f[1]; category = f[2];
                stock = toInt(f[3]); price = toDouble(f[4]);
                readable = true;
            }
        } else {                                             
            if (splitLine(line, ' ', f, 4) == 4
                && isInteger(f[0]) && isInteger(f[2]) && isDecimal(f[3])
                && !f[1].empty()) {
                id = toInt(f[0]); name = f[1]; category = "General";
                stock = toInt(f[2]); price = toDouble(f[3]);
                readable = true;
                migratedLines++;
            }
        }

        if (readable && (id <= 0 || stock < 0 || price < 0))
            readable = false;
        if (readable && !addProduct(Product(id, name, category, stock, price))) {
            readable = false;
            if (line.find('|') == string::npos)
                migratedLines--;
        }
        if (!readable)
            invalidLines++;
    }
    file.close();

    if (invalidLines > 0 || migratedLines > 0)
        backupFile(filename);         
    return true;
}

bool Inventory::save(const string& filename) const {
    ofstream file(filename.c_str());
    if (!file.is_open())
        return false;
    file << fixed << setprecision(2);
    for (int i = 0; i < count; i++) {
        file << products[i].getId() << "|"
             << products[i].getName() << "|"
             << products[i].getCategory() << "|"
             << products[i].getStock() << "|"
             << products[i].getPrice() << endl;
    }
    bool ok = file.good();
    file.close();
    return ok;
}
