#include "recipesdatabase.h"
int RecipesDataBase::CRecipesDataBase::m_nAmountOfRecipes = 0;
const char* RecipesDataBase::CRecipesDataBase::m_chPath = "";

//-----------------------------------------------------------------------------
RecipesDataBase::SRecipe::SRecipe() :
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
//-----------------------------------------------------------------------------
RecipesDataBase::CRecipesDataBase::CRecipesDataBase() 
{
	m_chPath = nullptr;
}
//-----------------------------------------------------------------------------
RecipesDataBase::CRecipesDataBase::CRecipesDataBase(const char* chPath) 
{
	m_chPath = chPath;
}
//-----------------------------------------------------------------------------
RecipesDataBase::CRecipesDataBase::~CRecipesDataBase()
{
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::InitRecipesDataBase(const char* chPath)
{
	m_chPath = chPath;
	bool bResult = false;
	bResult = CreateTable();
	return bResult;
}
//-----------------------------------------------------------------------------
void RecipesDataBase::CRecipesDataBase::SetAmountOfRecipes()
{
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SetAmountOfRecipes" << std::endl;
	}
	else {
		std::string sQuery = "SELECT COUNT(*) FROM RECIPES";
		nStatus = sqlite3_prepare_v2(
			sqliteDataBase, 
			sQuery.c_str(), 
			READ_UP_TO_ZERO_TERMINATOR, 
			&InfoFromDB, 
			NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "error " << sqlite3_errmsg(sqliteDataBase);
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				m_nAmountOfRecipes = sqlite3_column_int(InfoFromDB, 0);
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
}
//-----------------------------------------------------------------------------
int RecipesDataBase::CRecipesDataBase::GetAmountOfRecipes()
{
	return m_nAmountOfRecipes;
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::CreateTable()
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "CREATE TABLE IF NOT EXISTS RECIPES("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"NAME TEXT NOT NULL,"
		"CATEGORY TEXT NOT NULL,"
		"CUISINE TEXT NOT NULL,"
		"AMOUNT_OF_INGREDIENTS TEXT NOT NULL,"
		"UNIT_OF_MEASUREMENT TEXT NOT NULL,"
		"INGREDIENTS TEXT NOT NULL,"
		"COOKING_ALGORITHM TEXT NOT NULL,"
		"PORTIONS INTEGER,"
		"WEIGH DOUBLE,"
		"CALORIES DOUBLE,"
		"IS_ACCESSIBLE BOOLEAN,"
		"ID_OF_OWNER INTEGER);";
	bool bResult = false;
	char* chError = nullptr;

	do {
		int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
		if (nStatus != SQLITE_OK) {
			std::cerr << "Error opening table from CreateTable" << std::endl;
			break;
		}

		nStatus = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);

		if (nStatus != SQLITE_OK) {
			std::cerr << "Error create table" << std::endl;
			sqlite3_free(chError);
			break;
		}
		std::cout << "Table created successfully" << std::endl;
		sqlite3_close(sqliteDataBase);
		bResult = true;
	} while (false);
	return bResult;
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::InsertData(const SRecipe& recipe)
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "INSERT INTO RECIPES(NAME, CATEGORY, CUISINE, AMOUNT_OF_INGREDIENTS,"
		"UNIT_OF_MEASUREMENT, INGREDIENTS, COOKING_ALGORITHM, PORTIONS, WEIGH,"
		"CALORIES, IS_ACCESSIBLE, ID_OF_OWNER) VALUES('" +  recipe.m_sName+"','"+recipe.m_sCategory +
		"','" + recipe.m_sCuisine  + "','" +recipe.m_sAmountOfIngredients +
		"','"+ recipe.m_sUnitOfMeasurement + "','" + recipe.m_sIngredients+"','"+
		recipe.m_sCookingAlgorithm+ "',"+ std::to_string(recipe.m_nAmountOfPortions)+
		","+ std::to_string(recipe.m_dWeigh)+","+ std::to_string(recipe.m_dCalories)+
		"," + std::to_string(recipe.m_bIsAccessible)+"," + std::to_string(recipe.m_nIdOfOwner)+");";
	bool bResult = false;
	char* chError = nullptr;

	do {
		int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
		if (nStatus != SQLITE_OK) {
			std::cerr << "Error opening table from InsertData" << std::endl;
			break;
		}

		nStatus = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);

		if (nStatus != SQLITE_OK) {
			std::cerr << "Error insert" << sqlite3_errmsg(sqliteDataBase);
			sqlite3_free(chError);
			break;
		}
		std::cout << "Records created successfully" << std::endl;
		sqlite3_close(sqliteDataBase);
		bResult = true;
	} while (false);
	return bResult;
}
//-----------------------------------------------------------------------------
RecipesDataBase::SRecipe RecipesDataBase::CRecipesDataBase::SelectRecipeByName(
	std::string sRecipeName)
{
	SRecipe recipe;
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SelectRecipeByName" << std::endl;
	}
	else {
		std::string sQuery = "SELECT * FROM RECIPES WHERE NAME='" + sRecipeName + "'";
		nStatus = sqlite3_prepare_v2(
			sqliteDataBase, 
			sQuery.c_str(), 
			READ_UP_TO_ZERO_TERMINATOR, 
			&InfoFromDB, 
			NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing" << std::endl;
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				recipe.m_nId = sqlite3_column_int(InfoFromDB, 0);
				recipe.m_sName = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 1)));
				recipe.m_sCategory = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 2)));
				recipe.m_sCuisine = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 3)));
				recipe.m_sAmountOfIngredients = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 4)));
				recipe.m_sUnitOfMeasurement = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 5)));
				recipe.m_sIngredients = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 6)));
				recipe.m_sCookingAlgorithm = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 7)));
				recipe.m_nAmountOfPortions = sqlite3_column_int(InfoFromDB, 8);
				recipe.m_dWeigh = sqlite3_column_double(InfoFromDB, 9);
				recipe.m_dCalories = sqlite3_column_double(InfoFromDB, 10);
				recipe.m_bIsAccessible = sqlite3_column_int(InfoFromDB, 11);
				recipe.m_nIdOfOwner = sqlite3_column_int(InfoFromDB, 12);
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	return recipe;
}
//-----------------------------------------------------------------------------
std::vector<std::string> RecipesDataBase::CRecipesDataBase::SelectRecipes(
	std::vector<std::string> namesOfColumns, 
	std::vector<std::string> values, 
	std::vector<std::string> sIngredientsToFind,
	int& rSizeOfCreatedList)
{
	std::vector<std::string> NamesOfRecipes;
	std::string sTempIngredients = "";
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SelectRecipes" << std::endl;
	}
	else {
		std::string sQuery = "SELECT NAME, INGREDIENTS FROM RECIPES WHERE IS_ACCESSIBLE = 1 ";
		if (namesOfColumns.size() != 0) {
			for (int i = 0;i < namesOfColumns.size();i++) {
				sQuery += " AND " + namesOfColumns[i] + " = '" + values[i] + "'";
			}
		}
		nStatus = sqlite3_prepare_v2(
			sqliteDataBase, 
			sQuery.c_str(), 
			READ_UP_TO_ZERO_TERMINATOR, 
			&InfoFromDB, 
			NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing" << std::endl;
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				if (sIngredientsToFind.size() != 0) {
					sTempIngredients = reinterpret_cast<char*>
						(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 1)));
					if (CheckIfRecipeHasAnIngredient(sTempIngredients, sIngredientsToFind)) {
						std::cout << "recipes has been found" << std::endl;
						NamesOfRecipes.push_back(reinterpret_cast<char*>
							(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 0))));
					}
				}
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	rSizeOfCreatedList = sizeof(NamesOfRecipes);
	return NamesOfRecipes;
}
//-----------------------------------------------------------------------------
std::vector<std::string> RecipesDataBase::CRecipesDataBase::SelectRecipesByOwnersID(
	int nOwnersID, 
	int& SizeOfCreatedList)
{
	std::vector<std::string> NamesOfRecipes;
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SelectRecipes" << std::endl;
	}
	else {
		std::string sQuery = "SELECT NAME FROM RECIPES WHERE ID_OF_OWNER = " + std::to_string(nOwnersID);
		nStatus = sqlite3_prepare_v2(sqliteDataBase, sQuery.c_str(), READ_UP_TO_ZERO_TERMINATOR, &InfoFromDB, NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing" << std::endl;
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				NamesOfRecipes.push_back(reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 0))));
				std::cout << "recipes has been found" << std::endl;
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	
	SizeOfCreatedList = sizeof(NamesOfRecipes);
	return NamesOfRecipes;
}
//-----------------------------------------------------------------------------
std::vector<std::string> RecipesDataBase::CRecipesDataBase::SearchByName(
	std::string sWordToFind, 
	int& SizeOfCreatedList)
{
	std::vector<std::string> NamesOfRecipes;
	std::string sTempName = "";
	int nPosOfSubstring = 0;
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SelectRecipes" << std::endl;
	}
	else {
		std::string sQuery = "SELECT NAME FROM RECIPES WHERE IS_ACCESSIBLE = 1; ";
		nStatus = sqlite3_prepare_v2(
			sqliteDataBase, 
			sQuery.c_str(), 
			READ_UP_TO_ZERO_TERMINATOR, 
			&InfoFromDB, 
			NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing" << std::endl;
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				sTempName = reinterpret_cast<char*>
					(const_cast<unsigned char*>(sqlite3_column_text(InfoFromDB, 0)));
				nPosOfSubstring = sTempName.find(sWordToFind);
				if (nPosOfSubstring>=0) {
					std::cout << "recipes has been found" << std::endl;
					NamesOfRecipes.push_back(sTempName);
				
				}
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	SizeOfCreatedList = sizeof(NamesOfRecipes);
	return NamesOfRecipes;
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::CheckIfRecipeHasAnIngredient(
	std::string sIngredientsInRecipe, 
	std::vector<std::string> sIngredientsToFind) 
{
	bool bIsInRecipe = true;
	int nPosOfSubstring = 0;
	for (int i = 0;i < sIngredientsToFind.size();i++) {
		nPosOfSubstring = sIngredientsInRecipe.find(sIngredientsToFind[i]);
		if (nPosOfSubstring < 0) {
			bIsInRecipe = false;
			break;
		}
	}
	return bIsInRecipe;
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::SelectAllData()
{
	sqlite3* sqliteDataBase;
	int nExit = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error opening table from SelectAllData" << std::endl;
	}

	std::string sQuery = "SELECT * FROM RECIPES;";
	sqlite3_exec(sqliteDataBase, sQuery.c_str(), callback, NULL, NULL);
	return 0;
}
//-----------------------------------------------------------------------------
int RecipesDataBase::CRecipesDataBase::callback(void* NotUsed, int argc, char** argv, char** azColName) {
	for (int i = 0;i < argc;i++) {
			std::cout << azColName[i] << ": " << argv[i] << std::endl;
	}
	std::cout << std::endl;
	return 0;
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::UpdateRecipe(const SRecipe& SRecipe)
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "UPDATE RECIPES SET NAME ='"+ SRecipe.m_sName+"', "
		"CATEGORY ='"+ SRecipe.m_sCategory+"', "
		"CUISINE ='" + SRecipe.m_sCuisine + "', "
		"AMOUNT_OF_INGREDIENTS ='"+ SRecipe.m_sAmountOfIngredients+"', "
		"UNIT_OF_MEASUREMENT ='" + SRecipe.m_sUnitOfMeasurement+"', "
		"INGREDIENTS ='" + SRecipe.m_sIngredients+"', "
		"COOKING_ALGORITHM ='"+ SRecipe.m_sCookingAlgorithm+"', "
		"PORTIONS ="+ std::to_string(SRecipe.m_nAmountOfPortions)+", "
		"WEIGH =" + std::to_string(SRecipe.m_dWeigh)+", "
		"CALORIES =" + std::to_string(SRecipe.m_dCalories)+", "
		"IS_ACCESSIBLE =" + std::to_string(SRecipe.m_bIsAccessible)+", "
		"ID_OF_OWNER = " + std::to_string(SRecipe.m_nIdOfOwner)+
		" WHERE ID = "+ std::to_string(SRecipe.m_nId);
	bool bResult = false;
	char* chError = nullptr;

	do {
		int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
		if (nStatus != SQLITE_OK) {
			std::cerr << "Error opening table from AddToFavorites" << std::endl;
			break;
		}

		nStatus = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);

		if (nStatus != SQLITE_OK) {
			std::cerr << "Error insert" << std::endl;
			sqlite3_free(chError);
			break;
		}
		std::cout << "Records created successfully" << std::endl;
		sqlite3_close(sqliteDataBase);
		bResult = true;
	} while (false);
	return bResult;
}
//-----------------------------------------------------------------------------
bool RecipesDataBase::CRecipesDataBase::DeleteRecipe(int nIdOfRecipe)
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "DELETE FROM RECIPES WHERE ID=" + std::to_string(nIdOfRecipe);
	char* chError = nullptr;
	bool bResult = false;

	do {
		int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
		if (nStatus != SQLITE_OK) {
			std::cerr << "Error opening table from DeleteUser" << std::endl;
			break;
		}

		nStatus = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);

		if (nStatus != SQLITE_OK) {
			std::cerr << "Error deleting" << std::endl;
			sqlite3_free(chError);
			break;
		}
		std::cout << "Deleted successfully" << std::endl;
		sqlite3_close(sqliteDataBase);
		bResult = true;
	} while (false);

	return bResult;
}
//-----------------------------------------------------------------------------

