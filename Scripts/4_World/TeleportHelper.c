class TeleportHelper
{
    // Function for creating teleport interaction object
    static void CreateTeleportInteractionObject(string objectType, vector objectCoordinates, vector objectOrientation)
    {
        // Create visible object
        Object obj = GetGame().CreateObjectEx(objectType, objectCoordinates, ECE_NOSURFACEALIGN);
        // Object obj = GetGame().CreateObject(objectType, vector.Zero);
        if (obj)
        {
            // Apply orientation to object
            obj.SetOrientation(objectOrientation);
            Print("Object " + objectType + " created at position: " + objectCoordinates.ToString());

            // Spawn invisible object for interaction
            Object interactionObj = GetGame().CreateObjectEx("TeleportModPoint", objectCoordinates, ECE_NOSURFACEALIGN, RF_IGNORE);
            if (interactionObj)
            {
                Print("Object TeleportModPoint created at position: " + objectCoordinates.ToString() + " " + interactionObj);
            } else {
                Print("Failed to create object TeleportModPoint at position: " + objectCoordinates.ToString());
            }
        }
        else
        {
            Print("Failed to create object " + objectType + " at position: " + objectCoordinates.ToString());
        }
    }
}