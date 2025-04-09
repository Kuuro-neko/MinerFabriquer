// Inventory.hpp
#pragma once

#include "ItemStack.hpp"
#include <vector>
#include <string>

class Inventory {
public:
    void addItem(const ItemStack& itemStack) {
        for (auto& stack : items) {
            if (stack.getItemId() == itemStack.getItemId()) {
                stack.setQuantity(stack.getQuantity() + itemStack.getQuantity());
                return;
            }
        }
        items.push_back(itemStack);
        if (items.size() == 1) {
            selectedItem = 0;
        }
    }

    void removeItem(int itemId, int quantity) {
        for (auto& stack : items) {
            if (stack.getItemId() == itemId) {
                if (stack.getQuantity() > quantity) {
                    stack.setQuantity(stack.getQuantity() - quantity);
                } else {
                    items.erase(std::remove(items.begin(), items.end(), stack), items.end());
                }
                return;
            }
        }
    }

    ItemStack* getSelectedItem() {
        return (selectedItem >= 0 && selectedItem < items.size()) ? &items[selectedItem] : nullptr;
    }

    void scrollSelectedItem(int direction) {
        if (items.empty()) return;

        selectedItem -= direction;
        if (selectedItem < 0) {
            selectedItem = items.size() - 1;
        } else if (selectedItem >= items.size()) {
            selectedItem = 0;
        }
    }

    void tryToSelectItem(int blocId) {
        for (int i = 0; i < items.size(); ++i) {
            if (items[i].getItemId() == blocId) {
                selectedItem = i;
                return;
            }
        }
    }

    const std::vector<ItemStack>& getItems() const { return items; }


    const void printInventory() const {
        std::cout << "Inventory:" << std::endl;
        for (int i = 0; i < items.size(); ++i) {
            if (i == selectedItem) {
                std::cout << "-> ";
            } else {
                std::cout << "   ";
            }
            std::cout << "Item " << i << ": " << items[i].getItemName() << " (ID: " << items[i].getItemId() << ", Quantity: " << items[i].getQuantity() << ")" << std::endl;
        }
    }

private:
    std::vector<ItemStack> items;
    int selectedItem = -1;
};