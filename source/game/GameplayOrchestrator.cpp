#include "GameplayOrchestrator.h"
#include "Assets.h"
#include "Character.h"
#include "Core.h"
#include "Dialog.h"
#include "FollowCamera.h"
#include "MainScene.h"
#include "Sasquatch.h"
#include "Tree.h"
#include "UI.h"

#include "ncengine/graphics/SkeletalAnimator.h"

namespace
{
void EnableCharacterMovement(nc::ecs::Ecs world)
{
    auto character = world.GetEntityByTag(game::tag::VehicleFront);
    world.Emplace<game::CharacterController>(character);
}

void DisableCharacterMovement(nc::ecs::Ecs world)
{
    auto character = world.GetEntityByTag(game::tag::VehicleFront);
    world.Remove<game::CharacterController>(character);
}

void SetCameraTargetToCharacter(nc::ecs::Ecs world)
{
    const auto character = world.GetEntityByTag(game::tag::VehicleFront);
    auto mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera->SetTarget(character);
    mainCamera->SetFollowDistance(game::FollowCamera::DefaultFollowDistance);
    mainCamera->SetFollowHeight(game::FollowCamera::DefaultFollowHeight);
    mainCamera->SetFollowSpeed(game::FollowCamera::DefaultFollowSpeed);
}

void SetCameraTargetToFocusPoint(nc::ecs::Ecs world, std::string_view tag, float followDistance = 6.0f, float followHeight = 5.0f, float followSpeed = 2.0f)
{
    auto mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera->SetTarget(world.GetEntityByTag(tag));
    mainCamera->SetFollowDistance(followDistance);
    mainCamera->SetFollowHeight(followHeight);
    mainCamera->SetFollowSpeed(followSpeed);
}

void DisableGameplayMechanics(nc::ecs::Ecs world, float followDistance = 5.0f, float followHeight = 40.0f, float followSpeed = 0.25f)
{
    // Stop character controller
    DisableCharacterMovement(world);

    // Redirect camera focus
    auto mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera->SetFollowDistance(followDistance);
    mainCamera->SetFollowHeight(followHeight);
    mainCamera->SetFollowSpeed(followSpeed);
}
} // anonymous namespace

namespace game
{
// Keeps track of healthy/infected tree count
class TreeTracker
{
    public:
        TreeTracker(nc::ecs::ComponentRegistry& registry)
            : m_onAddHealthyConnection{registry.GetPool<HealthyTree>().OnAdd().Connect(this, &TreeTracker::AddHealthy)},
              m_onRemoveHealthyConnection{registry.GetPool<HealthyTree>().OnRemove().Connect(this, &TreeTracker::RemoveHealthy)},
              m_onAddInfectedConnection{registry.GetPool<InfectedTree>().OnAdd().Connect(this, &TreeTracker::AddInfected)},
              m_onRemoveInfectedConnection{registry.GetPool<InfectedTree>().OnRemove().Connect(this, &TreeTracker::RemoveInfected)}
        {
        }

        auto GetHealthyCount() const noexcept { return m_numHealthy; }
        auto GetInfectedCount() const noexcept { return m_numInfected; }
        void AddHealthy(HealthyTree&) { ++m_numHealthy; }
        void AddInfected(InfectedTree&) { ++m_numInfected; }
        void RemoveHealthy(nc::Entity) { NC_ASSERT(m_numHealthy > 0, "invalid design"); --m_numHealthy; }
        void RemoveInfected(nc::Entity) { NC_ASSERT(m_numInfected > 0, "invalid design");--m_numInfected; }

    private:
        size_t m_numHealthy = 0;
        size_t m_numInfected = 0;
        nc::Connection<HealthyTree&> m_onAddHealthyConnection;
        nc::Connection<nc::Entity> m_onRemoveHealthyConnection;
        nc::Connection<InfectedTree&> m_onAddInfectedConnection;
        nc::Connection<nc::Entity> m_onRemoveInfectedConnection;
};

void Cutscene::Enter(nc::ecs::Ecs world, std::string_view focusPointTag, std::span<const std::string_view> dialogSequence)
{
    ::DisableCharacterMovement(world);
    ::SetCameraTargetToFocusPoint(world, focusPointTag);
    m_dialogSequence = dialogSequence;
    m_currentDialog = 0;
    m_running = true;
    m_initialDialogPlayed = false;
}

void Cutscene::Exit(nc::ecs::Ecs world)
{
    ::EnableCharacterMovement(world);
    ::SetCameraTargetToCharacter(world);
    m_dialogSequence = {};
    m_currentDialog = 0;
    m_running = false;
    m_initialDialogPlayed = false;
}

auto Cutscene::IsRunning() -> bool
{
    return m_running;
}

void Cutscene::Update(nc::ecs::Ecs world, GameUI* ui)
{
    if (m_currentDialog > m_dialogSequence.size())
    {
        Exit(world);
        return;
    }
    else if (!m_initialDialogPlayed)
    {
        m_initialDialogPlayed = true;
        ui->AddNewDialog(m_dialogSequence[m_currentDialog++].data());
    }
    else if (KeyDown(nc::input::KeyCode::Space))
    {
        if (m_currentDialog < m_dialogSequence.size())
            ui->AddNewDialog(m_dialogSequence[m_currentDialog].data());

        // Do this separe from above, so we hang out on the last dialog until space pressed again
        ++m_currentDialog;
    }
}

GameplayOrchestrator::GameplayOrchestrator(nc::NcEngine* engine, GameUI* ui)
    : m_engine{engine},
      m_world{m_engine->GetRegistry()->GetEcs()},
      m_ui{ui},
      m_treeTracker{std::make_unique<TreeTracker>(engine->GetRegistry()->GetImpl())}
{
    NC_ASSERT(!GameplayOrchestrator::m_instance, "Already a GameplayOrchestrator instance");
    GameplayOrchestrator::m_instance = this;
}

GameplayOrchestrator::~GameplayOrchestrator() noexcept
{
    GameplayOrchestrator::m_instance = nullptr;
}

auto GameplayOrchestrator::Instance() -> GameplayOrchestrator&
{
    NC_ASSERT(GameplayOrchestrator::m_instance, "No GameplayOrchestrator instance");
    return *GameplayOrchestrator::m_instance;
}

void GameplayOrchestrator::FireEvent(Event event)
{
    switch (event)
    {
        case Event::Intro:           { HandleIntro();           break; }
        case Event::Begin:           { HandleBegin();           break; }
        case Event::DaveEncounter:   { HandleDaveEncounter();   break; }
        case Event::HeadToCamp:      { HandleHeadToCamp();      break; }
        case Event::CampEncounter:   { HandleCampEncounter();   break; }
        case Event::ElderEncounter:  { HandleElderEncounter();  break; }
        case Event::PutterEncounter: { HandlePutterEncounter(); break; }
        case Event::StartSpread:     { HandleStartSpread();     break; }
        case Event::TreesCleared:    { HandleTreesCleared();    break; }
        case Event::FlavorDialog:    { HandleFlavorDialog();    break; }
        case Event::NewGame:         { HandleNewGame();         break; }
        case Event::Win:             { HandleWin();             break; }
        case Event::Lose:            { HandleLose();            break; }
        default:
        {
            throw nc::NcError(fmt::format("Event not implemented '{}'", static_cast<int>(event)));
        }
    }
}

void GameplayOrchestrator::Run(float dt)
{
    constexpr auto flavorTextDelay = 10.0f;

    // need to do before cutscene because it can start them
    if (m_spreadStarted)
    {
        ProcessTrees(dt);
    }

    if (m_currentCutscene.IsRunning())
    {
        m_currentCutscene.Update(m_world, m_ui);
        return;
    }

#ifndef GAME_PROD_BUILD
    if (KeyDown(hotkey::SkipToSpreadEvent))
    {
        FireEvent(Event::StartSpread);
        return;
    }
#endif

    // If an event starts a cutscene, its case runs after it has finished
    switch (m_currentEvent)
    {
        case Event::Intro:
        {
            AttachDaveQuestTrigger(m_world);
            AttachCampQuestTrigger(m_world); // allow skipping Dave?
            FireEvent(Event::Begin);
            break;
        }
        case Event::DaveEncounter:
        {
            FireEvent(Event::HeadToCamp);
            break;
        }
        case Event::HeadToCamp:
        {
            m_timeInCurrentEvent += dt;
            if (m_timeInCurrentEvent > flavorTextDelay)
            {
                m_timeInCurrentEvent = 0.0f;
                if (m_currentFlavorDialogIndex >= dialog::ExploringMoonValleyFlavor.size())
                    SetEvent(Event::None);
                else
                    m_ui->AddNewDialog(std::string{dialog::ExploringMoonValleyFlavor.at(m_currentFlavorDialogIndex++)});
            }

            break;
        }
        case Event::CampEncounter:
        {
            // enable elder encounter after camp
            AttachElderQuestTrigger(m_world);
            SetEvent(Event::None);
            break;
        }
        case Event::ElderEncounter:
        {
            SetAnimatorState(m_world, DaveSittingStump, tag::Elder);
            AttachPutterQuestTrigger(m_world);
            SetEvent(Event::None);
            break;
        }
        case Event::PutterEncounter:
        {
            // start spreaders as soon as PutteraEncounter cutscene finishes
            FireEvent(Event::StartSpread);
            break;
        }
        case Event::Win:
        {
            if (m_timeInCurrentEvent == 0.0f)
            {
                ::DisableGameplayMechanics(m_world, 5.0f, 20.0f, 0.25f);
            }

            m_timeInCurrentEvent += dt;

            if (m_timeInCurrentEvent > 5.0f)
            {
                SetEvent(Event::None);
                m_ui->OpenMenu();
                return;
            }

            constexpr auto fadeFactor = 1.3f;
            auto lights = m_world.GetAll<nc::graphics::PointLight>();
            for (auto& light : lights)
            {
                auto ambient = light.GetAmbient();
                auto diffuseColor = light.GetDiffuseColor();
                ambient.x = nc::Lerp(ambient.x, 0.0f, dt * fadeFactor);
                ambient.y = nc::Lerp(ambient.y, 0.0f, dt * fadeFactor);
                ambient.z = nc::Lerp(ambient.z, 0.0f, dt * fadeFactor);
                diffuseColor.x = nc::Lerp(diffuseColor.x, 0.0f, dt * fadeFactor);
                diffuseColor.y = nc::Lerp(diffuseColor.y, 0.0f, dt * fadeFactor);
                diffuseColor.z = nc::Lerp(diffuseColor.z, 0.0f, dt * fadeFactor);
                light.SetAmbient(ambient);
                light.SetDiffuseColor(diffuseColor);
            }
            break;
        }
        default: break;
    }
}

void GameplayOrchestrator::Clear()
{
    m_currentEvent = Event::Intro;
    m_timeInCurrentEvent = 0.0f;
    m_currentFlavorDialogIndex = 0ull;
    m_spreadStarted = false;
    m_ui->Clear();
    m_currentCutscene = Cutscene{};
}

void GameplayOrchestrator::SetEvent(Event event)
{
    m_currentEvent = event;
}

void GameplayOrchestrator::HandleIntro()
{
    SetEvent(Event::Intro);
    m_currentCutscene.Enter(m_world, tag::IntroFocusPoint, dialog::Intro);
}

void GameplayOrchestrator::HandleBegin()
{
    SetEvent(Event::None);
    m_ui->AddNewDialog(dialog::Controls);
}

void GameplayOrchestrator::HandleDaveEncounter()
{
    SetEvent(Event::DaveEncounter);
    ReturnAnimatorToRootState(m_world, tag::Dave);
    m_currentCutscene.Enter(m_world, tag::DaveEncounterFocusPoint, dialog::DaveEncounterSequence);
}

void GameplayOrchestrator::HandleHeadToCamp()
{
    m_timeInCurrentEvent = 0.0f;
    m_currentFlavorDialogIndex = 0ull;
    SetEvent(Event::HeadToCamp);
}

void GameplayOrchestrator::HandleCampEncounter()
{
    SetEvent(Event::CampEncounter);
    m_currentCutscene.Enter(m_world, tag::CampEncounterFocusPoint, dialog::CampEncounterSequence);
}

void GameplayOrchestrator::HandleElderEncounter()
{
    SetEvent(Event::ElderEncounter);
    SetPlayOnceAnimation(m_world, DaveStandupStump, tag::Elder);
    m_currentCutscene.Enter(m_world, tag::ElderEncounterFocusPoint, dialog::ElderEncounterSequence);
}

void GameplayOrchestrator::HandlePutterEncounter()
{
    SetEvent(Event::PutterEncounter);
    m_currentCutscene.Enter(m_world, tag::Putter, dialog::PutterEncounterSequence);
}

void GameplayOrchestrator::HandleStartSpread()
{
    SetEvent(Event::None);
    GetComponentByEntityTag<CharacterController>(m_world, tag::VehicleFront)->EquipSprayer();
    m_spreadStarted = true;
    FinalizeTrees(m_world);
    m_ui->AddNewDialog(dialog::StartSpread);
}

void GameplayOrchestrator::HandleTreesCleared()
{
    SetEvent(Event::None);
    m_spreadStarted = false;
    AttachFinalQuestTrigger(m_world);
    m_ui->AddNewDialog(dialog::TreesCleared);
}

void GameplayOrchestrator::HandleFlavorDialog()
{
}

void GameplayOrchestrator::HandleNewGame()
{
    SetEvent(Event::NewGame);
    Clear();
    m_engine->QueueSceneChange(std::make_unique<MainScene>([this](float dt) { Run(dt); }));
}

void GameplayOrchestrator::HandleWin()
{
    m_timeInCurrentEvent = 0.0f;
    SetEvent(Event::Win);
    MoveSasquatchToCamp(m_world);
    m_currentCutscene.Enter(m_world, tag::ElderEncounterFocusPoint, dialog::WinSequence);
}

void GameplayOrchestrator::HandleLose()
{
    // basically duplicate code with HandleWin(), but keep separate for easier future tweaking
    SetEvent(Event::Lose);
    auto world = m_engine->GetRegistry()->GetEcs();
    m_ui->AddNewDialog(dialog::Lose);
    m_ui->OpenMenu();
    ::DisableGameplayMechanics(world);
    m_spreadStarted = false;
}

void GameplayOrchestrator::ProcessTrees(float dt)
{
    if constexpr (EnableGameplay)
    {
        auto registry = m_engine->GetRegistry();
        auto infectedTrees = m_world.GetAll<InfectedTree>();

        if (registry->StorageFor<InfectedTree>()->TotalSize() == 0)
        {
            FireEvent(Event::TreesCleared);
            return;
        }

        for (auto& infected : infectedTrees)
        {
            infected.Update(m_world, dt);
        }

        auto healthyTrees = m_world.GetAll<HealthyTree>();

        if (registry->StorageFor<HealthyTree>()->TotalSize() == 0)
        {
            FireEvent(Event::Lose);
            return;
        }

        for (auto& healthy : healthyTrees)
        {
            healthy.Update(dt);
            if (healthy.ShouldMorph())
            {
                MorphTreeToInfected(m_world, healthy.ParentEntity());
            }
        }
    }
}
} // namespace game
