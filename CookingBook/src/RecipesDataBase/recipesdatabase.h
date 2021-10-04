#pragma once
#include<iostream>
#include<list>
#include"sqlite3.h"
#include<string>
#include<vector>
#define READ_UP_TO_ZERO_TERMINATOR -1


namespace RecipesDataBase {

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

	class CRecipesDataBase
	{
	private:
		static const char* m_chPath;// path to the DB-file
		static int m_nAmountOfRecipes;

		CRecipesDataBase();
		CRecipesDataBase(const char* chPath);
		~CRecipesDataBase();
		//
		//this function takes a string of ingredients in recipes and a vector of 
		//ingredients, which we need to find in recipes. If the ingredients from 
		//the vector is found in the recipe, function returns true
		static bool CheckIfRecipeHasAnIngredient(
			std::string sIngredientsInRecipe, 
			std::vector<std::string> sIngredientsToFind);

		//
		// this function create table for recipes info in data base
		//Returns true, if table created successfully
		static bool CreateTable();
	public:
		//
		//this function takes path to the directory and creates DB there
		//Returns true, if initialization is successfull
		static bool InitRecipesDataBase(const char* chPath);

		static void SetAmountOfRecipes();
		static int GetAmountOfRecipes();

		
		static bool InsertData(const SRecipe& SRecipe);
		//
		//SelectRecipeByName takes a name of a recipe and returns
		//all information about that recipe
		static SRecipe SelectRecipeByName(std::string sRecipeName);
		//
		//SelectRecipes works as a filter and takes names of columns, which must be checked,
		//values of that columns, a vector of ingredients, what we want to find in recipes
		//and a size of created list, which is used to return a value of size of created 
		//list of recipes names.This function returns a list of names of found recipes
		static std::vector<std::string> SelectRecipes(
			std::vector<std::string> namesOfColumns,
			std::vector<std::string> values,
			std::vector<std::string> sIngredientsToFind,
			int& SizeOfCreatedList);
		//
		//this function takes an ID of recipe`s owner, size of created list, 
		//which is used to return a value of size of created list of recipes names.
		//This function returns a list of names of found recipes
		static std::vector<std::string> SelectRecipesByOwnersID(
			int nOwnersID, 
			int& SizeOfCreatedList);
		//this function takes a word, which must be found at the recipes names,
		//size of created list, which is used to return a value of size of created
		//list of recipes names.This function returns a list of names of found recipes
		static std::vector<std::string> SearchByName(
			std::string sWordToFind, 
			int& SizeOfCreatedList);

		static bool UpdateRecipe(const SRecipe& SRecipe);
		static bool DeleteRecipe(int nIdOfRecipe);
	
	};
}
