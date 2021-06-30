#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

char* new_archive = 0x0;

typedef struct header{

	int ftype;   
	unsigned int len_path; 
	unsigned int len_data; 
	char* path; 

} Header;


void print_header (Header *header){


	printf("ftype : %d\n length of the path : %u\n length of the file: %u\n path : %s\n",
			header->ftype, header->len_path, header->len_data, header->path);


}

Header* make_header ( char* path, Header *header ){

	struct stat sf;
	stat(path, &sf);
	if( (sf.st_mode & S_IFMT) == S_IFDIR ){
		header->ftype = 0;
	} else if ((sf.st_mode & S_IFMT) == S_IFREG ){
		header->ftype = 1;
	}	
	size_t len_path = sizeof(path) ;
	header->len_path = len_path;

	size_t len_data = sf.st_size;
	header->len_data = len_data;

	header->path = path;

	return header;
}

void write_file( Header* header ){

	FILE *fp_w, *fp_r;
	
	fp_w = fopen(new_archive, "ab");
	fp_r = fopen(header->path, "rb");
	
	if( fp_w == NULL || fp_r == NULL ) {
		printf("Fail to open file stream");
		exit(1);
	}

	size_t result_w = 0;
	size_t result_r = 0;
	char buffer[512] = { 0 };

	result_w = fwrite( &header->ftype, 1, sizeof(header->ftype), fp_w);
	if ( result_w != sizeof(header->ftype) ) 
		exit(1);
	result_w = fwrite( &header->len_path, 1, sizeof(header->len_path), fp_w);
	if ( result_w != sizeof(header->len_path) ) 
		exit(1);
	result_w = fwrite( &header->len_data, 1, sizeof(header->len_data), fp_w);
	if ( result_w != sizeof(header->len_data) ) 
                exit(1);
	result_w = fwrite( &header->path, 1, sizeof(header->path), fp_w);
	if ( result_w != sizeof(header->path) )
                exit(1);

	while( !feof(fp_r) ){
		result_r = fread(buffer, 1, 512, fp_r);
		fwrite(buffer, 1, result_r, fp_w);
		memset(buffer, 0, 512);
	}

	fclose(fp_r);
	fclose(fp_w);
}

char* manipul_path (char* path, char* d_name){

	int path_length;
	char new_path[PATH_MAX];

	path_length = snprintf(new_path, PATH_MAX, "%s/%s", path, d_name);

	char* ptr = &new_path[0];
	return ptr;
}


void retrieve_path (char* path){

	DIR* dir = opendir(path);
	struct dirent *ent;

	
	while( (ent = readdir(dir)) != NULL ){

		if(strcmp(ent->d_name, ".") == 0){
		 	continue;
		}
		if(strcmp(ent->d_name, "..") == 0){
			continue;
		}
		if(ent->d_type == DT_LNK){
			continue;
		}
		
		char* new_path = manipul_path(path, ent->d_name);
		Header *header = (Header*) malloc(sizeof(Header));
        	header = make_header(new_path, header);
		write_file(header);
		free(header); 
		
		if(ent->d_type == DT_DIR){
			retrieve_path(new_path);
		}
	}



}

void retrieve_header(char* archive){



}
int main(int argc, char* argv[]){

	char* target_path= 0x0;
	
	if( strcmp(argv[1], "archive") == 0 ){
		
		printf("archive mode\n");
		if ( argc != 4 ){
			printf("invalid argument.\n");
		} 

		if( access( argv[2], F_OK ) == 0 ){
			printf("The file with that name exists. You should enter name of new file.\n");
			exit(1);	
		}

		if( opendir (argv[3]) == NULL ){
                        printf("Can not open directory\n");
                        exit(1);
		}

		struct stat sf;
		if( stat(argv[3], &sf) == -1 ){
			printf("stat fail\n");
			exit(1);
		}

		if( ( sf.st_mode & S_IFMT ) == S_IFLNK ){
			exit(1);
		}		
		
		target_path = argv[3];
		new_archive = argv[2];
		retrieve_path(target_path);
	
	} else if (strcmp(argv[1], "list") == 0){
		
		printf("list mode\n");
		if( argc != 3){
			printf("invalid argument.\n");
		}

                if( access (argv[2], R_OK) == -1 ){
                        printf("Can not read file\n");
                        exit(1);
                }

		new_archive = argv[2];

	}else if (strcmp(argv[1], "extract") == 0){
		
		printf("extract mode\n");
		if( argc != 3){
			printf("invalid argument.\n");
		}

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
