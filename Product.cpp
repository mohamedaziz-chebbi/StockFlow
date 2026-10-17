
#include "Product.h"
using namespace std;

Product::Product() {
    id = 0;
    name = "";
    category = "";
    stock = 0;
    price = 0.0;
}

Product::Product(int id, string name, string category, int stock, double price) {
    this->id = id;
    this->name = name;
    this->category = category;
    this->stock = stock;
    this->price = price;
}

int Product::getId() const { return id; }
string Product::getName() const { return name; }
string Product::getCategory() const { return category; }
int Product::getStock() const { return stock; }
double Product::getPrice() const { return price; }

void Product::setName(string newName) { name = newName; }
void Product::setCategory(string newCategory) { category = newCategory; }

bool Product::setStock(int newStock) {
    if (newStock < 0)
        return false;
    stock = newStock;
    return true;
}

bool Product::setPrice(double newPrice) {
    if (newPrice < 0)
        return false;
    price = newPrice;
    return true;
}
