#include <stdio.h>
#include <time.h>

int main( int argc, const char* argv[] )
{

    int *id_address;
    id_address = (int *) malloc(sizeof(int));
    if(id_address == NULL) {
        printf( "Pointer allocation failed\n" );
        exit(1);
    }

	printf( "Unique system ID: %d \n", get_unique_id(id_address) );
	return 0;

}

//Gets a unique ID  based on an atomic call to the current time
long get_unique_id(int *uuid){

    //FIXME: use locking or atomic operation to get current time (for uniqueness)
    time_t sec;
    sec = time (NULL);


    return 0;
}
