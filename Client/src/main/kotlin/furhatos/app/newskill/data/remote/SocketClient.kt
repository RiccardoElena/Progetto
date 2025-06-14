package furhatos.app.newskill.data.remote

import furhatos.app.newskill.data.remote.protocol.ProtocolMessage
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.app.newskill.setting.TEST_MODE
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
 * Client socket per la comunicazione con il server di chat
 *
 * Gestisce l'autenticazione degli utenti e lo scambio di messaggi
 * attraverso una connessione socket TCP.
 *
 * @param host indirizzo IP o hostname del server
 * @param port porta di connessione del server
 * @param onServerInfo operazioni aggiuntive da fare quando la risposta del server è SERVER_INFO
 * @param onServerError operazioni aggiuntive da fare quando un errore è ricevuto
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
) {
    private var message: ProtocolMessage.RequestType = if (TEST_MODE) ProtocolMessage.TestRequest() else ProtocolMessage.AIRequest()

    private var socket: Socket? = null
    private var writer: PrintWriter? = null
    private var reader: BufferedReader? = null
    private var _isConnected: Boolean = false
    var onServerInfo: (String) -> Unit = {
        println(it)
    }
    var onServerError: (String) -> Unit = {
        println(it)
    }
    var onDisconnected: () -> Unit = {}
    var onMessageReceived: (String) -> Unit = { println(it) }

    /**
     * Verifica se il client è connesso al server
     *
     * @return true se la connessione è attiva, false altrimenti
     */
    val isConnected: Boolean get() = _isConnected

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
     * Invia un messaggio di chat al server
     *
     * @param payload contenuto del messaggio da inviare
     * @return true se il messaggio è stato inviato con successo, false altrimenti
     * @throws IllegalStateException se l'utente non è autenticato
     */

    suspend fun sendMessage(payload: String): Boolean {
        if (!_isConnected) {
            return false
        }

        return withContext(Dispatchers.IO) {
            try {
                message.payload = payload
                if (DEBUG_MODE) {
                    println(message.toString())
                }
                writer?.println(message.toString())
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
        CoroutineScope(Dispatchers.IO).launch {
            try {
                if (_isConnected) {
                    if (DEBUG_MODE) {
                        println("reader: $reader")
                        println("socket closed : ${socket?.isClosed}, socket connected : ${socket?.isConnected}")
                    }
                    val response = reader?.readLine()
                    if (DEBUG_MODE) {
                        println(response)
                    }
                    if (response != null) {
                        handleServerMessage(response)
                    }
                    _isConnected = false
                    onDisconnected()
                    disconnect()
                }
            } catch (e: Exception) {
                _isConnected = false
                onDisconnected()
                disconnect()
            } finally {
                disconnect()
            }
        }
    }

// TODO: update using number from config.h

    /**
     * Gestisce i messaggi ricevuti dal server e invoca le callback appropriate
     *
     * @param payload messaggio grezzo ricevuto dal server
     */
    private fun handleServerMessage(payload: String) {
        val response = Parser.toMessage(payload)

        when (response) {
            is ProtocolMessage.AIResponse -> {
                onMessageReceived(response.payload)
            }
            is ProtocolMessage.TestResponse -> {
                onServerInfo("Test Response received!")
                if (message !is ProtocolMessage.TestRequest) {
                    message = ProtocolMessage.TestRequest("")
                }
                onMessageReceived(response.payload)
            }
            is ProtocolMessage.Error -> {
                onServerError(response.payload)
                onDisconnected()
                _isConnected = false
                disconnect()
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
            _isConnected = false

            writer?.close()
            reader?.close()
            socket?.close()
        } catch (e: Exception) {
            println(e)
        }
    }
}
