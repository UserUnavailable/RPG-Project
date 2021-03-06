#include "../include/rpg.h"

// player
rpg::Player rpg::player("Player");

std::map<std::string, std::string> rpg::color {
	{"reset" , "\e[00m"  },
	{"red"   , "\e[1;31m"},
	{"green" , "\e[1;32m"},
	{"yellow", "\e[1;33m"},
	{"blue"  , "\e[1;34m"},
	{"purple", "\e[1;35m"},
	{"white" , "\e[1;37m"},
};

void rpg::clear() { std::cout << "\e[H\e[2J"; }

int rpg::rate(int range) {
	srand(time(NULL));
	return rand() % range + 1;
}

void rpg::hud_player(int choice) {
	if (choice == 0) {
		std::cout << rpg::color["red"] << "[" << player.type() << "] " << rpg::color["white"];
		std::cout << player.name() << "(" << player.lv() << ")" << rpg::color["reset"] << "  |  ";
		std::cout << rpg::color["green"] << "HP: " << player.hp() << rpg::color["reset"] << ", ";
		std::cout << rpg::color["blue"] << "MP: " << player.mp() << rpg::color["reset"] << "\n";
	} else {
		std::cout << rpg::color["yellow"] << "Class ..: " << rpg::color["reset"] << rpg::player.type() << "\n";
		std::cout << rpg::color["yellow"] << "Name ...: " << rpg::color["reset"] << rpg::player.name();
		std::cout << " (" << rpg::player.win() << "/" << rpg::player.lose() << ")\n";
		std::cout << rpg::color["yellow"] << "Level ..: " << rpg::color["reset"] << rpg::player.lv() << " [";
		std::cout << rpg::player.xp() << "/" << rpg::player.xp_lvl() << "]\n";
		std::cout << rpg::color["yellow"] << "Status .: " << rpg::color["reset"] << "HP " << rpg::player.hp() << ", ";
		std::cout << "MP " << rpg::player.mp() << "\n";
		std::cout << rpg::color["yellow"] << "Money ..: " << rpg::color["reset"] << rpg::player.money() << "\n";
	}
}

void rpg::hud_enemy(rpg::Enemies emy) {
	std::cout << color["red"] << emy.name() << color["reset"] << "  |  ";
	if (emy.name() == "??????") {
		std::cout << color["green"] << "HP: " << emy.name() << color["reset"] << ", ";
		std::cout << color["blue"] << "MP: " << emy.name() << color["reset"] << "\n";
	} else {
		std::cout << color["green"] << "HP: " << emy.hp() << color["reset"] << ", ";
		std::cout << color["blue"] << "MP: " << emy.mp() << color["reset"] << "\n";
	}
}

void rpg::battle(rpg::Enemies emy) {
	std::string action = "VS", damage;
	bool loop = true;
	int aws;

	auto attack_per = [&action, &damage] (auto* x, auto* y, int m) {
		int drate = rpg::rate(x->dmg_rate());
		damage = color["purple"];

		if (m == 2 && x->mp() >= 50) {
			drate *= 2;
			x->mp(x->mp() - 50); 
			damage += "(Magic) ";
		} else if (m == 3 && x->mp() >= 100) {
			drate *= 3;
			x->mp(x->mp() - 100);
			damage += "(Fire) ";
		}

		y->hp(y->hp() - drate);

		if (drate >= x->dmg_rate())
			action = x->name() + ": " + color["red"] + "*Critic*" + color["reset"];
		else
			action = x->name() + ": " + color["blue"] + "Attack" + color["reset"];
		
		damage += drate == 0 ? "*fail*" : ("-" + std::to_string(drate));
	};

	auto print_battle = [&emy, &action, &damage] (int slp) {
		rpg::clear();
		rpg::hud_enemy(emy);

		std::cout << "\n" << action << "\n";
		std::cout << damage << "\n\n";

		rpg::hud_player(0);

		sleep(slp);
	};

	while(loop) {
		if (emy.hp() == 0) {
			rpg::player.win_add();
			rpg::player.xp(rpg::player.xp() + emy.xp()); // emy.xp()
			rpg::player.money(rpg::player.money() + rpg::rate(emy.money()));
			return;
		} else if (player.hp() == 0) {
			rpg::player.lose_add();
			rpg::player.money(rpg::player.money() - rpg::rate(emy.money()));
			return;
		}

		std::cin.clear();
		std::cin.ignore();

		print_battle(0);
		std::cout << "[1] Melee, [2] Magic, [3] Fire, [0] Flee\n> ";
		
		if (std::cin >> aws) {
			switch (aws) { // player round
				case 1: attack_per(&player, &emy, 0); break;
				case 2: attack_per(&player, &emy, 2); break;
				case 3: attack_per(&player, &emy, 3); break;
				case 0: rpg::player.lose_add(); loop = false; break;
			} print_battle(1);
		}

		attack_per(&emy, &player, rpg::rate(3)); // enemy round
		print_battle(1);
	}
}

void rpg::save_game(rpg::Player& obj) {
	std::ofstream ofile("save");
	ofile << obj;
	ofile.close();
}

void rpg::load_game(rpg::Player& obj) {
	std::ifstream ifile("save");
	if (!ifile.fail()) {
		ifile >> obj;
		ifile.close();
	}
}
