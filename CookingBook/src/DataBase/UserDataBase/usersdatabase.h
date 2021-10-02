#ifndef USER_DATA_BASE
#define USER_DATA_BASE
#define READ_UP_TO_ZERO_TERMINATOR -1
#include<iostream>
#include"sqlite3.h"
#include<string>

namespace UsersDataBase {

	class CUsersDataBaseDestroyer;

	struct SUser {//structure which describes a User
		int m_nID;
		std::string m_sUserName;
		std::string m_sPassword;
		std::string m_sFavorites;//list of favourite recipes 
		SUser();
	};


	//CUsersDataBase holds an information about database of users
	//and contains methods to initialize and work with database
	class CUsersDataBase 
	{
	private:
		
		static CUsersDataBase* m_pUsersDataBase;
		static int m_nAmountOfUsers;
		static int m_nAmountOfObjects; //amount of CUsersDataBase objects
		static const char* m_chPath;// path to the DB-file
		static CUsersDataBaseDestroyer m_Destroyer; // an object which destroys an CUsersDataBase object
		

		CUsersDataBase();
		CUsersDataBase(const char* chPath);
		~CUsersDataBase();
		friend class CUsersDataBaseDestroyer;
	public:
		static CUsersDataBase* GetUsersDataBase(const char* chPath);//this function returns a pointer to the CUsersDataBase object

		static void SetAmountOfUsers();
		static int GetAmountOfUsers();

		static bool CreateDB();
		static bool CreateTable();

		static bool InsertData(const SUser& SUser);
		static bool AddToFavorites(const SUser& SUser, const int& nIdOfRecipe);

		static SUser SelectUsersInfo(const std::string& Username);

		static bool CheckUsersExistence(const SUser& SUser);
		static bool CheckUsersPassword(const SUser& SUser);

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
		void InitDestroyer(CUsersDataBase* pUsersDB);
	};
}
#endif