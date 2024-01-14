#include "GameplayOrchestrator.h"
#include "Core.h"
#include "MainScene.h"
#include "Tree.h"
#include "UI.h"

namespace
{
// placeholder
constexpr auto g_beginGameDialog =
R"(Oh no!!! The big, bad meteor boys are back in town and they're infecting the sasquatch reserve.
The poison will need to be cleared from the fruit trees before it spreads across the oasis.
I better scoot! (like, with WASD))";
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

GameplayOrchestrator::GameplayOrchestrator(nc::NcEngine* engine, GameUI* ui)
    : m_engine{engine},
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
        case Event::Begin:
        {
            HandleBegin();
            break;
        }
        case Event::NewGame:
        {
            HandleNewGame();
            break;
        }
        default:
        {
            throw nc::NcError(fmt::format("Event not implemented '{}'", static_cast<int>(event)));
        }
    }
}

void GameplayOrchestrator::Run()
{
    // TODO: add scripting for story sequence
}

void GameplayOrchestrator::Clear()
{
    m_ui->Clear();
}

void GameplayOrchestrator::HandleBegin()
{
    m_ui->AddNewDialog(g_beginGameDialog);
}

void GameplayOrchestrator::HandleNewGame()
{
    Clear();
    m_engine->QueueSceneChange(std::make_unique<MainScene>());
}
} // namespace game
