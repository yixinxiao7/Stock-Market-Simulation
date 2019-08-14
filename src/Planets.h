#include<queue>
#include<functional>

using namespace std;
struct General {
	unsigned int num_lost = 0;
	unsigned int num_jedi = 0;
	unsigned int num_sith = 0;
};

struct Battalion {
	int timestamp = 0;
	int gen_id = 0;
	int force_sense = -1;
	int num_troops = 0;
	int id;
};

struct sortLtoG {//jedi
	bool operator() (const Battalion &i, const Battalion &j) const {
		if (i.force_sense != j.force_sense) {
			return i.force_sense > j.force_sense;
		}
		//the forces are equal to each other, consider timestamps
		return i.id > j.id;
	}
};

struct sortGtoL {//sith
	bool operator() (const Battalion &i, const Battalion &j) const {
		if (i.force_sense != j.force_sense) {
			return i.force_sense < j.force_sense;
		}
		//the forces are equal to each other, consider timestamps
		return i.id > j.id;//double check
	}
};

class Planet {
private:
	unsigned int planetID;
public:
	Planet(int in_planetID) :
		planetID(in_planetID) {}

	priority_queue<Battalion, vector<Battalion>, sortLtoG > Jedi;
	priority_queue<Battalion, vector<Battalion>, sortGtoL > Sith;
	priority_queue<int> returnBiggest;
	priority_queue<int, vector<int>, greater<int> > returnSmallest;
	
	Battalion jedi_am;
	Battalion sith_am;
	Battalion sith_am2;
	bool jedi_exist = false;

	Battalion jedi_at;
	Battalion sith_at;
	Battalion jedi_at2;//?
	bool sith_exist = false;
	
	unsigned int counter_am = 0;
	unsigned int counter_at = 0;

	Battalion get_jedi_am() const {
		return jedi_am;
	}
	Battalion get_sith_am() const {
		return sith_am;
	}
	Battalion get_jedi_at() const {
		return jedi_at;
	}
	Battalion get_sith_at() const {
		return sith_at;
	}

	void update_movie_watcher(string side, const Battalion &b) {
		//ambush
		if (side == "SITH" && counter_am == 0) {//find first sith
			sith_am = b;
			++counter_am;
		}
		else if (side == "JEDI" && counter_am == 1) {//found first sith already
			if (b.force_sense <= sith_am.force_sense) {//if jedis force sense is less than or equal sith_am
				if (jedi_am.force_sense == -1) {
					jedi_am = b;
					jedi_exist = true;
				}
				else if (b.force_sense < jedi_am.force_sense) {//only consider jedis force which is less than current jedi 
					jedi_am = b;
					jedi_exist = true;

				}
				if (sith_am2.force_sense != -1) {//now check with sith_am2
					if ((sith_am2.force_sense - b.force_sense) > (sith_am.force_sense - jedi_am.force_sense)) {
						//the gap between the new jedi and the potential sith is greater
						sith_am = sith_am2;
						reset_battalion(sith_am2);
						jedi_am = b;//necessary?///////////////////@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
					}
				}
			}
			else if (sith_am2.force_sense != -1) {
				if ((sith_am2.force_sense - b.force_sense) > (sith_am.force_sense - jedi_am.force_sense)) {
					sith_am = sith_am2;
					reset_battalion(sith_am2);
					jedi_am = b;
				}
			}
			//otherwise skip
		}
		else if (side == "SITH" && counter_am == 1) {//found another sith
			if (!jedi_exist) {//jedi doesn't exist!!
				if (b.force_sense > sith_am.force_sense) {
					sith_am = b;
				}
			}
			else {//jedi already exists
				if (b.force_sense > sith_am.force_sense) {//but jedi has lower than previous
					if (sith_am2.force_sense == -1) {//jedi_at2 never existed
						sith_am2 = b;
					}
					else {//sith_am2 does exist
						if (b.force_sense > sith_am2.force_sense) {
							sith_am2 = b;
						}
					}
				}
				//otherwise skip
			}
			/*
			//if sith higher than previous
			if (b.force_sense > sith_am.force_sense) {
				if (sith_am2.force_sense == -1) {//if no sith_am2
					sith_am = b;//replace old one
				}
				else {
					sith_am2 = b;
				}
			}
			//otherwise skip
			*/
		}

		//attack
		if (side == "JEDI" && counter_at == 0) {//find first jedi
			jedi_at = b;
			++counter_at;
		}
		else if (side == "SITH" && counter_at == 1) {//found first jedi already
			if (jedi_at.force_sense <= b.force_sense) {//if current jedis force sense is less than or equal to sith_am
				if (b.force_sense > sith_at.force_sense) {//only consider sith force which is higher than the current
					sith_at = b;
					sith_exist = true;
				}
				if (jedi_at2.force_sense != -1) {//now check with jedi_at2
					if ((b.force_sense - jedi_at2.force_sense) > (sith_at.force_sense - jedi_at.force_sense)) {
						//the gap between the new sith and the potential jedi is greater
						jedi_at = jedi_at2;
						reset_battalion(jedi_at2);
						sith_at = b;//necessary?///////////////////@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
					}
				}
			}
			else if (jedi_at2.force_sense != -1) {//but what if the siths force is less than jedi_at force, but much greater than jedi_2 force?
				if ((b.force_sense - jedi_at2.force_sense) > (sith_at.force_sense - jedi_at.force_sense)) {
					jedi_at = jedi_at2;
					reset_battalion(jedi_at2);
					sith_at = b;
				}
			}
			//otherwise skip
		}
		else if (side == "JEDI" && counter_at == 1) {//found another JEDI
			if (!sith_exist) {//sith doesn't exist!!
				if (b.force_sense < jedi_at.force_sense) {
					jedi_at = b;
				}
			}
			else {//sith already exists
				if (b.force_sense < jedi_at.force_sense) {//but jedi has lower than previous
					if (jedi_at2.force_sense == -1) {//jedi_at2 never existed
						jedi_at2 = b;
					}
					else {//jedi_at2 does exist
						if (b.force_sense < jedi_at2.force_sense) {
							jedi_at2 = b;
						}
					}
				}
				//otherwise skip
			}
		}
	}


	void reset_battalion(Battalion &b){
		b.timestamp = 0;
		b.gen_id = 0;
		b.force_sense = -1;
		b.num_troops = 0;
	}

	int get_planetID() {
		return planetID;
	}
	void set_planetID(int id) {
		planetID = id;
	}
};
