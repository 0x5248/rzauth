#ifndef UPLOADSERVER_UPLOADREQUEST_H
#define UPLOADSERVER_UPLOADREQUEST_H

#include "Core/Object.h"
#include "uv.h"
#include <stdint.h>
#include <string>
#include <time.h>
#include <unordered_map>

namespace UploadServer {

class GameServerSession;

class UploadRequest : public Object {
	DECLARE_CLASS(UploadServer::UploadRequest)

public:
	UploadRequest(GameServerSession* gameServer,
	              uint32_t client_id,
	              uint32_t account_id,
	              uint32_t guild_sid,
	              uint32_t one_time_password);

	GameServerSession* getGameServer() { return gameServer; }
	uint32_t getClientId() { return client_id; }
	uint32_t getAccountId() { return account_id; }
	uint32_t getGuildId() { return guild_sid; }
	uint32_t getOneTimePassword() { return one_time_password; }

	static UploadRequest* pushRequest(GameServerSession* gameServer,
	                                  uint32_t client_id,
	                                  uint32_t account_id,
	                                  uint32_t guild_sid,
	                                  uint32_t one_time_password);
	static UploadRequest* popRequest(uint32_t client_id,
	                                 uint32_t account_id,
	                                 uint32_t guild_sid,
	                                 uint32_t one_time_password,
	                                 const std::string& gameServerName);
	static void removeServer(GameServerSession* server);  // remove all requests from this server
	static unsigned int getClientCount() { return (int) pendingRequests.size(); }

private:
	static uv_mutex_t initializeLock();

	static std::unordered_map<uint32_t, UploadRequest*> pendingRequests;
	static uv_mutex_t mapLock;

	GameServerSession* gameServer;
	uint32_t client_id;
	uint32_t account_id;
	uint32_t guild_sid;
	uint32_t one_time_password;
	time_t timestamp;
};

}  // namespace UploadServer

#endif  // UPLOADSERVER_UPLOADREQUEST_H
