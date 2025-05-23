package furhatos.app.newskill.flow.main

import furhatos.app.newskill.flow.betterYN
import furhatos.app.newskill.flow.main.auth.Login
import furhatos.app.newskill.flow.main.personalityTest.TestStart
import furhatos.app.newskill.flow.partials.Base
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val Greeting: State =
    state {
        // TODO: we need to understand if askYN accept something different than 'Yes' or 'No' (like 'Ok', 'Of course', etc.)
        onEntry {
            if (betterYN("Ciao, sono furhat. Vuoi conversare con me?")) {
                if (betterYN("Per caso ci conosciamo?")) {
                    furhat.say("Cavolo che sbadato, non ricordo proprio. Ricordami chi sei")
                    goto(Login)
                }
                furhat.say("Capisco, allora prima di iniziare fammi capire un po' che persona sei.")
                goto(TestStart)
            } else {
                furhat.say("Capisco. Quando vuoi sono qui se cambi idea!")
                goto(Idle)
            }
        }
        include(Base)
    }
