#include <getopt.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include "Planets.h"
#include "P2random.h"

using namespace std;

void options(int argc, char * argv[], char &v, char &m, char &g, char &w) {
	opterr = true;
	int choice;
	int option_index = 0;
	option long_options[] = {
		{ "verbose",  no_argument, nullptr, 'v'},
		{ "median", no_argument, nullptr, 'm'},
		{ "general-eval", no_argument, nullptr, 'g'},
		{ "watcher", no_argument, nullptr, 'w'},
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((choice = getopt_long(argc, argv, "vmgw", long_options, &option_index)) != -1) {
		switch (choice) {
		case 'v':
			v = 'o';
			break;
		case 'm':
			m = 'o';
			break;
		case 'g':
			g = 'o';
			break;
		case 'w':
			w = 'o';
			break;
		default:
			cerr << "Error: invalid option" << '\n';
			exit(1);
		} //switch
	} // while
}

void fill_b(Battalion &b, int ts, int gen_id, int force_sense, int num_troops, int id) {
	b.timestamp = ts;
	b.gen_id = gen_id;
	b.force_sense = force_sense;
	b.num_troops = num_troops;
	b.id = id;
}

int Lesser_Units(int i, int j) {
	if (i < j) {
		return i;
	}
	return j;
}

void movie_watcher(const Planet &planet, int i) {
	Battalion jedi_ambushed = planet.get_jedi_am();
	Battalion sith_ambusher = planet.get_sith_am();
	Battalion jedi_attacked = planet.get_jedi_at();
	Battalion sith_attacker = planet.get_sith_at();
	
	if (jedi_ambushed.force_sense == -1 || sith_ambusher.force_sense == -1) {
		cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time -1 and Jedi at time -1." << '\n';
	}
	else {
		cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time "
			<< sith_ambusher.timestamp << " and Jedi at time " << jedi_ambushed.timestamp << '.' << '\n';
	}
	if (jedi_attacked.force_sense == -1 || sith_attacker.force_sense == -1) {
		cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time -1 and Sith at time -1." << '\n';
	}
	else {
		cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time "
			<< jedi_attacked.timestamp << " and Sith at time " << sith_attacker.timestamp << '.' << '\n';
	}
}

void Subract_Units(Battalion &b1, Battalion &b2, int lost_units) {
	b1.num_troops -= lost_units;
	b2.num_troops -= lost_units;
}

void verbose(int gen_id, int gen_id2, int planet, int lost_units) {
	cout << "General " << gen_id << "'s battalion attacked General "
		<< gen_id2 << "'s battalion on planet " << planet << ". "
		<< (lost_units * 2) << " troops were lost." << '\n';
}

void median(vector<Planet> &Planets, int planet_id, int total_lost_units) {
	if (Planets[planet_id].returnBiggest.empty() && Planets[planet_id].returnSmallest.empty()) {
		Planets[planet_id].returnBiggest.push(total_lost_units);
	}
	else {
		if (total_lost_units < Planets[planet_id].returnBiggest.top()) {
			if (Planets[planet_id].returnBiggest.size() > Planets[planet_id].returnSmallest.size()) {
				int transfer = Planets[planet_id].returnBiggest.top();
				Planets[planet_id].returnBiggest.pop();
				Planets[planet_id].returnSmallest.push(transfer);
			}
			Planets[planet_id].returnBiggest.push(total_lost_units);
		}
		else if (total_lost_units == Planets[planet_id].returnBiggest.top()) {
			if (Planets[planet_id].returnBiggest.size() > Planets[planet_id].returnSmallest.size()) {
				Planets[planet_id].returnSmallest.push(total_lost_units);
			}
			else {
				Planets[planet_id].returnBiggest.push(total_lost_units);
			}
		}
		else {
			if (Planets[planet_id].returnSmallest.size() > Planets[planet_id].returnBiggest.size()) {
				int transfer = Planets[planet_id].returnSmallest.top();
				Planets[planet_id].returnSmallest.pop();
				Planets[planet_id].returnBiggest.push(transfer);
			}
			Planets[planet_id].returnSmallest.push(total_lost_units);
			if (Planets[planet_id].returnBiggest.top() > Planets[planet_id].returnSmallest.top()) {
				int transfer = Planets[planet_id].returnBiggest.top();
				Planets[planet_id].returnBiggest.pop();
				int transfer2 = Planets[planet_id].returnSmallest.top();
				Planets[planet_id].returnSmallest.pop();

				//restore
				Planets[planet_id].returnBiggest.push(transfer2);
				Planets[planet_id].returnSmallest.push(transfer);
			}
		}
	}
}

void median_print(const priority_queue<int> &returnLargest, const priority_queue<int, vector<int>, greater<int> > &returnSmallest, int planet_id, int time_stamp) {
	if (returnSmallest.size() != 0 || returnLargest.size() != 0) { 
		if (returnLargest.size() > returnSmallest.size()) {
			cout << "Median troops lost on planet " << planet_id
				<< " at time " << time_stamp << " is " << returnLargest.top() << '.' <<'\n';
		}
		else if (returnSmallest.size() > returnLargest.size()) {
			cout << "Median troops lost on planet " << planet_id
				<< " at time " << time_stamp << " is " << returnSmallest.top() << '.' <<'\n';
		}
		else {
			cout << "Median troops lost on planet " << planet_id
				<< " at time " << time_stamp << " is "
				<< ((returnLargest.top() + returnSmallest.top()) / 2) << '.' << '\n';
		}
	}
	else return;
}

void general_evaluation(General g, int i) {
	cout << "General " << i << " deployed " << g.num_jedi << " Jedi troops and " 
		<< g.num_sith << " Sith troops, and " << ((g.num_jedi + g.num_sith) - g.num_lost) 
		<< "/" << (g.num_jedi + g.num_sith) << " troops survived." << '\n';
}

void battles(vector<Planet> &Planets, Battalion &b, int planet_id, string side, int &num_battles, char v, vector<General> &General, char g, char m) {	
	if (side == "SITH") {
		General[b.gen_id].num_sith += b.num_troops;
		bool just_add_it = true;
		bool sith_won = true;
		if (!Planets[planet_id].Jedi.empty()) {
			just_add_it = false;
			Battalion jedi = Planets[planet_id].Jedi.top();
			while (jedi.force_sense <= b.force_sense && b.num_troops > 0) {
				++num_battles;
				int lost_units = 0;
				lost_units = Lesser_Units(jedi.num_troops, b.num_troops);
				if (g == 'o'){
					General[b.gen_id].num_lost += lost_units;
					General[jedi.gen_id].num_lost += lost_units;
				}
				if (m == 'o') {
					int total_lost_units = 2 * lost_units;
					median(Planets, planet_id, total_lost_units);
				}
				Planets[planet_id].Jedi.pop();
				Subract_Units(jedi, b, lost_units);
				
				if (v == 'o') {
					verbose(b.gen_id, jedi.gen_id, planet_id, lost_units);
				}

				if (jedi.num_troops != 0) {
					Planets[planet_id].Jedi.push(jedi);
					sith_won = false;
					break;
				}
				if (b.num_troops != 0) {
					if (!Planets[planet_id].Jedi.empty()) {
						jedi = Planets[planet_id].Jedi.top();
					}
					else {
						break;
					}	
				}
			}//while
			if (sith_won && b.num_troops > 0) {
				Planets[planet_id].Sith.push(b);
			}
		}//if empty
		if (just_add_it) {
			Planets[planet_id].Sith.push(b);
		}
	}//if SITH

	if (side == "JEDI") {
		General[b.gen_id].num_jedi += b.num_troops;
		bool just_add_it = true;
		bool jedi_won = true;
		if (!Planets[planet_id].Sith.empty()) {
			just_add_it = false;
			Battalion sith = Planets[planet_id].Sith.top();
			while (b.force_sense <= sith.force_sense && b.num_troops > 0) {
				
				++num_battles;
				int lost_units = 0;
				lost_units = Lesser_Units(b.num_troops, sith.num_troops);
				if (g == 'o') {
					General[b.gen_id].num_lost += lost_units;
					General[sith.gen_id].num_lost += lost_units;
				}
				
				if (m == 'o') {
					int total_lost_units = 2 * lost_units;
					median(Planets, planet_id, total_lost_units);
				}
				
				Planets[planet_id].Sith.pop();

				Subract_Units(b, sith, lost_units);

				if (v == 'o') {
					verbose(sith.gen_id, b.gen_id, planet_id, lost_units);
				}
				if (sith.num_troops != 0) {
					Planets[planet_id].Sith.push(sith);
					jedi_won = false;
					break;
				}
				if (b.num_troops != 0) {
					if (!Planets[planet_id].Sith.empty()) {
						sith = Planets[planet_id].Sith.top();
					}
					else {
						break;
					}
				}
			}//while
			if (jedi_won && b.num_troops > 0) {
				Planets[planet_id].Jedi.push(b);
			}
		}
		if (just_add_it) {
			Planets[planet_id].Jedi.push(b);
		}
	}//if JEDI
}

void check_errors(int ts, string general, string pl,string fs, string q, int CURRENT_TIMESTAMP, int num_generals, int num_planets) {
	if (ts < 0) {
		cerr << "Error: invalid ts" << '\n';
		exit(1);
	}
	if (stoi(general.substr(1)) >= (int)num_generals || stoi(pl.substr(1)) >= (int)num_planets) {
		cerr << "Error: invalid n_g or n_pl" << '\n';
		exit(1);
	}
	if (stoi(fs.substr(1)) <= 0 || stoi(q.substr(1)) <= 0) {
		cerr << "Error: invalid force sense or num_troops" << '\n';
		exit(1);
	}
	if (CURRENT_TIMESTAMP > ts) {
		cerr << "Error: invalid timestamp" << '\n';
		exit(1);
	}
}

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false);

	int CURRENT_TIMESTAMP = 0;
	char v = 'x';
	char m = 'x';
	char g = 'x';
	char w = 'x';
	options(argc, argv, v, m, g, w);

	string mode = "";
	unsigned int num_generals = 0;
	unsigned int num_planets = 0;

	string words;

	while (cin >> words) {
		if (words == "COMMENT:") {
			getline(cin, words);
		}
		else if (words == "MODE:") {
			cin >> words;
			mode = words;
		}
		else if (words == "NUM_GENERALS:") {
			cin >> num_generals;
		}
		else if (words == "NUM_PLANETS:") {
			cin >> num_planets;
			break;
		}
	}

	vector<Planet> Planets;
	Planets.reserve(num_planets);
	for (unsigned int i = 0; i < num_planets; ++i) {
		Planet p(i);
		Planets.push_back(p);
	}

	stringstream ss;
	if (mode == "PR") {
		int seed;
		int num_deployments;
		int arrival_rate;
		while (cin >> words) {
			if (words == "RANDOM_SEED:") {
				cin >> seed;
			}
			else if (words == "NUM_DEPLOYMENTS:") {
				cin >> num_deployments;
			}
			else if (words == "ARRIVAL_RATE:") {
				cin >> arrival_rate;
				break;
			}
		}
		P2random::PR_init(ss, seed, num_generals, num_planets, num_deployments, arrival_rate);
	}

	istream& inputStream = (mode == "PR") ? ss : cin;

	cout << "Deploying troops..." << '\n';

	int ts;
	string side;
	string general;
	string pl;
	string fs;
	string q;
	words = "";

	vector<General> Generals;
	Generals.resize(num_generals);

	int num_battles = 0;
	int jedi_id = 1;
	int sith_id = 1;

	//check for errors
	while (inputStream >> ts >> side >> general >> pl >> fs >> q) {
		check_errors(ts, general, pl,fs, q, CURRENT_TIMESTAMP, num_generals, num_planets);

		Battalion b;
		if (side == "JEDI") {
			fill_b(b, ts, stoi(general.substr(1)), stoi(fs.substr(1)), stoi(q.substr(1)), jedi_id);
			++jedi_id;
		}
		else if (side == "SITH") {
			fill_b(b, ts, stoi(general.substr(1)), stoi(fs.substr(1)), stoi(q.substr(1)), sith_id);
			++sith_id;
		}
		if (b.timestamp != CURRENT_TIMESTAMP) {
			if (m == 'o') {
				for (unsigned int i = 0; i < num_planets; ++i) {
					median_print(Planets[i].returnBiggest, Planets[i].returnSmallest, i, CURRENT_TIMESTAMP);
				}
			}
			CURRENT_TIMESTAMP = b.timestamp;
		}
		
		if (w == 'o') {
			Planets[stoi(pl.substr(1))].update_movie_watcher(side, b);
		}

		battles(Planets, b, stoi(pl.substr(1)), side, num_battles, v, Generals, g, m);
	}
	if (m == 'o') {
		for (unsigned int i = 0; i < num_planets; ++i) {
			median_print(Planets[i].returnBiggest, Planets[i].returnSmallest, i, CURRENT_TIMESTAMP);
		}
	}
	cout << "---End of Day---" << '\n';
	cout << "Battles: " << num_battles << "\n";
	if (g == 'o') {
		cout << "---General Evaluation---" << '\n';
		for (unsigned int i = 0; i < num_generals; ++i) {
			general_evaluation(Generals[i], i);
		}
	}
	if (w == 'o') {
		cout << "---Movie Watcher---" << '\n';
		for (unsigned int i = 0; i < num_planets; ++i) {
			movie_watcher(Planets[i], i);
		}
	}
}
