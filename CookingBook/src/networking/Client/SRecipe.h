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
