// origem: https://www.geeksforgeeks.org/wildcard-pattern-matching
//         https://www.geeksforgeeks.org/wildcard-character-matching/
//         https://stackoverflow.com/questions/35389757/function-to-count-the-amount-of-times-two-string-match-in-c
//         https://www.geeksforgeeks.org/frequency-substring-string/
			
// C++ program to implement wildcard 
// pattern matching algorithm 
#include <bits/stdc++.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
using namespace std; 

#ifndef VETSIZE
#define VETSIZE 5000000
#endif

std::stringstream vetor[VETSIZE];
int resultado[VETSIZE];
int auxbuf[VETSIZE];

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
		
		int rows = 0;
		int count = 0;
		std::ifstream file(filepath);
		std::string line;
		
		
		//iniciando mpi
		
		int size, rank;
		int val;
		MPI_Status status;
		
		cout << "chegou aqui" << endl;

		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		
		
		cout << "size: " << size << endl;
		cout << "rank: " << rank << endl;

		double t1,t2;
		t1 = MPI_Wtime();  // inicia a contagem do tempo

		int chunksize = 50000/size;//VETSIZE/size;
		
		//master
		if(rank == 0){
			//preenchendo vetor que será usado
			while (std::getline(file, line)) {
				vetor[rows] << line;
				rows++;
				//cout << rows << endl;
			}
			for(int i = rows; i<VETSIZE; i++){
				resultado[i] = 0;
				//cout << "completando resultado: " << i << endl;
			}
			//calcula chunksize
			//chunksize = rows/size;
			cout << "chunksize: " << chunksize << endl;
			
			//envia chunk pra cada processo
			for(int i=1; i<size; i++) {
				int begin = i*chunksize;
				//cout << "enviando chunk para filho: " << i << endl; 
				//cout << " com begin: " << begin << endl;
				MPI_Send(&vetor[begin], chunksize, MPI_INT,	i, 100, MPI_COMM_WORLD);
				//cout << "enviou" << endl;
			}
			
			cout << "indo processar chunk local" << endl;
			
			// Mestre processa chunk local
			for(int i=0; i<chunksize; i++) {
				cout << "processando " << i << "/" << chunksize << endl;
				resultado[i] = countFreq(pattern, vetor[i].str().c_str());
				cout << "foi " << i << "/" << chunksize << endl;
			}
			
			cout << "indo preceber respostas" << endl;
			
			// Recebe respostas dos outros processos
			for(int i=1; i<size; i++) {
				//int begin = i*chunksize;
				//MPI_Recv(&resultado[i], chunksize, MPI_INT,	i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				//count += resultado[i];
				MPI_Recv(auxbuf, chunksize, MPI_INT,
							MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				int source = status.MPI_SOURCE;
				int begin = source*chunksize;
				memcpy(&resultado[begin], auxbuf, chunksize*sizeof(int));
			}
			
		}
		else{
			cout << "nao eh processo 0" << endl;
			cout << "chunksize: " << chunksize << endl;
			
			// Recebe chunk
			MPI_Recv(vetor, chunksize, MPI_INT, 0,
			MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			cout << "chunksize no filho: " << chunksize << endl;

			// Processa chunk
			//for(int j=chunkini; j<chunksize; j++) {
				//cout << "j:" << j << endl;
				//cout << vetor[j].str().c_str() << endl;
			//}

			// Processa chunk
			for(int j=chunksize*rank; j<(chunksize*(rank+1))-1; j++) {
				//cout << "processando no filho: " << j << endl;
				resultado[j] = countFreq(pattern, vetor[j].str().c_str());
				//cout << "processou no filho: " << j << " " << resultado[j] << endl;
			}

			// Envia respostas
			MPI_Send(resultado, chunksize, MPI_INT, 0,
			100, MPI_COMM_WORLD);
		}
		
		if(rank == 0) {
			// Dump resultado
			for(int i=0; i<VETSIZE; i++) {
				count += resultado[i];
			}
			cout << "TOTAL: " << count << endl;
		
		
			t2 = MPI_Wtime(); // termina a contagem do tempo
		
			cout << "count: " << count << endl;
			printf("\nTempo de execucao: %f\n\n", t2-t1);
	
			MPI_Finalize();
		}
		
	}

	cout << "\n\nEnd !!!" << endl;
	cout << "Pedro L. Fraga" << endl;
	cout << "Rodrigo L. Silveira" << endl;
	
	return 0; 
} 
