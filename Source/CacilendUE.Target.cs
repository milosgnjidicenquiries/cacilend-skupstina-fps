using UnrealBuildTool;
using System.Collections.Generic;

public class CacilendUETarget : TargetRules
{
    public CacilendUETarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("CacilendUE");
    }
}
