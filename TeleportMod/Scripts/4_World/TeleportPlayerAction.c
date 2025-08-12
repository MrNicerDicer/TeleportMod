autoptr ref TStringArray g_activatedTeleportMods = {};
autoptr ref TStringArray g_cooldownTeleportMods = {};
autoptr ref TStringArray g_preLoadedAreas = {};

static ref TStringArray GetActivatedTeleportMods()
{
    if (!g_activatedTeleportMods) g_activatedTeleportMods = {};
    return g_activatedTeleportMods;
}

static ref TStringArray GetCooldownTeleportMods()
{
    if (!g_cooldownTeleportMods) g_cooldownTeleportMods = {};
    return g_cooldownTeleportMods;
}

static ref TStringArray GetPreLoadedAreas()
{
    if (!g_preLoadedAreas) g_preLoadedAreas = {};
    return g_preLoadedAreas;
}

static int SearchActivatedTeleportMod(string teleportName) {
    foreach (auto key, auto name: g_activatedTeleportMods)
    {
        if (name == teleportName) {
            return key;
        }
    }
    return -1;
}

static int SearchCooldownTeleportMod(string teleportName) {
    foreach (auto key, auto name: g_cooldownTeleportMods)
    {
        if (name == teleportName) {
            return key;
        }
    }
    return -1;
}

static int SearchPreLoadedArea(string areaKey) {
    foreach (auto key, auto name: g_preLoadedAreas)
    {
        if (name == areaKey) {
            return key;
        }
    }
    return -1;
}

static void InsertActivatedTeleportMod(string teleportName) {
    if (SearchActivatedTeleportMod(teleportName) < 0) {
        g_activatedTeleportMods.Insert(teleportName);
    }
}

static void InsertCooldownTeleportMod(string teleportName) {
    if (SearchCooldownTeleportMod(teleportName) < 0) {
        g_cooldownTeleportMods.Insert(teleportName);
    }
}

static void InsertPreLoadedArea(string areaKey) {
    if (SearchPreLoadedArea(areaKey) < 0) {
        g_preLoadedAreas.Insert(areaKey);
    }
}

static void RemoveActivatedTeleportMod(string teleportName) {
    int index = SearchActivatedTeleportMod(teleportName);
    if (index >= 0) {
        g_activatedTeleportMods.Remove(index);
    }
}

static void RemoveCooldownTeleportMod(string teleportName) {
    int index = SearchCooldownTeleportMod(teleportName);
    if (index >= 0) {
        g_cooldownTeleportMods.Remove(index);
    }
}

static void RemovePreLoadedArea(string areaKey) {
    int index = SearchPreLoadedArea(areaKey);
    if (index >= 0) {
        g_preLoadedAreas.Remove(index);
    }
}

static void StartTeleportCooldown(string teleportName, int cooldownSeconds) {
    InsertCooldownTeleportMod(teleportName);
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RemoveCooldownTeleportMod, cooldownSeconds * 1000, false, teleportName);
}

static void OnTeleportActiveTimeExpired(string teleportName, int cooldownSeconds) {
    RemoveActivatedTeleportMod(teleportName);
    
    // Start cooldown if configured
    if (cooldownSeconds > 0) {
        StartTeleportCooldown(teleportName, cooldownSeconds);
    }
}

static void PreLoadDestinationArea(vector destination, string teleportName) {
    string areaKey = teleportName + "_preload";
    
    if (SearchPreLoadedArea(areaKey) < 0) {
        // Force load area around destination using existing DayZ methods
        array<Object> preloadObjects = new array<Object>;
        GetGame().GetObjectsAtPosition(destination, 100.0, preloadObjects, null);
        
        // Additional method to ensure terrain loading
        float surfaceY = GetGame().SurfaceY(destination[0], destination[2]);
        
        InsertPreLoadedArea(areaKey);
        
        // Remove pre-load marker after 5 minutes to allow re-loading if needed
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RemovePreLoadedArea, 300000, false, areaKey);
        
        Print("Pre-loading destination area for teleport: " + teleportName + " at position: " + destination.ToString() + " (Found " + preloadObjects.Count() + " objects)");
    }
}

static vector GetSafeDestinationPosition(vector originalPos, bool isUnderground, float groundOffset) {
    if (isUnderground) {
        // For underground destinations, trust the configured coordinates
        return originalPos;
    } else {
        // For surface destinations, ensure we're properly positioned above ground
        float groundLevel = GetGame().SurfaceY(originalPos[0], originalPos[2]);
        vector safePos = Vector(originalPos[0], groundLevel + groundOffset, originalPos[2]);
        
        // Validate the position isn't inside an object
        array<Object> nearbyObjects = new array<Object>;
        GetGame().GetObjectsAtPosition(safePos, 2.0, nearbyObjects, null);
        
        // If there are buildings nearby, use original Y coordinate (might be on a building)
        foreach (Object obj : nearbyObjects) {
            if (obj.IsBuilding()) {
                return originalPos;
            }
        }
        
        return safePos;
    }
}

class TeleportModPlayerCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(3);
    }
}

class TeleportModPlayer: ActionContinuousBase
{
    protected ref LO_TeleportConfig m_TeleportConfig;  // Reference to teleport configuration object

    void TeleportModPlayer()
    {
        m_CallbackClass = TeleportModPlayerCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_RESTRAINTARGET; //CMD_ACTIONFB_INTERACT;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
        m_FullBody = true;
        m_Text = "#STR_Controls_Interact";
        LoadTeleportConfig();  // Load teleport configuration
    }

    protected void LoadTeleportConfig()
    {
        m_TeleportConfig = JsonConfigManager.LoadTeleportConfig();  // Load configuration using JsonConfigManager
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTBase; //CCTCursorParent(UAMaxDistances.DEFAULT);
        m_ConditionItem = new CCINone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        return true;
    }
    
    override void OnStartServer(ActionData action_data ) {}

    override void OnFinishProgressServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        ActionTarget target = action_data.m_Target;
        Object targetObject = target.GetObject();
        LO_TeleportEntry entry = GetTeleportEntry(targetObject);

        // Teleportation logic
        if (entry) {
            bool canTeleport = false;
            ItemBase heldItem = null;
            
            if (entry.EnableTeleport) {
                if ("TeleportModPoint" == targetObject.GetType() && vector.Distance(targetObject.GetPosition(), player.GetPosition()) <= entry.CheckRadius)
                {
                    // Check if teleport is in cooldown
                    if (SearchCooldownTeleportMod(entry.TeleportName) >= 0) {
                        player.MessageImportant(entry.CooldownMessage);
                        return;
                    }
                    
                    // Check if teleport is already activated
                    bool isActivated = SearchActivatedTeleportMod(entry.TeleportName) >= 0;
                    
                    if (!isActivated) {
                        // Teleport not activated - check for required item
                        if (entry.RequiredItem != "") {
                            heldItem = player.GetItemInHands();
                            if (heldItem && heldItem.GetType() == entry.RequiredItem)
                            {
                                int healthLevel = heldItem.GetHealthLevel();
                                if (healthLevel < GameConstants.STATE_RUINED) {
                                    // Apply damage if configured
                                    if (entry.RequiredItemDamagePercent > 0) {
                                        int currentHealth = heldItem.GetHealth();
                                        int maxHealth = heldItem.GetMaxHealth();
                                        heldItem.SetHealth(currentHealth - Math.Floor(maxHealth / 100 * entry.RequiredItemDamagePercent));
                                    }
                                    
                                    canTeleport = true;
                                    
                                    // Activate teleport for configured time
                                    if (entry.TeleportActiveTimeSeconds > 0) {
                                        InsertActivatedTeleportMod(entry.TeleportName);
                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnTeleportActiveTimeExpired, entry.TeleportActiveTimeSeconds * 1000, false, entry.TeleportName, entry.TeleportCooldownTimeSeconds);
                                    }
                                } else {
                                    player.MessageImportant("The " + entry.RequiredItem + " is too damaged to use.");
                                    return;
                                }
                            } else {
                                player.MessageImportant(entry.MissingItemMessage);
                                return;
                            }
                        } else {
                            // No item required
                            canTeleport = true;
                        }
                    } else {
                        // Teleport is activated - can be used without item
                        canTeleport = true;
                    }
                }

                if (canTeleport) {
                    // Consume/destroy the required item if configured (only on first activation)
                    if (entry.ConsumeRequiredItem && heldItem && entry.RequiredItem != "" && !isActivated) {
                        // Remove item from player's hands and destroy it
                        GetGame().ObjectDelete(heldItem);
                        Print("Consumed required item: " + entry.RequiredItem);
                    }
                    
                    // Check if black screen effect is enabled
                    if (entry.EnableBlackScreenEffect) {
                        // Show black screen with configured duration and message
                        ShowBlackScreen(player, entry.BlackScreenDurationSeconds, entry.BlackScreenMessage, entry.BlackScreenSoundFile);
                        
                        // Execute safe teleport after black screen duration (replacing the normal delay)
                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ExecuteSafeTeleportAfterBlackScreen, entry.BlackScreenDurationSeconds * 1000, false, player, entry);
                    } else {
                        // Execute safe teleport with normal delay (your original working logic)
                        ExecuteSafeTeleport(player, entry);
                    }
                }
            }
        }
    }

    void ShowBlackScreen(PlayerBase player, int duration, string message, string soundFile)
    {
        Print("=== INITIATING BLACK SCREEN EFFECT ===");
        Print("Duration: " + duration + ", Message: " + message);
        
        // Send RPC to client for Black Screen Effect
        if (GetGame().IsServer())
        {
            // Create parameter object for RPC
            Param3<int, string, string> params = new Param3<int, string, string>(duration, message, soundFile);
            
            // Send RPC to the client
            if (player && player.GetIdentity())
            {
                GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE + 100, params, true, player.GetIdentity());
                Print("RPC sent to client for black screen effect");
            }
            else
            {
                Print("ERROR: Could not send RPC - player or identity is null");
            }
        }
        
        // Server-side message
        player.MessageImportant(message);
        
        // Play sound on server
        if (soundFile != "" && FileExist(soundFile))
        {
            SEffectManager.PlaySound(soundFile, player.GetPosition());
        }
    }
    
    void ShowLoadingProgress(PlayerBase player, string progressMessage)
    {
        if (player && player.IsAlive()) {
            player.MessageImportant(progressMessage);
        }
    }
    
    void CloseBlackScreen(BlackScreenUI blackScreen)
    {
        if (blackScreen)
        {
            GetGame().GetUIManager().HideScriptedMenu(blackScreen);
            Print("Black screen teleport completed");
        }
    }
    
    void ExecuteSafeTeleportAfterBlackScreen(PlayerBase player, LO_TeleportEntry entry)
    {
        if (player && player.IsAlive()) {
            // Pre-load destination area if not already loaded
            PreLoadDestinationArea(entry.TeleportPosition, entry.TeleportName);
            
            // Calculate safe destination position
            vector safeDestination = GetSafeDestinationPosition(entry.TeleportPosition, entry.IsUndergroundDestination, entry.SafeGroundOffset);
            
            // Perform immediate teleport (no additional delay since black screen already provided the loading time)
            PerformTeleport(player, safeDestination, entry);
        }
    }

    void ExecuteSafeTeleport(PlayerBase player, LO_TeleportEntry entry) {
        // Pre-load destination area if not already loaded
        PreLoadDestinationArea(entry.TeleportPosition, entry.TeleportName);
        
        // Calculate safe destination position
        vector safeDestination = GetSafeDestinationPosition(entry.TeleportPosition, entry.IsUndergroundDestination, entry.SafeGroundOffset);
        
        // Play teleport sound at origin if enabled
        if (entry.EnableTeleportSound && entry.TeleportSoundFile != "") {
            PlayTeleportSound(player.GetPosition(), entry.TeleportSoundFile, entry.TeleportSoundRadius, entry.TeleportSoundVolume);
        }
        
        // Show teleporting message
        player.MessageImportant("Teleporting in " + entry.TeleportDelaySeconds + " seconds...");
        
        // Execute delayed teleport
        int delayMs = entry.TeleportDelaySeconds * 1000;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PerformTeleport, delayMs, false, player, safeDestination, entry);
    }
    
    void PerformTeleport(PlayerBase player, vector destination, LO_TeleportEntry entry) {
        if (player && player.IsAlive()) {
            // Final safety check - ensure area is loaded
            array<Object> testObjects = new array<Object>;
            GetGame().GetObjectsAtPosition(destination, 10.0, testObjects, null);
            
            // Perform teleport
            player.SetPosition(destination);
            
            // Play sound at destination if enabled
            if (entry.EnableTeleportSound && entry.TeleportSoundFile != "") {
                PlayTeleportSound(destination, entry.TeleportSoundFile, entry.TeleportSoundRadius, entry.TeleportSoundVolume);
            }
            
            // Success message
            player.MessageImportant("Teleportation complete!");
            
            Print("Safe teleport executed for player to: " + destination.ToString() + " (Underground: " + entry.IsUndergroundDestination + ")");
        }
    }

    void PlayTeleportSound(vector position, string soundFile, float radius, float volume)
    {
        // Method that works in DayZ 1.28 - using SEffectManager
        EffectSound effect = SEffectManager.PlaySound(soundFile, position);
        
        if (effect) {
            Print("Playing teleport sound: " + soundFile + " at position " + position.ToString());
        } else {
            Print("Failed to play teleport sound: " + soundFile);
        }
    }

    LO_TeleportEntry GetTeleportEntry(Object targetObject) {
        foreach (ref LO_TeleportEntry entry : m_TeleportConfig.TeleportEntries)
        {
            if (vector.Distance(targetObject.GetPosition(), entry.ObjectCoordinates) < 3) {
                return entry;
            }
        }
        return null;
    }
    
    override typename GetInputType()
    {
        return ContinuousInteractActionInput;
    }

    override bool HasProgress()
    {
        return true;
    }
    
    override bool HasTarget()
    {
        return true;
    }
    
    override bool CanBeUsedInVehicle()
    {
        return false;
    }
}

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        
        // Register new action
        actions.Insert(TeleportModPlayer);
    }
}

class TeleportModPoint extends ItemBase
{
    protected ref LO_TeleportConfig m_TeleportConfig;
    protected ref LO_TeleportEntry m_AssociatedEntry;
    
    void TeleportModPoint()
    {
        SetSynchDirty();
        SetLifetime(600); // Object lifetime if needed
        LoadTeleportConfig();
        
        // Start proximity checking for pre-loading
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckPlayerProximityForPreLoad, 2000, true);
    }
    
    protected void LoadTeleportConfig()
    {
        m_TeleportConfig = JsonConfigManager.LoadTeleportConfig();
        
        // Find associated teleport entry
        foreach (ref LO_TeleportEntry entry : m_TeleportConfig.TeleportEntries)
        {
            if (vector.Distance(GetPosition(), entry.ObjectCoordinates) < 3) {
                m_AssociatedEntry = entry;
                break;
            }
        }
    }
    
    void CheckPlayerProximityForPreLoad()
    {
        if (!m_AssociatedEntry || m_AssociatedEntry.PreLoadRadius <= 0) return;
        
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        
        foreach (Man player : players)
        {
            if (vector.Distance(GetPosition(), player.GetPosition()) <= m_AssociatedEntry.PreLoadRadius)
            {
                // Player is within pre-load radius - start pre-loading destination
                PreLoadDestinationArea(m_AssociatedEntry.TeleportPosition, m_AssociatedEntry.TeleportName);
                break;
            }
        }
    }
    
    override void SetActions()
    {
        super.SetActions();        
        AddAction(TeleportModPlayer);
    }

    override bool IsHealthVisible()
    {
        return false;
    }
    
    override bool IsBuilding()
    {
        return true;
    }
    
    override bool IsInventoryItem()
    {
        return false;
    }
    
    override bool CanPutInCargo( EntityAI parent )
    {
        if( !super.CanPutInCargo(parent) ) {return false;}
        return false;
    }

    override bool CanPutIntoHands( EntityAI player )
    {
        if( !super.CanPutIntoHands( parent ) )
        {
            return false;
        }
        return false;
    }
}