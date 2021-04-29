#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>

/* These are the only two global variables allowed in your program */
static int verbose = 0;
static int use_fork = 0;

typedef struct Params //struct to hold random parameters I don't know yet.
{
	int grid[9];
	char str[99];
	// ZERO IS NO ERROR
	int error;
}sudokuData;





// This is a simple function to parse the --fork argument.
// It also supports --verbose, -v
void parse_args(int argc, char *argv[])
{
    int c;
    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"fork",    no_argument,       0, 'f'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long (argc, argv, "vf", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {
            case 'f':
                use_fork = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                exit(1);
        }
    }
}


void *validRow(void *data){

	sudokuData* info = data;

	int	checkArray[9] = { 0 }; //initialize an array full of zeros
	for (int i = 0; i < 9; i++){
		checkArray[info->grid[i]-1] += 1;
		if(info->grid[i] > 9){
  	}
		//checkArray[info->grid[info->row][i]] += 1;
		// ^^^ adds 1 to corresponding index in check array if
		// value is found in a given grids row.
	}
	for(int j = 0; j < 9; j++){ //print checkArray for testing.
		if( checkArray[j] == 0 ){
			info->error = 1;
		}//if there is a 0 at any index, some value is missing
	}
	return NULL;
} // End validRow

void * validRowForkTwo(void *data){
	sudokuData * info = data;

	int	checkArray[9] = { 0 }; //initialize an array full of zeros
	for (int i = 0; i < 9; i++){
		checkArray[info->grid[i]-1] += 1;
		if(info->grid[i] > 9){
  	}
		//checkArray[info->grid[info->row][i]] += 1;
		// ^^^ adds 1 to corresponding index in check array if
		// value is found in a given grids row.
	}
	for(int j = 0; j < 9; j++){ //print checkArray for testing.
		if( checkArray[j] == 0 ){
			info->error = 1;
		}//if there is a 0 at any index, some value is missing
	}
	return info;
}
int main(int argc, char *argv[])
{


	  parse_args(argc, argv);
	 
	  if ( use_fork) {
		 	 printf("We are utilizing child processes.\n");
	  } else {
		 	 printf("We are using worker threads.\n");
	  }



	int puzz[9][9];
	sudokuData data;
	pthread_t tid[27];
	fseek(stdin, 0 , SEEK_SET);

	for(int i = 0; i < 9; i++){ //looping through the text file, storing values into grid.
		for(int k = 0; k < 9; k++){
			fscanf(stdin, "%d", &(puzz[i][k]));
			fseek(stdin,1,SEEK_CUR);
		}
	}



	struct Params arrayHolder[27];

	int arrIndexCounter = 0;
	// SETTING UP ROWS
	for(int p = 0; p<9; p++){
		sudokuData values;
		for(int i = 0; i < 9; i++){
			values.grid[i] = puzz[p][i];
		}
		values.error = 0;
		snprintf(values.str, 99, "Row %d ", p+1);
		// SET ROW/ COL AS WELL
		arrayHolder[arrIndexCounter] = values;
		arrIndexCounter++;

		}

		// SETTING UP COLS
		for(int p = 0; p<9; p++){
			sudokuData values;
			for(int i = 0; i < 9; i++){
				values.grid[i] = puzz[i][p];
			}
			snprintf(values.str, 99, "Col %d ", p+1);
			values.error = 0;
			arrayHolder[arrIndexCounter] = values;
			arrIndexCounter++;
		}

		// SETTING UP 3x3 far left col
		for(int p = 0; p<9; p=p+3){
			sudokuData values;
			int counter = 0;
			for(int i = 0; i < 3; i++){
				for(int k = 0; k<3; k++){
					values.grid[counter] = puzz[i+p][k];
					if(p == 0){
						char str1[99] = "The upper left subgrid ";
						strcpy(values.str, str1);
					}
					else if(p == 3){
						char str1[99] = "The middle left subgrid ";
						strcpy(values.str, str1);
					}
					else if(p == 6){
						char str1[99] = "The lower left subgrid ";
						strcpy(values.str, str1);
					}
					counter++;
				}
			}
			values.error = 0;
			arrayHolder[arrIndexCounter] = values;
			arrIndexCounter++;
		}

		// SETTING UP 3x3 MIDDLE COL, just cant be asked to do it in a better way
		for(int p = 0; p<9; p=p+3){
			sudokuData values;
			int counter = 0;
			for(int i = 0; i < 3; i++){
				for(int k = 0; k<3; k++){
					values.grid[counter] = puzz[i+p][k+3];
					counter++;
				}
			}
			if(p == 0){
				char str1[99] = "The upper middle subgrid ";
				strcpy(values.str, str1);
			}
			else if(p == 3){
				char str1[99] = "The middle middle subgrid ";
				strcpy(values.str, str1);
			}
			else if(p == 6){
				char str1[99] = "The lower middle subgrid ";
				strcpy(values.str, str1);
			}
			values.error = 0;
			arrayHolder[arrIndexCounter] = values;
			arrIndexCounter++;
		}
		// SETTING UP 3x3 3rd COL, again so bad but just cant be asked to be smarter
		for(int p = 0; p<9; p=p+3){
			sudokuData values;
			int counter = 0;
			for(int i = 0; i < 3; i++){
				for(int k = 0; k<3; k++){
					values.grid[counter] = puzz[i+p][k+6];
					counter++;
				}
			}
			if(p == 0){
				char str1[99] = "The upper right subgrid ";
				strcpy(values.str, str1);
			}
			else if(p == 3){
				char str1[99] = "The middle right subgrid ";
				strcpy(values.str, str1);
			}
			else if(p == 6){
				char str1[99] = "The lower right subgrid ";
				strcpy(values.str, str1);
			}
			values.error = 0;
			arrayHolder[arrIndexCounter] = values;
			arrIndexCounter++;
		}

	if(use_fork){
		pid_t child_pid;
		int status = 0;
		for(int p = 0; p < 27; p++){
			sudokuData * temp;
			if((child_pid = fork()) == 0){
				validRowForkTwo((void*)&arrayHolder[p]);
				if(arrayHolder[p].error == 1){
					printf("%s", arrayHolder[p].str);
					printf("%s", "doesn't have the required values");
					printf("%s","\n");
					exit(1);
				}
				else{
					exit(0);
				}
			}

		}
		int ret = 0;
		while(wait(&status) > 0){
			if(WEXITSTATUS(status) == 1){
				ret = 1;
			}
		}
		if(ret == 1){
			printf("%s", "The input is not a valid sudoku");
		}
		else{
			printf("%s", "The input is a valid sudoku");
		}
	}
	else{

		for(int k = 0; k < 27; k++){
			pthread_create(&tid[k], NULL, validRow, (void*)&arrayHolder[k]);
		}

		for(int i = 0; i < 27; i++){
			pthread_join(tid[i], NULL);
		}
		int anyError = 0;
		for(int k = 0; k < 27; k++){
			if(arrayHolder[k].error == 1){
				anyError = 1;
				printf("%s", arrayHolder[k].str);
				printf("%s", "doesn't have the required values");
				printf("%s","\n");
			}
		}
		if(anyError == 1){
			printf("%s", "The input is not a valid Sudoku.\n");
		}
		else{
			printf("%s", "The input is a valid Sudoku.\n");
		}

	}
    return 0;
}
