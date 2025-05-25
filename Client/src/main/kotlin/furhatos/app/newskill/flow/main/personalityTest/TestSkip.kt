package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.model.usecases.ComputePersonalityUseCase
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val TestSkip =
    state {
        onEntry {
            furhat.say(
                Localization.getLocalizedString("as_you_wish"),
            )

            goto(TestEnd(ComputePersonalityUseCase()()))
        }
        include(Base)
    }
