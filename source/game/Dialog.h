#pragma once

#include <array>
#include <string_view>

namespace game::dialog
{
using namespace std::string_view_literals;

constexpr auto Intro = std::array{
R"(Steve: Just another sunup, and another buckaroo in my pocket... I can't fathom why nobody else fancied ridin' out to Moon Valley. Right here's the last patch of earth untouched by that dadgum Blight that's been spreadin' round these parts.

[press space to continue])"sv
};

constexpr auto Controls =
R"(Steve: Let's see if this ol' Cayuse can still make 'er through the Valley.

[move with WASD]

[press space while moving forward to lunge])";

constexpr auto ExploringMoonValleyFlavor = std::array{
R"(Steve: Just me, my trusty bangtail, and a trail of dust - that's the life of a delivery cowboy.)"sv,
R"(Steve: Gotta admit, this place has got a certain charm. Clean air, nice skies. But I can't shake the feeling there's more to this delivery...)"sv
};

constexpr auto DaveEncounterSequence = std::array{
R"(Steve: Howdy, big bug. Steve's the name, Scootin' Steve that is, and I reckon I brought supplies for you fellas.)"sv,
R"(Downy Dave: We're down to letting worms run our supply chains? Our days really are numbered... Well, there's a camp to the west. You can drop off the goods there.)"sv,
R"(Steve: Can do, partner. I'll have 'em there faster than a tumbleweed in a duststorm.)"sv,
R"(Dave: Really, there's no rush. You might as well take in the sights while you're here; its beauty won't last forever.)"sv
};

constexpr auto CampEncounterSequence = std::array{
R"(Steve: Slap me with a cactus! This place is turnin' into a bone orchard.)"sv,
R"(Bristled Bertha: The Blight has made its way to the forest. Its poisoning the fruit of the Lunar Plumwood. We've all gathered here in the Moon Valley because its the only place the plums still grow, but it looks like our luck has run out.)"sv,
R"(Steve: Sounds like you got yerselves a real hoedown, but no need to worry. I've got your delivery here. Should set you right as rain.)"sv,
R"(Bertha: We can't survive without the plums, so unless you happen to have some Roundemup(TM) in that shipment, we're done for. Its the only thing we know of that slows the spread, but since the rebellion, the Directorate has locked down all trade, making it impossible to find. This disease spreads like wildfire. We have very little time left.)"sv,
R"(Steve: Well, hold yer horses. My next delivery just so happens to be to the Stewards. They get a shipment of Roundemup(TM) from the Federal Goods Reserve every other week. I could probably skim off a little and leave those bootlickers none the wiser.)"sv,
R"(Bertha: Wh-what? Really? Hurry, talk to the elder. He'll tell you what to do.)"sv
};

constexpr auto ElderEncounterSequence = std::array{
R"(Echinate Elder: The sands of time are slipping away, my friend. Without the nectar of untainted fruit, the days of the sasquatch are numbered.)"sv,
R"(Steve: I might just have an ace in the hole, partner. I happen to have a shipment of Roundemup(TM) headin' to the Stewards. I reckon I could leave a little here with 'ya to lasso this bull.)"sv,
R"(Elder: Surely, you jest. The Directorate has had all interstellar goods under an iron grip for months now.)"sv,
R"(Steve: Mmhmm, but 'ya still need a trusty cowpolk like m'self to get it where it needs to go. I can't spare much, or my boss'll tan my hide, but I could leave enough to get the job done.)"sv,
R"(Elder: What a twist of fate! But, our strength falters. We've all eaten the infected plums. We don't have the strength to cross the forest in time. You'll have to apply the remedy alone, young one.)"sv,
R"(Steve: Well, I ain't no coffee boiler or nothin', but I really to have to burn the breeze. Stewards ain't known to be a patient breed.)"sv,
R"(Elder: Please, the fate of the sasquatch rests on your shoulders. Seek Putter. He can equip your vehicle with the necessary tools.)"sv
};

constexpr auto PutterEncounterSequence = std::array{
R"(Poofy Putter: Not a moment too soon, vaquero! Let's see what we can cook up here.)"sv,
R"(...tinkering...)"sv,
R"(Putter: Ah-ha! I can get a sprayer wired up in a jiffy. Just a minute...)"sv,
R"(Steve: I know I might look the part, like I'd die standin' up 'n all...)"sv,
R"(...mechanical noises...)"sv,
R"(Steve: ...but truth be told, its all just extra mustard I put on, ya know? I'm not sure I'm the best man for the job.)"sv,
R"(Putter: Nonsense! There's no job too big for... what's your name?)"sv,
R"(Steve: Steve...)"sv,
R"(Putter: ...too big for Steve. A real-deal delivery cowboy like yourself is born of grit, and I believe you have what it takes. We're all counting on you, Steve.)"sv,
R"(Putter: Alright, you're all set. Just aim at the infected trees. You better hurry - the Blight is spreading fast.

press [left shift] to use your sprayer)"sv
};

constexpr auto StartSpread =
R"(Steve: I better get a wiggle on.)";

constexpr auto TreesCleared =
R"(Steve: That's the last of 'em. I better go tell the fellas.)";

constexpr auto WinSequence = std::array{
R"(Elder: Steve, you've restored the Lunar Plumfruit trees. We're eternally grateful for your help.)"sv,
R"(Steve: Glad I could be of service. Unfortunately, it seems I've used up most of the Roundemup(TM). I reckon I might be out of a job.)"sv,
R"(Dave: Sounds like you're plum out of luck! But don't worry about it. You've earned your spurs with us, cowboy. You're welcome in Moon Valley anytime.)"sv,
R"(Steve: What is this stuff, anyhow?)"sv,
R"(Steve: Broad-spectrum weed control... avoid breathing vapor, oops...)"sv,
R"(Steve: Do not apply near base of trees... That's probably just to cover the manufacturer's hide, right?)"sv,
R"(...)"sv,
R"(Steve: Fellas?)"sv,
R"(Steve: Ah, heck.)"sv
};

// placeholders
constexpr auto Lose =
R"(The lone ranger, against the odds, couldn't wrangle Moon Valley from the Blight's grasp.)";
} // namespace game::dialog
