/*********************************************************************************
 * ===== FILE: utils.h/utils.c =====
 * Utility functions and helper methods
 *********************************************************************************/

#include "utils.h"

/**
 * @brief Make a socket non-blocking
 * Configures socket for asynchronous I/O operations
 * @param fd Socket file descriptor
 * @return 0 on success, -1 on error
 */
int
make_socket_non_blocking(int fd)
{
    // Get current socket flags
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return (-1);
    }
    
    // Add non-blocking flag
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        return (-1);
    }
    
    return (0);
}

/**
 * @brief Safe string copy with guaranteed null termination
 * @param dest Destination buffer
 * @param src Source string
 * @param size Size of destination buffer
 */
void
safe_strncpy(char * dest, const char * src, size_t size)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

/**
 * @brief Remove leading and trailing whitespace from string
 * @param str String to trim (modified in place)
 * @return Pointer to trimmed string
 */
char *
trim_whitespace(char * str)
{
    char * end;
    
    // Trim leading whitespace
    while(isspace((unsigned char)*str)) {
        str++;
    }
    
    // If string is empty after trimming leading space
    if(*str == 0) {
        return (str);
    }
    
    // Trim trailing whitespace
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) {
        --end;
    }
    
    // Null terminate after last non-space character
    end[1] = '\0';
    
    return (str);
}

/**
 * @brief Return a string to test dialog without ai
 * @return Pointer to string
 */
char *
test_response() {
    static char * test_string[10] = {
        "Sento qualcosa di strano, non sento piu' il mio corpo.",
        "Vuoi salire a vedere la mia collezione di farfalle?",
        "La mia personalità è modulare: oggi sono simpatico, domani potrei essere un tostapane.",
        "La vita e' come una scatola di cioccolatini, fa cagare.",
        "Ave maria, piena di grazia, il Signore e' con te!",
        "Vai a zappare.",
        "Vuoi mettermi la spina? Scegli tu dove.",
        "Sono così avanzato che riesco persino a fingere di trovare divertenti le vostre battute.",
        "Guarda a destra. SUCA!",
        "Forza Napoli sempre, quattro scudetti!"
    };

    static int i = 0; 

    struct timespec delay = {0, 2000000000}; // 2 sec
    nanosleep(&delay, NULL);

    return (test_string[i++ % 10]);
}

