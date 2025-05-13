#include <TP/Input/KeyBinds.hpp>

std::vector<int> Keybinds::getKeysToMonitorForCharacter() {
    std::vector<int> keys;
    keys.insert(keys.end(), forward.begin(), forward.end());
    keys.insert(keys.end(), backward.begin(), backward.end());
    keys.insert(keys.end(), left.begin(), left.end());
    keys.insert(keys.end(), right.begin(), right.end());
    keys.insert(keys.end(), jump.begin(), jump.end());
    keys.insert(keys.end(), sneak.begin(), sneak.end());
    keys.insert(keys.end(), sprint.begin(), sprint.end());
    keys.insert(keys.end(), breakBlock.begin(), breakBlock.end());
    keys.insert(keys.end(), placeBlock.begin(), placeBlock.end());
    keys.insert(keys.end(), openInventory.begin(), openInventory.end());
    keys.insert(keys.end(), fireTNT.begin(), fireTNT.end());
    keys.insert(keys.end(), fireEnderPearl.begin(), fireEnderPearl.end());
    keys.insert(keys.end(), selectBlock.begin(), selectBlock.end());
    keys.insert(keys.end(), toggleHUD.begin(), toggleHUD.end());
    keys.insert(keys.end(), takeScreenshot.begin(), takeScreenshot.end());
    keys.insert(keys.end(), keyCameraLeft.begin(), keyCameraLeft.end());
    keys.insert(keys.end(), keyCameraRight.begin(), keyCameraRight.end());
    keys.insert(keys.end(), keyCameraUp.begin(), keyCameraUp.end());
    keys.insert(keys.end(), keyCameraDown.begin(), keyCameraDown.end());
    keys.insert(keys.end(), toggleDebug);
    keys.insert(keys.end(), toggleBoudingBoxes.begin(), toggleBoudingBoxes.end());
    keys.insert(keys.end(), toggleWireframe.begin(), toggleWireframe.end());
    keys.insert(keys.end(), toggleChunkBorders.begin(), toggleChunkBorders.end());
    keys.insert(keys.end(), toggleSpectator.begin(), toggleSpectator.end());
    keys.insert(keys.end(), toggleCreative.begin(), toggleCreative.end());
    keys.insert(keys.end(), reloadChunkMeshes.begin(), reloadChunkMeshes.end());
    keys.insert(keys.end(), spawnEntities.begin(), spawnEntities.end());
    keys.insert(keys.end(), togglePerspectiveAlternative.begin(), togglePerspectiveAlternative.end());
    keys.insert(keys.end(), togglePerspective.begin(), togglePerspective.end());
    keys.insert(keys.end(), toggleFullscreen.begin(), toggleFullscreen.end());
    keys.insert(keys.end(), escape);
    // Remove when the menu on ESCAPE is working, or the inventory on E is working :D 
    keys.insert(keys.end(), toggleFocus.begin(), toggleFocus.end());

    // remove duplicates 
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

    return keys;
}

std::vector<int> Keybinds::getKeysToMonitorForMenu() {
    std::vector<int> keys;
    keys.insert(keys.end(), takeScreenshot.begin(), takeScreenshot.end());
    keys.insert(keys.end(), toggleDebug);
    keys.insert(keys.end(), toggleFullscreen.begin(), toggleFullscreen.end());
    keys.insert(keys.end(), escape);

    // remove duplicates 
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

    return keys;
}