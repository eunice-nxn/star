#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

char* new_archive = 0x0;

typedef struct header{

	int ftype;   /*0: directory, 1:regular file*/
	unsigned int n; /*'n' length of file path | 4 bytes*/
	unsigned int m; /*'m' length of data | 4 bytes*/
	char* path; /*the file path | 'n' bytes*/

} Header;

/**
 * It makes new header by reading file.
 * Input: the path of file to be written
 * Return: header
 * */

Header* make_header ( char* path, Header *header ){

	// Determine file type 
	struct stat sf;
	stat(path, &sf);
	if( (sf.st_mode & S_IFMT) == S_IFDIR ){
		header->ftype = 0;
	} else if ((sf.st_mode & S_IFMT) == S_IFREG ){
		header->ftype = 1;
	}	
	// Read len_path
	size_t len_path = sizeof(path) / sizeof(char) ;
	header->n = len_path;

	// Read len_data
	size_t len_data = sf.st_size;
	header->m = len_data;

	// Store the path of file 
	header->path = path;

	return header;
}

/**
 * It writes star file by taking headers
 * argument: the header of file
 * Return: header
 * */
void write_file( Header* header ){

	FILE* fp;
	fp = fopen(new_archive, "a");
	if( fp == NULL ) {
		printf("Fail to open file stream");
	}

	int result = 0;

	fwrite( &header, sizeof(header), 1, fp);
	fwrite( &header->ftype, 1, 1, fp);
	fwrite( &header->n, 4, 1, fp);
       	fwrite( &header->m, 4, 1, fp);
	fwrite( &header->path, header->n, 1, fp);
	if( result == (int) sizeof(header) ){
		printf(" result is identical to header size\n");
	}

}



int main(int argc, char* argv[]){

	char* target_path= 0x0;
	
	
	if( strcmp(argv[1], "archive") == 0 ){
		
		printf("archive mode\n");
		if ( argc != 4 ){
			printf("invalid argument.\n");
		} 

		/*argv[2] validation*/
		if( access( argv[2], F_OK ) == 0 ){
			printf("The file with that name exists. You should enter name of new file.\n");
			exit(1);	
		}

		/*argv[3] validation*/
		if( opendir (argv[3]) == NULL ){
                        printf("Can not open directory\n");
                        exit(1);
		}

		struct stat sf;
		if( stat(argv[3], &sf) == -1 ){
			printf("stat fail\n");
			exit(1);
		}

		printf("Hi\n");
		if( ( sf.st_mode & S_IFMT ) == S_IFLNK ){
			exit(1);
		}		
		target_path = argv[3];
		new_archive = argv[2];
		// Declare new header
        	Header *header = (Header*) malloc(sizeof(Header));
		header = make_header(target_path, header);
		write_file(header);	
		free(header);
	} else if (strcmp(argv[1], "list") == 0){
		
		printf("list mode\n");
		if( argc != 3){
			printf("invalid argument.\n");
		}

		/*argv[2] validation*/
                if( access (argv[2], R_OK) == -1 ){
                        printf("Can not read file\n");
                        exit(1);
                }

	}else if (strcmp(argv[1], "extract") == 0){
		
		printf("extract mode\n");
		if( argc != 3){
			printf("invalid argument.\n");
		}

		/*argv[2] validation*/
		if( access( argv[2], F_OK ) == -1 ){
                        printf("Invalid file\n");
                        exit(1);
                }
                if( access (argv[2], R_OK) == -1 ){
                        printf("Can not read file\n");
                        exit(1);
                }

	} else {
		printf("invalid command\n");
	
	}




	return 0;

}
