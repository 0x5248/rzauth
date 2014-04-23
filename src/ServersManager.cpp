#include "ServersManager.h"
#include "GlobalConfig.h"

#include "AuthServer/ClientSession.h"
#include "AuthServer/GameServerSession.h"
#include "AuthServer/DB_Account.h"

#include "UploadServer/ClientSession.h"
#include "UploadServer/GameServerSession.h"
#include "UploadServer/IconServerSession.h"

#include "AdminServer/TelnetSession.h"

ServersManager* ServersManager::instance = nullptr;

ServersManager::ServersManager()
{
	banManager.loadFile();
	if(instance == nullptr)
		instance = this;
	else
		error("Several ServersManager instance !\n");

	addServer("auth.clientserver", new RappelzServer<AuthServer::ClientSession>, CONFIG_GET()->auth.client.listenIp, CONFIG_GET()->auth.client.port, &banManager);
	addServer("auth.gameserver", new RappelzServer<AuthServer::GameServerSession>, CONFIG_GET()->auth.game.listenIp, CONFIG_GET()->auth.game.port);

	addServer("upload.clientserver", new RappelzServer<UploadServer::ClientSession>, CONFIG_GET()->upload.client.listenIp, CONFIG_GET()->upload.client.port, &banManager);
	addServer("upload.iconserver", new RappelzServer<UploadServer::IconServerSession>, CONFIG_GET()->upload.client.listenIp, CONFIG_GET()->upload.client.webPort, &banManager);
	addServer("upload.gameserver", new RappelzServer<UploadServer::GameServerSession>, CONFIG_GET()->upload.game.listenIp, CONFIG_GET()->upload.game.port);

	addServer("admin.telnet", new RappelzServer<AdminServer::TelnetSession>, CONFIG_GET()->admin.telnet.listenIp, CONFIG_GET()->admin.telnet.port);
}

ServersManager::~ServersManager()
{
	std::unordered_map<std::string, ServerInfo*>::iterator it, itEnd;

	for(it = servers.begin(), itEnd = servers.end(); it != itEnd; ++it) {
		ServerInfo* & server = it->second;
		delete server->server;
		delete server;
		server = nullptr;
	}
}

void ServersManager::addServer(const char* name, RappelzServerCommon* server, ConfigValue& listenIp, ConfigValue& listenPort, BanManager* banManager) {
	ServerInfo* serverInfo = new ServerInfo(server, &listenIp, &listenPort, banManager);

	servers.insert(std::pair<std::string, ServerInfo*>(std::string(name), serverInfo));
}

RappelzServerCommon* ServersManager::getServer(const std::string& name) {
	std::unordered_map<std::string, ServerInfo*>::iterator it = servers.find(name);
	if(it == servers.end())
		return nullptr;

	return it->second->server;
}

bool ServersManager::start() {
	std::unordered_map<std::string, ServerInfo*>::const_iterator it, itEnd;

	for(it = servers.cbegin(), itEnd = servers.cend(); it != itEnd; ++it) {
		ServerInfo* serverInfo = it->second;
		serverInfo->server->startServer(serverInfo->listenIp->getString(),
										serverInfo->listenPort->getInt(),
										serverInfo->banManager);
	}

	return servers.size() > 0;
}

bool ServersManager::stop() {
	std::unordered_map<std::string, ServerInfo*>::const_iterator it, itEnd;

	for(it = servers.cbegin(), itEnd = servers.cend(); it != itEnd; ++it) {
		ServerInfo* serverInfo = it->second;
		serverInfo->server->stop();
	}

	return servers.size() > 0;
}

bool ServersManager::start(const std::string& name) {
	std::unordered_map<std::string, ServerInfo*>::const_iterator it = servers.find(name);
	if(it != servers.end()) {
		ServerInfo* serverInfo = it->second;
		serverInfo->server->startServer(serverInfo->listenIp->getString(),
										serverInfo->listenPort->getInt(),
										serverInfo->banManager);

		return true;
	}

	return false;
}

bool ServersManager::stop(const std::string& name) {
	std::unordered_map<std::string, ServerInfo*>::const_iterator it = servers.find(name);
	if(it != servers.end()) {
		ServerInfo* serverInfo = it->second;
		serverInfo->server->stop();

		return true;
	}

	return false;
}
