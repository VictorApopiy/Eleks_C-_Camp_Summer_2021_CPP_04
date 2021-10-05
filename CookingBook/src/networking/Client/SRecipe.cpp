#include "SRecipe.h"
SRecipe::SRecipe() :
	m_nId(0),
	m_sName(""),
	m_sCategory(""),
	m_sCuisine(""),
	m_sAmountOfIngredients(""),
	m_sIngredients(""),
	m_sCookingAlgorithm(""),
	m_nAmountOfPortions(0),
	m_dWeigh(0.0),
	m_dCalories(0.0),
	m_bIsAccessible(true),
	m_nIdOfOwner(0)
{}