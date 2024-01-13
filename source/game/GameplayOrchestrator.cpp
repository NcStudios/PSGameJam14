#include "GameplayOrchestrator.h"
#include "Core.h"
#include "MainScene.h"
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
void FireEvent(Event event)
{
    GameplayOrchestrator::Instance().FireEvent(event);
}

GameplayOrchestrator::GameplayOrchestrator(nc::NcEngine* engine, GameUI* ui)
    : m_engine{engine},
      m_ui{ui}
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
