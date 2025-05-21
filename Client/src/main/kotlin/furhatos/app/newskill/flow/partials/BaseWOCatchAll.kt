package furhatos.app.newskill.flow.partials

import furhatos.app.newskill.flow.main.Idle
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onInterimResponse
import furhatos.flow.kotlin.onNoResponse
import furhatos.flow.kotlin.onUserEnter
import furhatos.flow.kotlin.onUserLeave
import furhatos.flow.kotlin.partialState
import furhatos.flow.kotlin.users
import furhatos.gestures.Gestures

val BaseWOCatchAll =
    partialState {

        onUserEnter(instant = true) {
            when { // "it" is the user that entered
                furhat.isAttendingUser -> furhat.glance(it) // Glance at new users entering
                !furhat.isAttendingUser -> furhat.attend(it) // Attend user if not attending anyone
            }
        }

        onUserLeave(instant = true) {
            when {
                !users.hasAny() -> { // last user left
                    furhat.attendNobody()
                    goto(Idle)
                }
                furhat.isAttending(it) -> furhat.attend(users.other) // current user left
                !furhat.isAttending(it) -> furhat.glance(it.head.location) // other user left, just glance
            }
        }
        onInterimResponse(endSil = 700) {
            // We give some feedback to the user, "okay" or a nod gesture.
            random(
                // Note that we need to set async = true, since we are in an instant trigger
                { furhat.say("okay", async = true) },
                // Gestures are async per default, so no need to set the flag
                { furhat.gesture(Gestures.Nod) },
            )
        }

        onNoResponse {
            // Catches silence
            furhat.ask("Scusa, non ho sentito. Puoi ripetere?")
        }
    }
