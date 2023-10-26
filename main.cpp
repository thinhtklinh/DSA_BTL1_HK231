#include "Restaurant.cpp"
#include <fstream>

void simulate(string filename, imp_res* r) {
    ifstream ss(filename);
    string str, maxsize, name, energy, num;
    while (ss >> str) {
        if (str == "MAXSIZE") {
            ss >> maxsize;
            MAXSIZE = stoi(maxsize);
        }
		
		else if (str == "RED") // RED <NAME> <ENERGY>
        {
            //cout << "RED\n";
            ss >> name;
            ss >> energy;
            r->RED(name, stoi(energy));
        }
		
		else if (str == "BLUE") // BLUE <NUM>
        {
            // cout << "BLUE\n";
            ss >> num;
            r->BLUE(stoi(num));
        }
		
		else if (str == "PURPLE") // PURPLE
        {
            // cout << "PURPLE\n";
            r->PURPLE();
        }
		
		else if (str == "REVERSAL") // REVERSAL
        {
            //cout << "REVERSAL\n";
            r->REVERSAL();
        }
		
		else if (str == "UNLIMITED_VOID") // UNLIMITED_VOID
        {
            // cout << "UNLIMITED_VOID\n";
            r->UNLIMITED_VOID();
        }
		
		else if (str == "DOMAIN_EXPANSION") // DOMAIN_EXPANSION
        {
            r->DOMAIN_EXPANSION();
        }
		
		else // LIGHT <NUM>
        {
            ss >> num;
            cout << "LIGHT "<<num<<"\n";
            r->LIGHT(stoi(num));
            // cout << "-----------------------------------\n";
        }
    }
}

int main(int argc, char* argv[]) {
    // if (argc < 2)
    //	return 1;
    //  string fileName = argv[1];
    imp_res* r = new imp_res();
    string fileName = "test2.txt";
    simulate(fileName, r);
    delete r;
    return 0;
}
