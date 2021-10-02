#include "usersdatabase.h"

UsersDataBase::CUsersDataBase* UsersDataBase::CUsersDataBase::m_pUsersDataBase=nullptr;
int UsersDataBase::CUsersDataBase::m_nAmountOfUsers = 0;
int UsersDataBase::CUsersDataBase::m_nAmountOfObjects = 0;
UsersDataBase::CUsersDataBaseDestroyer UsersDataBase::CUsersDataBase::m_Destroyer;
const char* UsersDataBase::CUsersDataBase::m_chPath = "";
//UsersDataBase::SUser UsersDataBase::CUsersDataBase::m_user;

//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase::CUsersDataBase()
{
	m_chPath = nullptr;
}
//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase::CUsersDataBase(const char* chPath)
{
	m_chPath = const_cast<char*>(chPath);
}
//-----------------------------------------------------------------------------
UsersDataBase::CUsersDataBase::~CUsersDataBase()
{
}
//-----------------------------------------------------------------------------
void UsersDataBase::CUsersDataBase::SetAmountOfUsers()
{
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SetAmountOfUsers" << std::endl;
	}
	else {
		std::string sQuery = "SELECT COUNT(*) FROM USERS";
		nStatus = sqlite3_prepare_v2(sqliteDataBase, sQuery.c_str(), READ_UP_TO_ZERO_TERMINATOR, &InfoFromDB, NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "error " << sqlite3_errmsg(sqliteDataBase);
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				m_nAmountOfUsers = sqlite3_column_int(InfoFromDB, 0);
			}
		}
		sqlite3_finalize(InfoFromDB);
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
	int nExit = sqlite3_open(m_chPath, &sqliteDataBase);

	if (nExit == SQLITE_OK) {
		std::cout << "sqliteDataBase created successfully";
	}
	else {
		std::cout << "Error creation sqliteDataBase";
	}
	sqlite3_close(sqliteDataBase);
	return true;
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
	bool bResult = false;
	char* chError=nullptr;

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
bool UsersDataBase::CUsersDataBase::InsertData(const SUser& SUser)
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "INSERT INTO USERS(USERNAME, PASSWORD,FAVORITES ) VALUES('"
		+ SUser.m_sUserName + "','" + SUser.m_sPassword + "','" + SUser.m_sFavorites + "');";
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

//-----------------------------------------------------------------------------

UsersDataBase::CUsersDataBase* UsersDataBase::CUsersDataBase::GetUsersDataBase(const char* chPath)
{
	if (m_nAmountOfObjects == 0) {
		m_pUsersDataBase = new CUsersDataBase(chPath);
		m_Destroyer.InitDestroyer(m_pUsersDataBase);
		m_nAmountOfObjects++;
	}
	return m_pUsersDataBase;
}
//-----------------------------------------------------------------------------

UsersDataBase::SUser UsersDataBase::CUsersDataBase::SelectUsersInfo(const std::string& Username)
{
	SUser user;
	sqlite3_stmt* InfoFromDB;
	sqlite3* sqliteDataBase;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from SelectUsersInfo" << std::endl;
	}
	else {
		std::string sQuery = "SELECT * FROM USERS WHERE USERNAME='" + Username + "'";
		nStatus = sqlite3_prepare_v2(sqliteDataBase, sQuery.c_str(), READ_UP_TO_ZERO_TERMINATOR, &InfoFromDB, NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing" << std::endl;
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				user.m_nID = sqlite3_column_int(InfoFromDB, 0);
				user.m_sUserName = (char*)sqlite3_column_text(InfoFromDB, 1);
				user.m_sPassword = (char*)sqlite3_column_text(InfoFromDB, 2);
				user.m_sFavorites = (char*)sqlite3_column_text(InfoFromDB, 3);
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	return user;
}
bool UsersDataBase::CUsersDataBase::CheckUsersExistence(const SUser& SUser)
{
	sqlite3* sqliteDataBase;
	sqlite3_stmt* InfoFromDB;
	bool bUserExistence = false;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from CheckUsersExistence" << std::endl;
	}
	else {
		std::string sQuery = "SELECT COUNT(*) FROM USERS WHERE USERNAME='" + SUser.m_sUserName + "'";
		nStatus = sqlite3_prepare_v2(sqliteDataBase, sQuery.c_str(), READ_UP_TO_ZERO_TERMINATOR, &InfoFromDB, NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing " << sqlite3_errmsg(sqliteDataBase);
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				bUserExistence = (bool)sqlite3_column_int(InfoFromDB, 0);
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	return bUserExistence;
}
bool UsersDataBase::CUsersDataBase::CheckUsersPassword(const SUser& SUser)
{
	sqlite3* sqliteDataBase;
	sqlite3_stmt* InfoFromDB;
	bool bUserExistence = false;
	int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
	if (nStatus != SQLITE_OK) {
		std::cerr << "Error opening table from CheckUsersPassword" << std::endl;
	}
	else {
		std::string sQuery = "SELECT COUNT(*) FROM USERS WHERE USERNAME='" + SUser.m_sUserName + "' AND PASSWORD = '" + SUser.m_sPassword + "'";
		nStatus = sqlite3_prepare_v2(sqliteDataBase, sQuery.c_str(), READ_UP_TO_ZERO_TERMINATOR, &InfoFromDB, NULL);
		if (nStatus != SQLITE_OK) {
			std::cout << "Error preparing " << sqlite3_errmsg(sqliteDataBase);
		}
		else {
			while ((nStatus = sqlite3_step(InfoFromDB) == SQLITE_ROW)) {
				bUserExistence = (bool)sqlite3_column_int(InfoFromDB, 0);
			}
		}
		sqlite3_finalize(InfoFromDB);
	}
	return bUserExistence;
}
//-----------------------------------------------------------------------------
bool UsersDataBase::CUsersDataBase::AddToFavorites(const SUser& SUser, const int& nIdOfRecipe)
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "UPDATE USERS SET FAVORITES='" + SUser.m_sFavorites +
		std::to_string(nIdOfRecipe) + "/' WHERE ID=" + std::to_string(SUser.m_nID);
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
bool UsersDataBase::CUsersDataBase::DeleteFromFavorites(SUser& SUser, const int& nIdOfRecipe)
{
	std::string sIdOfRecipe = std::to_string(nIdOfRecipe) + "/";
	int nPosOfID = SUser.m_sFavorites.find(sIdOfRecipe);
	bool bResult = false;

	if (nPosOfID >= 0) {
		SUser.m_sFavorites.erase(nPosOfID, sIdOfRecipe.length());

		sqlite3* sqliteDataBase;
		std::string sQuery = "UPDATE USERS SET FAVORITES='" + SUser.m_sFavorites + "' WHERE ID=" + std::to_string(SUser.m_nID);
		char* chError = nullptr;

		do {
			int nStatus = sqlite3_open(m_chPath, &sqliteDataBase);
			if (nStatus != SQLITE_OK) {
				std::cerr << "Error opening table from DeleteFromFavorites" << std::endl;
				break;
			}

			nStatus = sqlite3_exec(sqliteDataBase, sQuery.c_str(), NULL, NULL, &chError);

			if (nStatus != SQLITE_OK) {
				std::cerr << "Error updating" << std::endl;
				sqlite3_free(chError);
				break;
			}
			std::cout << "Updated successfully" << std::endl;
			sqlite3_close(sqliteDataBase);
			bResult = true;
		} while (false);
	}	
	return bResult;
}
bool UsersDataBase::CUsersDataBase::DeleteUser(const int& nUsersID)
{
	sqlite3* sqliteDataBase;
	std::string sQuery = "DELETE FROM USERS WHERE ID=" + std::to_string(nUsersID);
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

UsersDataBase::CUsersDataBaseDestroyer::~CUsersDataBaseDestroyer()
{
	if (m_pUsersDB) {
		delete m_pUsersDB;
	}
}
//-----------------------------------------------------------------------------
void UsersDataBase::CUsersDataBaseDestroyer::InitDestroyer(CUsersDataBase* pUsersDB)
{
	this->m_pUsersDB = pUsersDB;
}

UsersDataBase::SUser::SUser():m_nID{0},m_sUserName{""},m_sPassword{""},m_sFavorites{""}
{
}
