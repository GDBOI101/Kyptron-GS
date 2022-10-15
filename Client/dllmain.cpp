// Atomic STW
# ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
# endif
# ifndef VC_EXTRALEAN
#   define VC_EXTRALEAN
# endif
# ifndef NOMINMAX
#   define NOMINMAX
# endif

#include "minhook/MinHook.h"
#pragma comment(lib,"minhook/minhook.lib")
#include "Sigs.h"
#include "Offsets.h"
#include "Utils.h"
#include "Patches.h"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Psapi.h>
#include <string>
#include <string>
#include <locale>
#include <set>
#include <fstream>
#include "client.h"

bool IsSTW = false;
bool IsX = false;
UObject* JumpFunc;
UObject* SprintFunc;
UObject* GetAnimHRFunc;
UObject* PlayMontageFunc;
TEnumAsByte<EFortCustomBodyType> MyBodyType;
TEnumAsByte<EFortCustomGender> MyGender;

void DumpUObjects() {
    std::ofstream log("GObjects.log");
    log << "Fortnite GObjects Dump (using Neon Obj Dumper) by Array0x\nPlease Credit me if you use one of the files in your project\n\n";

    for (auto i = 0x0; i < GObjs->Num(); ++i)
    {
        auto object = GObjs->GetByIndex(i);
        if (object == nullptr)
        {
            continue;
        }
        std::string className = object->Class->GetFullName();
        std::string objectName = object->GetFullName();
        std::string item = "\n[" + std::to_string(i) + "] Object:[" + objectName + "] Class:[" + className +
            "]\n";
        log << item;
    }
}

UObject* GetPC() {
    return GFPC(GWorld);
}

UObject* STWCreateCM() {
    return *reinterpret_cast<UObject**>(__int64(GPC) + __int64(FindOffset("ObjectProperty /Script/Engine.PlayerController.CheatManager")));
}

void GiveItems() {
    for (int i = 0; i < GObjs->Num(); i++)
    {
        UObject* Object = GObjs->GetItemByIndex(i)->Object;

        if (Object == nullptr)
        {
            continue;
        }
        if (Object->GetFullName().find("FortniteGame") == std::string::npos) {
            if (Object->GetFullName().find("FortAmmoItemDefinition") != std::string::npos) {
                Inventory::AddItemToInventory(Object, 999, 0, EFortQuickBars::Primary);
            }
            if (Object->GetFullName().find("FortWeaponRangedItemDefinition") != std::string::npos || Object->GetFullName().find("FortWeaponMeleeItemDefinition") != std::string::npos)
            {
                Inventory::AddItemToInventory(Object, 1, 0, EFortQuickBars::Primary);
            }
        }
    }
}



void FixBuilds() {

    FVector* Pos = GetLocation(GPawn);
    UObject* B1 = SpawnActor(GWorld, FindObject("Class /Script/FortniteGame.BuildingPlayerPrimitivePreview"), Pos, {}, FActorSpawnParameters());
    UObject* B2 = SpawnActor(GWorld, FindObject("Class /Script/FortniteGame.BuildingPlayerPrimitivePreview"), Pos, {}, FActorSpawnParameters());
    UObject* B3 = SpawnActor(GWorld, FindObject("Class /Script/FortniteGame.BuildingPlayerPrimitivePreview"), Pos, {}, FActorSpawnParameters());
    UObject* B4 = SpawnActor(GWorld, FindObject("Class /Script/FortniteGame.BuildingPlayerPrimitivePreview"), Pos, {}, FActorSpawnParameters());

    bool SetHidden = true;
    UObject* SAH = FindObject("Function /Script/Engine.Actor.SetActorHiddenInGame");
    ProcessEventOG(B1, SAH, &SetHidden);
    ProcessEventOG(B2, SAH, &SetHidden);
    ProcessEventOG(B3, SAH, &SetHidden);
    ProcessEventOG(B4, SAH, &SetHidden);
}
LPVOID PEHook(UObject* Object, UObject* Function, void* Params) {
    std::string FuncName = Function->GetFullName();

    if (FuncName.find("StartButton") != std::string::npos)
    {
        //CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ConnectToServer, 0, 0, 0);
        Client::Connect("25.6.91.25");
        /*IsSTW = true;
        Fixes* fixes = new Fixes();
        fixes->Update();
        FString Map = L"Zone_Outpost_CannyValley";
        struct {
            FString InMap;
        } params;
        params.InMap = Map;
        ProcessEventOG(GPC, FindObject("Function /Script/Engine.PlayerController.SwitchLevel"), &params);*/
    }

    class Fixes {
    public:
        void Update() {
            uintptr_t GWorldAddr = FindPattern(GWorld_1_8);
            auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
            GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);
            GPC = GetPC();
        }
    };

    if (FuncName.find("BP_StartButton") != std::string::npos)
    {
        printf("\nLOG: BR Start Button Pressed\n");
    }

    if (FuncName.find("ReadyToStartMatch") != std::string::npos)
    {
        Fixes* fixes = new Fixes();
        fixes->Update();
        if (Client::IsSentToLobby == true)
        {
            printf("\nMODE CHANGE: Going back to Lobby.\nif you are experiencing this either\n1. The host has closed the server.\n2. You have encountered a error.\n");
        }
        else
        {
            if (Client::IsPlayButton == true)
            {
                Writer* writer = new Writer();
                writer->WriteString("ReadyToStartMatch");
                Client::Send(writer->Dump());
                printf("\nMODE CHANGE: Going In-game");
                PlayMontageFunc = FindObject("Function /Script/Engine.AnimInstance.Montage_Play");
                GetAnimHRFunc = FindObject("Function /Script/FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference");
                GCM = STWCreateCM();
                JumpFunc = FindObject("Function /Script/Engine.Character.Jump");
                SprintFunc = FindObject("Class /Script/FortniteGame.FortGameplayAbility_Sprint");
                // Normal -> GPawn = SpawnActor(GWorld, FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C"), new FVector(-3600, -300, -1522), new FVector(0, 275, 0), FActorSpawnParameters());
                GPawn = SpawnActor(GWorld, FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C"), new FVector(0, 0, 5500), new FVector(0, 0, 0), FActorSpawnParameters());
                ProcessEventOG(GPC, FindObject("Function /Script/Engine.Controller.Possess"), &GPawn);
                ProcessEventOG(GCM, FindObject("Function /Script/Engine.CheatManager.God"), nullptr);
                UObject* GM = *reinterpret_cast<UObject**>(__int64(GWorld) + __int64(Offsets::World::AuthorityGameMode));
                ProcessEventOG(GM, FindObject("Function /Script/Engine.GameMode.StartMatch"), nullptr);
                ProcessEventOG(GM, FindObject("Function /Script/Engine.GameModeBase.StartPlay"), nullptr);
                FString FGSZ_Class = L"FortGameStateZone";
                ProcessEventOG(GCM, FindObject("Function /Script/Engine.CheatManager.Summon"), &FGSZ_Class);
                UObject* PlayerState = *reinterpret_cast<UObject**>(__int64(GPC) + __int64(FindOffset("ObjectProperty /Script/Engine.Controller.PlayerState")));
                ProcessEventOG(PlayerState, FindObject("Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterParts"), nullptr);
                UObject* SKM = *reinterpret_cast<UObject**>(__int64(GPawn) + __int64(FindOffset("ObjectProperty /Script/Engine.Character.Mesh")));
                ProcessEventOG(SKM, FindObject("Function /Script/Engine.SkeletalMeshComponent.GetAnimInstance"), &GAI);
                ProcessEventOG(GCM, FindObject("Function /Script/FortniteGame.FortCheatManager.ToggleInfiniteAmmo"), nullptr);
                CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Client::TickThread, 0, 0, 0);
            }
            else
            {
                printf("\nMODE CHANGE: Going into STW/BR Menus");
            }
        }
    }

    if (FuncName.find("ServerLoadingScreenDropped") != std::string::npos)
    {
        if (Client::IsSentToLobby == true)
        {

        }
        else
        {
            if (Client::IsPlayButton == true)
            {
                Inventory::InitInventory();
                GiveItems();
                Inventory::AddItemToInventory(FindObject("FortWeaponMeleeItemDefinition /Game/Items/Weapons/Melee/Harvest/WID_Harvest_Pickaxe_C_T02.WID_Harvest_Pickaxe_C_T02"), 1, 0, EFortQuickBars::Primary);
                Inventory::AddItemToInventory(FindObject("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"), 1, 0, EFortQuickBars::Secondary);
                Inventory::AddItemToInventory(FindObject("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"), 1, 1, EFortQuickBars::Secondary);
                Inventory::AddItemToInventory(FindObject("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"), 1, 2, EFortQuickBars::Secondary);
                Inventory::AddItemToInventory(FindObject("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"), 1, 3, EFortQuickBars::Secondary);
                Inventory::UpdateInventory();
                Inventory::AddItemToInventory(FindObject("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"), 999, 0, EFortQuickBars::Secondary);
                Inventory::AddItemToInventory(FindObject("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"), 999, 0, EFortQuickBars::Secondary);
                Inventory::AddItemToInventory(FindObject("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"), 999, 0, EFortQuickBars::Secondary);
                FixBuilds();
                Client::IsInGame = true;
            }
            else
            {

            }
        }
    }

    if (FuncName.find("B_Prj_Athena_DanceGrenade_C.OnExploded") != std::string::npos) {
        struct BoogieBombExploded_Params {
            TArray<UObject*> HitActors;
            TArray<UObject*> HitResults;
        };
        BoogieBombExploded_Params* BB_Params = reinterpret_cast<BoogieBombExploded_Params*>(Params);
        for (int i = 0; i < BB_Params->HitActors.Num(); i++) {
            if (BB_Params->HitActors[i]->GetFullName().find("Pawn") != std::string::npos) {
                struct
                {
                    UObject* MontageToPlay;
                    float InPlayRate;
                    EMontagePlayReturnType ReturnValueType;
                    float InTimeToStartMontageAt;
                    bool bStopAllMontages;
                    float ReturnValue;
                } params2;
                params2.MontageToPlay = FindObject("AnimMontage /Game/Animation/Game/MainPlayer/Emotes/Emote_DG_Disco_M.Emote_DG_Disco_M");
                params2.InPlayRate = 1;
                params2.InTimeToStartMontageAt = 0;
                params2.bStopAllMontages = true;
                params2.ReturnValueType = EMontagePlayReturnType::Duration;
                UObject* AnimInst;
                UObject* SKM = *reinterpret_cast<UObject**>(__int64(BB_Params->HitActors[i]) + __int64(FindOffset("ObjectProperty /Script/Engine.Character.Mesh")));
                ProcessEventOG(SKM, FindObject("Function /Script/Engine.SkeletalMeshComponent.GetAnimInstance"), &AnimInst);
                ProcessEventOG(AnimInst, PlayMontageFunc, &params2);
            }
        }
    }

    if (FuncName.find("ServerEmote") != std::string::npos) {
        struct EmoteParams {
            FName EmoteAsset;
        };
        std::string EmoteName = static_cast<EmoteParams*>(Params)->EmoteAsset.ToString().c_str();
        EmoteName += "." + EmoteName;
        auto EmoteDef = FindObject(EmoteName);
        struct
        {
            TEnumAsByte<EFortCustomBodyType> BodyType;
            TEnumAsByte<EFortCustomGender> Gender;
            UObject* ReturnValue;
        } params;
        params.BodyType = MyBodyType;
        params.Gender = MyGender;
        ProcessEventOG(EmoteDef, GetAnimHRFunc, &params);
        struct
        {
            UObject* MontageToPlay;
            float InPlayRate;
            EMontagePlayReturnType ReturnValueType;
            float InTimeToStartMontageAt;
            bool bStopAllMontages;
            float ReturnValue;
        } params2;
        params2.MontageToPlay = params.ReturnValue;
        params2.InPlayRate = 1;
        params2.InTimeToStartMontageAt = 0;
        params2.bStopAllMontages = true;
        params2.ReturnValueType = EMontagePlayReturnType::Duration;
        ProcessEventOG(GAI, PlayMontageFunc, &params2);
    }

    if (FuncName.find("ServerExecuteInventoryItem") != std::string::npos) {
        FGuid* GUID = reinterpret_cast<FGuid*>(Params);
        Inventory::EquipInventoryItem(GUID);
    }

    if (FuncName.find("Tick") != std::string::npos && IsSTW == true) {
        if (GetAsyncKeyState(VK_SPACE) & 0x1) {
            bool IsJumping = *reinterpret_cast<bool*>(__int64(GAI) + __int64(Offsets::FortAnimInstance::bIsJumping));
            if (IsJumping == false) {
                ProcessEventOG(GPawn, JumpFunc, nullptr);
            }
        }

        if (GetAsyncKeyState(VK_LSHIFT) & 0x1) {
          ProcessEventOG(GPawn, SprintFunc, nullptr);
        }

        if (GetAsyncKeyState(VK_F2) & 0x1) 
        {
            DumpUObjects();
        }
    }

    if (FuncName.find("ClientOnPawnDied") != std::string::npos && IsSTW == true)
    {
        printf("\nLOG: A Client has Died\n");
    }

    if (FuncName.find("ClientOnPawnRevived") != std::string::npos && IsSTW == true)
    {
        printf("\nLOG: A Client has been Revived\n");
    }

    if (FuncName.find("ClientOnPawnSpawned") != std::string::npos && IsSTW == true)
    {
        printf("\nLOG: New Client Connected!\n");
    }

    return ProcessEventOG(Object, Function, Params);
}


void Initalize()
{
    MH_Initialize();
    uintptr_t GObjsAddr;
    uintptr_t GWorldAddr;
    uintptr_t FNTSAddr;
    uintptr_t FreeAddr;
    uintptr_t SAAddr;
    uintptr_t ProcessEventAddr;

    // 1.0
    if (FNVersion.find("1.") != std::string::npos)
    {
        GObjsAddr = FindPattern(GObjects_1_8);
        auto GObjsOffset = *reinterpret_cast<uint32_t*>(GObjsAddr + 3);
        GObjs = reinterpret_cast<UObjectArray*>(GObjsAddr + 7 + GObjsOffset);

        GWorldAddr = FindPattern(GWorld_1_8);
        auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
        GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);

        FNTSAddr = FindPattern(FNTS_1_8);
        FNameToString = decltype(FNameToString)(FNTSAddr);

        FreeAddr = FindPattern(Free_1_8);
        StaticConstructObjectInternal = decltype(StaticConstructObjectInternal)(FindPattern(SCOI_1_8));

        SAAddr = FindPattern(SpawnActor_1_8);
        printf("\nLOG: 1.X DETECTED\n");
    }

    // 2.0
    if (FNVersion.find("2.") != std::string::npos)
    {
        GObjsAddr = FindPattern(GObjects_1_8);
        auto GObjsOffset = *reinterpret_cast<uint32_t*>(GObjsAddr + 3);
        GObjs = reinterpret_cast<UObjectArray*>(GObjsAddr + 7 + GObjsOffset);

        GWorldAddr = FindPattern(GWorld_1_8);
        auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
        GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);

        FNTSAddr = FindPattern(FNTS_1_8);
        FNameToString = decltype(FNameToString)(FNTSAddr);

        FreeAddr = FindPattern(Free_1_8);
        StaticConstructObjectInternal = decltype(StaticConstructObjectInternal)(FindPattern(SCOI_1_8));

        SAAddr = FindPattern(SpawnActor_1_8);
        printf("\nLOG: 2.X DETECTED\n");
    }

    // 3.0
    if (FNVersion.find("3.") != std::string::npos)
    {
        GObjsAddr = FindPattern(GObjects_1_8);
        auto GObjsOffset = *reinterpret_cast<uint32_t*>(GObjsAddr + 3);
        GObjs = reinterpret_cast<UObjectArray*>(GObjsAddr + 7 + GObjsOffset);

        GWorldAddr = FindPattern(GWorld_1_8);
        auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
        GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);

        FNTSAddr = FindPattern(FNTS_1_8);
        FNameToString = decltype(FNameToString)(FNTSAddr);

        FreeAddr = FindPattern(Free_1_8);
        StaticConstructObjectInternal = decltype(StaticConstructObjectInternal)(FindPattern(SCOI_1_8));

        SAAddr = FindPattern(SpawnActor_1_8);
        printf("\nLOG: 3.X DETECTED\n");
    }

    // 4.0
    if (FNVersion.find("4.") != std::string::npos)
    {
        GObjsAddr = FindPattern(GObjects_1_8);
        auto GObjsOffset = *reinterpret_cast<uint32_t*>(GObjsAddr + 3);
        GObjs = reinterpret_cast<UObjectArray*>(GObjsAddr + 7 + GObjsOffset);

        GWorldAddr = FindPattern(GWorld_1_8);
        auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
        GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);

        FNTSAddr = FindPattern(FNTS_1_8);
        FNameToString = decltype(FNameToString)(FNTSAddr);

        FreeAddr = FindPattern(Free_1_8);
        StaticConstructObjectInternal = decltype(StaticConstructObjectInternal)(FindPattern(SCOI_1_8));

        SAAddr = FindPattern(SpawnActor_1_8);

        printf("\nLOG: 4.X DETECTED\n");
    }

    // 6.X
    if (FNVersion.find("6.") != std::string::npos)
    {
        GObjsAddr = FindPattern(C1S5_GOBJECTS);
        auto GObjsOffset = *reinterpret_cast<uint32_t*>(GObjsAddr + 3);
        GObjs = reinterpret_cast<UObjectArray*>(GObjsAddr + 7 + GObjsOffset);

        GWorldAddr = FindPattern(GWorld_1_8);
        auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
        GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);

        FNTSAddr = FindPattern(C1S5_GETOBJECTNAME);
        FNameToString = decltype(FNameToString)(FNTSAddr);

        FreeAddr = FindPattern(C1S5_FREE);
        StaticConstructObjectInternal = decltype(StaticConstructObjectInternal)(FindPattern(C1S5_SCOI));

        SAAddr = FindPattern(C1S5_SPAWNACTOR);

        printf("\nLOG: 6.X DETECTED\n");
    }

    // 10.X
    if (FNVersion.find("10.") != std::string::npos)
    {
        GObjsAddr = FindPattern(C1S9_GOBJECTS);
        auto GObjsOffset = *reinterpret_cast<uint32_t*>(GObjsAddr + 3);
        GObjs = reinterpret_cast<UObjectArray*>(GObjsAddr + 7 + GObjsOffset);

        GWorldAddr = FindPattern(C1S9_GWORLD);
        auto GWorldOffset = *reinterpret_cast<uint32_t*>(GWorldAddr + 3);
        GWorld = *reinterpret_cast<UObject**>(GWorldAddr + 7 + GWorldOffset);

        FNTSAddr = FindPattern(C1S9_FNTS);
        FNameToString = decltype(FNameToString)(FNTSAddr);

        FreeAddr = FindPattern(C1S9_FREE);
        StaticConstructObjectInternal = decltype(StaticConstructObjectInternal)(FindPattern(C1S9_SCOI));

        SAAddr = FindPattern(C1S9_SPAWNACTOR);

        ProcessEventAddr = FindPattern(C1_PROCESSEVENT);
        printf("\nLOG: 10.X DETECTED\n");
        IsX = true;
    }

    FnFree = decltype(FnFree)(FreeAddr);
    SpawnActor = decltype(SpawnActor)(SAAddr);
    GFPC = decltype(GFPC)(FindPattern("83 B9 ? ? ? ? ? 7E ? 48 8B 89 ? ? ? ? E9"));
    void** Engine = *reinterpret_cast<void***>(FindObject("FortEngine_"));
    auto PEAddr = (uintptr_t)Engine[64];
    ProcessEventOG = decltype(ProcessEventOG)(PEAddr);
    Offsets::InitOffsets();
    MH_CreateHook((LPVOID)PEAddr, PEHook, (LPVOID*)&ProcessEventOG);
    MH_EnableHook((LPVOID)PEAddr);
    printf("\nLOG: Legacy PEHook Loaded, You may now go in-game\n");

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Utils::InitConsole();
        printf("\n-----------------------------------\n");
        printf("       Welcome to Atomic STW\n");
        printf("  Created by Array (Credits to GD)\n");
        printf("-----------------------------------\n");
        Initalize();
        break;
    }
    return TRUE;
}

