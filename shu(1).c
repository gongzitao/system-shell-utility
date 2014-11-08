/* Tao Lu ID:1000919138 
 CSE 3320
*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
int main(int argc, char * argv[]) {
	pid_t child;
	DIR * d;
	struct dirent * de;
	int i;
	char s[2048], cmd[2048];
	time_t t;
	unsigned int num;
	char c;
	char p[256], tmp[2];
	char flist[1024][2048], dlist[1024][2048];
	int findex, dindex;
	int fsize, dsize;
	int pagesize, pageindex;
	pagesize = 10;
	pageindex = 1;
	/* use argument as the initial working dir */
	if (argc == 2){
		strcpy(s, argv[1]);
		if (d = opendir( s )){
			closedir( d );
			chdir( s );
		}
		else{
			printf("%s can not be opened.\n", s);
			return -1;
		}
	}
	if (argc > 2){
		printf("shu can only have 0 or 1 argument.\n");
		return -1;
	}

	while (1) {
		getcwd(s, 2048);
		printf( "\nCurrent Working Dir: %s \n", s);
		t = time( NULL );
		printf( "It is now: %s\n", ctime( &t ));
		
		d = opendir( "." );
		/* handle current working dir exception */
		if (NULL == d){
			printf("%s can not be opened.\n", s);
			return -1;
		}

		/* store files and directories to their lists */
		dindex = 0;
		findex = 0;
		while ((de = readdir(d))){
			if ((de->d_type) & DT_DIR) {
				/* ensure dir list will not overflow */
				if (dindex < 1024) {
					strcpy( dlist[dindex], de->d_name );
					dindex++;
				}
			}
			else {	
				/* ensure file list will not overflow */
				if (findex < 1024) {
					strcpy( flist[findex], de->d_name );
					findex++;
				}
			}
		}
		dsize = dindex;
		fsize = findex;
		closedir( d );

		/* print one page of files and directories from both lists */
		for ( i=(pageindex-1)*pagesize; i<pageindex*pagesize && i<fsize; i++){
			if (0 == i%pagesize)
				printf("Files:\t\t%d.\t%s\n", i, flist[i]);
			else
				printf("\t\t%d.\t%s\n", i, flist[i]);
		}
		for ( i=(pageindex-1)*pagesize; i<pageindex*pagesize && i<dsize; i++){
			if (0 == i%pagesize)
				printf("\nDirectories:\t%d.\t%s\n", i, dlist[i]);
			else
				printf("\t\t%d.\t%s\n", i, dlist[i]);
		}
		printf( "\nOperations:\tE Edit\n\t\tR Run\n\t\tC Change Directory\n" );
		/* print Prev and Next if available */
		if (pageindex > 1)
			printf("\t\tP Prev Page\n");
		if (pageindex*pagesize < fsize || pageindex*pagesize < dsize )
			printf("\t\tN Next Page\n");

		printf("\t\tQ Quit\n");

		setbuf(stdin, NULL);
		c = getchar( );
		switch (c) {
			case 'Q':
			case 'q': return 0; /* quit */
			case 'E':
			case 'e': printf( "Edit what?(enter a file number between 0 to %d, default 0):", fsize - 1 );
				/* input an available file number */
				num = 0;
				scanf( "%d", &num );
				while (num < 0 || num > fsize - 1){
					printf("%d is a wrong number, reenter one number (0 - %d) please:", num, fsize - 1);
					setbuf(stdin, NULL);
					scanf( "%d", &num );
				}
				/* create the edit command */
				strcpy( cmd, "pico ");
				strcat( cmd, flist[num] );
				printf("Edit: %s\n", cmd);

				system( cmd );
				break;
			case 'R':
			case 'r': printf( "Run what?(enter a file number 0 - %d, default 0, and followed by parameters, press enter to end):", fsize - 1 );
				/* input an available file number */
				num = 0;
				scanf( "%d", &num );
				while (num < 0 || num > fsize - 1){
					printf("%d is a wrong number, reenter one number (0 - %d) please:", num, fsize - 1);
					setbuf(stdin, NULL);
					scanf( "%d", &num );
				}

				/* read the parameters by getchar(), 
				and store in string p */
				setbuf(stdin, NULL);
				strcpy(p, "");
				while((c = getchar( ))!='\n'){
					tmp[0] = c;
					tmp[1] = '\0';
					strcat(p, tmp);
				}

				/* create the run command */
				strcpy( cmd, "./");
				strcat( cmd, flist[num] );
				strcat( cmd, " ");
				strcat( cmd, p );
				printf("Run: %s\n", cmd);

				system( cmd );
				
				printf("Press enter to continue...\n", s);
				setbuf(stdin, NULL);
				getchar();
				break;
			case 'C':
			case 'c': printf( "Change To?(enter a dir number between 0 to %d, default 0):", dsize - 1 );
				/* input an available dir number */
				num = 0;
				scanf( "%d", &num );
				while (num < 0 || num > dsize - 1){
					printf("%d is a wrong number, reenter one number (0 - %d) please:", num, dsize - 1);
					setbuf(stdin, NULL);
					scanf( "%d", &num );
				}

				printf("Change To: %s\n", dlist[num]);
				
				/* handle chdir exception */
				if (chdir( dlist[num] )) {
					printf("Error: can not change to directory %s/%s. Press enter to continue...\n", s, dlist[num]);
					setbuf(stdin, NULL);
					getchar();
				}
				else
					/* reset pageindex of the new dir to the first page */
					pageindex = 1;
				break;
			case 'P':
			case 'p': if (pageindex > 1)
					pageindex--;
				break;
			case 'N':
			case 'n': if (pageindex*pagesize < fsize || pageindex*pagesize < dsize )
					pageindex++;
				break;
			default:break;
		}
	}
}
