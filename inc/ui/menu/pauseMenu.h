#ifndef PAUSE_MENU
#define PAUSE_MENU

#include "paginatedMenu.h"

#include "paginatedMenu.h"

enum class InventoryMode {
    Normal,
    SelectingQuickSlot
};

class PauseMenu : public PaginatedMenu {
public:
    InventoryMode inventoryMode;

    PauseMenu(Hero* hero);
    ~PauseMenu();

    virtual void draw() override;
    virtual void onIndexUp() override;
    virtual void onIndexDown() override;
    virtual void onIndexLeft() override;
    virtual void onIndexRight() override;
    void showSubMenu();
    void hideSubMenu();
    void onSubMenuAction();
    void confirmQuickSlotSelection();
    void cancelQuickSlotSelection();

private:
    vector<SDL_Texture*> menuTextTextures;
    vector<string> cachedMenuTexts;

    SDL_Texture* itemNameTexture;
    SDL_Texture* itemDescTexture;
    string lastItemName;
    string lastItemDesc;

    vector<class Item*> inventory;
    Item* selectedItem;
    int infVisibleItemsLimit;
    int supVisibleItemsLimit;
    SDL_Texture* itemsBg;
    SDL_Texture* leftArrowTexture;
    SDL_Texture* rightArrowTexture;
    class SubMenu* subMenu;
    

    // Novos membros para controle de modo
    
    int quickAccessIndex;
    int itemToAddId;

    void drawPageInitialCheck();
    void drawInventoryItems();
    void drawSelectedItemNameAndDescription();
    void drawQuickInventory();
    void drawMenuForeground();
    void toggleQuickAccessItem();
    void enterQuickSlotSelectionMode();
    void drawQuickAccessSelectionBox();

    virtual void setUp() override;
    virtual void drawMenuBackground() override;
    virtual void drawText() override;
    virtual void drawSelectionBox() override;
    virtual void drawPage1() override;
    virtual void drawPage2() override;
    virtual void drawPage3() override;
    virtual void drawPage4() override;
    virtual void drawPage5() override;

    SDL_Point calculateRectSelectionBoxPosition();
    SDL_Point calculateQuickAccessRectSelectionBoxPosition();
};

#endif // !PAUSE_MENU
