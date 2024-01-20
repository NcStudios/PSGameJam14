#pragma once

#include <array>
#include <string_view>

namespace game::dialog
{
using namespace std::string_view_literals;

constexpr auto Intro = std::array{
R"(Just another sunup, and another buckaroo in my pocket... I can't fathom why nobody else fancied ridin' out to the Moon Valley. It's the last patch of earth untouched by that darn Blight stench.
[press space to continue])"sv
};

constexpr auto BeginGame =
R"(Gotta admit, this place has got a certain charm. Clean air, nice skies. But I can't shake the feeling there's more to this delivery...)";

constexpr auto DaveEncounterSequence = std::array{
R"(Steve: Howdy, big bug. Steve's the name, Scootin' Steve that is, and I reckon I brought supplies for you fellas.)"sv,
R"(Dave: We're down to letting worms run supply chains? Our days really are numbered... Well, there's a camp to the west. You can drop off the goods there.)"sv,
R"(Steve: Can do, partner. I'll have 'em there faster than a tumbleweed in a duststorm!)"sv
};

constexpr auto CampEncounterSequence = std::array{
R"(Steve: Slap me with a cactus! This place is turnin' into a bone orchard.)"sv,
R"(Sasquatch: The Blight has made its way to the forest. Its poisoning the fruit of the [xxx] tree.)"sv,
R"(Steve: Sounds like you got yerselves a real hoedown! No need to worry though, I've got your delivery here. That should set you right as rain.)"sv,
R"(Sasquatch: We can't survive without the [xxx] trees, and from what I've seen, this disease spreads like wildfire. The sasquatch have all gathered in this forest because it was the only place free of the Blight. You can go talk to the elder. Maybe he can make use of what you have.)"sv
};

constexpr auto ElderEncounterSequence = std::array{
R"(Elder: My people's days are numbered. Without access to untained [xxx] fruit, we won't survive much longer.)"sv,
R"(Steve: Surely the food in your delievery can last you until next week?)"sv,
R"(Elder: I'm afraid not. Unless you happen to have Roundemup(TM) in there, there's no hope. But that's impossible to come by with the new space trade restrictions.)"sv,
R"(Steve: I might just have an ace in the hole, friend! My next delivery is to [xxx], and they get a shipment from the federal reserve every other week... Yup, got some right here.)"sv,
R"(Elder: Wh-what? Really? You have to hurry and apply it to all the infected trees before its too late!)"sv,
R"(Steve: How about I just leave you with a little? My boss'll tan my hide if I'm late with this next shipment.)"sv,
R"(Elder: We've all eaten the tainted fruit. We simply don't have the strength to cross the forest. The fate of the sasquatch rests on your shoulders, little one.)"sv
};

constexpr auto StartSpread =
R"(I better get a wiggle on!)";

// placeholders
constexpr auto Win =
R"([placeholder] You saved the sasquatch oasis!)";

constexpr auto Lose =
R"([placeholder] This planet cannot be saved.)";
} // namespace game::dialog
