#pragma once
#include <string>
#include <vector>
using namespace std;

#undef PlaySound
enum SOUNDS
{
	NO_SOUND,
	SND_FireBlast,
	SND_FireballMove,
	SND_EnchantCast,
	SND_BurnCast,
	SND_CounterSpellCast,
	SND_DispellUndeadCast,
	SND_EboltCast,
	SND_EboltFire,
	SND_EarthquakeCast,
	SND_ForceFieldOn,
	SND_ForceFieldOff,
	SND_AnchorOn,
	SND_AnchorOff,
	SND_DeathRayCast,
	SND_LightningCast,
	SND_LightningFire,
	SND_ProtectionOn,
	SND_ProtectionOff,
	SND_Heal,
	SND_GHealOn,
	SND_GHealOff,
	SND_HasteOn,
	SND_HasteOff,
	SND_Inversion,
	SND_MissilesMove,
	SND_MissilesBlast,
	SND_ManaDrain,
	SND_ManaAbsorb,
	SND_ManaDrainCast,
	SND_ManaBurn,
	SND_Pull,
	SND_Push,
	SND_RSOn,
	SND_RSOff,
	SND_ShockOn,
	SND_ShockOff,
	SND_RingOfFireCast,
	SND_SlowOn,
	SND_SlowOff,
	SND_Teleport,
	SND_Swap,
	SND_WallOn,
	SND_WallHurt,
	SND_WallOff,
	SND_FistCast,
	SND_FistFall,
	SND_VampirismOn,
	SND_VampirismOff,
	SND_StunOn,
	SND_StunOff,
	SND_ToxicCloudCast,
	SND_ForceOfNatureCast,
	SND_ForceOfNatureMove,
	SND_PixiesCast,
	SND_PixieHit,
	SND_CharmCast,
	SND_CharmSuccess,
	SND_CharmFail,
	SND_MeteorCast,
	SND_MeteorFall,
	SND_PoisonOn,
	SND_PoisonOff,
	SND_HammerStrike,
	SND_LongSwordStrike,
	SND_BowShot,
	SND_FireStaffShot,
	SND_TripleStaffShot,
	SND_ShurikenThrow,
	SND_ShurikenMove,
	SND_FireSpark,
	SND_ArrowMove,
	SND_EnchantMove,
	SND_PixieMove,
	SND_Poison,
	SND_ForceFieldHit,
	SND_FONReflect,
	SND_RSReflect,
	SND_NullOn,
	SND_NullOff,
	SND_ShockHit,
	SND_WallCrush,
	SND_ShieldHit,
	SND_NoMana,
	SND_Berserker,
	SND_HarpoonReleased,
	SND_HarpoonCut,
	SND_HarpoonHooked,
	SND_WarCry,
    SND_GESTURE_UN,
    SND_GESTURE_KA,
    SND_GESTURE_ET,
    SND_GESTURE_RO,
    SND_GESTURE_ZO,
    SND_GESTURE_DO,
    SND_GESTURE_CHA,
    SND_GESTURE_IN
};
class Sound
{
	const string name;
	const int variations;
public:
	void PlaySound(int volume) const;   //plays the sound at given volume
	Sound(int variations, const string name);
};

namespace Loader{
    typedef vector<Sound> SoundContainer;
    void loadSounds(SoundContainer& container);
};