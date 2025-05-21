package furhatos.app.newskill.flow.main

import furhatos.app.newskill.flow.main.personalityTest.TestStart
import furhatos.app.newskill.flow.partials.Base
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state
import furhatos.nlu.common.No
import furhatos.nlu.common.Yes

val Greeting: State =
    state {
        onEntry {
            furhat.ask("Ciao, sono furhat. Vuoi conversare con me?")
        }

        onResponse<Yes> {
            furhat.say("Ottimo, prima però vorrei capire un po' che tipo sei.")
            goto(TestStart)
        }

        onResponse<No> {
            furhat.say("Capisco. Quando vuoi sono qui se cambi idea!")
            goto(Idle)
        }
        include(Base)
    }
