package furhatos.app.newskill.flow.main

import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onUserEnter
import furhatos.flow.kotlin.state

val Idle: State =
    state {
        onEntry {
//            furhat.ledStrip.solid(
//                if (BackendService.socket.isConnected) {
//                    java.awt.Color.WHITE
//                } else {
//                    java.awt.Color(127, 0, 0)
//                },
//            )
//            CoroutineScope(Dispatchers.Default).launch {
//                delay(3000)
//                furhat.ledStrip.solid(java.awt.Color(0, 0, 0))
//            }
            furhat.attendNobody()
        }

        onUserEnter {
//            if (!BackendService.socket.isConnected) {
//                runBlocking {
//                    val deferredResult =
//                        async {
//                            BackendService.socket.connect()
//                        }
//                    if (deferredResult.await()) {
//                        furhat.attend(it)
//                        goto(Greeting)
//                    } else {
//                        reentry()
//                    }
//                }
//            }
            furhat.attend(it)
            goto(Greeting)
        }
    }
