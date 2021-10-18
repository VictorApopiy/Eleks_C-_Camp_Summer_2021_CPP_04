#include <string>
struct SRecipe {
		int m_nId;
		std::string m_sName;
		std::string m_sCategory;
		std::string m_sCuisine;
		std::string m_sAmountOfIngredients;
		std::string m_sUnitOfMeasurement;
		std::string m_sIngredients;
		std::string m_sCookingAlgorithm;
		int m_nAmountOfPortions;
		double m_dWeigh;
		double m_dCalories;
		bool m_bIsAccessible;
		int m_nIdOfOwner;
		SRecipe();
};
enum ECategories{
    SNACKS_AND_SANDWICHES=1,
    SALADS=2,
    FIRST_COURSES=3,
    SIDE_DISHES=4,
    SECOND_COURSES=5,
    DESSERTS=6,
    SAUCES=7,
    DRINKS=8
};
