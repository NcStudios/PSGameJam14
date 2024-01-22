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

constexpr auto Controls =
R"(Let's see if this ol' Cayuse can still make 'er through the Valley.

[move with WASD]

[press space while moving forward to lunge])";

constexpr auto ExploringMoonValleyFlavor = std::array{
R"(Just me, my trusty bangtail, and a trail of dust - that's the life of a delivery cowboy.)"sv,
R"(Gotta admit, this place has got a certain charm. Clean air, nice skies. But I can't shake the feeling there's more to this delivery...)"sv
};

constexpr auto DaveEncounterSequence = std::array{
R"(Steve: Howdy, big bug. Steve's the name, Scootin' Steve that is, and I reckon I brought supplies for you fellas.)"sv,
R"(Dave: We're down to letting worms run supply chains? Our days really are numbered... Well, there's a camp to the west. You can drop off the goods there.)"sv,
R"(Steve: Can do, partner. I'll have 'em there faster than a tumbleweed in a duststorm!)"sv
};

constexpr auto CampEncounterSequence = std::array{
R"(Steve: Slap me with a cactus! This place is turnin' into a bone orchard.)"sv,
R"(Sasquatch: The Blight has made its way to the forest. Its poisoning the fruit of the Lunar Plumwood. We've all gathered here in the Moon Valley because its the only place the plums still grow.)"sv,
R"(Steve: Sounds like you got yerselves a real hoedown! No need to worry though, I've got your delivery here. That should set you right as rain.)"sv,
R"(Sasquatch: We can't survive without the plums, so unless you happen to have some Roundemup(TM) in that shipment, we're done for. But since the rebellion, the Directorate has locked down all trade. From what I've seen, this disease spreads like wildfire. We have very little time left.)"sv,
R"(Steve: Well, hold yer horses. My next delivery just so happens to be to the Stewards. They get a shipment of Roundemup(TM) from the Federal Goods Reserve every other week. I could probably skim off a little and leave those bootlickers none the wiser.)"sv,
R"(Sasquatch: Wh-what? Really? Hurry, talk to the elder. He'll tell you what to do.)"sv
};

constexpr auto ElderEncounterSequence = std::array{
R"(Elder: The sands of time are slipping away, my friend. Without the nectar of untainted fruit, the days of the sasquatch are numbered.)"sv,
R"(Steve: I might just have an ace in the hole, partner. I happen to have a shipment of Roundemup(TM) headin' to the Stewards. I reckon I could leave a little here with 'ya to lasso this bull.)"sv,
R"(Elder: Surely, you jest. The Directorate has placed had all interstellar goods under an iron grip since the rebellion.)"sv,
R"(Steve: Mmhmm, but 'ya still need a trusty cowpolk like m'self to get it where it needs to go. I can't spare much, or my boss'll tan my hide, but I could leave enough to get the job done.)"sv,
R"(Elder: What a twist of fate! But, our strength falters. We've all eaten the infected plums. We don't have the strength to cross the forest in time. You'll have to apply the remedy alone, young one.)"sv,
R"(Steve: Well, I ain't no coffee boiler or nothin', but I really to have to burn the breeze. Stewards ain't known to be a patient breed.)"sv,
R"(Elder: Please, the fate of the sasquatch rests on your shoulders. Seek Putter. He can equip your vehicle with the necessary tools.)"sv
};

constexpr auto PutterEncounterSequence = std::array{
R"(Putter: Alrighty then! Let's see what we can cook up here.)"sv,
R"(...tinkering...)"sv,
R"(Ah-ha! I can wire a sprayer for you in a jiffy. Just a minute...)"sv,
R"(Steve: I know I might look the part, like I'd die standin' up 'n all...)"sv,
R"(...mechanical noises...)"sv,
R"(Steve: ...but truth be told, its all just extra mustard I put on, ya know? I really don't think I'm the best man for the job.)"sv,
R"(Putter: Nonsense! There's no job too big for... what's your name?)"sv,
R"(Steve: Steve...)"sv,
R"(Putter: ...too big for Steve. A real-deal delivery cowboy like yourself is born of grit, and I believe you have what it takes. We're all counting on you, Steve. Alright, you're all set. Press [left shift] to try out your new sprayer!)"sv
};

constexpr auto StartSpread =
R"(I better get a wiggle on!)";

// placeholders
constexpr auto Win =
R"([placeholder] You saved the sasquatch oasis!)";

constexpr auto Lose =
R"([placeholder] This planet cannot be saved.)";

constexpr auto SteveFlavor = std::array{
R"()"sv
};
} // namespace game::dialog
