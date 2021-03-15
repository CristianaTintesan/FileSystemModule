#include <stdio.h>
#include <string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>

int list_directory (char *s,char *directory_path)
{
	DIR *dir=NULL;
	struct dirent *dir_entry=NULL;


	dir=opendir(directory_path);
	if(dir==NULL){
        printf("ERROR\n");
		printf("invalid directory path\n");
		return -1;//ceva nu a mers bine.
	}
	else {
		printf("SUCCESS\n");
	}

	while ((dir_entry= readdir(dir))!=NULL){
        if(strcmp(s,dir_entry->d_name+(strlen(dir_entry->d_name)-strlen(s)))==0){
		printf("%s/%s\n",directory_path,dir_entry->d_name );
       }
	}
	if(closedir(dir)== -1){
		perror("Faild to close the directory\n");
		return -1;
	}
	return 0;

}
int list_directory2(char *directory_path)
{
	DIR *dir=NULL;
	int p;
	int rez_octal;
	int rest;
	struct dirent *dir_entry=NULL;
	char full_path[260];
    struct stat statbuf;
	dir=opendir(directory_path);
	if(dir==NULL){
        printf("ERROR\n");
		printf("invalid directory path\n");
		return -1;//ceva nu a mers bine.
	}
	else{
		printf("SUCCESS\n");
	}
	while((dir_entry=readdir(dir))!=NULL){
		snprintf(full_path,260,"%s/%s",directory_path,dir_entry->d_name);
		if(lstat(full_path,&statbuf)==0)  {        
		if(strcmp(dir_entry->d_name,".")==0 || strcmp(dir_entry->d_name,"..")==0){
			continue;
		}
	    p=1;
	    rez_octal=0;
	    int a=statbuf.st_mode & 0777;
	    while(a>0){
	    	rest=a%8;
	    	rez_octal=rez_octal+rest*p;
	    	p=p*10;
	    	a=(int)a/8;
	    }
	    int c= (int)rez_octal/100;
	    if(c%2==1){
	    	printf("%s/%s\n",directory_path,dir_entry->d_name );

	    }
       }
	}
	if(closedir(dir)== -1){
		perror("Faild to close the directory\n");
		return -1;
	}
	return 0;

}

int ok=0;
void list_recursive(const char *path)
{
	DIR *dir=NULL;
	struct dirent *entry=NULL;
	char fullPath[512];
	struct stat statbuf;
	dir= opendir(path);
	if(dir==NULL){
		printf("ERROR\n");
		printf("invalid directory path\n");
		return ;
	}else if(ok==0){
		printf("SUCCESS\n");
		ok=1;

	}
	while((entry=readdir(dir))!=NULL){
		if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){
			snprintf(fullPath,512,"%s/%s",path,entry->d_name);
			if(lstat(fullPath,&statbuf)==0){
				printf("%s\n",fullPath);
				if(S_ISDIR(statbuf.st_mode)){
					list_recursive(fullPath);
				}
			}
		}
	}
	closedir(dir);

}

int ok2=0;
void list_simple(const char *path)
{
	DIR *dir=NULL;
	struct dirent *entry=NULL;
	char fullPath[512];
	struct stat statbuf;
	dir= opendir(path);
	if(dir==NULL){
		printf("ERROR\n");
		printf("invalid directory path\n");
		return ;
	}else if(ok2==0){
		printf("SUCCESS\n");
		ok2=1;

	}
	while((entry=readdir(dir))!=NULL){
		if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){
			snprintf(fullPath,512,"%s/%s",path,entry->d_name);
			if(lstat(fullPath,&statbuf)==0){
				printf("%s\n",fullPath);
		
			}
		}
	}
	closedir(dir);

}


int format_SF(char* path) {
	int fd = -1;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	lseek(fd, 0, SEEK_SET);
	char* magic = malloc(sizeof(char) * 4);
	int header_size = 0;
	int version = 0;
	int nr_sections = 0;
	lseek(fd, 0, SEEK_SET);
	read(fd, magic, 4);
	read(fd, &header_size, 2);
	read(fd, &version, 1);
	read(fd, &nr_sections, 1);

	if (strcmp(magic, "PL4b") != 0)
	{
		close(fd);
		free(magic);
		printf("ERROR\n");
		printf("wrong magic\n");
		return -2;
	}

	if (version < 53 || version > 126)
	{
		close(fd);
		free(magic);
		printf("ERROR\n");
		printf("wrong version\n");
		return -2;
	}

	if (nr_sections < 5 || nr_sections > 14)
	{
		close(fd);
		free(magic);
		printf("ERROR\n");
		printf("wrong sect_nr\n");
		return -2;
	}


	char sect_name[14] = { 0 };
	int sect_type = 0;
	int sect_offset =0;
	int sect_size = 0;


	int ok = 1;
	for (int i = 0; i < nr_sections; i++) {
		read(fd, &sect_name, 14);
		read(fd, &sect_type, 2);
		if (sect_type != 62 && sect_type != 38)
		{
			close(fd);
			free(magic);
			printf("ERROR\n");
			printf(" wrong sect_types");
			return -2;
		}
		read(fd, &sect_offset, 4);
		read(fd, &sect_size, 4);
	}
		if (ok == 1)
		{

			printf("SUCCESS\n");
			printf("version=%d\n", version);
			printf("nr_sections=%d\n", nr_sections);
			lseek(fd, 0, SEEK_SET);
			lseek(fd, 8, SEEK_CUR);
			for (int i = 0; i < nr_sections; i++) {
				read(fd, &sect_name, 14);
				read(fd, &sect_type, 2);
				read(fd, &sect_offset, 4);
				read(fd, &sect_size, 4);

				printf("section%d: %s %d %d\n", i+1, sect_name, sect_type, sect_size);
			}

		}
	
	close(fd);
	free(magic);

	return 0;
}

int sectiuni(char *path,char* nr_sect, char* nr_linie){
	int fd = -1;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	lseek(fd, 0, SEEK_SET);
	char* magic = malloc(sizeof(char) * 4);
	int header_size = 0;
	int version = 0;
	int nr_sections = 0;
	lseek(fd, 0, SEEK_SET);
	read(fd, magic, 4);
	read(fd, &header_size, 2);
	read(fd, &version, 1);
	read(fd, &nr_sections, 1);

	if (strcmp(magic, "PL4b") != 0)
	{
		close(fd);
		free(magic);
		printf("ERROR\n");
		printf("invalid file\n");
		return -2;
	}

	if (version < 53 || version > 126)
	{
		close(fd);
		free(magic);
		printf("ERROR\n");
		printf("invalid file\n");
		return -2;
	}

	if (nr_sections < 5 || nr_sections > 14)
	{
		close(fd);
		free(magic);
		printf("ERROR\n");
		printf("invalid file\n");
		return -2;
	}
	char sect_name[14] = { 0 };
	int sect_type = 0;
	int sect_offset =0;
	int sect_size = 0;


	for (int i = 0; i < nr_sections; i++) {
		read(fd, &sect_name, 14);
		read(fd, &sect_type, 2);
		if (sect_type != 62 && sect_type != 38)
		{
			close(fd);
			free(magic);
			printf("ERROR\n");
			printf(" invalid file");
			return -2;
		}
		read(fd, &sect_offset, 4);
		read(fd, &sect_size, 4);
	}

	lseek(fd, 0, SEEK_SET);
	lseek(fd, 8, SEEK_CUR);
	for (int i = 0; i < nr_sections; i++) 
	{
		read(fd, &sect_name, 14);
		read(fd, &sect_type, 2);
		read(fd, &sect_offset, 4);
		read(fd, &sect_size, 4);
	}



	int y=atoi(nr_sect);
	if(y>nr_sections){
		printf("ERROR\n");
		printf("invalid section\n");
		return -2;
	}

	lseek(fd,0,SEEK_SET);
	char* magic2 = malloc(sizeof(char) * 4);
	header_size = 0;
    version = 0;
    nr_sections = 0;
	read(fd, magic2, 4);
	read(fd, &header_size, 2);
	read(fd, &version, 1);
	read(fd, &nr_sections, 1);
    char sect_name2[14]={0};
	sect_type = 0;
	sect_offset =0;
	sect_size = 0;


	for(int  j=0; j<y; j++)
	{
		read(fd,&sect_name2,14);
		read(fd,&sect_type,2);
		read(fd,&sect_offset,4);
		read(fd,&sect_size,4);
	
	}
	lseek(fd,sect_offset,SEEK_SET);	
	char *continut=malloc(sizeof(char)*sect_size);
	read(fd,continut,sect_size);
	int n=strlen(continut);
	int linii=0;
	for (int k=0;k<n;k++)
	{
       if(continut[k]==0x0A) 
       {
       	linii++;
       } 

	}
	linii++;
	
    int x=atoi(nr_linie);
  
    if(x>linii)
    {
       printf("ERROR\n");
       printf("invalid line");
       return -2;
    }


    printf("SUCCESS\n");
    
    char *rest=NULL;
    char *token;
    int linii2=0;
    for(token= strtok_r(continut,"\n",&rest);
    token!=NULL;
    token=strtok_r(NULL,"\n",&rest))
    {
    	linii2++;
       if(linii2==x){
       	char c;
       	int l=strlen(token);
       	for(int j=l;j>0;j--)
       	{
       		c=token[j-1];
       		printf("%c",c );
       	}

       
	   }

    }
  
    
    free(continut);
    free(magic);
    free(magic2);
    return 0;
}

int ok4=0;
void sf_recursiv2(char *path)
{

	DIR *dir=NULL;
	struct dirent *entry=NULL;
	char fullPath[512];
	struct stat statbuf;
	dir= opendir(path);
	if(dir==NULL)
	{
		printf("ERROR\n");
		printf("invalid directory path\n");
		return ;
	}

    if(ok4==0){
    	ok4=1;
    	printf("SUCCESS\n");
    }
    while((entry=readdir(dir))!=NULL)
    {
		if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)
		{
			snprintf(fullPath,512,"%s/%s",path,entry->d_name);
			if(lstat(fullPath,&statbuf)==0)
			{
         		if(S_ISREG(statbuf.st_mode))
         		{

                        int fd = -1;
                        int ok_sf=1;

						fd = open(fullPath, O_RDONLY);
						if (fd < 0) {
							return ;
						}

						lseek(fd, 0, SEEK_SET);
						char* magic = malloc(sizeof(char) * 4);
						int header_size = 0;
						int version = 0;
						int nr_sections = 0;
						lseek(fd, 0, SEEK_SET);
						read(fd, magic, 4);
						read(fd, &header_size, 2);
						read(fd, &version, 1);
						read(fd, &nr_sections, 1);

						if (strcmp(magic, "PL4b") != 0)
						{
							ok_sf=0;
						}

						if (version < 53 || version > 126)
						{
							ok_sf=0;
							
						}

						if (nr_sections < 5 || nr_sections > 14)
						{
							ok_sf=0;
							
						}


						char sect_name[14] = { 0 };
						int sect_type = 0;
						int sect_offset =0;
						int sect_size = 0;


						int ok = 1;
						for (int i = 0; i < nr_sections; i++)
						 {
							read(fd, &sect_name, 14);
							read(fd, &sect_type, 2);
							if (sect_type != 62 && sect_type != 38)
							{
								ok=0;
							
							}
							read(fd, &sect_offset, 4);
							read(fd, &sect_size, 4);
							if(sect_size >1425)
							{
								
								ok=0;
							
							}
						}
							

						  	
						lseek(fd, 0, SEEK_SET);
						lseek(fd, 8, SEEK_CUR);
						for (int i = 0; i < nr_sections; i++) 
						{
						read(fd, &sect_name, 14);
						read(fd, &sect_type, 2);
						read(fd, &sect_offset, 4);
						read(fd, &sect_size, 4);
                        }
                        if(ok==1 && ok_sf==1)
                        {
						printf("%s\n", fullPath );
						
						}

							
  						close(fd);
				     	free(magic);
				 }
				 
			}
			

           if(S_ISDIR(statbuf.st_mode))
			{
			 sf_recursiv2(fullPath);
			} 

				 
		}
		

	}	

	closedir(dir);		     	
				     
				
}				



  

int main(int argc, char **argv){

    if(argc >= 2)
    {
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("97390\n");
        }
        if(strcmp(argv[1],"list")==0 && strncmp(argv[2],"name_ends_with=",15)==0 && strncmp(argv[3],"path=",5)==0)
        	return list_directory(argv[2]+15,argv[3]+5);
        if(strcmp(argv[1],"list")==0 && strcmp(argv[2],"has_perm_execute")==0 && strncmp(argv[3],"path=",5)==0)
        	return list_directory2(argv[3]+5);
        if(strcmp(argv[1],"list")==0 && strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"path=",5)==0)
        	list_recursive(argv[3]+5);
        if(strcmp(argv[1],"list")==0 && strncmp(argv[2],"path=",5)==0)
        	list_simple(argv[2]+5);
        if(strcmp(argv[1],"parse")==0 && strncmp(argv[2],"path=",5)==0)
        	return format_SF(argv[2]+5);
        if(strcmp(argv[1],"extract")==0 && strncmp(argv[2],"path=",5)==0 && strncmp(argv[3],"section=",8)==0 && strncmp(argv[4],"line=",5)==0 )
        	return sectiuni(argv[2]+5,argv[3]+8,argv[4]+5);
        if(strcmp(argv[1],"findall")==0 && strncmp(argv[2],"path=",5)==0)
        	sf_recursiv2(argv[2]+5);
    }   
    return 0;
}