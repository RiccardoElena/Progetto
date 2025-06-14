package furhatos.app.newskill.flow.main

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.flow.main.conversation.Conversation
import furhatos.app.newskill.flow.main.personalityTest.TestStart
import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.flow.utils.betterYN
import furhatos.app.newskill.model.PersonalityVector
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val Greeting: State =
    state {
        // TODO: we need to understand if askYN accept something different than 'Yes' or 'No' (like 'Ok', 'Of course', etc.)
        onEntry {
            if (betterYN("intro_furhat")) {

                furhat.say(Localization.getLocalizedString("start_understanding"))
                goto(TestStart)
            } else {
                furhat.say(Localization.getLocalizedString("understand_if_change"))
                // TODO: test idle reactivness
                goto(Idle)
            }
        }
        include(Base)
    }
