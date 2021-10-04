#ifndef USER_DATA_BASE
#define USER_DATA_BASE
#define READ_UP_TO_ZERO_TERMINATOR -1
#include<iostream>
#include"sqlite3.h"
#include<string>

namespace UsersDataBase {

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
		static int m_nAmountOfUsers;
		static const char* m_chPath;// path to the DB-file
		
		CUsersDataBase();
		CUsersDataBase(const char* chPath);
		~CUsersDataBase();
		//
		// this function create table for user`s info in data base
		//Returns true, if table created successfully
		static bool CreateTable();
	public:
		//
		//this function takes a path to the directory and creates DB there
		//Returns true, if initialization is successfull
		static bool InitUsersDataBase(const char* chPath);

		static void SetAmountOfUsers();
		static int GetAmountOfUsers();

		static bool InsertData(const SUser& SUser);
		static bool AddToFavorites(const SUser& SUser, const int& nIdOfRecipe);

		static SUser SelectUsersInfo(const std::string& Username);
		//
		//this function takes a structure of User, checks if a username exists in 
		//data base and returns true, if the username exists
		static bool CheckUsersExistence(const SUser& SUser);
		//
		//this function takes a structure of User, checks if a username and 
		// a password are appropriate to each other.
		//Return true, if it is appropriate 
		static bool CheckUsersPassword(const SUser& SUser);

		static bool DeleteFromFavorites(SUser& SUser, const int& nIdOfRecipe);
		static bool DeleteUser(const int& nUsersID);
	};
}
#endif