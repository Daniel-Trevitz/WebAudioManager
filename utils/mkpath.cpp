#include "mkpath.h"

#include <sys/stat.h>
#include <sys/types.h>

bool mkpath(const std::string &path)
{
    bool bSuccess = false;
    int nRC = ::mkdir( path.c_str(), 0775 );
    if( nRC == -1 )
    {
        switch( errno )
        {
        case ENOENT:
            //parent didn't exist, try to create it
            if( mkpath( path.substr(0, path.find_last_of('/')) ) )
                //Now, try to create again.
                bSuccess = 0 == ::mkdir( path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            else
                bSuccess = false;
            break;
        case EEXIST:
            //Done!
            bSuccess = true;
            break;
        default:
            bSuccess = false;
            break;
        }
    }
    else
    {
        bSuccess = true;
    }

    return bSuccess;
}
