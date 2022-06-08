#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <iostream>

#define _PROGRAM_NAME_ "App"
#define _PROGRAM_VERSION_ "1.0.0"
#define _STD_OUT_FILENAME_ "/tmp/" _PROGRAM_NAME_ "_" _PROGRAM_VERSION_ "-out.log"
#define _STD_ERR_FILENAME_ "/tmp/" _PROGRAM_NAME_ "_" _PROGRAM_VERSION_ "-err.log"

struct SProjectConf
{
	SProjectConf()
		:
		isDaemon(false)
	{ }

	// [ProjectDefaultParam]
	bool isDaemon;
};

/// @brief 单例可执行程序
/// @return true: 已有一个或多个进程在运行
///         false: 无进程运行
static bool mySingletonProgram();

/// @brief 更改程序路径为当前工作目录
/// @return true: 成功
/// 		false: 失败
static bool myChangeWorkPath();

/// @brief 设置程序本地字符集
static void myLocalCharacterSet();

/// @brief 初始化信号
static void myInitSignal();

/// @brief 初始化日志
static void myInitLogger();

/// @brief 读取工程配置文件
/// @param conf [out] 配置文件结构体
/// @return true: 成功
/// 		false: 失败
static bool myReadConfig(SProjectConf* conf);

/// @brief 默认设置
static void myDefaultSetting();

/// @brief 通过工程配置文件配置进程
/// @param conf [in] 配置文件结构体
/// @return true: 成功
/// 		false: 失败
static bool myConfigureProcess(const SProjectConf& conf);

int main(int argc, char* argv[])
{
	// 单例可执行程序
	if (mySingletonProgram()) { return 1; }

	// 更改程序路径为当前工作目录
	if (!myChangeWorkPath()) { return 2; }

	// 设置程序本地字符集
	myLocalCharacterSet();

	// 初始化信号
	myInitSignal();

	// 初始化日志
	myInitLogger();

	// 读取工程配置文件
	SProjectConf conf;
	if (!myReadConfig(&conf)) { return 3; }

	// 默认工程设置
	myDefaultSetting();

	// 通过工程配置文件配置进程
	if (!myConfigureProcess(conf)) { return 4; }

	return 0;
}

bool mySingletonProgram()
{
	return false;
}

bool myChangeWorkPath()
{
	char processdir[PATH_MAX] = { 0 };
	if (readlink("/proc/self/exe", processdir, sizeof(processdir)) <= 0) { return false; }
	auto path_end = strrchr(processdir, '/');
	if (path_end == NULL) { return false; }
	++path_end; *path_end = '\0';

	return chdir(processdir) == 0;
}


void myLocalCharacterSet()
{

}

void funcSignalHandle(int signum)
{

}
void myInitSignal()
{
	signal(SIGINT, funcSignalHandle);
	signal(SIGTERM, funcSignalHandle);
	signal(SIGHUP, funcSignalHandle);
	signal(SIGQUIT, funcSignalHandle);
	signal(SIGTSTP, funcSignalHandle);
}

void myInitLogger()
{

}

bool myReadConfig(SProjectConf* conf)
{
	conf->isDaemon = true;

	std::cout << "----------[Program Config]----------" << std::endl;
	std::cout << "isDaemon: " << conf->isDaemon << std::endl;
	return true;
}

void myDefaultSetting()
{

}

void runAsDaemon()
{
	int pid = fork();
	if (pid < 0)
	{
		// fork fail
		exit(-1);
	}
	else if (pid > 0)
	{
		// parent process
		exit(0);
	}
	else
	{
		// child process
		setsid();

		pid = fork();
		if (pid < 0)
		{
			// fork fail
			exit(-2);
		}
		else if (pid > 0)
		{
			// parent process
			exit(0);
		}
		else
		{
			// child process
			/*
			* /dev/stdin ：0
			* /dev/stdout：1
			* /dev/stderr：2
			*/
			int fd = 0;
			for (fd = 0; fd < 3; ++fd)
			{
				close(fd);
			}

			fd = open("/dev/null", O_RDONLY); dup2(fd, 0);
			fd = open(_STD_OUT_FILENAME_, O_RDWR | O_CREAT, 0666); dup2(fd, 1);
			fd = open(_STD_ERR_FILENAME_, O_RDWR | O_CREAT, 0666); dup2(fd, 2);

			umask(0);  //reset file desc
		}
	}
}
bool myConfigureProcess(const SProjectConf& conf)
{
	if (conf.isDaemon) { runAsDaemon(); }
	return true;
}
