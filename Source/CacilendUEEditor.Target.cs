using UnrealBuildTool;
using System.Collections.Generic;

public class CacilendUEEditorTarget : TargetRules
{
    public CacilendUEEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("CacilendUE");
    }
}
