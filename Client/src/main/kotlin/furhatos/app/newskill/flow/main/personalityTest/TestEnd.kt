package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.flow.betterYN
import furhatos.app.newskill.flow.main.auth.Register
import furhatos.app.newskill.model.PersonalityVector
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

@Suppress("ktlint:standard:function-naming")
fun TestEnd(personalityVector: PersonalityVector) =
    state {
        onEntry {
            if (DEBUG_MODE) {
                furhat.say("Il vettore di personalità a te associato è $personalityVector")
            }
            if (betterYN("Vuoi che io mi ricordi di te la prossima volta che ci incontreremo?")) {
                call(Register(personalityVector))
            }
            // goto(conversation)
        }
    }
