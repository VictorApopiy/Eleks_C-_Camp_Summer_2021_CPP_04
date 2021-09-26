#ifndef USER_DATA_BASE
#define USER_DATA_BASE
#include<iostream>
#include"sqlite3.h"
#include<string>


namespace UsersDataBase {

	class CUsersDataBaseDestroyer;

	struct SUser {//strucrure which describes a User
		int m_nID;
		std::string m_sUserName;
		std::string m_sPassword;
		std::string m_sFavorites;//list of favourite recipes 
	};


	//CUsersDataBase holds an information about database of users
	//and contains methods to initialize and work with database
	class CUsersDataBase 
	{
	private:
		
		static CUsersDataBase* m_pUsersDataBase;
		static int m_nAmountOfUsers;
		static int m_nAmountOfObjects; //amount of CUsersDataBase objects
		static const char* chPath;// path to the DB-file
		static CUsersDataBaseDestroyer m_Destroyer; // an object which destroys an CUsersDataBase object

		CUsersDataBase();
		CUsersDataBase(const char* chPath);
		~CUsersDataBase();

		static int callback(void* NotUsed, int argc, char** argv, char** azColName);
		static int callbackForGettingAmount(void* NotUsed, int argc, char** argv, char** azColName);

		friend class CUsersDataBaseDestroyer;
	public:
		
		static CUsersDataBase& GetUsersDataBase();//this function returns a referens to the CUsersDataBase object

		static void SetAmountOfUsers();
		static int GetAmountOfUsers();

		static bool CreateDB();
		static bool CreateTable();

		static bool InsertData(const SUser& SUser);

		static bool SelectAllData();
		static bool SelectUsersInfo(const SUser& SUser);

		static bool AddToFavorites(const SUser& SUser, const int& nIdOfRecipe);
		static bool DeleteFromFavorites(SUser& SUser, const int& nIdOfRecipe);
		static bool DeleteUser(const int& nUsersID);
	};


	//CUsersDataBaseDestroyer is used for correct deleting CUsersDataBase
	//object
	class CUsersDataBaseDestroyer {
	private:
		CUsersDataBase* m_pUsersDB;
	public:
		~CUsersDataBaseDestroyer();
		void InitDestroyer(CUsersDataBase* m_pUsersDB);
	};
}
#endif