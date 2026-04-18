// NPCDialogueComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "NPCDialogueComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueLineReady, const FString&, DialogueLine);

/**
 * NPCDialogueComponent
 *
 * Drives LLM-generated villain dialogue using the GitHub Models API.
 * Features:
 *   - Sends fight context (combo count, player health, environment) to the model
 *   - Streams responses and fires OnDialogueLineReady when a sentence is complete
 *   - Escalating personality prompts as the fight intensifies
 *   - Omni-Man style mission briefing narration
 *
 * Setup:
 *   Set GITHUB_TOKEN environment variable or configure ApiKey in Project Settings.
 *   Model endpoint: https://models.inference.ai.azure.com/chat/completions
 */
UCLASS(ClassGroup=(VilNature), meta=(BlueprintSpawnableComponent))
class VILNATURE_API UNPCDialogueComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNPCDialogueComponent();

    // ── API ───────────────────────────────────────────────────────────────────

    /**
     * Request a contextual taunt line from the LLM.
     * @param ComboCount    Current player combo count (higher = more panicked villain)
     * @param PlayerHealth  Player health fraction 0-1
     * @param Context       Free-text scene context, e.g. "player just destroyed a building"
     */
    UFUNCTION(BlueprintCallable, Category="NPC|Dialogue")
    void RequestTaunt(int32 ComboCount, float PlayerHealth, const FString& Context);

    /**
     * Request a mission briefing from Omni-Man.
     * @param MissionDescription  High-level description of the mission objective.
     */
    UFUNCTION(BlueprintCallable, Category="NPC|Dialogue")
    void RequestMissionBriefing(const FString& MissionDescription);

    // ── Events ────────────────────────────────────────────────────────────────

    /** Fired when the model response contains a complete sentence. */
    UPROPERTY(BlueprintAssignable, Category="NPC|Dialogue")
    FOnDialogueLineReady OnDialogueLineReady;

    // ── Configuration ─────────────────────────────────────────────────────────

    /** GitHub Models endpoint URL. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC|LLM")
    FString ModelEndpoint = TEXT("https://models.inference.ai.azure.com/chat/completions");

    /** Model identifier (e.g. gpt-4o, phi-3-medium). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC|LLM")
    FString ModelId = TEXT("gpt-4o");

    /** GitHub API token (read from env var GITHUB_TOKEN at runtime if empty). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC|LLM",
              meta=(ToolTip="Leave empty to read GITHUB_TOKEN env var at runtime"))
    FString ApiKey;

    /** Max tokens per response (keep short for snappy in-fight taunts). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC|LLM",
              meta=(ClampMin="10", ClampMax="256"))
    int32 MaxTokens = 60;

    /** Villain persona system prompt prefix. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC|LLM",
              meta=(MultiLine=true))
    FString VillainPersona = TEXT(
        "You are a powerful supervillain fighting the hero Invincible. "
        "You speak with arrogance and escalating desperation as the fight turns against you. "
        "Keep responses to ONE punchy sentence suitable for voiced dialogue."
    );

protected:
    virtual void BeginPlay() override;

private:
    FString GetEffectiveApiKey() const;

    void SendChatRequest(const FString& SystemPrompt, const FString& UserMessage,
                         TFunction<void(const FString&)> OnComplete);

    void OnHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response,
                        bool bWasSuccessful,
                        TFunction<void(const FString&)> OnComplete);

    FString BuildTauntUserMessage(int32 ComboCount, float PlayerHealth,
                                  const FString& Context) const;
};
