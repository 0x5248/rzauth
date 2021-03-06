#include "GlobalConfig.h"
#include "Cipher/DbPasswordCipher.h"
#include "Config/GlobalCoreConfig.h"
#include "Core/Utils.h"
#include "rzauthGitVersion.h"

GlobalConfig* GlobalConfig::get() {
	static GlobalConfig config;
	return &config;
}

void GlobalConfig::init() {
	GlobalConfig::get();
	CFG_CREATE("global.version", rzauthVersion);
	GlobalCoreConfig::get()->app.configfile.setDefault("auth.opt");
	GlobalCoreConfig::get()->admin.listener.port.setDefault(4501);

#ifdef _WIN32
	GlobalConfig::get()->auth.db.driver.setDefault("SQL Server");
#else
	GlobalConfig::get()->auth.db.driver.setDefault("FreeTDS");
#endif
}

void DbConfig::updateConnectionString(IListener* instance) {
	DbConfig* thisInstance = (DbConfig*) instance;

	std::string cryptedConnectionStringHex = thisInstance->cryptedConnectionString.get();
	if(cryptedConnectionStringHex.size() > 0) {
		thisInstance->connectionString.setDefault(
		    DbPasswordCipher::decrypt(Utils::convertHexToData(cryptedConnectionStringHex)));
	} else {
		std::string password;
		std::string cryptedPasswordHex = thisInstance->cryptedPassword.get();
		if(cryptedPasswordHex.size() > 0) {
			password = DbPasswordCipher::decrypt(Utils::convertHexToData(cryptedPasswordHex));
		} else {
			password = thisInstance->password.get();
		}

		thisInstance->connectionString.setDefault(
		    "DRIVER=" + thisInstance->driver.get() + ";Server=" + thisInstance->server.get() + "," +
		    Utils::convertToString(thisInstance->port.get()) + ";Database=" + thisInstance->name.get() +
		    ";UID=" + thisInstance->account.get() + ";PWD=" + password + ";");
	}
}
