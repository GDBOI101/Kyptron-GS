#pragma once
#include <signal.h>
#include <time.h>
#include <string>
#include <iostream>

enum class A_GameCodes {
	JOINGAME = 1,
	SPAWNPLAYER = 2,
	RTP = 3,
	INGAME,
	DEATH,
	SUICIDE
};

enum class A_GameAbilities {
	RIFT,
	ENTERVEHICLE,
	EXITVEHICLE,
	KICK,
	BAN,
	STARTEVENT,
	STOPEVENT,
	SUMMONHUSK,
	SHOOT,
	AIM
};