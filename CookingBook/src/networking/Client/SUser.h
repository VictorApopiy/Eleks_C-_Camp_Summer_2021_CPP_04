#ifndef SUSER
#define SUSER
#include <string>
struct SUser {//strucrure which describes a User
		int m_nID;
		std::string m_sUserName;
		std::string m_sPassword;
		std::string m_sFavorites;//list of favourite recipes 
		SUser();
};

#endif //SUSER