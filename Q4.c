#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void cp_file(char* path, char* dest_path){

	int status;
	status = open(path, O_RDONLY);
	if( status == -1 ){
		fputs("Fail to Open", stderr);
		exit(1);
	} else {
		size_t result_r, result_w;
		FILE *f_r, *f_w;
		char buffer[1024];
		f_r = fopen(path, "rb");
		if( f_r == NULL ){
			fputs ("File Open error",stderr); 
			exit (EXIT_FAILURE);
		}
		
		f_w = fopen(dest_path, "wb");
		if( f_w == NULL ){
			fputs ("File Open error",stderr);
			exit (EXIT_FAILURE);
		}  

		while( !feof(f_r) ){
			result_r = fread( buffer, 1024, 1, f_r);
			fwrite( buffer, 1024, 1, f_w);
		}	

		status = chmod(dest_path, 0755);
		fclose(f_r);
		fclose(f_w);
	}


}

void display_dir(char* origin_path, char* dest_path){


	DIR* dir;
	struct dirent* ent;
	dir = opendir(origin_path);

	while( (ent = readdir(dir)) != NULL ){

		if( !ent ){
			break;
		}

		const char* d_name;
		d_name = ent->d_name;
		int path_length_origin;
		int path_length_dest;
		char path[PATH_MAX];
		char path_d[PATH_MAX];

		path_length_origin = snprintf(path, PATH_MAX, "%s/%s", origin_path, d_name);
		path_length_dest = snprintf(path_d, PATH_MAX, "%s/%s", dest_path, d_name);

		if (path_length_origin >= PATH_MAX || path_length_dest >= PATH_MAX ) {
			fprintf (stderr, "Path length has got too long.\n");
			exit (EXIT_FAILURE);
	       	}
		struct stat sf;
		if( stat(path, &sf) == -1){
			fputs("stat fail\n", stdeerr);
			exit(1);
		} else if( (sf.st_mode & S_IFMT) != S_IFLNK ){
			if( (sf.st_mode & S_IFMT) == S_IFREG ){
				cp_file(path, path_d);
			} else if((sf.st_mode & S_IFMT) == S_IFDIR){
				
				if(strcmp(d_name, ".") != 0 && strcmp(d_name, "..") != 0 ){
					int status;
					status = mkdir(path_d, 0755);
					if( status < 0 ){
						fputs("mkdir failed\n", stderr);
						exit(EXIT_FAILURE);
					} else if ( status >= 0 ) {
						display_dir(path, path_d);
					}
				}		
			}
		}
	}
	closedir(dir);
}

int main(int argc, char* argv[]){

	DIR* dir;
	char* origin_path;
	char* dest_path;
	char* resolved_origin_path;
	origin_path = argv[1];
	dest_path = argv[2];

	if( (dir = opendir(dest_path)) == NULL){
		int status;
		status = mkdir(dest_path, 0755);
		if( status == -1 ){
			fputs("mkdir failed\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	if( (dir = opendir(origin_path)) != NULL ){
		display_dir(origin_path, dest_path);
	} else {
		fputs("Directory Open Error", stderr);
		exit(-1); // positive number
	}
	return 0;
}
