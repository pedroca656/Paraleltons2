// origem: https://www.geeksforgeeks.org/wildcard-pattern-matching
//         https://www.geeksforgeeks.org/wildcard-character-matching/
//         https://stackoverflow.com/questions/35389757/function-to-count-the-amount-of-times-two-string-match-in-c
//         https://www.geeksforgeeks.org/frequency-substring-string/
			
// C++ program to implement wildcard 
// pattern matching algorithm 
#include <bits/stdc++.h> 
#include <omp.h>
using namespace std; 


int countFreq(const char* pat, const char* txt) 
{ 
    int res = 0; 
    
    int lenPat = strlen(pat);
    int lenTxt = strlen(txt);
    
    int idxPat, idxIni, idxTxt;
    
    bool bOk = false;
    bool bCoringa = false;
    
    //omp_set_num_threads(6);
    //#pragma omp parallel private ( idxPat, idxIni, idxTxt, bOk, bCoringa )
    //#pragma omp for schedule (static)
    for (idxIni = 0; idxIni < lenTxt; idxIni++) {
		
		idxPat = 0;
		idxTxt = idxIni;
		bOk = false;
		
		for (idxPat = 0; idxPat < lenPat; ) {
		
			if (idxTxt >= lenTxt) break;
			
			/*
			cout << "Ini: " << idxIni << "\t"
				<< pat[idxPat] << "(" << idxPat << ")/" 
				<< txt[idxTxt] << "(" << idxTxt << ")" << endl;
			//*/
			
			
			if (pat[idxPat] == txt[idxTxt] || pat[idxPat] == '?'){
			
				bOk = true;
				bCoringa = false;
				idxPat++; 
				idxTxt++;
			
			} else {
			
				if (bCoringa) {
					
					idxTxt++;
					
				}
				else if (pat[idxPat] == '*'){
					
					bCoringa = true;
					idxPat++;
					
				} else {
					
					bOk = false;
					break;
					
				}		
			}
			
		}
		
		if (bOk && idxPat == lenPat) {
			res++;
			//cout << "match!" << endl;
		}
		
	}
    
    return res;
}    
  
int main(int argc, char* argv[]) 
{ 
        
	// Check the number of parameters
    if (argc < 2) {
        // Tell the user how to run the program
        cout << "Usage: " << argv[0] << " pattern (to digit a word) or" << std::endl;
        cout << "Usage: " << argv[0] << " pattern file (to read words from file)" << std::endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }
    
    char pattern[150];
    strcpy(pattern, argv[1]);
     
    cout << "Pattern is " << pattern << endl;
    
    
    if (argc == 2) {
		// read from output
		
		char word[150];
		while (true) {
			
			cout << "Write a word (q to quit): ";
			cin >> word;
			
			if (strcmp("q", word) == 0) break;
			
			cout << "count(" << word << "): " << countFreq(pattern, word) << endl;
			
		}
		
	}
	else if (argc == 3){
		// read from file
		const int intervalo = 5000;
		double starttime, stoptime;
		
		char filepath[150];
		strcpy(filepath, argv[2]);
		cout << "Read file " << filepath << endl;
		
		int row = 0;
		int count = 0;
		std::ifstream file(filepath);
		std::string line;
		std::stringstream l;
		while (std::getline(file, line)) {
			// line contains the current line
			//cout << "count(" << line << "): " << countFreq(pattern, line.c_str()) << endl;
			//count += countFreq(pattern, line.c_str());
			l << line;
		}
		
		
		starttime = omp_get_wtime(); 
		
		count = countFreq(pattern, l.str().c_str());
		
		cout << "count: " << count << endl;
		stoptime = omp_get_wtime();
		printf("Tempo de execução: %3.2f segundos\n", stoptime-starttime);
	}
	

	cout << "\n\nEnd !!!" << endl;
	cout << "Pedro L. Fraga" << endl;
	cout << "Rodrigo L. Silveira" << endl;
	
	return 0; 
} 