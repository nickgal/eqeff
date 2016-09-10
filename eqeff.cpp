// eqeff.cpp

#include "iostream"
#include "iomanip"
#include "fstream"
#include "vector"
#include "algorithm"
#include "functional"
#include "string"
#include "regex"

// http://www.eqemulator.org/forums/showthread.php?t=39747
struct EffSoundEntry
{
public:
	int32_t UnkRef00;
	int32_t UnkRef04;
	int32_t Reserved;
	int32_t Sequence;
	float X;
	float Y;
	float Z;
	float Radius;
	int32_t Cooldown1;
	int32_t Cooldown2;
	int32_t RandomDelay;
	int32_t Unk44;
	int32_t SoundID1;
	int32_t SoundID2;
	unsigned char SoundType;
	unsigned char UnkPad57;
	unsigned char UnkPad58;
	unsigned char UnkPad59;
	int32_t AsDistance;
	int32_t UnkRange64;
	int32_t FadeOutMS;
	int32_t UnkRange72;
	int32_t FullVolRange;
	int32_t UnkRange80;
};

// based on http://forums.codeguru.com/showthread.php?269648-C-Structure-How-do-I-write-a-structure-to-a-file
template<typename T>
void write_pod_vector(std::ofstream& out, std::vector<T>& vect)
{
	out.write(reinterpret_cast<char*>(&vect.front()), vect.size() * sizeof(T));
}

std::string build_path(std::string directory, std::string filename)
{
	if(directory.empty())
	{
		return filename;
	}
	else
	{
		return directory + "/" + filename;
	}
}

std::string get_directory(const std::string& path)
{
	size_t found = path.find_last_of("/\\");
	if (found == std::string::npos)
	{
		return "";
	}
	return(path.substr(0, found));
}

std::string get_filename(std::string filename)
{
	const size_t last_slash_idx = filename.find_last_of("/\\");
	if (std::string::npos != last_slash_idx)
	{
		filename.erase(0, last_slash_idx + 1);
	}
	return filename;
}

std::string zone_from_sounds_eff(std::string sound_file)
{
	std::string zone;

	std::regex zone_regex("(.+)_sounds\\.eff");
	std::smatch zone_regex_matches;
	std::regex_search(get_filename(sound_file), zone_regex_matches, zone_regex);
	zone = zone_regex_matches.str(1);

	return zone;
}

std::vector<std::string> read_mp3_index(std::string directory)
{
	std::ifstream mp3_index_file(build_path(directory, "mp3index.txt"));

	if (!mp3_index_file.is_open())
	{
		std::cerr << "Unable to open mp3_index_file" << "\n";
		std::exit(-1);
	}

	std::vector<std::string> mp3_names;
	std::string line;
	while (std::getline(mp3_index_file, line))
	{
		mp3_names.push_back(line);
	}

	mp3_index_file.close();
	return mp3_names;
}

void append_mp3_name(std::string directory, std::string mp3_name)
{
	std::ofstream mp3_index_file(build_path(directory, "mp3index.txt"), std::ofstream::out | std::ofstream::app);

	if (!mp3_index_file.is_open())
	{
		std::cerr << "Unable to open mp3_index_file" << "\n";
		std::exit(-1);
	}

	mp3_index_file << std::endl << mp3_name;
	mp3_index_file.close();
}

// TODO refactor so that we dont keep opening and closing the file
int update_mp3_index(std::string directory, std::string mp3_name)
{
	std::vector<std::string> mp3_names;
	int mp3_index;
	mp3_names = read_mp3_index(directory);

	auto it = std::find(mp3_names.begin(), mp3_names.end(), mp3_name);
	if (it == mp3_names.end())
	{
		// name not in vector
		append_mp3_name(directory, mp3_name);
		mp3_index = mp3_names.size() + 1;
		printf("Appending %s to mp3index and using index: %d \n", mp3_name.c_str(), mp3_index);
	}
	else
	{
		auto index = std::distance(mp3_names.begin(), it);
		printf("%s found on mp3index line %ld\n", mp3_name.c_str(), index + 1);
		mp3_index = index + 1;
	}

	return mp3_index;
}

void process_zone_eff(std::string sound_file)
{
	std::vector<EffSoundEntry> sound_entries;
	std::string directory = get_directory(sound_file);
	std::string zone = zone_from_sounds_eff(sound_file);

	std::string src_filename = get_filename(sound_file);
	std::string dest_filename = src_filename;
	// std::string dest_filename = src_filename + "-patched";

	std::string full_src_path = build_path(directory, src_filename);
	std::string full_dest_path = build_path(directory, dest_filename);

	std::ifstream eff_file(full_src_path, std::ios::binary);

	std::string xmi_mp3_filename;

	int num_changes = 0;

	if (!eff_file.is_open())
	{
		std::cerr << "Unable to open eff_file: " << full_src_path << std::endl;
		std::exit(-1);
	}

	std::cout << "Reading " << full_src_path << std::endl << std::endl;
	while (!eff_file.eof())
	{
		EffSoundEntry sound_entry;

		eff_file.read((char*)&sound_entry, sizeof(sound_entry));
		if (eff_file.gcount() < sizeof(sound_entry))
		{
			break; // Ran out of file before we had a full new sound entry
		}

		int pad = 15;
		std::cout << "Sound entry parsed" << std::endl;
		std::cout << std::setw(pad) << "UnkRef00 " << sound_entry.UnkRef00 << std::endl;
		std::cout << std::setw(pad) << "UnkRef04 " << sound_entry.UnkRef04 << std::endl;
		std::cout << std::setw(pad) << "Reserved " << sound_entry.Reserved << std::endl;
		std::cout << std::setw(pad) << "Sequence " << sound_entry.Sequence << std::endl;
		std::cout << std::setw(pad) << "Loc " << sound_entry.X << "," << sound_entry.Y << "," << sound_entry.Z << std::endl;
		std::cout << std::setw(pad) << "Radius " << sound_entry.Radius << std::endl;
		std::cout << std::setw(pad) << "Cooldown1 " << sound_entry.Cooldown1 << std::endl;
		std::cout << std::setw(pad) << "Cooldown2 " << sound_entry.Cooldown2 << std::endl;
		std::cout << std::setw(pad) << "RandomDelay " << sound_entry.RandomDelay << std::endl;
		std::cout << std::setw(pad) << "Unk44 " << sound_entry.Unk44 << std::endl;
		std::cout << std::setw(pad) << "SoundID1 " << sound_entry.SoundID1 << std::endl;
		std::cout << std::setw(pad) << "SoundID2 " << sound_entry.SoundID2 << std::endl;
		std::cout << std::setw(pad) << "SoundType " << sound_entry.SoundType - 0 << std::endl;
		std::cout << std::setw(pad) << "UnkPad57 " << sound_entry.UnkPad57 - 0 << std::endl;
		std::cout << std::setw(pad) << "UnkPad58 " << sound_entry.UnkPad58 - 0 << std::endl;
		std::cout << std::setw(pad) << "UnkPad59 " << sound_entry.UnkPad59 - 0 << std::endl;
		std::cout << std::setw(pad) << "AsDistance " << sound_entry.AsDistance << std::endl;
		std::cout << std::setw(pad) << "UnkRange64 " << sound_entry.UnkRange64 << std::endl;
		std::cout << std::setw(pad) << "FadeOutMS " << sound_entry.FadeOutMS << std::endl;
		std::cout << std::setw(pad) << "UnkRange72 " << sound_entry.UnkRange72 << std::endl;
		std::cout << std::setw(pad) << "FullVolRange " << sound_entry.FullVolRange << std::endl;
		std::cout << std::setw(pad) << "UnkRange80 " << sound_entry.UnkRange80 << std::endl << std::endl;

		if (sound_entry.SoundType == 1 && sound_entry.SoundID1 == sound_entry.SoundID2 && sound_entry.SoundID1 >= 0)
		{
			std::string subsong = std::to_string(sound_entry.SoundID1 + 1);
			xmi_mp3_filename = zone + ".xmi(" + subsong + ").mp3";
			int mp3_index = update_mp3_index(directory, xmi_mp3_filename) * -1;
			sound_entry.SoundID1 = mp3_index;
			sound_entry.SoundID2 = mp3_index;

			std::cout << "Updating subsong " << subsong << " to reference " << xmi_mp3_filename << " with an index of " <<  mp3_index << std::endl << std::endl;
			num_changes++;
		}

		sound_entries.push_back(sound_entry);
	}
	eff_file.close();

	std::ofstream patched_eff_file(full_dest_path, std::ios::binary);
	if (!patched_eff_file.is_open())
	{
		std::cerr << "Unable to open patched_eff_file: " << full_dest_path << std::endl;
		std::exit(-1);
	}

	if (num_changes > 0)
	{
		write_pod_vector(patched_eff_file, sound_entries);
		std::cout << full_dest_path << " updated with " << num_changes << " changes" << std::endl;
	}
	else
	{
		std::cout << "No changes" << std::endl;
	}

	patched_eff_file.close();
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Wrong number of arguments, try: eqeff qeytoqrg_sounds.eff" << "\n";
		return -1;
	}

	process_zone_eff(argv[1]);

	// std::cout << std::endl << "Done. Press any key to exit.";
	// std::cin.get();
	return 0;
}
