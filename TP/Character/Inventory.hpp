// Inventory.hpp
#pragma once

#include "ItemStack.hpp"
#include <vector>
#include <string>

class Inventory {
public:
    Inventory() {
        items.reserve(inventorySize);
        for (int i = 0; i < inventorySize; ++i) {
            items.emplace_back(AIR, 0); // Initialize empty item stacks
        }
    }

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

    

    void addItem(const ItemStack& itemStack) {
        for (auto& stack : items) {
            if (stack.getItemId() == itemStack.getItemId()) {
                stack.setQuantity(stack.getQuantity() + itemStack.getQuantity());
                return;
            }
        }
        for (auto& stack : items) {
            if (stack.getItemId() == AIR) {
                stack = itemStack;
                return;
            }
        }
        std::cout << "Inventory is full, cannot add item: " << itemStack.getItemName() << std::endl;
    }
    void removeItem(int itemId, int quantity) {
        for (auto& stack : items) {
            if (stack.getItemId() == itemId) {
                if (stack.getQuantity() > quantity) {
                    stack.setQuantity(stack.getQuantity() - quantity);
                } else {
                    stack.setQuantity(0);
                    stack.setItemId(AIR);
                }
                return;
            }
        }
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

    ItemStack* getSelectedItem() {
        return (selectedItem >= 0 && selectedItem < items.size()) ? &items[selectedItem] : nullptr;
    }

    inline int getSelectedIndex() const {
        return selectedItem;
    }

private:
    std::vector<ItemStack> items;
    int selectedItem = 0;
    int inventorySize = 9; // Number of slots in the inventory
};