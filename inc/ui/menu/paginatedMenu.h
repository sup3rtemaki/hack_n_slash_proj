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

};

#endif // !PAGINATED_MENU
