class Enchant
{
public:
    const enumStrings name;
	const Sound* sound_on;
	const Sound* sound_off;
    const int time_to_last, id;
    const bool does_wearoff_auto;
    Enchant::Enchant(const enumStrings name, const Sound* snd_on, const Sound* snd_off, 
        const int time, const int id, const bool wear_off_auto = true):
    name(name), sound_on(snd_on), sound_off(snd_off), time_to_last(time), 
        id(id), does_wearoff_auto(wear_off_auto) {}

    int MyTexture() const { return 0; }
};