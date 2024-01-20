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

void SetCameraTargetToFocusPoint(nc::ecs::Ecs world, std::string_view tag)
{
    auto mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera->SetTarget(world.GetEntityByTag(tag));
    mainCamera->SetFollowDistance(5.0f);
    mainCamera->SetFollowHeight(5.0f);
    mainCamera->SetFollowSpeed(2.25f);
}

void DisableGameplayMechanics(nc::ecs::Ecs world)
{
    // Stop character controller
    DisableCharacterMovement(world);

    // Redirect camera focus
    auto mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera->SetFollowDistance(5.0f);
    mainCamera->SetFollowHeight(40.0f);
    mainCamera->SetFollowSpeed(0.25f);
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
        case Event::CampEncounter:   { HandleCampEncounter();   break; }
        case Event::ElderEncounter:  { HandleElderEncounter();  break; }
        case Event::PutterEncounter: { HandlePutterEncounter(); break; }
        case Event::StartSpread:     { HandleStartSpread();     break; }
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
    if (m_currentCutscene.IsRunning())
    {
        m_currentCutscene.Update(m_world, m_ui);
        return;
    }

    if (m_spreadStarted)
    {
        ProcessTrees(dt);
    }

    switch (m_currentEvent)
    {
        case Event::Intro:
        {
            FireEvent(Event::Begin);
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
            AttachPutterQuestTrigger(m_world);
            SetEvent(Event::None);
            break;
        }
        case Event::PutterEncounter:
        {
            // start spreaders as soon as PutteraEncounter cutscene finishes
            // TODO: need to attach spread here
            FireEvent(Event::StartSpread);
            break;
        }
        default: break;
    }
    // TODO: add scripting for story sequence
}

void GameplayOrchestrator::Clear()
{
    m_currentEvent = Event::Intro;
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
    // Need to remove character controller, but it was added this frame. Workaround for engine defect
    m_engine->GetRegistry()->CommitStagedChanges();
    SetEvent(Event::Intro);
    // some bug here
    m_currentCutscene.Enter(m_world, tag::IntroFocusPoint, dialog::Intro);
    m_ui->AddNewDialog(std::string{dialog::Intro.at(0)});
}

void GameplayOrchestrator::HandleBegin()
{
    SetEvent(Event::Begin);
    m_ui->AddNewDialog(dialog::BeginGame);
}

void GameplayOrchestrator::HandleDaveEncounter()
{
    SetEvent(Event::None);
    const auto dave = m_world.GetEntityByTag(tag::Dave);
    auto animator = m_world.Get<nc::graphics::SkeletalAnimator>(dave);
    NC_ASSERT(animator, "expected dave to have an animator");
    animator->LoopImmediate(DaveIdle, [](){return false;}, nc::graphics::anim::RootState);
    m_currentCutscene.Enter(m_world, tag::DaveEncounterFocusPoint, dialog::DaveEncounterSequence);
}

void GameplayOrchestrator::HandleCampEncounter()
{
    SetEvent(Event::CampEncounter);
    m_currentCutscene.Enter(m_world, tag::CampEncounterFocusPoint, dialog::CampEncounterSequence);
}

void GameplayOrchestrator::HandleElderEncounter()
{
    SetEvent(Event::ElderEncounter);
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
    m_spreadStarted = true;
    FinalizeTrees(m_world);
    m_ui->AddNewDialog(dialog::StartSpread);
}

void GameplayOrchestrator::HandleNewGame()
{
    SetEvent(Event::NewGame);
    Clear();
    m_engine->QueueSceneChange(std::make_unique<MainScene>([this](float dt) { Run(dt); }));
}

void GameplayOrchestrator::HandleWin()
{
    SetEvent(Event::Win);
    auto world = m_engine->GetRegistry()->GetEcs();
    m_ui->AddNewDialog(dialog::Win);
    m_ui->OpenMenu();
    ::DisableGameplayMechanics(world);
    m_spreadStarted = false;
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
            FireEvent(Event::Win);
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
