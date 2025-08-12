class LO_TeleportConfig
{
    ref array<ref LO_TeleportEntry> TeleportEntries;

    void LO_TeleportConfig()
    {
        TeleportEntries = new array<ref LO_TeleportEntry>();
    }
}

class LO_TeleportEntry
{
    bool EnableTeleport;                    // Whether teleport is enabled
    string TeleportName;                    // Teleport name, unique for each teleport
    string ObjectType;                      // Object type (e.g., "Land_Barn_Wooden")
    vector ObjectCoordinates;               // Object coordinates in world
    vector ObjectOrientation;               // Object orientation (angles)
    vector TeleportPosition;                // Position for player teleportation
    float CheckRadius;                      // Check radius
    string RequiredItem;                    // Item that must be in player's hands
    float RequiredItemDamagePercent;        // Damage to item in hands when used, in percent. 0 - no damage, 100 - destroyed, 25 - lasts 4 uses etc.
    bool ConsumeRequiredItem;               // Whether to consume/destroy the required item after use
    int TeleportActiveTimeSeconds;          // Time in seconds that teleport will be active after first use and won't require item (0 - item required for each use)
    string MissingItemMessage;              // Message about missing item
    
    // Cooldown configuration
    int TeleportCooldownTimeSeconds;        // Cooldown time in seconds after active time expires
    string CooldownMessage;                 // Message displayed during cooldown period
    
    // Safe teleport configuration
    float PreLoadRadius;                    // Radius to start pre-loading destination (0 = disabled)
    int TeleportDelaySeconds;               // Delay before teleport execution (for loading)
    bool IsUndergroundDestination;          // Whether destination is underground (bunker/cave)
    float SafeGroundOffset;                 // Height offset above ground for surface teleports
    
    // Black screen teleport effect
    bool EnableBlackScreenEffect;           // Enable black screen during teleport
    int BlackScreenDurationSeconds;         // Duration of black screen (10-20+ seconds)
    string BlackScreenMessage;              // Message displayed during black screen
    string BlackScreenSoundFile;            // Sound file to play during black screen (optional)
    
    // Sound configuration
    bool EnableTeleportSound;               // Whether to play teleport sound
    string TeleportSoundFile;               // Path to the sound file (OGG format)
    float TeleportSoundRadius;              // Radius in meters where the sound can be heard
    float TeleportSoundVolume;              // Volume of the sound (0.0 - 1.0)

    void LO_TeleportEntry()
    {
        EnableTeleport = true;
        TeleportName = "";
        CheckRadius = 5.0;
        
        // Initialize coordinates using vector
        ObjectCoordinates = "0 0 0";          
        ObjectOrientation = "0 0 0";          
        TeleportPosition = "0 0 0";           

        RequiredItem = "";
        RequiredItemDamagePercent = 0;
        ConsumeRequiredItem = false;        // Default: don't consume item
        TeleportActiveTimeSeconds = 0;
        MissingItemMessage = "";
        
        // Cooldown defaults
        TeleportCooldownTimeSeconds = 0;    // Default: no cooldown (0 disables feature)
        CooldownMessage = "This teleport is currently on cooldown. Please wait before using it again.";
        
        // Safe teleport defaults
        PreLoadRadius = 15.0;               // Start pre-loading when player is 15m away
        TeleportDelaySeconds = 3;           // 3 second delay for loading
        IsUndergroundDestination = false;   // Default: surface destination
        SafeGroundOffset = 1.5;             // 1.5m above ground for surface teleports
        
        // Black screen defaults
        EnableBlackScreenEffect = false;    // Default: no black screen
        BlackScreenDurationSeconds = 15;    // 15 seconds default duration
        BlackScreenMessage = "Teleporting... Please wait.";
        BlackScreenSoundFile = "";          // No sound by default
        
        // Sound defaults
        EnableTeleportSound = false;
        TeleportSoundFile = "";
        TeleportSoundRadius = 50.0;         // Default 50 meter radius
        TeleportSoundVolume = 1.0;          // Default 100% volume
    }
}