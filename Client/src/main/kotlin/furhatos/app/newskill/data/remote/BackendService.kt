package furhatos.app.newskill.data.remote

import furhatos.app.newskill.setting.HOST
import furhatos.app.newskill.setting.PORT

object BackendService {
    val socket: SocketClient = SocketClient(HOST, PORT)
}
