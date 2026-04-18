// NPCDialogueComponent.cpp
#include "AI/NPCDialogueComponent.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/EnvironmentVariables.h"

UNPCDialogueComponent::UNPCDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UNPCDialogueComponent::BeginPlay()
{
    Super::BeginPlay();
}

// ─── Public API ───────────────────────────────────────────────────────────────

void UNPCDialogueComponent::RequestTaunt(int32 ComboCount, float PlayerHealth,
                                          const FString& Context)
{
    const FString UserMsg = BuildTauntUserMessage(ComboCount, PlayerHealth, Context);

    SendChatRequest(VillainPersona, UserMsg, [this](const FString& Line)
    {
        OnDialogueLineReady.Broadcast(Line);
    });
}

void UNPCDialogueComponent::RequestMissionBriefing(const FString& MissionDescription)
{
    const FString SystemPrompt = TEXT(
        "You are Omni-Man delivering a mission briefing to Invincible. "
        "Be stern, proud, and slightly ominous. One paragraph maximum."
    );
    const FString UserMsg = FString::Printf(
        TEXT("Mission: %s  — Deliver the briefing now."), *MissionDescription);

    SendChatRequest(SystemPrompt, UserMsg, [this](const FString& Line)
    {
        OnDialogueLineReady.Broadcast(Line);
    });
}

// ─── Private ──────────────────────────────────────────────────────────────────

FString UNPCDialogueComponent::GetEffectiveApiKey() const
{
    if (!ApiKey.IsEmpty()) return ApiKey;
    // Fall back to environment variable (safe for CI / server deployments)
    FString EnvToken = FPlatformMisc::GetEnvironmentVariable(TEXT("GITHUB_TOKEN"));
    return EnvToken;
}

void UNPCDialogueComponent::SendChatRequest(const FString& SystemPrompt,
                                             const FString& UserMessage,
                                             TFunction<void(const FString&)> OnComplete)
{
    const FString Token = GetEffectiveApiKey();
    if (Token.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
               TEXT("NPCDialogueComponent: No API key found. "
                    "Set GITHUB_TOKEN env var or fill ApiKey property."));
        return;
    }

    // Build JSON body
    TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("model"), ModelId);
    Root->SetNumberField(TEXT("max_tokens"), MaxTokens);
    Root->SetBoolField(TEXT("stream"), false);

    TArray<TSharedPtr<FJsonValue>> Messages;

    auto MakeMsg = [](const FString& Role, const FString& Content) -> TSharedPtr<FJsonValue>
    {
        TSharedPtr<FJsonObject> Msg = MakeShared<FJsonObject>();
        Msg->SetStringField(TEXT("role"), Role);
        Msg->SetStringField(TEXT("content"), Content);
        return MakeShared<FJsonValueObject>(Msg);
    };

    Messages.Add(MakeMsg(TEXT("system"), SystemPrompt));
    Messages.Add(MakeMsg(TEXT("user"),   UserMessage));
    Root->SetArrayField(TEXT("messages"), Messages);

    FString BodyString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
    FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

    // Fire HTTP request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpReq =
        FHttpModule::Get().CreateRequest();
    HttpReq->SetURL(ModelEndpoint);
    HttpReq->SetVerb(TEXT("POST"));
    HttpReq->SetHeader(TEXT("Content-Type"),  TEXT("application/json"));
    HttpReq->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
    HttpReq->SetContentAsString(BodyString);

    HttpReq->OnProcessRequestComplete().BindUObject(
        this,
        &UNPCDialogueComponent::OnHttpResponse,
        OnComplete
    );

    HttpReq->ProcessRequest();
}

void UNPCDialogueComponent::OnHttpResponse(FHttpRequestPtr /*Request*/,
                                            FHttpResponsePtr Response,
                                            bool bWasSuccessful,
                                            TFunction<void(const FString&)> OnComplete)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("NPCDialogueComponent: HTTP request failed."));
        return;
    }

    const FString ResponseBody = Response->GetContentAsString();

    TSharedPtr<FJsonObject> JsonObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
    if (!FJsonSerializer::Deserialize(Reader, JsonObj) || !JsonObj.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("NPCDialogueComponent: Failed to parse JSON response."));
        return;
    }

    // Extract choices[0].message.content
    const TArray<TSharedPtr<FJsonValue>>* Choices;
    if (!JsonObj->TryGetArrayField(TEXT("choices"), Choices) || Choices->IsEmpty())
        return;

    const TSharedPtr<FJsonObject>* FirstChoice;
    if (!(*Choices)[0]->TryGetObject(FirstChoice)) return;

    const TSharedPtr<FJsonObject>* Message;
    if (!(*FirstChoice)->TryGetObjectField(TEXT("message"), Message)) return;

    FString Content;
    if (!(*Message)->TryGetStringField(TEXT("content"), Content)) return;

    Content.TrimStartAndEndInline();
    if (!Content.IsEmpty())
    {
        OnComplete(Content);
    }
}

FString UNPCDialogueComponent::BuildTauntUserMessage(int32 ComboCount, float PlayerHealth,
                                                       const FString& Context) const
{
    // Intensity level drives the villain's escalating desperation
    const FString Intensity = (ComboCount > 30) ? TEXT("desperate and shocked")
                            : (ComboCount > 15) ? TEXT("frustrated and aggressive")
                                                : TEXT("arrogant and dismissive");

    return FString::Printf(
        TEXT("Current fight state — combo: %d, player health: %.0f%%, context: %s. "
             "Tone: %s. Give ONE taunt line."),
        ComboCount,
        PlayerHealth * 100.0f,
        *Context,
        *Intensity
    );
}
