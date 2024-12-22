using UnrealBuildTool;

public class RetroLib : ModuleRules
{
    public RetroLib(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicIncludePaths.Add("$(ModuleDir)/include");
        PublicDefinitions.Add("RETROLIB_WITH_MODULES=0");
    }
}