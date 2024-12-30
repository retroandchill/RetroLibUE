using UnrealBuildTool;

public class RetroLib : ModuleRules
{
    public RetroLib(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicIncludePaths.Add("$(ModuleDir)/include");
        PublicDefinitions.Add("RETROLIB_WITH_MODULES=0");

        PublicDefinitions.Add(Target.bEnableCppCoroutinesForEvaluation
            ? "RETROLIB_WITH_COROUTINES=1"
            : "RETROLIB_WITH_COROUTINES=0");
    }
}