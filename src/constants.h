#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_PATH_LEN            4096
#define COMMIT_ID_LEN           20

#include <string.h>

static char CONFIG_FILE_NAME[] = ".cupot_conf";
static char ALIAS_FILE_NAME[] = ".cupot_alias";
static char STAGE_NAME[] = "staging_area";
static char BACKUP_STAGE_NAME[] = ".staging_area";
static char COMMITS_NAME[] = "commits";
static char COMMIT_CONF_FILE[] = "commit.conf";
static char COMMIT_MESSAGE_ALIAS[] = "messages.alias";
static char TEMP_LATEST_VERSION[] = "lstversion";
static char MASTER_BRANCH_NAME[] = "master";
static char TRACK_FILE_NAME[] = "tracker";

#define MAX_CONFIG_LENGHT       100
#define MAX_CONFIG_ENTRIES      20    
#define MAX_COMMIT_MESSAGE_LEN  1024
#define MAX_BRANCHES            128
#define MAX_FILE_COUNT          4096

#define MAX(a,b)                (a) > (b) ? (a) : (b)

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


#endif