#ifndef PAGINATED_MENU
#define PAGINATED_MENU

#include "menu.h"

enum class MenuPage {
	PAGE1,
	PAGE2,
	PAGE3,
	PAGE4,
	PAGE5
};

class PaginatedMenu : public Menu {
public:
	MenuPage currentPage;
protected:
	MenuPage previousPage;
	int maxPages;

	virtual void drawPage1() = 0;
	virtual void drawPage2() = 0;
	virtual void drawPage3() = 0;
	virtual void drawPage4() = 0;
	virtual void drawPage5() = 0;
};

#endif // !PAGINATED_MENU
