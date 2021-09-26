#include "cUsersDataBase.h"

UsersDataBase::CUsersDataBase* UsersDataBase::CUsersDataBase::m_pUsersDataBase=nullptr;
int UsersDataBase::CUsersDataBase::m_nAmountOfUsers = 0;
int UsersDataBase::CUsersDataBase::m_nAmountOfObjects = 0;
UsersDataBase::CUsersDataBaseDestroyer UsersDataBase::CUsersDataBase::m_Destroyer;
const char* UsersDataBase::CUsersDataBase::chPath = "C:/Users/User/source/repos/Cooking book/Cooking book/Users.db";


//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase::CUsersDataBase()
{
	chPath = nullptr;
}
//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase::CUsersDataBase(const char* chPath)
{
	chPath = const_cast<char*>(chPath);
}
//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase::~CUsersDataBase()
{
}
//-----------------------------------------------------------------------------
void UsersDataBase::CUsersDataBase::SetAmountOfUsers()
{
	sqlite3* sqliteDataBase;
	int nExit = sqlite3_open(chPath, &sqliteDataBase);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error opening table from SetAmountOfUsers" << std::endl;
	}
	else {
		std::string sQuery = "SELECT COUNT(*) FROM USERS";
		sqlite3_exec(sqliteDataBase, sQuery.c_str(), callbackForGettingAmount, NULL, NULL);
	}
}
//-----------------------------------------------------------------------------
int UsersDataBase::CUsersDataBase::GetAmountOfUsers()
{
	return m_nAmountOfUsers;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::CreateDB()
{
	sqlite3* sqliteDataBase;
	int nExit = sqlite3_open(chPath, &sqliteDataBase);

	if (nExit == SQLITE_OK) {
		std::cout << "sqliteDataBase created successfully";
	}
	else {
		std::cout << "Error creation sqliteDataBase";
	}
	sqlite3_close(sqliteDataBase);
	return 0;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::CreateTable()
{
	sqlite3* sqliteDataBase;

	std::string sQuery = "CREATE TABLE IF NOT EXISTS USERS("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"USERNAME TEXT NOT NULL,"
		"PASSWORD TEXT NOT NULL,"
		"FAVORITES TEXT);";

	try {
		int nExit = sqlite3_open(chPath, &sqliteDataBase);
		if (nExit != SQLITE_OK) {
			std::cerr << "Error opening table from CreateTable" << std::endl;
		}
		char* chError;
		nExit = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);

		if (nExit != SQLITE_OK) {
			std::cerr << "Error create table" << std::endl;
			sqlite3_free(chError);
		}
		else std::cout << "Table created successfully" << std::endl;
		sqlite3_close(sqliteDataBase);

	}
	catch (const std::exception& e) {
		std::cerr << e.what();
	}
	return 0;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::InsertData(const SUser& SUser)
{
	sqlite3* sqliteDataBase;
	char* chErr;

	int nExit = sqlite3_open(chPath, &sqliteDataBase);
	std::string sQuery = "INSERT INTO USERS(USERNAME, PASSWORD,FAVORITES ) VALUES('"
		+ SUser.m_sUserName + "','" + SUser.m_sPassword + "','" + SUser.m_sFavorites + "');";

	nExit = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chErr);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error insert" << std::endl;
		sqlite3_free(chErr);
	}
	else std::cout << "Records created successfully" << std::endl;
	sqlite3_close(sqliteDataBase);
	return 0;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::SelectAllData()
{
	sqlite3* sqliteDataBase;
	int nExit = sqlite3_open(chPath, &sqliteDataBase);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error opening table from SelectAllData" << std::endl;
	}

	std::string sQuery = "SELECT * FROM USERS;";
	sqlite3_exec(sqliteDataBase, sQuery.c_str(), callback, NULL, NULL);
	return 0;
}
//-----------------------------------------------------------------------------
int UsersDataBase::CUsersDataBase::callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	int i = 0;
	SUser user;
	user.m_nID = atoi(argv[0]);
	user.m_sUserName = argv[1];
	user.m_sPassword = argv[2];
	user.m_sFavorites = argv[3];
	//TODO: insert function for sending info to Client


	//for (i = 0;i < argc;i++) {
	//	std::cout << azColName[i] << ": " << argv[i] << std::endl;

	//	
	//}
	//std::cout << std::endl;
	return 0;
}
//-----------------------------------------------------------------------------
int UsersDataBase::CUsersDataBase::callbackForGettingAmount(void* NotUsed, int argc, char** argv, char** azColName) {
	m_nAmountOfUsers = atoi(argv[0]);
	return 0;
}
//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase& UsersDataBase::CUsersDataBase::GetUsersDataBase()
{
	if (m_nAmountOfObjects == 0) {
		m_pUsersDataBase = new CUsersDataBase("C:/Users/User/source/repos/Cooking book/Cooking book/Users.db");
		m_Destroyer.InitDestroyer(m_pUsersDataBase);
		m_nAmountOfObjects++;
	}
	return *m_pUsersDataBase;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::SelectUsersInfo(const SUser& SUser)
{
	sqlite3* sqliteDataBase;
	int nExit = sqlite3_open(chPath, &sqliteDataBase);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error opening table from SelectUsersInfo" << std::endl;
	}
	else {
		std::string sQuery = "SELECT * FROM USERS WHERE USERNAME='" + SUser.m_sUserName + "'";
		sqlite3_exec(sqliteDataBase, sQuery.c_str(), callback, NULL, NULL);
	}
	return 0; 
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::AddToFavorites(const SUser& SUser, const int& nIdOfRecipe)
{
	sqlite3* sqliteDataBase;
	char* chError;

	int nExit = sqlite3_open(chPath, &sqliteDataBase);
	std::string sQuery = "UPDATE USERS SET FAVORITES='"+SUser.m_sFavorites +
		std::to_string(nIdOfRecipe) +"/' WHERE ID="+ std::to_string(SUser.m_nID);

	nExit = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error insert" << std::endl;
		sqlite3_free(chError);
	}
	else std::cout << "Records created successfully" << std::endl;
	sqlite3_close(sqliteDataBase);
	return 0;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::DeleteFromFavorites(SUser& SUser, const int& nIdOfRecipe)
{
	std::string sIdOfRecipe = std::to_string(nIdOfRecipe)+"/";
	int nPosOfID = 2;
	nPosOfID = SUser.m_sFavorites.find(sIdOfRecipe);

	if (nPosOfID >= 0) {
		SUser.m_sFavorites.erase(nPosOfID, sIdOfRecipe.length());

		sqlite3* sqliteDataBase;
		char* chError;

		int nExit = sqlite3_open(chPath, &sqliteDataBase);
		if (nExit != SQLITE_OK) {
			std::cerr << "Error opening table from DeleteFromFavorites" << std::endl;
		}
		else {
			std::string sQuery = "UPDATE USERS SET FAVORITES='" + SUser.m_sFavorites + "' WHERE ID=" + std::to_string(SUser.m_nID);

			nExit = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);
			if (nExit != SQLITE_OK) {
				std::cerr << "Error insert" << std::endl;
				sqlite3_free(chError);
			}
			else std::cout << "Records created successfully" << std::endl;
			sqlite3_close(sqliteDataBase);
		}
		
	}
	return 0;
}
bool UsersDataBase::CUsersDataBase::DeleteUser(const int& nUsersID)
{
	sqlite3* sqliteDataBase;
	int nExit = sqlite3_open(chPath, &sqliteDataBase);
	if (nExit != SQLITE_OK) {
		std::cerr << "Error opening table from DeleteUser" << std::endl;
	}
	else {
		std::string sQuery = "DELETE FROM USERS WHERE ID=" + std::to_string(nUsersID);
		sqlite3_exec(sqliteDataBase, sQuery.c_str(), callback, NULL, NULL);
	}
	return 0;
}
//-----------------------------------------------------------------------------

UsersDataBase::CUsersDataBaseDestroyer::~CUsersDataBaseDestroyer()
{
	delete m_pUsersDB;
}
//-----------------------------------------------------------------------------
void UsersDataBase::CUsersDataBaseDestroyer::InitDestroyer(CUsersDataBase* m_pUsersDB)
{
	this->m_pUsersDB = m_pUsersDB;
}
