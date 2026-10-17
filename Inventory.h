
#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include "Product.h"

const int MAX_PRODUCTS = 100;  
class Inventory {
private:
    Product products[MAX_PRODUCTS];
    int count;

    void backupFile(const std::string& filename) const;

public:
    Inventory();

    int getCount() const;
    bool isEmpty() const;
    bool isFull() const;

    Product& getProduct(int index);
    const Product& getProduct(int index) const;

    int findIndexById(int id) const;                             
    int findByName(const std::string& text, int results[]) const; 
    bool addProduct(const Product& product);  
    bool removeById(int id);                  
    double totalValue() const;               
   
    bool load(const std::string& filename, int& invalidLines, int& migratedLines);
    bool save(const std::string& filename) const;
};

#endif
