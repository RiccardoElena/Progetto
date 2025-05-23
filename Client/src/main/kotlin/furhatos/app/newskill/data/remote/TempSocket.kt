package furhatos.app.newskill.data.remote

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.BufferedReader
import java.io.InputStreamReader
import java.io.PrintWriter
import java.net.Socket

/*FIXME:
   This file is only a general boilerplate and is not coherent with the rest of the program and with the current architecture.
   Don't use it as it is, but modify it properly.

   Also, this implementation is improvable using kotlin's flow and state, making the code more complex tho
*  */

/**
 * Rappresenta un utente del sistema
 *
 * @property username nome utente univoco
 * @property password password dell'utente
 */
data class User(
    val username: String,
    val password: String,
)

/**
 * Risultato di un'operazione di autenticazione
 *
 * @property success true se l'operazione è riuscita, false altrimenti
 * @property message messaggio descrittivo del risultato
 */
data class AuthResult(
    val success: Boolean,
    val message: String,
)

/**
 * Rappresenta un messaggio di chat
 *
 * @property sender nome utente del mittente
 * @property content contenuto del messaggio
 * @property timestamp timestamp di creazione del messaggio in millisecondi
 */
data class ChatMessage(
    val sender: String,
    val content: String,
    val timestamp: Long = System.currentTimeMillis(),
)

/**
 * Client socket per la comunicazione con il server di chat
 *
 * Gestisce l'autenticazione degli utenti e lo scambio di messaggi
 * attraverso una connessione socket TCP.
 *
 * @param host indirizzo IP o hostname del server
 * @param port porta di connessione del server
 * @param onServerInfo operazioni aggiuntive da fare quando la risposta del server è SERVER_INFO
 * @param onDisconnected operazioni aggiuntive da fare quando il server risulta disconnesso
 * @param onMessageReceived operazioni aggiuntive da fare quando un messaggio è ricevuto
 *
 * @author Riccardo Elena
 * @since 1.0
 *
 */
class SocketClient(
    private val host: String,
    private val port: Int,
    var onServerInfo: (String) -> Unit = { println(it) },
    var onDisconnected: () -> Unit = {},
    var onMessageReceived: (ChatMessage) -> Unit,
) {
    private var socket: Socket? = null
    private var writer: PrintWriter? = null
    private var reader: BufferedReader? = null
    private var _isConnected: Boolean = false
    private var _isAuthenticated: Boolean = false
    private var _currentUser: User? = null

    /**
     * Verifica se il client è connesso al server
     *
     * @return true se la connessione è attiva, false altrimenti
     */
    val isConnected: Boolean get() = _isConnected

    /**
     * Verifica se l'utente è autenticato
     *
     * @return true se l'utente è autenticato, false altrimenti
     */
    val isAuthenticated: Boolean get() = _isAuthenticated

    /**
     * Restituisce l'utente attualmente autenticato
     *
     * @return l'oggetto User dell'utente autenticato, null se nessuno è autenticato
     */
    val currentUser: User? get() = _currentUser

    /**
     * Stabilisce la connessione socket con il server
     *
     * @return true se la connessione è avvenuta con successo, false altrimenti
     * @throws java.io.IOException se si verificano errori di rete
     */
    suspend fun connect(): Boolean =
        withContext(Dispatchers.IO) {
            try {
                socket = Socket(host, port)
                writer = PrintWriter(socket!!.getOutputStream(), true)
                reader = BufferedReader(InputStreamReader(socket!!.getInputStream()))
                _isConnected = true
                true
            } catch (e: Exception) {
                false
            }
        }

    /**
     * Autentica un utente esistente sul server
     *
     * @param user oggetto User contenente username e password
     * @return AuthResult contenente l'esito dell'operazione
     * @throws java.io.IOException se si verificano errori di comunicazione
     * @see register per registrare un nuovo utente
     */
    suspend fun login(user: User): AuthResult {
        if (!_isConnected) {
            return AuthResult(false, "Non connesso al server")
        }

        return withContext(Dispatchers.IO) {
            try {
                val loginMessage = "LOGIN:${user.username}:${user.password}"
                writer?.println(loginMessage)

                val response = reader?.readLine()

                when {
                    response?.startsWith("LOGIN_SUCCESS") == true -> {
                        _isAuthenticated = true
                        _currentUser = user
                        AuthResult(true, "Login effettuato con successo")
                    }
                    response?.startsWith("LOGIN_FAILED") == true -> {
                        AuthResult(false, "Credenziali non valide")
                    }
                    else -> {
                        AuthResult(false, "Risposta inaspettata dal server")
                    }
                }
            } catch (e: Exception) {
                AuthResult(false, "Errore durante il login: ${e.message}")
            }
        }
    }

    /**
     * Registra un nuovo utente sul server
     *
     * @param user oggetto User contenente username e password per il nuovo account
     * @return AuthResult contenente l'esito della registrazione
     * @throws java.io.IOException se si verificano errori di comunicazione
     * @see login per autenticare un utente esistente
     */
    suspend fun register(user: User): AuthResult {
        if (!_isConnected) {
            return AuthResult(false, "Non connesso al server")
        }

        return withContext(Dispatchers.IO) {
            try {
                val registerMessage = "REGISTER:${user.username}:${user.password}"
                writer?.println(registerMessage)

                val response = reader?.readLine()

                when {
                    response?.startsWith("REGISTER_SUCCESS") == true -> {
                        _isAuthenticated = true
                        _currentUser = user
                        AuthResult(true, "Registrazione completata con successo")
                    }
                    response?.startsWith("REGISTER_FAILED") == true -> {
                        AuthResult(false, "Username già esistente")
                    }
                    else -> {
                        AuthResult(false, "Risposta inaspettata dal server")
                    }
                }
            } catch (e: Exception) {
                AuthResult(false, "Errore durante la registrazione: ${e.message}")
            }
        }
    }

    /**
     * Invia un messaggio di chat al server
     *
     * @param content contenuto del messaggio da inviare
     * @return true se il messaggio è stato inviato con successo, false altrimenti
     * @throws IllegalStateException se l'utente non è autenticato
     */

    suspend fun sendMessage(content: String): Boolean {
        if (!_isAuthenticated || _currentUser == null) {
            return false
        }

        return withContext(Dispatchers.IO) {
            try {
                val message = "MESSAGE:${_currentUser!!.username}:$content"
                writer?.println(message)
                true
            } catch (e: Exception) {
                false
            }
        }
    }

    /**
     * Avvia l'ascolto dei messaggi provenienti dal server
     *
     * Deve essere chiamato dopo un'autenticazione riuscita.
     * I messaggi ricevuti verranno notificati tramite le callback appropriate.
     *
     * @throws IllegalStateException se l'utente non è autenticato
     * @see onMessageReceived
     * @see onServerInfo
     * @see onDisconnected
     */
    fun startListening() {
        if (!_isAuthenticated) {
            return
        }

        CoroutineScope(Dispatchers.IO).launch {
            try {
                while (_isConnected && _isAuthenticated) {
                    val message = reader?.readLine()

                    if (message != null) {
                        handleServerMessage(message)
                    } else {
                        _isConnected = false
                        onDisconnected()
                        break
                    }
                }
            } catch (e: Exception) {
                _isConnected = false
                onDisconnected()
            }
        }
    }

    /**
     * Gestisce i messaggi ricevuti dal server e invoca le callback appropriate
     *
     * @param message messaggio grezzo ricevuto dal server
     */
    private fun handleServerMessage(message: String) {
        val parts = message.split(":", limit = 3)

        when {
            parts.size >= 3 && parts[0] == "MESSAGE" -> {
                val chatMessage =
                    ChatMessage(
                        sender = parts[1],
                        content = parts[2],
                    )
                onMessageReceived(chatMessage)
            }
            message.startsWith("SERVER_INFO:") -> {
                val info = message.substringAfter("SERVER_INFO:")
                onServerInfo(info)
            }
            message == "DISCONNECT" -> {
                _isConnected = false
                _isAuthenticated = false
                onDisconnected()
            }
        }
    }

    /**
     * Chiude la connessione con il server e libera le risorse
     *
     * Invia un messaggio di disconnessione al server se l'utente è autenticato,
     * quindi chiude tutte le stream e il socket.
     */
    fun disconnect() {
        try {
            if (_isAuthenticated && _currentUser != null) {
                writer?.println("QUIT:${_currentUser!!.username}:Disconnessione")
            }
            _isConnected = false
            _isAuthenticated = false
            _currentUser = null
            writer?.close()
            reader?.close()
            socket?.close()
        } catch (e: Exception) {
            // Ignora errori di disconnessione
        }
    }
}
