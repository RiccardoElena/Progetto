package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.flow.main.Idle
import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.model.usecases.ComputePersonalityUseCase
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val TestSkip =
    state {
        onEntry {
            furhat.say(
                "Come vuoi. Assumero una personalità media dunque! " +
                    "Il vettore di personalità a te associato è ${ComputePersonalityUseCase()()}",
            )

            goto(Idle)
        }
        include(Base)
    }
