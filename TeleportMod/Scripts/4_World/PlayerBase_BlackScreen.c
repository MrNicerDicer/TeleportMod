modded class PlayerBase
{
    protected ref Timer m_BlackScreenTimer;
    protected float m_BlackScreenIntensity = 0.0;
    protected bool m_IsBlackScreenActive = false;
    protected int m_BlackScreenDuration = 0;
    protected float m_BlackScreenStartTime = 0;
    protected float m_OriginalEyeAccom = 1.0;
    
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
        
        // Custom RPC for Black Screen
        if (rpc_type == ERPCs.RPC_USER_ACTION_MESSAGE + 100)
        {
            Param3<int, string, string> data;
            if (ctx.Read(data))
            {
                int duration = data.param1;
                string message = data.param2;
                string soundFile = data.param3;
                
                if (GetGame().GetPlayer() == this)
                {
                    StartClientBlackScreen(duration, message, soundFile);
                }
            }
        }
    }
    
    void StartClientBlackScreen(int duration, string message, string soundFile)
    {
        Print("[CLIENT] Starting black screen effect");
        
        m_IsBlackScreenActive = true;
        m_BlackScreenDuration = duration;
        m_BlackScreenStartTime = GetGame().GetTime();
        
        // Show message in red color using different message types
        if (GetGame().GetPlayer() == this)
        {
            // Option 1: Use MessageImportant for red/orange color
            MessageImportant(message);
            
            // Option 2: Alternative - Use Message with color parameter
            // Message(message, "colorImportant");
            
            // Option 3: Alternative - Use MessageAction for yellow/orange
            // MessageAction(message);
        }
        
        // Store original eye accommodation
        if (GetGame() && GetGame().GetWorld())
        {
            m_OriginalEyeAccom = GetGame().GetWorld().GetEyeAccom();
        }
        
        // Start fade-in timer
        if (m_BlackScreenTimer)
            m_BlackScreenTimer.Stop();
            
        m_BlackScreenTimer = new Timer();
        m_BlackScreenTimer.Run(0.05, this, "UpdateBlackScreen", null, true);
        
        // Play sound if available
        if (soundFile != "")
        {
            SEffectManager.PlaySound(soundFile, GetPosition());
        }
    }
    
    void UpdateBlackScreen()
    {
        if (!m_IsBlackScreenActive)
        {
            if (m_BlackScreenTimer)
            {
                m_BlackScreenTimer.Stop();
                m_BlackScreenTimer = null;
            }
            return;
        }
        
        float currentTime = GetGame().GetTime();
        float elapsedTime = (currentTime - m_BlackScreenStartTime) / 1000.0; // Convert to seconds
        
        if (elapsedTime < 1.0)
        {
            // Fade in (first second)
            m_BlackScreenIntensity = Math.Lerp(0.0, 1.0, elapsedTime);
        }
        else if (elapsedTime < m_BlackScreenDuration - 1.0)
        {
            // Full black screen
            m_BlackScreenIntensity = 1.0;
        }
        else if (elapsedTime < m_BlackScreenDuration)
        {
            // Fade out (last second)
            float fadeOutProgress = (elapsedTime - (m_BlackScreenDuration - 1.0));
            m_BlackScreenIntensity = Math.Lerp(1.0, 0.0, fadeOutProgress);
        }
        else
        {
            // End
            m_BlackScreenIntensity = 0.0;
            m_IsBlackScreenActive = false;
            ResetPostProcessEffects();
            return;
        }
        
        // Apply black screen effect
        ApplyBlackScreenEffect(m_BlackScreenIntensity);
    }
    
    void ApplyBlackScreenEffect(float intensity)
    {
        // Simple approach using eye accommodation only
        if (GetGame() && GetGame().GetWorld())
        {
            // Darken screen by reducing eye accommodation
            // Lower values = darker screen
            float targetAccom = Math.Lerp(m_OriginalEyeAccom, 0.01, intensity);
            GetGame().GetWorld().SetEyeAccom(targetAccom);
            
            // Additional approach: Set camera post-process if player is local
            if (GetGame().GetPlayer() == this)
            {
                // Use SetTargetViewDistance for fog-like effect
                if (intensity > 0.5)
                {
                    GetGame().GetWorld().SetViewDistance(Math.Lerp(3000, 10, intensity));
                }
            }
        }
    }
    
    void ResetPostProcessEffects()
    {
        // Reset all effects
        if (GetGame() && GetGame().GetWorld())
        {
            GetGame().GetWorld().SetEyeAccom(m_OriginalEyeAccom);
            GetGame().GetWorld().SetViewDistance(3000); // Reset view distance
        }
        
        if (m_BlackScreenTimer)
        {
            m_BlackScreenTimer.Stop();
            m_BlackScreenTimer = null;
        }
    }
}