class BlackScreenUI extends UIScriptedMenu
{
    protected TextWidget m_LoadingText;
    protected TextWidget m_DotsAnimation;
    protected Widget m_BlackPanel;
    protected ref Timer m_AnimationTimer;
    protected int m_DotCount;
    protected string m_OriginalMessage;
    
    override Widget Init()
    {
        Print("=== DEBUG: BlackScreenUI Init called ===");
        
        Print("DEBUG: Attempting to load layout: TeleportMod/GUI/BlackScreen.layout");
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("TeleportMod/GUI/BlackScreen.layout");
        
        if (!layoutRoot) {
            Print("ERROR: Failed to load BlackScreen.layout!");
            return null;
        }
        
        Print("DEBUG: Layout loaded successfully");
        
        Print("DEBUG: Finding widgets...");
        m_LoadingText = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingText"));
        m_DotsAnimation = TextWidget.Cast(layoutRoot.FindAnyWidget("DotsAnimation"));
        m_BlackPanel = layoutRoot.FindAnyWidget("BlackPanel");
        
        if (!m_LoadingText) {
            Print("ERROR: LoadingText widget not found!");
        } else {
            Print("DEBUG: LoadingText widget found");
        }
        
        if (!m_DotsAnimation) {
            Print("ERROR: DotsAnimation widget not found!");
        } else {
            Print("DEBUG: DotsAnimation widget found");
        }
        
        if (!m_BlackPanel) {
            Print("ERROR: BlackPanel widget not found!");
        } else {
            Print("DEBUG: BlackPanel widget found");
        }
        
        m_DotCount = 0;
        
        // Start dots animation
        m_AnimationTimer = new Timer();
        m_AnimationTimer.Run(0.5, this, "UpdateDotsAnimation", null, true);
        
        // Make sure it's on top of everything
        layoutRoot.SetSort(99999);
        
        Print("DEBUG: BlackScreenUI Init completed successfully");
        return layoutRoot;
    }
    
    void SetMessage(string message)
    {
        m_OriginalMessage = message;
        if (m_LoadingText)
            m_LoadingText.SetText(message);
    }
    
    void UpdateDotsAnimation()
    {
        if (!m_DotsAnimation) return;
        
        m_DotCount++;
        if (m_DotCount > 3) m_DotCount = 0;
        
        string dots = "";
        for (int i = 0; i < m_DotCount; i++)
        {
            dots += ".";
        }
        
        m_DotsAnimation.SetText(dots);
    }
    
    override void OnShow()
    {
        super.OnShow();
        SetFocus(layoutRoot);
        
        // Hide UI cursor during black screen
        GetGame().GetUIManager().ShowUICursor(false);
    }
    
    override void OnHide()
    {
        super.OnHide();
        
        // Stop animation timer
        if (m_AnimationTimer)
        {
            m_AnimationTimer.Stop();
            delete m_AnimationTimer;
        }
    }
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        // Prevent any clicks during black screen
        return true;
    }
    
    override bool OnKeyPress(Widget w, int x, int y, int key)
    {
        // Prevent any key presses during black screen
        return true;
    }
    
    void ForceClose()
    {
        Close();
    }
}