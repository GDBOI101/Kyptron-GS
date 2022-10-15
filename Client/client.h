#pragma once
#define WIN32_LEAN_AND_MEAN
#include "BW.h"
#include "BR.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <shlobj.h>
#include "Player.h"

using boost::asio::ip::udp;
using namespace boost::asio;
using ip::udp;

using namespace std;
class Fixes {
public:
	void Update() {
		uintptr_t GWorldAddr = FindPattern(GWorld_1_8);
		auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
		GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);
		GPC = GetPC();
	}
	UObject* GetPC() {
		return GFPC(GWorld);
	}
};

namespace PlayerUtils {
	inline FortPlayer* GetPlayerByName(std::string Name) {
		for (FortPlayer* P : PlayerList)
		{
			if (P->PlayerID == Name)
			{
				return P;
			}
		}

		return nullptr;
	}

	void ServerChoosePart(UObject* Target, TEnumAsByte<EFortCustomPartType> Type, UObject* ChosenCharacterPart)
	{
		struct
		{
			TEnumAsByte<EFortCustomPartType> Type;
			UObject* ChosenCharacterPart;
		} Params;

		Params.Type = Type;
		Params.ChosenCharacterPart = ChosenCharacterPart;

		ProcessEventOG(Target, FindObject("Function /Script/FortniteGame.FortPlayerPawn.ServerChoosePart"), &Params);

	}
	void SetActorGodMode(UObject* InActor, bool bGodMode)
	{
		struct
		{
			UObject* Actor;
			bool bCanBeDamaged;
		} SetCanBeDamagedParams;

		SetCanBeDamagedParams.Actor = InActor;
		SetCanBeDamagedParams.bCanBeDamaged = !bGodMode;
		ProcessEventOG(FindObject("FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"), FindObject("Function /Script/FortniteGame.FortKismetLibrary.SetCanBeDamaged"), &SetCanBeDamagedParams);
	}

	inline void TeleportTo(UObject* Target, FVector Loc, FRotator Rot = FRotator(0, 0, 0))
	{
		struct {
			FVector Loc;
			FRotator Rot = Rot;
		} params;
		params.Loc = Loc;
		params.Rot = Rot;
		ProcessEventOG(Target, SetActorLocation, &params);
	}

	inline void StartPlayer(std::string Name)
	{
		auto PlayerPawn = SpawnActor(GWorld, FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C"), new FVector(0, 0, 5500), new FVector(0, 0, 0), FActorSpawnParameters());

		auto PlayerController = SpawnActor(GWorld, FindObject("Class /Script/FortniteGame.FortPlayerController"), {}, {}, FActorSpawnParameters());
		
		ProcessEventOG(PlayerController, FindObject("Function /Script/Engine.Controller.Possess"), &PlayerPawn);
		SetActorGodMode(PlayerPawn, true);
		
		UObject* PlayerGameState = *reinterpret_cast<UObject**>(__int64(PlayerController) + __int64(FindOffset("ObjectProperty /Script/Engine.Controller.PlayerState")));
		
		UObject* HeadCharacterPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
		UObject* BodyCharacterPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));
		ServerChoosePart(PlayerPawn, EFortCustomPartType::Body, BodyCharacterPart);
		ServerChoosePart(PlayerPawn, EFortCustomPartType::Head, HeadCharacterPart);

		ProcessEventOG(PlayerGameState, FindObject("Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterParts"), nullptr);
		
		UObject* Mesh = *reinterpret_cast<UObject**>(__int64(PlayerPawn) + __int64(FindOffset("ObjectProperty /Script/Engine.Character.Mesh")));
		
		PlayerList.push_back(new FortPlayer(Name, PlayerPawn, PlayerController, PlayerGameState));
	}

}

UObject* GAL_Func = nullptr;

FVector* GetLocation(UObject* Target) {
	FVector Ret;
	if (GAL_Func == nullptr) {
		GAL_Func = FindObject("Function /Script/Engine.Actor.K2_GetActorLocation");
	}
	ProcessEventOG(Target, GAL_Func, &Ret);
	return new FVector(Ret);
}

UObject* GAR_Func = nullptr;

FVector* GetRotation(UObject* Target) {
	FVector Ret;
	if (GAR_Func == nullptr) {
		GAR_Func = FindObject("Function /Script/Engine.Actor.K2_GetActorRotation");
	}
	ProcessEventOG(Target, GAR_Func, &Ret);
	return new FVector(Ret);
}


namespace Client {
	io_service ios;
	udp::socket socket_(ios);

	FortPlayer* RequestedPlayer;
	std::string IP_;
	bool IsSentToLobby = false;
	bool IsPlayButton = false;
	bool IsInGame = false;
	bool IsSent = false;
	string MyName = "";
	void SwitchLevel(const char* Map) {
		std::string SMap(Map);
		std::wstring WMap = std::wstring(SMap.begin(), SMap.end());
		FString URL = WMap.c_str();
		ProcessEventOG(GPC, FindObject("Function /Script/Engine.PlayerController.SwitchLevel"), &URL);
	}
	char buffer[1024];
	int bufferlen = 1024;

	void __stdcall MessageThread(LPVOID Param)
	{
		while (true)
		{
			udp::endpoint endpoint_(ip::address::from_string("127.0.0.1"), 7777);
			char data[1024] = {};
			Writer* BitW = new Writer();
			socket_.receive_from(boost::asio::buffer(data), endpoint_);

			std::vector<unsigned char> chars;
			for (char c : data)
			{
				chars.push_back(c);
			}

			Reader* BitReader = new Reader(chars);

			std::string Function = BitReader->ReadString();

			if (Function == "SwitchLevel")
			{
				Fixes* fixes = new Fixes();
				fixes->Update();
				MyName = BitReader->ReadString();
				IsPlayButton = true;
				printf("\nSwitchLevel was Called.\n");
				FString Map = L"Zone_Outpost_CannyValley";
				struct {
					FString InMap;
				} params;
				params.InMap = Map;
				ProcessEventOG(GPC, FindObject("Function /Script/Engine.PlayerController.SwitchLevel"), &params);
			}

			if (Function == "InitPlayer")
			{
				printf("\nNEW PLAYER. SUMMONING\n");
				std::string PlayerName = BitReader->ReadString();
				PlayerUtils::StartPlayer(PlayerName);
			}

			if (Function == "ForceLeave")
			{
				printf("\nCrashing Server\n");
				IsSentToLobby = true;
				Fixes* fixes = new Fixes();
				fixes->Update();
				FString Map = L"Frontend";
				struct {
					FString InMap;
				} params;
				params.InMap = Map;
				ProcessEventOG(GPC, FindObject("Function /Script/Engine.PlayerController.SwitchLevel"), &params);
			}

			if (Function == "Tick" && IsInGame == true)
			{
				std::string TargetPlayerName = BitReader->ReadString();
				FortPlayer* TargetPlayer = PlayerUtils::GetPlayerByName(TargetPlayerName);
				if (TargetPlayer != nullptr) {
					float X = BitReader->ReadFloat();
					float Y = BitReader->ReadFloat();
					float Z = BitReader->ReadFloat();
					float P = BitReader->ReadFloat();
					float YA = BitReader->ReadFloat();
					float R = BitReader->ReadFloat();

					PlayerUtils::TeleportTo(TargetPlayer->GetPawn(), FVector(X, Y, Z), FRotator(P, YA, R));
				}
			}

		}
	}
	void Send(std::vector<unsigned char> Data)
	{
		udp::endpoint endpoint_(ip::address::from_string(IP_), 7777);
		socket_.send_to(boost::asio::buffer(Data), endpoint_);
	}

	void Connect(std::string IP)
	{
		IP_ = IP;
		socket_.open(udp::v4());
		socket_.set_option(socket_base::broadcast(true));

		Writer* BitW = new Writer();

		wchar_t* RoamingAppData;
		SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &RoamingAppData);
		//std::wstringstream Stream;
		//Stream << RoamingAppData << L"/CreateFN/DN.txt";
		//std::ifstream _FILE(Stream.str());
		//std::string Line;
		//std::getline(_FILE, Line);
		Fixes* fixes = new Fixes();
		fixes->Update();
		//UObject* PlayerState = *reinterpret_cast<UObject**>(__int64(GPC) + __int64(FindOffset("ObjectProperty /Script/Engine.Controller.PlayerState")));
		UObject* MCPG = *reinterpret_cast<UObject**>(__int64(GPC) + __int64(FindOffset("ObjectProperty /Script/FortniteGame.FortPlayerController.McpProfileGroup")));
		string MyPlayerName = (*reinterpret_cast<FString*>(__int64(MCPG) + __int64(FindOffset("StrProperty /Script/McpProfileSys.McpProfileGroup.PlayerName")))).ToString();
		MyName = MyPlayerName;
		BitW->WriteString("Join");
		BitW->WriteString(MyPlayerName);

		Send(BitW->Dump());

		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&MessageThread), 0, 0, nullptr);
	}

	void TickThread()
	{
		while (true)
		{
			Sleep(1000 / 30);

			Writer* writer = new Writer();
			writer->WriteString("PlayerTick");
			writer->WriteString(MyName);
			FVector* Loc = GetLocation(GPawn);
			writer->Writefloat(Loc->X);
			writer->Writefloat(Loc->Y);
			writer->Writefloat(Loc->Z);
			FVector* Rot = GetRotation(GPawn);
			writer->Writefloat(Rot->X);
			writer->Writefloat(0);
			writer->Writefloat(Rot->Z);
			Client::Send(writer->Dump());

		}
	}
}
