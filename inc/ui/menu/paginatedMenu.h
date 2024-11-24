#ifndef PAGINATED_MENU
#define PAGINATED_MENU

#include "menu.h"

class PaginatedMenu : public Menu {
public:
	int maxPages;
	int currentPage;
};

#endif // !PAGINATED_MENU
