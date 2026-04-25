#include "NexusActor.h"
#include "NexusGuardianGameMode.h"
#include "Components/WidgetComponent.h"
ANexusActor::ANexusActor()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComp;
    CurrentHP = MaxHP;

    Tags.Add(FName("Nexus"));

    HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthWidgetComp->SetupAttachment(RootComponent);
    HealthWidgetComp->SetRelativeLocation(FVector(0, 0, 100)); // 머리 위 높이
    HealthWidgetComp->SetDrawSize(FVector2D(80.f, 10.f));
    HealthWidgetComp->SetWidgetSpace(EWidgetSpace::Screen); // 항상 화면을 향함
}

float ANexusActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHP -= DamageAmount;
    if (CurrentHP <= 0)
    {
        if (auto GM = Cast<ANexusGuardianGameMode>(GetWorld()->GetAuthGameMode()))
        {
            GM->OnNexusDestroyed(); // 게임 오버 호출
        }
        Destroy();
    }
    return ActualDamage;
}

void ANexusActor::BeginPlay()
{
    Super::BeginPlay();

    CurrentHP = MaxHP;

    if (HealthWidgetComp)
    {
        UUserWidget* HealthWidget = HealthWidgetComp->GetUserWidgetObject();
        if (HealthWidget)
        {
            FObjectProperty* TargetActorProp = FindFieldChecked<FObjectProperty>(HealthWidget->GetClass(), FName("TargetActor"));
            if (TargetActorProp)
            {
                TargetActorProp->SetObjectPropertyValue_InContainer(HealthWidget, this);
            }
        }
    }
}
