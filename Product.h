
#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product {
private:
    int id;
    std::string name;
    std::string category;
    int stock;
    double price;

public:
    Product();                                  
    Product(int id, std::string name, std::string category, int stock, double price);

    int getId() const;
    std::string getName() const;
    std::string getCategory() const;
    int getStock() const;
    double getPrice() const;

    void setName(std::string newName);
    void setCategory(std::string newCategory);
    bool setStock(int newStock);
    bool setPrice(double newPrice);
};

#endif
