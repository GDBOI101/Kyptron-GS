class FortPlayer
{
private:
    FVector* CurrentLocation;
    FRotator* CurrentRotation;
    FVector* Velocity;

    UObject* CurrentPawn;
    UObject* CurrentController;

    UObject* CurrentPlayerState;

public:
    std::string PlayerID;


    FortPlayer(std::string ID, UObject* CurrentPawn, UObject* CurrentController, UObject* CurrentPlayerState)
    {
        this->PlayerID = ID;
        this->CurrentPawn = CurrentPawn;
        this->CurrentController = CurrentController;
        this->CurrentPlayerState = CurrentPlayerState;
    }

    FVector* GetCurrentLocation()
    {
        return this->CurrentLocation;
    }

    FRotator* GetCurrentRotation()
    {
        return this->CurrentRotation;
    }

    void SetCurrentLocationAndRotation(FVector* Location, FRotator* Rotation)
    {
        this->CurrentLocation = Location;
        this->CurrentRotation = Rotation;
    }

    UObject* GetPawn()
    {
        return this->CurrentPawn;
    }

    UObject* GetPlayerController()
    {
        return this->CurrentController;
    }

    UObject* GetPlayerState()
    {
        return this->CurrentPlayerState;
    }
};

std::vector<FortPlayer*> PlayerList;