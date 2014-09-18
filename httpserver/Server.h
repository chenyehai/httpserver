#pragma once

#include <cstddef>

#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <csignal>

#include "SocketList.h"
#include "DataVariantAbstract.h"
#include "ServerApplicationsTree.h"
#include "ServerApplicationDefaultSettings.h"
#include "Module.h"
#include "Event.h"

namespace HttpServer
{
	class Server
	{
	protected:
		std::unordered_map<std::string, DataVariantAbstract *> variants;

		std::unordered_map<std::string, std::string> settings;
		std::unordered_map<std::string, std::string> mimes_types;

		std::vector<Module> modules;

		ServerApplicationsTree apps_tree;

		SocketList sockets_list;

		Event *eventNotFullQueue;
		Event *eventProcessQueue;
		Event *eventUpdateModule;

		// Флаг, означающий - активированы ли главные циклы сервера
		// (с помощью этого флага можно деактивировать циклы, чтобы завершить работу сервера)
		sig_atomic_t process_flag;
		sig_atomic_t restart_flag;

	protected:
		int cycleQueue(std::queue<Socket> &);
		void sendStatus(const Socket &, const std::chrono::milliseconds &, const size_t) const;
		int threadRequestProc(Socket) const;
		int transferFilePart(const Socket &, const std::chrono::milliseconds &, const std::string &, const time_t, const size_t, const std::string &, const std::string &, const std::string &, const bool) const;
		int transferFile(const Socket &, const std::chrono::milliseconds &, const std::string &, const std::unordered_map<std::string, std::string> &, const std::map<std::string, std::string> &, const std::string &, const bool) const;
		bool parseIncomingVars(std::unordered_multimap<std::string, std::string> &, const std::string &) const;

	//	void accept(std::vector<Socket> &sockets, const System::native_socket_type max_val) const;

		bool loadConfig();
			bool includeConfigFile(const std::string &, std::string &, const size_t);
			bool addApplication(const std::unordered_map<std::string, std::string> &, const ServerApplicationDefaultSettings &);

		bool init();
		int run();
		void clear();

		System::native_processid_type getPidFromFile() const;

		void updateModules();
			bool updateModule(Module &, std::unordered_set<ServerApplicationSettings *> &, const size_t);

	private:
		void addDataVariant(DataVariantAbstract *);

	public:
		Server();
		~Server() = default;

		void stopProcess();

		inline void unsetProcess()
		{
			process_flag = false;
		}

		inline void setRestart()
		{
			restart_flag = true;
		}

		inline void setUpdateModule()
		{
			if (eventUpdateModule)
			{
				eventUpdateModule->notify();
			}
		}

		inline void setProcessQueue()
		{
			if (eventProcessQueue)
			{
				eventProcessQueue->notify();
			}
		}

		int command_help(const int argc, const char *argv[]) const;
		int command_start(const int argc, const char *argv[]);
		int command_restart(const int argc, const char *argv[]) const;
		int command_terminate(const int argc, const char *argv[]) const;
		int command_update_module(const int argc, const char *argv[]) const;
	};
};