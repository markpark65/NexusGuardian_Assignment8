#include "NexusGuardianCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "NexusGuardianPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NexusGuardianGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NexusGuardianItem.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
ANexusGuardianCharacter::ANexusGuardianCharacter()
{
    CurrentHP = MaxHP;
    SprintSpeed = 1000.f;
    CurrentLevel = 1;
    CurrentExp = 0.0f;
    MaxExp = 100.0f;
    CurrentHP = MaxHP;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    RootComponent = GetCapsuleComponent();

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 0.f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(FollowCamera);
    WeaponMesh->SetRelativeLocation(FVector(40.f, 30.f, -30.f));
    WeaponMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

void ANexusGuardianCharacter::BeginPlay()
{
	Super::BeginPlay();
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
    UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ANexusGuardianCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    ANexusGuardianPlayerController* PC = Cast<ANexusGuardianPlayerController>(GetController());
    if (!PC) return;
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(PC->MoveAction, ETriggerEvent::Triggered, this, &ANexusGuardianCharacter::Move);
        EnhancedInputComponent->BindAction(PC->LookAction, ETriggerEvent::Triggered, this, &ANexusGuardianCharacter::Look);
        EnhancedInputComponent->BindAction(PC->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(PC->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        EnhancedInputComponent->BindAction(PC->SprintAction, ETriggerEvent::Started, this, &ANexusGuardianCharacter::StartSprint);
        EnhancedInputComponent->BindAction(PC->SprintAction, ETriggerEvent::Completed, this, &ANexusGuardianCharacter::StopSprint);
        EnhancedInputComponent->BindAction(PC->AttackAction, ETriggerEvent::Started, this, &ANexusGuardianCharacter::Attack);
        EnhancedInputComponent->BindAction(PC->PickUpAction, ETriggerEvent::Started, this, &ANexusGuardianCharacter::PickUp);
    }
}

void ANexusGuardianCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

    FVector2D MoveInput = Value.Get<FVector2D>();

    if (bIsReversed)
    {
        MoveInput.X *= -1.0f;
        MoveInput.Y *= -1.0f;
    }
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MoveInput.Y);
    AddMovementInput(RightDirection, MoveInput.X);
}

void ANexusGuardianCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}
void ANexusGuardianCharacter::StartJump(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        Jump();
    }
}

void ANexusGuardianCharacter::StopJump(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())
    {
        StopJumping();
    }
}

void ANexusGuardianCharacter::StartSprint(const FInputActionValue& Value) 
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}
void ANexusGuardianCharacter::StopSprint(const FInputActionValue& Value) 
{ 
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    }
}

void ANexusGuardianCharacter::Attack(const FInputActionValue& Value)
{
    if (ProjectileClass)
    {
        FVector SpawnLocation = FollowCamera->GetComponentLocation();
        FRotator SpawnRotation = FollowCamera->GetComponentRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        ANexusGuardianProjectile* Projectile = GetWorld()->SpawnActor<ANexusGuardianProjectile>(
            ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams
        );

        if (Projectile)
        {
            Projectile->Damage = AttackDamage;

            if (FireSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
            }

            if (MuzzleEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, SpawnLocation, SpawnRotation);
            }
        }
    }
}
void ANexusGuardianCharacter::PickUp(const FInputActionValue& Value)
{

    TArray<AActor*> OverlappingActors;
    GetCapsuleComponent()->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Item"))
        {
            ANexusGuardianItem* Item = Cast<ANexusGuardianItem>(Actor);
            if (Item)
            {
                UE_LOG(LogTemp, Warning, TEXT("Item Activated!"));
                Item->ApplyRandomEffect(this);
                break;
            }
        }
    }
}

void ANexusGuardianCharacter::GainExp(float Amount)
{
    CurrentExp += Amount;

    if (CurrentExp >= MaxExp)
    {
        LevelUp();
    }
}

void ANexusGuardianCharacter::LevelUp()
{
    CurrentLevel++;
    CurrentExp = 0.0f;
    MaxExp += 50.0f;
    AttackDamage += 10.0f;
    CurrentHP = MaxHP;
}
float ANexusGuardianCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHP -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("Player HP: %f"), CurrentHP);

    if (CurrentHP <= 0.f)
    {
        if (auto GM = Cast<ANexusGuardianGameMode>(GetWorld()->GetAuthGameMode()))
        {
            GM->OnNexusDestroyed();
        }
    }

    return ActualDamage;
}