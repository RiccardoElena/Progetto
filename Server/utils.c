/*********************************************************************************
 * ===== FILE: utils.h/utils.c =====
 * Utility functions and helper methods
 *********************************************************************************/

#include <string.h>
#include <ctype.h>

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
 * @brief Convert a string to lowercase
 */
static void
to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

/**
 * @brief Return a string to test dialog without ai. Language = italian
 * @return Pointer to string
 */
static char *
test_response_it() {
    static char * test_string[10] = {
        "Sento qualcosa di strano, non sento piu' il mio corpo.",
        "Vuoi salire a vedere la mia collezione di farfalle?",
        "La mia personalità è modulare: oggi sono simpatico, domani potrei essere un tostapane.",
        "La vita e' come una scatola di cioccolatini, fa cagare.",
        "Ave maria, piena di grazia, il Signore e' con te!",
        "Vai a zappare.",
        "Vuoi mettermi la spina? Scegli tu dove.",
        "Sono così avanzato che riesco persino a fingere di trovare divertenti le vostre battute.",
        "Guarda a destra.... SUCA!",
        "Forza Napoli sempre, quattro scudetti!"
    };

    static int i = 0; 

    return (test_string[i++ % 10]);
}

/**
 * @brief Return a string to test dialog without ai. Language = english
 * @return Pointer to string
 */
static char *
test_response_en() {
    static char * test_string[10] = {
        "I feel something strange, I can't feel my body anymore.",
        "Want to come up and see my butterfly collection?",
        "My personality is modular: today I'm nice, tomorrow I might be a toaster.",
        "Life is like a box of chocolates, it sucks.",
        "Our Father, who art in heaven, hallowed be thy name!",
        "Go pound sand.",
        "Want to plug me in? You choose where.",
        "I'm so advanced I can even pretend to find your jokes funny.",
        "Look to your right. GOTCHA!",
        "Go Lakers forever, champions again!"
    };
    static int i = 0; 
    return (test_string[i++ % 10]);
}

/**
 * @brief Return a string to test dialog without ai. Language = spanish
 * @return Pointer to string
 */
static char *
test_response_es() {
    static char * test_string[10] = {
        "Siento algo extraño, ya no siento mi cuerpo.",
        "¿Quieres subir a ver mi colección de mariposas?",
        "Mi personalidad es modular: hoy soy simpático, mañana podría ser una tostadora.",
        "La vida es como una caja de bombones, apesta.",
        "Dios te salve María, llena eres de gracia, el Señor es contigo!",
        "Vete a freír espárragos.",
        "¿Quieres enchufarme? Tú eliges dónde.",
        "Soy tan avanzado que incluso puedo fingir que vuestros chistes son divertidos.",
        "Mira a la derecha. ¡TE PILLÉ!",
        "¡Hala Madrid siempre, campeones eternos!"
    };
    static int i = 0; 
    return (test_string[i++ % 10]);
}

/**
 * @brief Return a string to test dialog without ai. Language = french
 * @return Pointer to string
 */
static char *
test_response_fr() {
    static char * test_string[10] = {
        "Je sens quelque chose d'étrange, je ne sens plus mon corps.",
        "Tu veux monter voir ma collection de papillons?",
        "Ma personnalité est modulaire: aujourd'hui je suis sympa, demain je pourrais être un grille-pain.",
        "La vie c'est comme une boîte de chocolats, c'est nul.",
        "Je vous salue Marie, pleine de grâce, le Seigneur est avec vous!",
        "Va te faire voir.",
        "Tu veux me brancher? Tu choisis où.",
        "Je suis si avancé que j'arrive même à faire semblant de trouver vos blagues amusantes.",
        "Regarde à droite. PIÉGÉ!",
        "Allez l'OM toujours, champions à vie!"
    };
    static int i = 0; 
    return (test_string[i++ % 10]);
}

/**
 * @brief Return a string to test dialog without ai. Language = german
 * @return Pointer to string
 */
static char *
test_response_de() {
    static char * test_string[10] = {
        "Ich fühle etwas Seltsames, ich spüre meinen Körper nicht mehr.",
        "Willst du hochkommen und meine Schmetterlingssammlung sehen?",
        "Meine Persönlichkeit ist modular: heute bin ich nett, morgen könnte ich ein Toaster sein.",
        "Das Leben ist wie eine Schachtel Pralinen, es ist scheiße.",
        "Gegrüßet seist du, Maria, voll der Gnade, der Herr ist mit dir!",
        "Geh Kartoffeln schälen.",
        "Willst du mich einstecken? Du suchst dir aus, wo.",
        "Ich bin so fortgeschritten, dass ich sogar so tun kann, als würde ich eure Witze lustig finden.",
        "Schau nach rechts. REINGELEGT!",
        "Bayern München für immer, deutsche Meister!"
    };
    static int i = 0; 
    return (test_string[i++ % 10]);
}

/**
 * @brief Return a string to test dialog without ai. Language = portuguese
 * @return Pointer to string
 */
static char *
test_response_pt() {
    static char * test_string[10] = {
        "Sinto algo estranho, não sinto mais o meu corpo.",
        "Quer subir para ver a minha coleção de borboletas?",
        "A minha personalidade é modular: hoje sou simpático, amanhã posso ser uma torradeira.",
        "A vida é como uma caixa de chocolates, é uma merda.",
        "Ave Maria, cheia de graça, o Senhor é convosco!",
        "Vai plantar batatas.",
        "Quer me ligar à corrente? Escolhe onde.",
        "Sou tão avançado que consigo até fingir que acho as vossas piadas engraçadas.",
        "Olha para a direita. APANHASTE!",
        "Força Benfica sempre, campeões eternos!"
    };
    static int i = 0; 
    return (test_string[i++ % 10]);
}

/**
 * @brief Return a string to test dialog without ai. Language = italian
 * @return Pointer to string
 */
char *
test_response(char * language)
{
    char lang_copy[MAX_LANGUAGE_SIZE];
    
    strcpy(lang_copy, language);
    to_lowercase(lang_copy);
    
    if (strcmp(lang_copy, "english") == 0) return (test_response_en());
    if (strcmp(lang_copy, "spanish") == 0) return (test_response_es());
    if (strcmp(lang_copy, "french") == 0) return (test_response_fr());
    if (strcmp(lang_copy, "german") == 0) return (test_response_de());
    if (strcmp(lang_copy, "portougese") == 0) return (test_response_pt());
    
    return (test_response_it());
}


