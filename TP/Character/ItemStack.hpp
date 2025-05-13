// ItemStack.hpp
#pragma once

#include <string>
#include <TP/Database/BlocTypes.hpp>

class ItemStack {
public:
    ItemStack(int itemId, int quantity) 
        : quantity(quantity) {
            itemData = BlocDatabase::getInstance().getBloc(itemId);
        }

    inline const std::string& getItemName() const { return itemData->name; }
    inline int getItemId() const { return itemData->id; }
    inline int getQuantity() const { return quantity; }
    inline void setQuantity(int newQuantity) { quantity = newQuantity; }

    inline bool operator==(const ItemStack& other) const {
        if (itemData == nullptr || other.itemData == nullptr) {
            return false;
        }
        return itemData->id == other.itemData->id;
    }

private:
    BlockData* itemData;
    int quantity;
};