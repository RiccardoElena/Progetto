package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.flow.main.Greeting
import furhatos.app.newskill.flow.main.conversation.Conversation
import furhatos.app.newskill.model.PersonalityVector
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.app.newskill.setting.LOCAL_MODE
import furhatos.flow.kotlin.state

@Suppress("ktlint:standard:function-naming")
fun TestEnd(personalityVector: PersonalityVector) =
    state {
        onEntry {
            if (DEBUG_MODE) {
                println("Personality vector: $personalityVector")
            }
//            if (betterYN("Vuoi che io mi ricordi di te la prossima volta che ci incontreremo?")) {
//                call(Register(personalityVector))
//            }
            if (LOCAL_MODE) goto(Greeting)
            goto(Conversation(personalityVector))
        }
    }
