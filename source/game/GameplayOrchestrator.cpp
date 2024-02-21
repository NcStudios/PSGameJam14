#include "GameplayOrchestrator.h"
#include "Assets.h"
#include "Character.h"
#include "Core.h"
#include "Dialog.h"
#include "FollowCamera.h"
#include "MainScene.h"
#include "Sasquatch.h"
#include "Tree.h"
#include "ui/UI.h"

#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/scene/NcScene.h"

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
    auto& mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera.SetTarget(character);
    mainCamera.SetFollowDistance(game::FollowCamera::DefaultFollowDistance);
    mainCamera.SetFollowHeight(game::FollowCamera::DefaultFollowHeight);
    mainCamera.SetFollowSpeed(game::FollowCamera::DefaultFollowSpeed);
}

void SetCameraTargetToFocusPoint(nc::ecs::Ecs world, std::string_view tag, float followDistance = 7.0f, float followHeight = 6.0f, float followSpeed = 64.0f)
{
    auto& mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera.SetTarget(world.GetEntityByTag(tag));
    mainCamera.SetFollowDistance(followDistance);
    mainCamera.SetFollowHeight(followHeight);
    mainCamera.SetFollowSpeed(followSpeed);
}

void DisableGameplayMechanics(nc::ecs::Ecs world, float followDistance = 5.0f, float followHeight = 40.0f, float followSpeed = 8.0f)
{
    // Stop character controller
    DisableCharacterMovement(world);

    // Redirect camera focus
    auto& mainCamera = game::GetComponentByEntityTag<game::FollowCamera>(world, game::tag::MainCamera);
    mainCamera.SetFollowDistance(followDistance);
    mainCamera.SetFollowHeight(followHeight);
    mainCamera.SetFollowSpeed(followSpeed);
}

void StopMusic(nc::ecs::Ecs world)
{
    auto& music = game::GetComponentByEntityTag<nc::audio::AudioSource>(world, game::tag::Music);
    if (music.IsPlaying()) music.Stop();
}

auto GetPointLightValues(std::span<nc::graphics::PointLight> lights) -> std::vector<std::pair<nc::Vector3, nc::Vector3>>
{
    auto out = std::vector<std::pair<nc::Vector3, nc::Vector3>>{};
    out.reserve(lights.size());
    for (auto& light : lights)
    {
        out.emplace_back(light.ambientColor, light.diffuseColor);
    }

    return out;
}

void SetPointLightsToBlack(std::span<nc::graphics::PointLight> lights)
{
    for (auto& light : lights)
    {
        light.ambientColor = nc::Vector3{};
        light.diffuseColor = nc::Vector3{};
    }
}

void StepFadeIn(std::span<nc::graphics::PointLight> lights, const std::vector<std::pair<nc::Vector3, nc::Vector3>>& initialValues, float dt)
{
    constexpr auto fade = [](const nc::Vector3& from, const nc::Vector3& to, float factor)
    {
        auto out = nc::Vector3{};
        out.x = std::clamp(std::lerp(from.x, to.x, factor), 0.0f, to.x);
        out.y = std::clamp(std::lerp(from.y, to.y, factor), 0.0f, to.y);
        out.z = std::clamp(std::lerp(from.z, to.z, factor), 0.0f, to.z);
        return out;
    };

    constexpr auto fadeFactor = 0.15f;
    const auto factor = std::clamp(fadeFactor * dt, 0.0f, 1.0f);

    for (auto [light, value] : std::views::zip(lights, initialValues))
    {
        light.ambientColor = fade(light.ambientColor, value.first, factor);
        light.diffuseColor = fade(light.diffuseColor, value.second, factor);
    }
}

void StepFadeToBlack(nc::ecs::Ecs world, float dt)
{
    constexpr auto fade = [](const nc::Vector3& in, float factor)
    {
        auto out = nc::Vector3{};
        out.x = nc::Lerp(in.x, 0.0f, factor);
        out.y = nc::Lerp(in.y, 0.0f, factor);
        out.z = nc::Lerp(in.z, 0.0f, factor);
        return out;
    };

    constexpr auto fadeFactor = 1.3f;
    const auto factor = fadeFactor * dt;

    for (auto& light : world.GetAll<nc::graphics::PointLight>())
    {
        light.ambientColor = fade(light.ambientColor, factor);
        light.diffuseColor = fade(light.diffuseColor, factor);
    }
}

class LightFader : public nc::FreeComponent
{
    public:
        LightFader(nc::Entity self, nc::ecs::Ecs world)
            : nc::FreeComponent{self}, m_world{world} {}

        void Run(nc::Entity self, nc::Registry*, float dt)
        {
            auto lights = m_world.GetAll<nc::graphics::PointLight>();
            static auto lightValues = ::GetPointLightValues(lights);

            if (m_runTime == 0.0f)
                ::SetPointLightsToBlack(lights);

            m_runTime += dt;

            if (m_runTime < 2.0f)
                return;

            if (m_runTime < 10.0f)
                ::StepFadeIn(lights, lightValues, dt + m_runTime * 0.015f);
            else
            {
                for (auto [light, value] : std::views::zip(lights, lightValues))
                {
                    light.ambientColor = value.first;
                    light.diffuseColor = value.second;
                }
                m_world.Remove<nc::Entity>(self);
            }
        }

    private:
        nc::ecs::Ecs m_world;
        float m_runTime = 0.0f;
};
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

void Cutscene::Enter(nc::ecs::Ecs world, std::string_view focusPointTag, std::span<const dialog::DialogEntry> dialogSequence)
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

void Cutscene::Update(nc::ecs::Ecs world, nc::Signal<DialogEvent>& onDialog)
{
    if (m_currentDialog > m_dialogSequence.size())
    {
        Exit(world);
        return;
    }
    else if (!m_initialDialogPlayed)
    {
        m_initialDialogPlayed = true;
        onDialog.Emit(StartDialogEvent{m_dialogSequence[m_currentDialog++]});
    }
    else if (KeyDown(nc::input::KeyCode::Space))
    {
        if (m_currentDialog < m_dialogSequence.size())
            onDialog.Emit(NextDialogEvent{m_dialogSequence[m_currentDialog]});
        else
            onDialog.Emit(EndDialogEvent{});

        // Do this separe from above, so we hang out on the last dialog until space pressed again
        ++m_currentDialog;
    }
}

GameplayOrchestrator::GameplayOrchestrator(nc::NcEngine* engine, GameEvents events, ui::GameUI* ui)
    : m_engine{engine},
      m_world{m_engine->GetComponentRegistry()},
      m_events{std::move(events)},
      m_ui{ui},
      m_treeTracker{std::make_unique<TreeTracker>(engine->GetComponentRegistry())}
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
        case Event::TitleScreen:     { HandleTitleScreen();     break; }
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
        m_ui->SetTreeCounts(m_healthyCount, m_infectedCount);
    }

    if (m_currentCutscene.IsRunning())
    {
        m_currentCutscene.Update(m_world, m_events.onDialog);
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
        case Event::TitleScreen:
        {
            m_timeInCurrentEvent += dt;
            if (m_timeInCurrentEvent > 4.0f)
            {
                FireEvent(Event::Intro);
            }

            break;
        }
        case Event::Intro:
        {
            SetAnimatorState(m_world, DaveWave, tag::Dave);
            AttachDaveQuestTrigger(m_world);
            FireEvent(Event::Begin);
            break;
        }
        case Event::DaveEncounter:
        {
            AttachCampQuestTrigger(m_world); // don't allow skip - cutscenes remove controller/sprayer state
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
                    m_events.onDialog.Emit(OneShotDialogEvent{dialog::ExploringMoonValleyFlavor.at(m_currentFlavorDialogIndex++)});
            }

            break;
        }
        case Event::CampEncounter:
        {
            // enable elder encounter after camp
            SetAnimatorState(m_world, DaveSittingStump, tag::Elder);
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
                ::DisableGameplayMechanics(m_world, 5.0f, 20.0f);
                ::StopMusic(m_world);
                GetComponentByEntityTag<nc::audio::AudioSource>(m_world, tag::Music).Play(music::EndingIndex);
            }

            m_timeInCurrentEvent += dt;
            if (m_timeInCurrentEvent > 5.0f)
            {
                SetEvent(Event::None);
                m_ui->SetMenuToEndGameMenu();
                m_ui->OpenMenu();
                return;
            }

            ::StepFadeToBlack(m_world, dt);
            break;
        }
        case Event::Lose:
        {
            m_timeInCurrentEvent += dt;
            if (m_timeInCurrentEvent > 5.0f)
            {
                SetEvent(Event::None);
                m_ui->SetMenuToEndGameMenu();
                m_ui->OpenMenu();
                return;
            }

            ::StepFadeToBlack(m_world, dt);
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
    m_healthyCount = 0ull;
    m_infectedCount = 0ull;
    m_ui->Clear();
    m_currentCutscene = Cutscene{};
}

void GameplayOrchestrator::SetEvent(Event event)
{
    m_currentEvent = event;
}

void GameplayOrchestrator::HandleTitleScreen()
{
    SetEvent(Event::TitleScreen);
    ::SetCameraTargetToFocusPoint(m_world, tag::IntroFocusPoint);

    auto fader = m_world.Emplace<nc::Entity>({
        .tag = "PointLightFader",
        .flags = nc::Entity::Flags::NoSerialize
    });

    m_world.Emplace<LightFader>(fader, m_world);
    m_world.Emplace<nc::FrameLogic>(fader, nc::InvokeFreeComponent<LightFader>{});

    auto& camTrans = GetComponentByEntityTag<nc::Transform>(m_world, tag::MainCamera);
    auto title = m_world.Emplace<nc::Entity>({
        .position = camTrans.Position() + camTrans.Forward() * 5.0f,
        .flags = nc::Entity::Flags::NoSerialize
    });

    m_world.Emplace<nc::graphics::ParticleEmitter>(title, nc::graphics::ParticleInfo{
        .emission = {
            .initialEmissionCount = 1,
        },
        .init = {
            .lifetime = 4.5f,
            .rotationMin = 1.570795f,
            .rotationMax = 1.570795f,
            .scaleMin = 2.0f,
            .scaleMax = 2.0f,
            .particleTexturePath = TitleScreenParticle
        },
        .kinematic = {
            .rotationOverTimeFactor = 0.0f,
        }
    });

}

void GameplayOrchestrator::HandleIntro()
{
    SetEvent(Event::Intro);
    m_currentCutscene.Enter(m_world, tag::IntroFocusPoint, dialog::Intro);
}

void GameplayOrchestrator::HandleBegin()
{
    SetEvent(Event::None);
    m_events.onDialog.Emit(OneShotDialogEvent{dialog::Controls});
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
    GetComponentByEntityTag<CharacterController>(m_world, tag::VehicleFront).EquipSprayer();
    m_spreadStarted = true;
    FinalizeTrees(m_world);
    m_events.onDialog.Emit(OneShotDialogEvent{dialog::StartSpread});
    m_ui->ToggleTreeCounter(true);
    // StopMusic
}

void GameplayOrchestrator::HandleTreesCleared()
{
    SetEvent(Event::None);
    m_spreadStarted = false;
    AttachFinalQuestTrigger(m_world);
    m_ui->ToggleTreeCounter(false);
    m_events.onDialog.Emit(OneShotDialogEvent{dialog::TreesCleared});
    ::StopMusic(m_world);
    GetComponentByEntityTag<nc::audio::AudioSource>(m_world, tag::Music).Play(music::BlightClearedIndex);
}

void GameplayOrchestrator::HandleFlavorDialog()
{
}

void GameplayOrchestrator::HandleNewGame()
{
    SetEvent(Event::NewGame);
    Clear();
    auto ncScene = m_engine->GetModuleRegistry()->Get<nc::NcScene>();
    ncScene->Queue(std::make_unique<MainScene>([this](float dt) { Run(dt); }));
    ncScene->ScheduleTransition();
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
    m_timeInCurrentEvent = 0.0f;
    SetEvent(Event::Lose);
    m_events.onDialog.Emit(OneShotDialogEvent{dialog::Lose});
    ::DisableGameplayMechanics(m_world);
    m_spreadStarted = false;
    ::StopMusic(m_world);
    GetComponentByEntityTag<nc::audio::AudioSource>(m_world, tag::Music).Play(music::LoseIndex);
}

void GameplayOrchestrator::ProcessTrees(float dt)
{
    if constexpr (EnableGameplay)
    {
        auto& registry = m_engine->GetComponentRegistry();
        auto infectedTrees = m_world.GetAll<InfectedTree>();
        m_infectedCount = registry.GetPool<InfectedTree>().TotalSize();
        if (m_infectedCount == 0)
        {
            FireEvent(Event::TreesCleared);
            return;
        }

        for (auto& infected : infectedTrees)
        {
            infected.Update(m_world, dt);
        }

        auto healthyTrees = m_world.GetAll<HealthyTree>();
        m_healthyCount = registry.GetPool<HealthyTree>().TotalSize();
        if (m_healthyCount == 0)
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
